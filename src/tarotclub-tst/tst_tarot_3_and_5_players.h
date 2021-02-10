#ifndef TST_TAROT_3_5_PLAYERS_H
#define TST_TAROT_3_5_PLAYERS_H

#include <QString>
#include <QtTest>
#include <QCoreApplication>
#include <cstdint>

class Tarot3And5Players : public QObject
{
    Q_OBJECT

public:
    Tarot3And5Players();

private Q_SLOTS:

    void TestDeal3Players();
    void TestDeal5Players();
    void TestFullGame3Players();

private:
    void TestFullGame(uint32_t nbPlayers);
};

#endif // TST_TAROT_3_5_PLAYERS_H
