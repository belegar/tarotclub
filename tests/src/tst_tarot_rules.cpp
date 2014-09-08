
#include <QString>
#include <QtTest>
#include <QCoreApplication>
#include <cstdint>
#include <iostream>

#include "tst_tarot_rules.h"
#include "Card.h"
#include "Deck.h"
#include "TarotDeck.h"
#include "ServerConfig.h"
#include "Player.h"
#include "Deal.h"

TarotRules::TarotRules()
{
}

void TarotRules::TestCard(const std::string &card, bool expected)
{
    bool actual = player.CanPlayCard(Card(card), currentTrick);
    QCOMPARE(actual, expected);
}


void TarotRules::TestCanPlayCard()
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

void TarotRules::TestScoreCalculation()
{
    Deal deal;
    Deck handle;
    Score score;

    // --------------------------------------------------------------------
    // Test 1: Load the TarotMag issue 17 example deal

    bool actual_bool = deal.LoadGameDealLog(Util::ExecutablePath() + "/../../tests/played_deals/tarotmag_17.json");
    QCOMPARE(actual_bool, true);

    // Launch score calculation
    deal.AnalyzeGame(4U);
    deal.CalculateScore();

    score = deal.GetScore();

    QCOMPARE(score.Winner(), DEFENSE);
    QCOMPARE(score.GetAttackScore(), -102); // -34 multiple 3 players
    QCOMPARE(score.oudlers, 1);
    QCOMPARE(score.handlePoints, 0);
    QCOMPARE(score.slamPoints, 0);
    QCOMPARE(score.littleEndianPoints, 0);

    // --------------------------------------------------------------------
    // Test 2: The taker loses some oudlers
    // On a pris le petit du preneur, et le preneur joue son excuse au dernier pli.
    // Malgré cela, au calcul des points, le logiciel lui conserve les 3 bouts

    actual_bool = deal.LoadGameDealLog(Util::ExecutablePath() + "/../../tests/played_deals/taker_loses_two_oudlers.json");
    QCOMPARE(actual_bool, true);

    // Launch score calculation
    handle.SetCards("00-T;01-T;02-T;03-T;04-T;05-T;06-T;07-T;08-T;09-T;10-T;11-T;12-T;13-T;14-T"); // fake handle (the size must be correct)
    deal.SetHandle(handle, ATTACK);
    deal.AnalyzeGame(4U);
    deal.CalculateScore();

    score = deal.GetScore();

    QCOMPARE(score.Winner(), ATTACK);
    QCOMPARE(score.pointsAttack, 67);
    QCOMPARE(score.GetAttackScore(), 612);
    QCOMPARE(score.oudlers, 1);
    QCOMPARE(score.handlePoints, 40);
    QCOMPARE(score.slamPoints, 0);
    QCOMPARE(score.littleEndianPoints, 0);

    // --------------------------------------------------------------------
    // Test 3:  Load the TarotMag issue 18 example deal

    actual_bool = deal.LoadGameDealLog(Util::ExecutablePath() + "/../../tests/played_deals/tarotmag_18.json");
    QCOMPARE(actual_bool, true);

    // Launch score calculation
    deal.AnalyzeGame(4U);
    deal.CalculateScore();

    score = deal.GetScore();

    QCOMPARE(score.Winner(), ATTACK);
    QCOMPARE(score.GetAttackScore(), 27 * 3);
    QCOMPARE(score.oudlers, 2);
    QCOMPARE(score.handlePoints, 0);
    QCOMPARE(score.slamPoints, 0);
    QCOMPARE(score.littleEndianPoints, 0);


    // --------------------------------------------------------------------
    // Test 4:  Little trump at the last trick

    actual_bool = deal.LoadGameDealLog(Util::ExecutablePath() + "/../../tests/played_deals/one_trump_last_trick.json");
    QCOMPARE(actual_bool, true);

    // Launch score calculation
    deal.AnalyzeGame(4U);
    deal.CalculateScore();

    score = deal.GetScore();

    QCOMPARE(score.Winner(), DEFENSE);
    QCOMPARE(score.GetAttackScore(), -43 * 3);
    QCOMPARE(score.oudlers, 1);
    QCOMPARE(score.handlePoints, 0);
    QCOMPARE(score.slamPoints, 0);
    QCOMPARE(score.littleEndianPoints, 10);


    // FIXME: add test cases:
      // Slam done by the attack
      // Slam done by the defense and declared by the attack, and the reverse situation
      // Little endian at the last trick
      // Generate end of deal log
      // bad End of deal log format (error use cases)


}

