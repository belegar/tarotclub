
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

    // ------------------------------------------------------------------------
    // Test 1: Add all tarot cards into the deck
    for (std::uint32_t i = 0U; i < 78U; i++)
    {
        Card *c = TarotDeck::GetCard(i);
        myDeck.Append(c);
    }
    QCOMPARE(myDeck.Size(), 78U);


    // ------------------------------------------------------------------------
    // Test 2: Add a list of cards (string format)
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
    // Then add them to the deck
    myDeck.SetCards(listOfCards);
    QCOMPARE(myDeck.Size(), 78U);

    // ------------------------------------------------------------------------
    // Test 3: sorting and shuffling tests
    // Spades, Diamonds, Trumps, Clubs, Hearts

    myDeck.Shuffle(1); // Use always the same seed so that the result is always the same

    std::string expected_string = "05-H;07-D;14-H;04-T;12-S;14-T;12-T;06-H;06-D;00-T;14-D;14-S;12-C;08-C;13-D;13-C;09-S;10-S;11-T;03-S;17-T;03-D;09-C;11-H;10-D;11-C;08-D;05-S;16-T;04-C;06-C;06-T;01-H;10-T;07-H;20-T;15-T;08-S;04-D;10-C;19-T;11-S;01-C;07-C;01-S;11-D;06-S;03-H;04-H;02-S;12-H;12-D;02-T;09-D;03-T;07-S;05-T;02-H;05-D;09-H;05-C;08-H;02-C;03-C;14-C;09-T;13-T;01-D;18-T;07-T;04-S;02-D;01-T;21-T;13-S;13-H;10-H;08-T";
    std::cout << "Deck shuffled: \n";
    std::string actual_string =  myDeck.GetCardList();
    std::cout << actual_string << std::endl;
    QCOMPARE(expected_string, actual_string);

    myDeck.Sort("SDTCH");

    expected_string = "14-S;13-S;12-S;11-S;10-S;09-S;08-S;07-S;06-S;05-S;04-S;03-S;02-S;01-S;14-D;13-D;12-D;11-D;10-D;09-D;08-D;07-D;06-D;05-D;04-D;03-D;02-D;01-D;21-T;20-T;19-T;18-T;17-T;16-T;15-T;14-T;13-T;12-T;11-T;10-T;09-T;08-T;07-T;06-T;05-T;04-T;03-T;02-T;01-T;00-T;14-C;13-C;12-C;11-C;10-C;09-C;08-C;07-C;06-C;05-C;04-C;03-C;02-C;01-C;14-H;13-H;12-H;11-H;10-H;09-H;08-H;07-H;06-H;05-H;04-H;03-H;02-H;01-H";
    std::cout << "Deck ordered: \n";
    actual_string =  myDeck.GetCardList();
    std::cout << actual_string << std::endl;
    QCOMPARE(expected_string, actual_string);

    // ------------------------------------------------------------------------
    // Test 4: set non-valid cards
    Deck one, two, three;
    std::uint32_t expected_size = 0U;

    one.SetCards(";;;;;;;;;");
    QCOMPARE(one.Size(), expected_size);
    one.SetCards("00T;00T00;T00T0;0T0;000T00F0D;hjfkqskl;00;,:://^^$ù*7893728074208àç)('à(ç'é)(-_(_'çà;");
    QCOMPARE(one.Size(), expected_size);
    one.SetCards("15-D;22-T;7-C");
    QCOMPARE(one.Size(), expected_size);
    one.SetCards("08-s;14-t");
    QCOMPARE(one.Size(), expected_size);
    one.SetCards("10-D;15-T;01-D;04-C;;;18-H;38-T;0009-D;"); // mixed valid and non-valid cards
    expected_size = 4U;
    QCOMPARE(one.Size(), expected_size);

    // ------------------------------------------------------------------------
    // Test 5: Testing overloaded operators

    expected_size = 3U;
    std::string str_one = "01-T;02-T;03-T";
    one.SetCards(str_one);
    QCOMPARE(one.Size(), expected_size);

    std::string str_two = "04-T;05-T;06-T";
    two.SetCards(str_two);
    QCOMPARE(two.Size(), expected_size);

    three = one + two;
    expected_size = 6U;
    QCOMPARE(three.Size(), expected_size);

    // ------------------------------------------------------------------------
    // Test 6: Deck specific access and manipulation

    actual_string = three.Mid(3U).GetCardList(); // gets the 3 last cards
    QCOMPARE(actual_string, str_two);
    three += Deck("05-H;07-D;14-H;04-T;12-S;14-T;12-T"); // add 7 cards
    QCOMPARE(three.Size(), 6U + 7U);
    expected_string = "06-T;05-H;07-D;14-H";
    actual_string = three.Mid(5U, 4U).GetCardList(); // gets some cards in the middle
    QCOMPARE(actual_string, expected_string);

    // Test with bad offset, outside the allowed range
    expected_string = "01-T;02-T;03-T;04-T;05-T;06-T;05-H;07-D;14-H;04-T;12-S;14-T;12-T";
    actual_string = three.Mid(57U, 109U).GetCardList();
    QCOMPARE(actual_string, expected_string);

    // Remove 3 cards
    three.Remove(TarotDeck::GetCard("05-T"));
    three.Remove(TarotDeck::GetCard("05-H"));
    three.Remove(TarotDeck::GetCard("12-S"));
    QCOMPARE(three.Size(), 10U);
    three.Remove(TarotDeck::GetCard("14-C")); // non existing card
    QCOMPARE(three.Size(), 10U);

    // Count the number of cards in the deck
    QCOMPARE(three.Count(TarotDeck::GetCard("04-T")), 2U);
    QCOMPARE(three.Count(TarotDeck::GetCard("12-T")), 1U);
    QCOMPARE(three.Count(TarotDeck::GetCard("14-C")), 0U);

    // ------------------------------------------------------------------------
    // Test 7: Deck contents analysis

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


