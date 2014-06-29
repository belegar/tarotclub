#ifndef TST_HASH_H
#define TST_HASH_H

#include <QString>
#include <QtTest>
#include <QCoreApplication>
#include <cstdint>

class LibraryHash : public QObject
{
    Q_OBJECT

public:
    LibraryHash();

private Q_SLOTS:
    void TestBase64();


private:

};

#endif // TST_HASH_H
