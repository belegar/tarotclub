
#include <QtCore>
#include <QtTest>
#include <cstdint>
#include <iostream>

#include "tst_tarot_base.h"
#include "tst_tarot_rules.h"
#include "tst_tarot_protocol.h"
#include "tst_tarot_3_and_5_players.h"

#include "Log.h"

/*****************************************************************************/
class Logger : public Observer<Log::Infos>
{
public:
    Logger()
        : Observer()
    {

    }

    void Update(const Log::Infos &info)
    {
        if (info.event == Log::Error)
        {
            std::cout << info.ToString() << std::endl;
            QFAIL(info.message.c_str());
        }
    }
};
/*****************************************************************************/
int main(int argc, char *argv[])
{
    std::uint32_t testSuccesses = 0;
    std::uint32_t testFailures = 0;

    QCoreApplication app(argc, argv);

    Logger logger;
    Log::EnableLog(false);
    Log::RegisterListener(logger);

    TarotBase tst_base;
    if (QTest::qExec(&tst_base, argc, argv) == 0)
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

    Tarot3And5Players tst_3_and_5_players;
    if (QTest::qExec(&tst_3_and_5_players, argc, argv) == 0)
    {
        testSuccesses++;
    }
    else
    {
        testFailures++;
    }
/*
    TarotProtocol tst_protocol;
    if (QTest::qExec(&tst_protocol, argc, argv) == 0)
    {
        testSuccesses++;
    }
    else
    {
        testFailures++;
    }
    */

    std::cout << std::endl << "=============================================";
    std::cout << std::endl << "Success : " << testSuccesses;
    std::cout << std::endl << "Failures: " << testFailures;
    std::cout << std::endl << "=============================================" << std::endl;

    return 0;
}

