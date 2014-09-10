
#include <QString>
#include <QtTest>
#include <QCoreApplication>
#include <cstdint>
#include <iostream>

#include "tst_tarot_base.h"
#include "Card.h"
#include "Deck.h"
#include "ServerConfig.h"
#include "Player.h"
#include "System.h"

TarotBase::TarotBase()
{
}

void TarotBase::TestCardClass()
{
    float total = 0.0f;
    Deck myDeck;
    myDeck.CreateTarotDeck();

    std::cout << "Card names: \n";
    for (Deck::Iterator it = myDeck.Begin(); it != myDeck.End(); ++it)
    {
        Card c = (*it);

        std::cout << c.GetName() << " ";
        if (c.GetValue() == 14U && c.GetSuit() != Card::TRUMPS)
        {
            std::cout << "\n";
        }

        total += c.GetPoints();
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
    myDeck.CreateTarotDeck();
    QCOMPARE(myDeck.Size(), 78U);


    // ------------------------------------------------------------------------
    // Test 2: Deck owner

    // Default is no team
    QCOMPARE(myDeck.GetOwner(), NO_TEAM);
    myDeck.SetOwner(DEFENSE);
    QCOMPARE(myDeck.GetOwner(), DEFENSE);

    // ------------------------------------------------------------------------
    // Test 3: sorting and shuffling tests
    // Spades, Diamonds, Trumps, Clubs, Hearts

    myDeck.Shuffle(1); // Use always the same seed so that the result is always the same

    std::string expected_string = "05-H;07-D;14-H;04-T;12-S;14-T;12-T;06-H;06-D;00-T;14-D;14-S;12-C;08-C;13-D;13-C;09-S;10-S;11-T;03-S;17-T;03-D;09-C;11-H;10-D;11-C;08-D;05-S;16-T;04-C;06-C;06-T;01-H;10-T;07-H;20-T;15-T;08-S;04-D;10-C;19-T;11-S;01-C;07-C;01-S;11-D;06-S;03-H;04-H;02-S;12-H;12-D;02-T;09-D;03-T;07-S;05-T;02-H;05-D;09-H;05-C;08-H;02-C;03-C;14-C;09-T;13-T;01-D;18-T;07-T;04-S;02-D;01-T;21-T;13-S;13-H;10-H;08-T";
    std::cout << "Deck shuffled: \n";
    std::string actual_string =  myDeck.ToString();
    std::cout << actual_string << std::endl;
    QCOMPARE(expected_string, actual_string);

    myDeck.Sort("SDTCH");

    expected_string = "14-S;13-S;12-S;11-S;10-S;09-S;08-S;07-S;06-S;05-S;04-S;03-S;02-S;01-S;14-D;13-D;12-D;11-D;10-D;09-D;08-D;07-D;06-D;05-D;04-D;03-D;02-D;01-D;21-T;20-T;19-T;18-T;17-T;16-T;15-T;14-T;13-T;12-T;11-T;10-T;09-T;08-T;07-T;06-T;05-T;04-T;03-T;02-T;01-T;00-T;14-C;13-C;12-C;11-C;10-C;09-C;08-C;07-C;06-C;05-C;04-C;03-C;02-C;01-C;14-H;13-H;12-H;11-H;10-H;09-H;08-H;07-H;06-H;05-H;04-H;03-H;02-H;01-H";
    std::cout << "Deck ordered: \n";
    actual_string =  myDeck.ToString();
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

    actual_string = three.Mid(3U).ToString(); // gets the 3 last cards
    QCOMPARE(actual_string, str_two);
    three += Deck("05-H;07-D;14-H;04-T;12-S;14-T;12-T"); // add 7 cards
    QCOMPARE(three.Size(), 6U + 7U);
    expected_string = "06-T;05-H;07-D;14-H";
    actual_string = three.Mid(5U, 4U).ToString(); // gets some cards in the middle
    QCOMPARE(actual_string, expected_string);

    // Test with bad offset, outside the allowed range
    expected_string = "01-T;02-T;03-T;04-T;05-T;06-T;05-H;07-D;14-H;04-T;12-S;14-T;12-T";
    actual_string = three.Mid(57U, 109U).ToString();
    QCOMPARE(actual_string, expected_string);

    // Remove 3 cards
    three.Remove(Card("05-T"));
    three.Remove(Card("05-H"));
    three.Remove(Card("12-S"));
    QCOMPARE(three.Size(), 10U);
    three.Remove(Card("14-C")); // non existing card
    QCOMPARE(three.Size(), 10U);

    // Count the number of cards in the deck
    QCOMPARE(three.Count(Card("04-T")), 2U);
    QCOMPARE(three.Count(Card("12-T")), 1U);
    QCOMPARE(three.Count(Card("14-C")), 0U);

    Card c = three.GetCard("12-T");
    if (!c.IsValid())
    {
        QFAIL("Card must exist");
    }
    QCOMPARE(three.HasCard(c), true);

    c = three.GetCard("14-C");
    if (c.IsValid())
    {
        QFAIL("Card must not exist");
    }

    QCOMPARE(three.HasCard(Card("12-H")), false);

    // ------------------------------------------------------------------------
    // Test 7: Deck contents analysis

    // Has specific cards?
    one.SetCards("00-T;01-T");
    QCOMPARE(one.HasFool(), true);
    QCOMPARE(one.HasOneOfTrump(), true);

    one.SetCards("02-T;21-T");
    QCOMPARE(one.HasFool(), false);
    QCOMPARE(one.HasOneOfTrump(), false);

    one.SetCards("00-T;04-T;12-S;14-H;12-T;02-C;20-T");
    c = one.HighestTrump();
    if (c.IsValid())
    {
        expected_string = "20-T";
        QCOMPARE(c.GetName(), expected_string);
    }
    else
    {
        QFAIL("Cannot be invalid");
    }

    one.SetCards("12-S;14-H;02-C;10-D");
    c = one.HighestTrump();
    if (c.IsValid())
    {
        QFAIL("Must be invalid (no trump)");
    }

    one.SetCards("12-S;14-H;00-T;02-C;10-D");
    c = one.HighestTrump();
    if (c.IsValid())
    {
        QFAIL("Must be invalid (no trump other than the fool)");
    }

    // Highest suit, used to detect the winner of a trick
    // The suit used to detect it is the first one played
    one.SetCards("01-T;02-T;03-T;04-T;05-T");
    c = one.HighestSuit();
    if (c.IsValid())
    {
        QFAIL("Must be invalid (no any suits)");
    }

    // Highest suit, used to detect the winner of a trick
    // The suit used to detect it is the first one played after any trump
    one.SetCards("00-T;01-T;03-D;04-D;05-C;12-C;02-T;01-D");
    c = one.HighestSuit();
    if (c.IsValid())
    {
        expected_string = "04-D";
        QCOMPARE(c.GetName(), expected_string);
    }
    else
    {
        QFAIL("Cannot be invalid (there is a result!)");
    }

    // ------------------------------------------------------------------------
    // Test 8: Deck statistics

    Deck::Statistics stats;

    // Full Tarot deck
    expected_string = "05-H;07-D;14-H;04-T;12-S;14-T;12-T;06-H;06-D;00-T;14-D;14-S;12-C;08-C;13-D;13-C;09-S;10-S;11-T;03-S;17-T;03-D;09-C;11-H;10-D;11-C;08-D;05-S;16-T;04-C;06-C;06-T;01-H;10-T;07-H;20-T;15-T;08-S;04-D;10-C;19-T;11-S;01-C;07-C;01-S;11-D;06-S;03-H;04-H;02-S;12-H;12-D;02-T;09-D;03-T;07-S;05-T;02-H;05-D;09-H;05-C;08-H;02-C;03-C;14-C;09-T;13-T;01-D;18-T;07-T;04-S;02-D;01-T;21-T;13-S;13-H;10-H;08-T";
    one.SetCards(expected_string);

    stats.Reset();
    one.AnalyzeSuits(stats);
    one.AnalyzeTrumps(stats);

    QCOMPARE(stats.nbCards, (std::uint8_t)78U);
    QCOMPARE(stats.trumps, (std::uint8_t)22U);
    QCOMPARE(stats.oudlers, (std::uint8_t)3U);
    QCOMPARE(stats.majorTrumps, (std::uint8_t)7U);
    QCOMPARE(stats.kings, (std::uint8_t)4U);
    QCOMPARE(stats.queens, (std::uint8_t)4U);
    QCOMPARE(stats.knights, (std::uint8_t)4U);
    QCOMPARE(stats.jacks, (std::uint8_t)4U);
    QCOMPARE(stats.weddings, (std::uint8_t)4U);
    QCOMPARE(stats.longSuits, (std::uint8_t)4U);
    QCOMPARE(stats.cuts, (std::uint8_t)0U);
    QCOMPARE(stats.singletons, (std::uint8_t)0U);
    QCOMPARE(stats.sequences, (std::uint8_t)4U);
    QCOMPARE(stats.suits[0], (std::uint8_t)14U);
    QCOMPARE(stats.suits[1], (std::uint8_t)14U);
    QCOMPARE(stats.suits[2], (std::uint8_t)14U);
    QCOMPARE(stats.suits[3], (std::uint8_t)14U);
    QCOMPARE(stats.littleTrump, true);
    QCOMPARE(stats.bigTrump, true);
    QCOMPARE(stats.fool, true);
    QCOMPARE(stats.points, 91.0F);

    // Specific test of the sequence detection: we can have two sequences per suit!!
    // Test singleton and cut

    // Cut: no spades
    // singleton: one heart
    // sequence: two sequences in diamond
    // Insert various cards (trumps, clubs), to make the detection more difficult
    expected_string = "07-D;07-C;05-H;01-D;11-D;01-T;08-D;11-T;09-D;14-C;12-C;02-D;03-D;04-D;05-D;10-C;10-D";
    one.SetCards(expected_string);
    stats.Reset();
    one.AnalyzeSuits(stats);
    one.AnalyzeTrumps(stats);

    QCOMPARE(stats.nbCards, (std::uint8_t)17U);
    QCOMPARE(stats.trumps, (std::uint8_t)2U);
    QCOMPARE(stats.oudlers, (std::uint8_t)1U);
    QCOMPARE(stats.majorTrumps, (std::uint8_t)0U);
    QCOMPARE(stats.kings, (std::uint8_t)1U);
    QCOMPARE(stats.queens, (std::uint8_t)0U);
    QCOMPARE(stats.knights, (std::uint8_t)1U);
    QCOMPARE(stats.jacks, (std::uint8_t)1U);
    QCOMPARE(stats.weddings, (std::uint8_t)0U);
    QCOMPARE(stats.longSuits, (std::uint8_t)1U);
    QCOMPARE(stats.cuts, (std::uint8_t)1U);
    QCOMPARE(stats.singletons, (std::uint8_t)1U);
    QCOMPARE(stats.sequences, (std::uint8_t)2U);
    QCOMPARE(stats.suits[Card::SPADES], (std::uint8_t)0U);
    QCOMPARE(stats.suits[Card::HEARTS], (std::uint8_t)1U);
    QCOMPARE(stats.suits[Card::DIAMONDS], (std::uint8_t)10U);
    QCOMPARE(stats.suits[Card::CLUBS], (std::uint8_t)4U);
    QCOMPARE(stats.littleTrump, true);
    QCOMPARE(stats.bigTrump, false);
    QCOMPARE(stats.fool, false);
    QCOMPARE(stats.points, 19.5F);

}

void TarotBase::TestConfig()
{
    ServerOptions options;
    ServerConfig conf;

    options = ServerConfig::GetDefault();
    conf.SetOptions(options);
    conf.Save(System::HomePath() + DEFAULT_SERVER_CONFIG_FILE);
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


