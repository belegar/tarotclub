#ifndef TST_TAROT_BASE_H
#define TST_TAROT_BASE_H

#include <QString>
#include <QtTest>
#include <QCoreApplication>
#include <cstdint>

class TarotBase : public QObject
{
    Q_OBJECT

public:
    TarotBase();

private Q_SLOTS:
    void TestCardClass();


private:

};

#endif // TST_TAROT_BASE_H
