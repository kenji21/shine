/*
 * Copyright 2013 Michael Zanetti
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; version 2.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Authors:
 *      Michael Zanetti <michael_zanetti@gmx.net>
 */

#include "light.h"
#include "huebridgeconnection.h"

#include <QColor>
#include <QDebug>
#include <QGenericMatrix>
#include <math.h>

Light::Light(int id, const QString &name, QObject *parent):
    LightInterface(parent),
    m_id(id),
    m_name(name),
    m_on(false),
    m_busyStateChangeId(-1),
    m_hueDirty(false),
    m_satDirty(false),
    m_briDirty(false),
    m_ctDirty(false),
    m_xyDirty(false)
{
    m_timeout.setInterval(250);
    connect(&m_timeout, &QTimer::timeout, this, &Light::timeout);
}

int Light::id() const
{
    return m_id;
}

QString Light::name() const
{
    return m_name;
}

void Light::setName(const QString &name)
{
    if (m_name != name) {
        QVariantMap params;
        params.insert("name", name);
        HueBridgeConnection::instance()->put("lights/" + QString::number(m_id), params, this, "setDescriptionFinished");
    }
}

QString Light::modelId() const
{
    return m_modelId;
}

void Light::setModelId(const QString &modelId)
{
    if (m_modelId != modelId) {
        m_modelId = modelId;
        emit modelIdChanged();
    }
}

QString Light::type() const
{
    return m_type;
}

void Light::setType(const QString &type)
{
    if (m_type != type) {
        m_type = type;
        emit typeChanged();
    }
}

QString Light::swversion() const
{
    return m_swversion;
}

void Light::setSwversion(const QString &swversion)
{
    if (m_swversion != swversion) {
        m_swversion = swversion;
        emit swversionChanged();
    }
}

bool Light::on() const
{
    return m_on;
}

void Light::setOn(bool on)
{
    if (m_on != on) {
        QVariantMap params;
        params.insert("on", on);
        HueBridgeConnection::instance()->put("lights/" + QString::number(m_id) + "/state", params, this, "setStateFinished");
    }
}

quint8 Light::bri() const
{
    return m_bri;
}

void Light::setBri(quint8 bri)
{
    if (m_bri != bri) {
        qDebug() << "setting brightness to" << bri << m_busyStateChangeId;
        if (m_busyStateChangeId == -1) {
            QVariantMap params;
            params.insert("bri", bri);
            params.insert("on", true);
            m_busyStateChangeId = HueBridgeConnection::instance()->put("lights/" + QString::number(m_id) + "/state", params, this, "setStateFinished");
            m_timeout.start();
        } else {
            m_dirtyBri = bri;
            m_briDirty = true;
        }
    }
}

quint16 Light::hue() const
{
    return m_hue;
}

void Light::setHue(quint16 hue)
{
    if (m_hue != hue) {
        m_hue = hue;
        emit stateChanged();
    }
}

quint8 Light::sat() const
{
    return m_sat;
}

void Light::setSat(quint8 sat)
{
    if (m_sat != sat) {
        m_sat = sat;
        emit stateChanged();
    }
}

QColor Light::color() const
{
    return QColor::fromHsv(m_hue * 360 / 65535, m_sat, 255);
}

void Light::setColorWithXY(const QColor &color)
{
    // For the hue bulb the corners of the triangle are:
    // -Red: 0.675, 0.322
    // -Green: 0.4091, 0.518
    // -Blue: 0.167, 0.04
    double normalizedToOne[3];
    float cred, cgreen, cblue;
    cred = color.redF();
    cgreen = color.greenF();
    cblue = color.blueF();
    normalizedToOne[0] = (cred / 255);
    normalizedToOne[1] = (cgreen / 255);
    normalizedToOne[2] = (cblue / 255);
    float red, green, blue;

    // Make red more vivid
    if (normalizedToOne[0] > 0.04045) {
        red = (float) pow(
                (normalizedToOne[0] + 0.055) / (1.0 + 0.055), 2.4);
    } else {
        red = (float) (normalizedToOne[0] / 12.92);
    }

    // Make green more vivid
    if (normalizedToOne[1] > 0.04045) {
        green = (float) pow((normalizedToOne[1] + 0.055)
                / (1.0 + 0.055), 2.4);
    } else {
        green = (float) (normalizedToOne[1] / 12.92);
    }

    // Make blue more vivid
    if (normalizedToOne[2] > 0.04045) {
        blue = (float) pow((normalizedToOne[2] + 0.055)
                / (1.0 + 0.055), 2.4);
    } else {
        blue = (float) (normalizedToOne[2] / 12.92);
    }

    float X = (float) (red * 0.649926 + green * 0.103455 + blue * 0.197109);
    float Y = (float) (red * 0.234327 + green * 0.743075 + blue * 0.022598);
    float Z = (float) (red * 0.0000000 + green * 0.053077 + blue * 1.035763);

    float x = X / (X + Y + Z);
    float y = Y / (X + Y + Z);

    int bri = color.value();

    qDebug() << "setting color" << color  << "busy:" << m_busyStateChangeId;
    if (m_busyStateChangeId == -1) {
        qDebug() << "setting color" << color << "for light" << QString::number(m_id);

        params.insert("hue", hue);
        params.insert("sat", sat);
        // FIXME: There is a bug in the API that it doesn't report back the set state of "sat"
        // Lets just assume it always succeeds
        m_sat = sat;

//        QVariantList xyList;
//        xyList << x << y;
//        params.insert("xy", xyList);
        Q_UNUSED(x); Q_UNUSED(y);

        QVariantList xyList;
        xyList << x << y;
        params.insert("xy", xyList);
        params.insert("bri", bri);

        params.insert("on", true);
        qDebug() << "Starting timeout and PUT ... " << "for light" << QString::number(m_id);
        m_timeout.start();
        m_busyStateChangeId = HueBridgeConnection::instance()->put("lights/" + QString::number(m_id) + "/state", params, this, "setStateFinished");
        m_timeout.start();
    } else {
        qDebug() << "PUT already running (going dirty bri and xy)"
            << m_busyStateChangeId << "not setting color" << color
            << "for light" << QString::number(m_id);

        m_briDirty = true;
        m_dirtyBri = bri;
        m_xyDirty = true;
        m_dirtyXy = QPointF(x, y);
    }
}

