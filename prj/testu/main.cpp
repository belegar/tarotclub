
#include <QtCore>
#include <QtTest>

#include "tst_tarot_base.h"
#include "tst_utilities.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    app.setAttribute(Qt::AA_Use96Dpi, true);
    Utilities tst_utils;
    QTest::qExec(&tst_utils, argc, argv);

    TarotBase tst_base;
    QTest::qExec(&tst_base, argc, argv);

    return 0;
}

