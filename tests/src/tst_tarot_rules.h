#ifndef TST_TAROT_RULES_H
#define TST_TAROT_RULES_H

#include <QString>
#include <QtTest>
#include <QCoreApplication>
#include <cstdint>
#include "Player.h"

class TarotRules : public QObject
{
    Q_OBJECT

public:
    TarotRules();

private Q_SLOTS:
    void TestCanPlayCard();
    void TestScoreCalculation();

private:
    Player player;
    Deck currentTrick;

    void TestCard(const std::string &card, bool expected);
};

#endif // TST_TAROT_RULES_H
