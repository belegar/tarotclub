#ifndef TST_JSENGINE_H
#define TST_JSENGINE_H

#include <QString>
#include <QtTest>
#include <QCoreApplication>
#include <cstdint>

class JsonTest : public QObject
{
    Q_OBJECT

public:
    JsonTest();

private Q_SLOTS:
    void ParseFile();

private:

};

#endif // TST_JSENGINE_H
