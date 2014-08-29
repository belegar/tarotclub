#ifndef TST_DATABASE_H
#define TST_DATABASE_H

#include <QString>
#include <QtTest>
#include <QCoreApplication>

class TstDataBase : public QObject
{
    Q_OBJECT

public:
    TstDataBase();

private Q_SLOTS:
    void Query();

private:

};

#endif // TST_DATABASE_H
