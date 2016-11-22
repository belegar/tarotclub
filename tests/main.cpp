
#include <QtCore>
#include <QtTest>
#include <cstdint>
#include <iostream>

#include "tst_utilities.h"
#include "tst_json.h"
#include "tst_hash.h"
#include "tst_webstuff.h"
#include "tst_database.h"
#include "tst_tcp_socket.h"

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

    JsonTest tst_json;
    if (QTest::qExec(&tst_json, argc, argv) == 0)
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

    TcpSocketTest tst_sock;
    if (QTest::qExec(&tst_sock, argc, argv) == 0)
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

