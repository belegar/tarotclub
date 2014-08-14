#ifndef TST_TAROT_BASE_H
#define TST_TAROT_BASE_H

#include <QString>
#include <QtTest>
#include <QCoreApplication>
#include <cstdint>
#include "Player.h"

class TarotBase : public QObject
{
    Q_OBJECT

public:
    TarotBase();

private Q_SLOTS:
    void TestCardClass();
    void TestDeckClass();
    void TestConfig();
    void TestCommon();
    void TestCanPlayCard();

private:

    Player player;
    Deck currentTrick;

    void TestCard(const std::string &card, bool expected);
};

#endif // TST_TAROT_BASE_H
