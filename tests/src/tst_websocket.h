#ifndef TST_WEBSOCKET_H
#define TST_WEBSOCKET_H

#include <QString>
#include <QtTest>
#include <QCoreApplication>

class TstWebSocket : public QObject
{
    Q_OBJECT

public:
    TstWebSocket();

private Q_SLOTS:
    void Upgrade();

private:

};

#endif // TST_WEBSOCKET_H