void Light::setColor(const QColor &color)
{
    setColorWithXY(color);
}

QPointF Light::xy() const
{
    return m_xy;
}

void Light::setXy(const QPointF &xy)
{
    if (m_xy != xy) {
        m_xy = xy;
        emit stateChanged();
    }
}

quint16 Light::ct() const
{
    return m_ct;
}

void Light::setCt(quint16 ct)
{
    if (m_busyStateChangeId == -1) {
        QVariantMap params;
        params.insert("ct", ct);
        params.insert("on", true);
        m_busyStateChangeId = HueBridgeConnection::instance()->put("lights/" + QString::number(m_id) + "/state", params, this, "setStateFinished");
        m_timeout.start();
    } else {
        m_dirtyCt = ct;
        m_ctDirty = true;
    }
}

QString Light::alert() const
{
    return m_alert;
}

void Light::setAlert(const QString &alert)
{
    qDebug() << "settings alert" << alert << m_alert;
    if (m_alert != alert) {
        QVariantMap params;
        params.insert("alert", alert);
        if (alert != "none") {
            params.insert("on", true);
        }
        HueBridgeConnection::instance()->put("lights/" + QString::number(m_id) + "/state", params, this, "setStateFinished");
    }
}

QString Light::effect() const
{
    return m_effect;
}

void Light::setEffect(const QString &effect)
{
    if (m_effect != effect) {
        QVariantMap params;
        params.insert("effect", effect);
        if (effect != "none") {
            params.insert("on", true);
        }
        HueBridgeConnection::instance()->put("lights/" + QString::number(m_id) + "/state", params, this, "setStateFinished");
    }
}

LightInterface::ColorMode Light::colorMode() const
{
    return m_colormode;
}

bool Light::reachable() const
{
    return m_reachable;
}

void Light::refresh()
{
    if( m_refreshed == false )
    {
        qDebug() << " Light::refresh()" << endl;
        m_timeout.start();
        HueBridgeConnection::instance()->get("lights/" + QString::number(m_id), this, "responseReceived");
        m_refreshed = true;
    }
}

void Light::setReachable(bool reachable)
{
    if (m_reachable != reachable) {
        m_reachable = reachable;
        emit stateChanged();
    }
}

void Light::responseReceived(int id, const QVariant &response)
{
    Q_UNUSED(id)
    QVariantMap attributes = response.toMap();

    setModelId(attributes.value("modelid").toString());
    setType(attributes.value("type").toString());
    setSwversion(attributes.value("swversion").toString());

    QVariantMap stateMap = attributes.value("state").toMap();
    m_on = stateMap.value("on").toBool();
    m_bri = stateMap.value("bri").toInt();
    m_hue = stateMap.value("hue").toInt();
    m_sat = stateMap.value("sat").toInt();
    m_xy = stateMap.value("xy").toPointF();
    m_ct = stateMap.value("ct").toInt();
    m_alert = stateMap.value("alert").toString();
    m_effect = stateMap.value("effect").toString();
    QString colorModeString = stateMap.value("colormode").toString();
    if (colorModeString == "hs") {
        m_colormode = ColorModeHS;
    } else if (colorModeString == "xy") {
        m_colormode = ColorModeXY;
    } else if (colorModeString == "ct") {
        m_colormode = ColorModeCT;
    }
    m_reachable = stateMap.value("reachable").toBool();
    emit stateChanged();

   qDebug() << "got light response" << m_modelId << m_type << m_swversion << m_on << m_bri << m_reachable;
}

