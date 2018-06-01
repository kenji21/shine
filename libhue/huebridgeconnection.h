/*
 * Copyright 2013 Michael Zanetti
 *                Christian Muehlhaeuser
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
 *      Christian Muehlhaeuser <muesli@gmail.com>
 */

#ifndef HUEBRIDGECONNECTION_H
#define HUEBRIDGECONNECTION_H

#include <QObject>
#include <QHash>
#include <QHostAddress>
#include <QVariantMap>
#include <QPointer>
#include "discovery.h"

class QNetworkAccessManager;
class QNetworkReply;

class CallbackObject
{
public:
    CallbackObject(QPointer<QObject> sender = 0, const QString &slot = QString()):
        m_sender(sender),
        m_slot(slot)
    {}
    QPointer<QObject> sender() const { return m_sender; }
    QString slot() const { return m_slot; }
private:
    QPointer<QObject> m_sender;
    QString m_slot;
};

class HueBridgeConnection: public QObject
{
    Q_OBJECT
    Q_ENUMS(BridgeStatus)

    Q_PROPERTY(QString apiKey READ apiKey WRITE setApiKey NOTIFY apiKeyChanged)
    Q_PROPERTY(bool discoveryError READ discoveryError NOTIFY discoveryErrorChanged)
    // TODO: Convert this to a model holding all the discovered bridges
    Q_PROPERTY(bool bridgeFound READ bridgeFound NOTIFY bridgeFoundChanged)
    Q_PROPERTY(QString connectedBridge READ connectedBridge NOTIFY connectedBridgeChanged)
    Q_PROPERTY(BridgeStatus status READ status NOTIFY statusChanged)

public:
    enum BridgeStatus {
        BridgeStatusSearching,
        BridgeStatusConnecting,
        BridgeStatusAuthenticationFailure,
        BridgeStatusConnected
    };

    static HueBridgeConnection* instance();
    Discovery *m_discovery;

    QString apiKey() const;
    void setApiKey(const QString &apiKey);

    QString bridgeId() const;

    bool discoveryError() const;
    bool bridgeFound() const;
    QString connectedBridge() const;

    BridgeStatus status() const;
    void findBridges();

    Q_INVOKABLE void createUser(const QString &devicetype);

    int get(const QString &path, QObject *sender, const QString &slot);
    int deleteResource(const QString &path, QObject *sender, const QString &slot);
    int post(const QString &path, const QVariantMap &params, QObject *sender, const QString &slot);
    int put(const QString &path, const QVariantMap &params, QObject *sender, const QString &slot);

signals:
    void apiKeyChanged();
    void discoveryErrorChanged();
    void bridgeFoundChanged();
    void noBridgesFound();
    void connectedBridgeChanged();
    void statusChanged();

    void createUserFailed(const QString &errorMessage);

private slots:
    void onDiscoveryError();
    void onFoundBridge(QHostAddress bridge, QString bridgeid);
    void onNoBridgesFound();

    void createUserFinished();
    void checkForUpdateFinished();
    void slotOpFinished();

private:
    HueBridgeConnection();
    static HueBridgeConnection *s_instance;

    QNetworkAccessManager *m_nam;

    QHostAddress m_bridge;
    QString m_bridgeid;
    bool m_discoveryError;
    QString m_apiKey;
    QString m_baseApiUrl;
    BridgeStatus m_bridgeStatus;

    int m_requestCounter;
    QHash<QNetworkReply*, int> m_requestIdMap;
    QHash<int, CallbackObject> m_requestSenderMap;

    // This is used to store write operations so clients can be notfied to refresh after those succeed.
    QList<QNetworkReply*> m_writeOperationList;
};

#endif
