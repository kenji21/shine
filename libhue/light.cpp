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

Light::Light(int id, const QString &name, QObject *parent):
    LightInterface(parent),
    m_id(id),
    m_name(name),
    m_on(false),
    m_busyStateChangeId(-1),
    m_hueDirty(false),
    m_satDirty(false),
    m_briDirty(false),
    m_ctDirty(false)
{
    refresh();
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
            m_busyStateChangeId = HueBridgeConnection::instance()->put("lights/" + QString::number(m_id) + "/state", params, this, "setStateFinished");
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

void Light::setColor(const QColor &color)
{
    // Transform from RGB to Hue/Sat
    quint16 hue = color.hue() * 65535 / 360;
    quint8 sat = color.saturation();

    // Transform from RGB to XYZ
    QGenericMatrix<3, 3, qreal> rgb2xyzMatrix;
    rgb2xyzMatrix(0, 0) = 0.412453;    rgb2xyzMatrix(0, 1) = 0.357580;    rgb2xyzMatrix(0, 2) = 0.180423;
    rgb2xyzMatrix(1, 0) = 0.212671;    rgb2xyzMatrix(1, 1) = 0.715160;    rgb2xyzMatrix(1, 2) = 0.072169;
    rgb2xyzMatrix(2, 0) = 0.019334;    rgb2xyzMatrix(2, 1) = 0.119193;    rgb2xyzMatrix(2, 2) = 0.950227;

    QGenericMatrix<1, 3, qreal> rgbMatrix;
    rgbMatrix(0, 0) = 1.0 * color.red() / 255;
    rgbMatrix(1, 0) = 1.0 * color.green() / 255;
    rgbMatrix(2, 0) = 1.0 * color.blue() / 255;

    QGenericMatrix<1, 3, qreal> xyzMatrix = rgb2xyzMatrix * rgbMatrix;

    // transform from XYZ to CIELUV u' and v'
    qreal u = 4*xyzMatrix(0, 0) / (xyzMatrix(0, 0) + 15*xyzMatrix(1, 0) + 3*xyzMatrix(2, 0));
    qreal v = 9*xyzMatrix(1, 0) / (xyzMatrix(0, 0) + 15*xyzMatrix(1, 0) + 3*xyzMatrix(2, 0));

    // Transform from CIELUV to (x,y)
    qreal x = 27*u / (18*u - 48*v + 36);
    qreal y = 12*v / (18*u - 48*v + 36);

    if (m_busyStateChangeId == -1) {
        QVariantMap params;

//        params.insert("hue", hue);
//        params.insert("sat", sat);

        QVariantList xyList;
        xyList << x << y;
        params.insert("xy", xyList);

        // FIXME: There is a bug in the API that it doesn't report back the set state of "sat"
        // Lets just assume it always succeeds
        m_sat = m_dirtySat;

        m_busyStateChangeId = HueBridgeConnection::instance()->put("lights/" + QString::number(m_id) + "/state", params, this, "setStateFinished");
    } else {
        m_dirtyHue = hue;
        m_hueDirty = true;
        m_dirtySat = sat;
        m_satDirty = true;
    }
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
        m_busyStateChangeId = HueBridgeConnection::instance()->put("lights/" + QString::number(m_id) + "/state", params, this, "setStateFinished");
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
    if (m_alert != alert) {
        m_alert = alert;
        emit stateChanged();
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
    HueBridgeConnection::instance()->get("lights/" + QString::number(m_id), this, "responseReceived");
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

//    qDebug() << "got light response" << m_modelId << m_type << m_swversion << m_on << m_bri << m_reachable;
}

void Light::setDescriptionFinished(int id, const QVariant &response)
{
    Q_UNUSED(id)
    qDebug() << "setDescription finished" << response;
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
    QVariantMap result = response.toList().first().toMap();

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
        emit stateChanged();
    }

    if (m_busyStateChangeId == id) {
        m_busyStateChangeId = -1;
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
        } else if(m_ctDirty) {
            QVariantMap params;
            params.insert("ct", m_dirtyCt);
            m_ctDirty = false;

            m_busyStateChangeId = HueBridgeConnection::instance()->put("lights/" + QString::number(m_id) + "/state", params, this, "setStateFinished");
        }
    }
}
