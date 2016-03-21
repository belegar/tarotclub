
#include <QtCore>
#include <QtTest>
#include <cstdint>
#include <iostream>

#include "tst_tarot_base.h"
#include "tst_tarot_rules.h"
#include "tst_utilities.h"
#include "tst_json.h"
#include "tst_hash.h"
#include "tst_webstuff.h"
#include "tst_database.h"

int main(int argc, char *argv[])
{
    std::uint32_t testSuccesses = 0;
    std::uint32_t testFailures = 0;

    QCoreApplication app(argc, argv);
    app.setAttribute(Qt::AA_Use96Dpi, true);
    Utilities tst_utils;
    if (QTest::qExec(&tst_utils, argc, argv) == 0)
    {
        testSuccesses++;
    }
    else
    {
        testFailures++;
    }

    TarotBase tst_base;
    if (QTest::qExec(&tst_base, argc, argv) == 0)
    {
        testSuccesses++;
    }
    else
    {
        testFailures++;
    }

    JsonTest tst_json;
    if (QTest::qExec(&tst_json, argc, argv) == 0)
    {
        testSuccesses++;
    }
    else
    {
        testFailures++;
    }

    TarotRules tst_rules;
    if (QTest::qExec(&tst_rules, argc, argv) == 0)
    {
        testSuccesses++;
    }
    else
    {
        testFailures++;
    }

    LibraryHash tst_hash;
    if (QTest::qExec(&tst_hash, argc, argv) == 0)
    {
        testSuccesses++;
    }
    else
    {
        testFailures++;
    }

    TstWebStuff tst_webstuff;
    if (QTest::qExec(&tst_webstuff, argc, argv) == 0)
    {
        testSuccesses++;
    }
    else
    {
        testFailures++;
    }

    TstDataBase tst_db;
    if (QTest::qExec(&tst_db, argc, argv) == 0)
    {
        testSuccesses++;
    }
    else
    {
        testFailures++;
    }

    std::cout << std::endl << "=============================================";
    std::cout << std::endl << "Success : " << testSuccesses;
    std::cout << std::endl << "Failures: " << testFailures;
    std::cout << std::endl << "=============================================" << std::endl;


    return 0;
}

