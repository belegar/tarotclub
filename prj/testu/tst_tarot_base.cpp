
#include <QString>
#include <QtTest>
#include <QCoreApplication>
#include <cstdint>
#include <iostream>

#include "tst_tarot_base.h"
#include "Card.h"
#include "Deck.h"
#include "TarotDeck.h"
#include "ServerConfig.h"

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

void TarotBase::TestDeckClass()
{
    Deck myDeck;

    // Add all tarot cards into the deck
    for (std::uint32_t i = 0U; i < 78U; i++)
    {
        Card *c = TarotDeck::GetCard(i);
        myDeck.Append(c);
    }

    QCOMPARE(myDeck.Size(), 78U);

    myDeck.Clear();

    QCOMPARE(myDeck.Size(), 0U);

    // Build a string list of cards
    std::string listOfCards;
    for (std::uint32_t i = 0U; i < 78U; i++)
    {
        Card *c = TarotDeck::GetCard(i);
        listOfCards += c->GetName();
        if (i != 77)
        {
            listOfCards += ";";
        }
    }

    myDeck.SetCards(listOfCards);

    QCOMPARE(myDeck.Size(), 78U);

    // Spades, Diamonds, Trumps, Clubs, Hearts
    myDeck.Sort("SDTCH");

    std::cout << "Deck ordered: \n";
    std::cout << myDeck.GetCardList() << std::endl;

    myDeck.Shuffle(1);

    std::cout << "Deck shuffled: \n";
    std::cout << myDeck.GetCardList() << std::endl;

    // Testing overloaded operators
    Deck one, two, three;

    one.SetCards("1-T;2-T;3-T");
    two.SetCards("4-T;5-T;6-T");

    three = one + two;

    std::cout << "Added deck: \n";
    std::cout << three.GetCardList() << std::endl;

}

void TarotBase::TestConfig()
{
    ServerOptions options;
    ServerConfig conf;

    conf.SetDefault(options);
    conf.SetOptions(options);
    conf.Save();
}


