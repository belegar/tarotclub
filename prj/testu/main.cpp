
#include <QtCore>
#include <QtTest>

#include "tst_tarot_base.h"
#include "tst_utilities.h"
#include "tst_jsengine.h"
#include "tst_hash.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    app.setAttribute(Qt::AA_Use96Dpi, true);
    Utilities tst_utils;
    QTest::qExec(&tst_utils, argc, argv);

    TarotBase tst_base;
    QTest::qExec(&tst_base, argc, argv);

    JsonTest tst_json;
    QTest::qExec(&tst_json, argc, argv);

    LibraryHash tst_hash;
    QTest::qExec(&tst_hash, argc, argv);

    return 0;
}