void Light::setDescriptionFinished(int id, const QVariant &response)
{
    Q_UNUSED(id)
    QVariantMap result = response.toList().first().toMap();

    if (result.contains("success")) {
        QVariantMap successMap = result.value("success").toMap();
        if (successMap.contains("/lights/" + QString::number(m_id) + "/name")) {
            m_name = successMap.value("/lights/" + QString::number(m_id) + "/name").toString();
            emit nameChanged();
        }
    }
}

void Light::setStateFinished(int id, const QVariant &response)
{
    if( id != m_busyStateChangeId )
    {
        qDebug() << "PUT finished but not for current query id " << QString::number(m_busyStateChangeId) << "... (received " << QString::number(id) << ")";
        return;
    }
    if( m_xyDirty || m_hueDirty || m_satDirty || m_briDirty || m_ctDirty )
    {
        qDebug() << "PUT finished (with dirty states m_briDirty: " << m_briDirty
            << ", m_xyDirty: " << m_xyDirty
            << ", m_hueDirty: " << m_hueDirty
            << ", m_satDirty: " << m_satDirty
            << ", m_briDirty: " << m_briDirty
            << ", m_ctDirty: " << m_ctDirty
            << ") for light" << QString::number(m_id);
    }

    qDebug() << "set state finished" << response;
    foreach (const QVariant &resultVariant, response.toList()) {
        QVariantMap result = resultVariant.toMap();
        if (result.contains("success")) {
            QVariantMap successMap = result.value("success").toMap();
            if (successMap.contains("/lights/" + QString::number(m_id) + "/state/on")) {
                m_on = successMap.value("/lights/" + QString::number(m_id) + "/state/on").toBool();
            }
            if (successMap.contains("/lights/" + QString::number(m_id) + "/state/hue")) {
                m_hue = successMap.value("/lights/" + QString::number(m_id) + "/state/hue").toInt();
                m_colormode = ColorModeHS;
            }
            if (successMap.contains("/lights/" + QString::number(m_id) + "/state/bri")) {
                m_bri = successMap.value("/lights/" + QString::number(m_id) + "/state/bri").toInt();
            }
            if (successMap.contains("/lights/" + QString::number(m_id) + "/state/sat")) {
                m_sat = successMap.value("/lights/" + QString::number(m_id) + "/state/sat").toInt();
                m_colormode = ColorModeHS;
            }
            if (successMap.contains("/lights/" + QString::number(m_id) + "/state/xy")) {
                m_xy = successMap.value("/lights/" + QString::number(m_id) + "/state/xy").toPoint();
                m_colormode = ColorModeXY;
            }
            if (successMap.contains("/lights/" + QString::number(m_id) + "/state/ct")) {
                m_ct = successMap.value("/lights/" + QString::number(m_id) + "/state/ct").toInt();
                m_colormode = ColorModeCT;
            }
            if (successMap.contains("/lights/" + QString::number(m_id) + "/state/effect")) {
                m_effect = successMap.value("/lights/" + QString::number(m_id) + "/state/effect").toString();
            }
            if (successMap.contains("/lights/" + QString::number(m_id) + "/state/alert")) {
                m_alert = successMap.value("/lights/" + QString::number(m_id) + "/state/alert").toString();
            }
        }
    }
    emit stateChanged();
    emit writeOperationFinished();

    if (m_busyStateChangeId == id) {
        m_busyStateChangeId = -1;
        m_timeout.stop();
        if (m_hueDirty || m_satDirty || m_briDirty) {
            QVariantMap params;
            if (m_hueDirty) {
                params.insert("hue", m_dirtyHue);
                m_hueDirty = false;
            }
            if (m_satDirty) {
                params.insert("sat", m_dirtySat);
                m_satDirty = false;
            }
            if (m_briDirty) {
                params.insert("bri", m_dirtyBri);
                m_briDirty = false;
            }

            // FIXME: There is a bug in the API that it doesn't report back the set state of "sat"
            // Lets just assume it always succeeds
            m_sat = m_dirtySat;

            m_busyStateChangeId = HueBridgeConnection::instance()->put("lights/" + QString::number(m_id) + "/state", params, this, "setStateFinished");
            m_timeout.start();
        } else if(m_ctDirty) {
            QVariantMap params;
            params.insert("ct", m_dirtyCt);
            m_ctDirty = false;

            m_busyStateChangeId = HueBridgeConnection::instance()->put("lights/" + QString::number(m_id) + "/state", params, this, "setStateFinished");
            m_timeout.start();
        } else if (m_xyDirty) {
            QVariantMap params;
            QVariantList xyList;
            xyList << m_dirtyXy.x() << m_dirtyXy.y();
            params.insert("xy", xyList);
            m_xyDirty = false;

            m_busyStateChangeId = HueBridgeConnection::instance()->put("lights/" + QString::number(m_id) + "/state", params, this, "setStateFinished");
            m_timeout.start();
        }
    }
}

void Light::timeout()
{
    if (m_busyStateChangeId != -1) {
        setStateFinished(m_busyStateChangeId, QVariant());
    }
}
