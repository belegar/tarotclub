
#include <QString>
#include <QtTest>
#include <QCoreApplication>
#include <cstdint>
#include <iostream>

#include "tst_tarot_base.h"
#include "Card.h"
#include "TarotDeck.h"

TarotBase::TarotBase()
{
}

void TarotBase::TestCardClass()
{
    TarotDeck::Initialize();
    float total = 0.0f;

    std::cout << "Card names: \n";
    for (std::uint32_t i = 0U; i < 78U; i++)
    {
        Card *c = TarotDeck::GetCard(i);
        std::cout << c->GetName() << " ";
        if (c->GetValue() == 14 && c->GetSuit() != Card::TRUMPS)
        {
            std::cout << "\n";
        }

        total += c->GetPoints();
    }

    // A Tarot deck has a total of 91 points
    QCOMPARE(total, 91.0f);

    std::cout << "\n";
}


