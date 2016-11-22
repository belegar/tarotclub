#ifndef TST_TCP_SOCKET_H
#define TST_TCP_SOCKET_H

#include <QString>
#include <QtTest>
#include <QCoreApplication>
#include <cstdint>

class TcpSocketTest : public QObject
{
    Q_OBJECT

public:
    TcpSocketTest();

private Q_SLOTS:
    void SimpleTcpEchoServer();

private:

};

#endif // TST_TCP_SOCKET_H
