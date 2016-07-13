#ifndef TST_TAROT_BASE_H
#define TST_TAROT_BASE_H

#include <QString>
#include <QtTest>
#include <QCoreApplication>
#include <cstdint>
#include "Player.h"

/**
 * @brief The TarotBase class
 *
 * This clas tests all the base classes of the game
 */
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
    void TestUniqueId();
    void TestWorkThread();

};

#endif // TST_TAROT_BASE_H
