/*
 * Copyright 2013 Christian Muehlhaeuser
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
 *      Christian Muehlhaeuser <muesli@gmail.com>
 */

#ifndef DISCOVERY_H
#define DISCOVERY_H

#include <QUdpSocket>
#include <QHostAddress>

class QTimer;

class Discovery: public QUdpSocket
{
    Q_OBJECT

public:
    Discovery(QObject *parent);
    void findBridges();

signals:
    void error();
    void foundBridge(QHostAddress bridge, QString bridgeid);
    void noBridgesFound();

private slots:
    void onTimeout();
    void onReadyRead();

private:
    QList<QHostAddress> m_reportedBridges;
    QTimer *m_timeout;
};

#endif
