#ifndef TST_TAROT_PROTOCOL_H
#define TST_TAROT_RULES_H

#include <QString>
#include <QtTest>
#include <QCoreApplication>
#include <cstdint>
#include "Player.h"

class TarotProtocol : public QObject
{
    Q_OBJECT

public:
    TarotProtocol();


private Q_SLOTS:
    void TestEmptyPacket();
    void TestCiphering();
    void TestPacketStream();
    void TestPlayerJoinQuitAndPlayerList();
    void TestBotsFullGame();

private:

};

#endif // TST_TAROT_RULES_H
