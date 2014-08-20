
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
#include "Player.h"

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
        Place p(2U);
        c->SetOwner(p);
        QCOMPARE(p.Value(), c->GetOwner().Value());

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

    options = ServerConfig::GetDefault();
    conf.SetOptions(options);
    conf.Save();
}

void TarotBase::TestCommon()
{
    // Test the size of the objects, they must be 1 byte length in memory
    std::uint32_t actual = sizeof(Place);
    std::uint32_t expected = 1U;
    QCOMPARE(actual, expected);

    actual = sizeof(Contract);
    expected = 1U;
    QCOMPARE(actual, expected);
}

void TarotBase::TestCard(const std::string &card, bool expected)
{
    Card *c = TarotDeck::GetCard(card);
    bool actual = player.CanPlayCard(c, currentTrick);
    QCOMPARE(actual, expected);
}


void TarotBase::TestCanPlayCard()
{
    player.SetCards("12-C;12-H;10-H;02-H;03-H;11-C;11-T;07-D;03-T;09-C;13-C;05-H;00-T");

    // ===================================
    // Test 1: the player must cut (he does not have the requested suit)
    // ===================================
    currentTrick.SetCards("13-S;03-S;06-S");
    // A trump is valid
    TestCard("11-T", true);
    TestCard("03-T", true);
    TestCard("00-T", true); // the fool is ok
    TestCard("05-T", false); // not this one! he does not have it
    // A suit is not valid (because he must cut)
    TestCard("02-H", false);
    TestCard("12-C", false);
    TestCard("07-D", false);
    // The requested suit is not valid (because he has not this card)
    TestCard("02-S", false);

    // ===================================
    // Test 2: the player is the first to play, all the cards are valid
    // ===================================
    currentTrick.Clear();
    TestCard("11-T", true);
    TestCard("03-T", true);
    TestCard("05-T", false); // not this one! he does not have it
    TestCard("02-H", true);
    TestCard("12-C", true);
    TestCard("07-D", true);
    TestCard("00-T", true); // the fool is ok
    TestCard("02-S", false);  // not this one! he does not have it

    // ===================================
    // Test 3: the player is the second to play, after a played fool: all the cards are valid
    // ===================================
    currentTrick.SetCards("00-T");
    TestCard("11-T", true);
    TestCard("03-T", true);
    TestCard("05-T", false); // not this one! he does not have it
    TestCard("02-H", true);
    TestCard("12-C", true);
    TestCard("07-D", true);
    TestCard("02-S", false);  // not this one! he does not have it

    // ===================================
    // Test 3: plays the fool after different cards (first, after a suit, after a trump ...), must cut, has the suit
    // ===================================
    // He has the suit but he can play the fool
    currentTrick.SetCards("01-D;02-D;03-D");
    TestCard("00-T", true); // the fool is ok

    // he does not have to overcut
    currentTrick.SetCards("05-T;06-T;07-T");
    TestCard("00-T", true); // the fool is ok

    // ===================================
    // Test 4: The player must over-cut
    // ===================================
    // We remove the fool from its hand
    player.SetCards("12-C;12-H;10-H;02-H;03-H;11-C;11-T;07-D;03-T;09-C;13-C;05-H");

    currentTrick.SetCards("05-T;06-T;07-T");
    TestCard("00-T", false); // he does not have this card
    TestCard("03-T", false); // too little
    TestCard("02-H", false); // not a trump!
    TestCard("11-T", true);

    // The player does not have higher trump, he can't overcut but it still needs to play a trump
    player.SetCards("12-C;12-H;10-H;02-H;03-H;11-C;07-D;03-T;09-C;13-C;05-H");
    TestCard("00-T", false); // he does not have this card
    TestCard("03-T", true); // the only trump he has
    TestCard("02-H", false); // not a trump!
    TestCard("11-T", false);// he does not have this card

    // ===================================
    // Test 5: The player does not have the requested suit and no any trumps
    // ===================================
    player.SetCards("12-C;12-H;10-H;02-H;03-H;11-C;07-D;09-C;13-C;05-H"); // not spades and trumps
    currentTrick.SetCards("13-S;03-S;06-S");

    TestCard("11-T", false);// he does not have this card
    TestCard("05-S", false);// he does not have this card

    // Any card is correct
    TestCard("12-C", true);
    TestCard("10-H", true);
    TestCard("07-D", true);
    TestCard("13-C", true);

}


