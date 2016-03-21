
#include <QString>
#include <QtTest>
#include <QCoreApplication>
#include <cstdint>
#include <iostream>

#include "tst_tarot_rules.h"
#include "Card.h"
#include "Deck.h"
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


void TarotRules::TestAutoDiscard()
{
    player.SetCards("09-S;15-T;06-D;17-T;20-T;18-T;04-H;14-T;04-D;08-T;07-D;14-C;12-D;14-S;02-S;03-D;10-H;07-S");
    Deck dog("11-C;05-T;09-T;02-D;05-D;11-D");
    Deck discard = player.AutoDiscard(dog, 4U);

    std::cout << discard.ToString() << std::endl;
    QCOMPARE(player.TestDiscard(discard, dog, 4U), true);

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

    // ===================================
    // Test 6: The player has only the excuse and trick suit is trump ;
    // He can play any card.
    // ===================================
    player.SetCards("12-C;12-H;10-H;02-H;03-H;13-C;05-H;00-T;11-C;07-D;09-C"); // only one excuse

    currentTrick.SetCards("19-T;12-T;10-T");

    // Any card is correct
    TestCard("00-T", true);
    TestCard("12-C", true);
    TestCard("07-D", true);
    TestCard("02-H", true);
}

void TarotRules::TestScoreCalculation()
{
    Deal deal;
    Points points;

    // --------------------------------------------------------------------
    // Test 1: Load the TarotMag issue 17 example deal

    bool actual_bool = deal.LoadGameDealLog(Util::ExecutablePath() + "/../../tests/played_deals/tarotmag_17.json");
    QCOMPARE(actual_bool, true);

    // Launch score calculation
    points.Clear();
    deal.AnalyzeGame(points, 4U);

    QCOMPARE(points.Winner().Value(), Team::DEFENSE);
    QCOMPARE(points.GetPoints(Team::ATTACK, deal.GetBid()), -102); // -34 multiple 3 players
    QCOMPARE(points.oudlers, 1);
    QCOMPARE(points.handlePoints, 0);
    QCOMPARE(points.slamDone, false);
    QCOMPARE(points.GetLittleEndianPoints(), 0);

    // --------------------------------------------------------------------
    // Test 2: The taker loses some oudlers
    // On a pris le petit du preneur, et le preneur joue son excuse au dernier pli.
    // Malgré cela, au calcul des points, le logiciel lui conserve les 3 bouts

    actual_bool = deal.LoadGameDealLog(Util::ExecutablePath() + "/../../tests/played_deals/taker_loses_two_oudlers.json");
    QCOMPARE(actual_bool, true);

    // Launch score calculation
    points.Clear();
    deal.AnalyzeGame(points, 4U);

    QCOMPARE(points.Winner().Value(), Team::ATTACK);
    QCOMPARE(points.oudlers, 1);
    QCOMPARE(points.slamDone, false);
    QCOMPARE(points.GetLittleEndianPoints(), 0);

    // --------------------------------------------------------------------
    // Test 3:  Load the TarotMag issue 18 example deal

    actual_bool = deal.LoadGameDealLog(Util::ExecutablePath() + "/../../tests/played_deals/tarotmag_18.json");
    QCOMPARE(actual_bool, true);

    // Launch score calculation
    points.Clear();
    deal.AnalyzeGame(points, 4U);

    QCOMPARE(points.Winner().Value(), Team::ATTACK);
    QCOMPARE(points.GetPoints(Team::ATTACK, deal.GetBid()), 54 * 3);
    QCOMPARE(points.oudlers, 2);
    QCOMPARE(points.handlePoints, 0);
    QCOMPARE(points.slamDone, false);
    QCOMPARE(points.GetLittleEndianPoints(), 0);

    // --------------------------------------------------------------------
    // Test 4: One of trump played at last trick by the defense. North is the taker and loose the game.

    actual_bool = deal.LoadGameDealLog(Util::ExecutablePath() + "/../../tests/played_deals/01-T_lasttrickbydefense_game_lost.json");
    QCOMPARE(actual_bool, true);

    // Launch score calculation
    points.Clear();
    deal.AnalyzeGame(points, 4U);

    QCOMPARE(points.Winner().Value(), Team::DEFENSE);
    QCOMPARE(points.GetPoints(Team::ATTACK, deal.GetBid()), -58 * 3);
    QCOMPARE(points.oudlers, 1);
    QCOMPARE(points.handlePoints, 0);
    QCOMPARE(points.slamDone, false);
    QCOMPARE(points.GetLittleEndianPoints(), 10); // won by the defense


    // --------------------------------------------------------------------
    // Test 4b: One of trump played at last trick by the defense. North is the taker and win the game.

    /* FIXME: deal to be generated
    actual_bool = deal.LoadGameDealLog(Util::ExecutablePath() + "/../../tests/played_deals/01-T_lasttrickbydefense_game_won.json");
    QCOMPARE(actual_bool, true);

    // Launch score calculation
    points.Clear();
    deal.AnalyzeGame(points, 4U);

    QCOMPARE(points.Winner().Value(), Team::DEFENSE);
    QCOMPARE(points.GetPoints(Team::ATTACK, deal.GetBid()), -58 * 3);
    QCOMPARE(points.oudlers, 1);
    QCOMPARE(points.handlePoints, 0);
    QCOMPARE(points.slamDone, false);
    QCOMPARE(points.GetLittleEndianPoints(), -10); // won by the defense
*/

    // --------------------------------------------------------------------
    // Test 5:  3 oudlers at the end of the game

    actual_bool = deal.LoadGameDealLog(Util::ExecutablePath() + "/../../tests/played_deals/3_oudlers.json");
    QCOMPARE(actual_bool, true);

    // Launch score calculation
    points.Clear();
    deal.AnalyzeGame(points, 4U);

    QCOMPARE(points.Winner().Value(), Team::ATTACK);
    QCOMPARE(points.GetPoints(Team::ATTACK, deal.GetBid()), 216);
    QCOMPARE(points.oudlers, 3);
    QCOMPARE(points.handlePoints, 0);
    QCOMPARE(points.slamDone, false);
    QCOMPARE(points.GetLittleEndianPoints(), 0);

    // --------------------------------------------------------------------
    // Test 6:  4 oudlers (bug) at the end of the game

    actual_bool = deal.LoadGameDealLog(Util::ExecutablePath() + "/../../tests/played_deals/4_oudlers_bug.json");
    QCOMPARE(actual_bool, true);

    // Launch score calculation
    points.Clear();
    deal.AnalyzeGame(points, 4U);

    QCOMPARE(points.Winner().Value(), Team::ATTACK);
    QCOMPARE(points.oudlers, 3);

    // --------------------------------------------------------------------
    // Test 7: Maximum points possible (guard without + slam + triple handle + little oudler last trick

    actual_bool = deal.LoadGameDealLog(Util::ExecutablePath() + "/../../tests/played_deals/maximum_points.json");
    QCOMPARE(actual_bool, true);

    // Launch score calculation
    points.Clear();
    deal.AnalyzeGame(points, 4U);

    QCOMPARE(points.Winner().Value(), Team::ATTACK);
    QCOMPARE(points.GetPoints(Team::ATTACK, deal.GetBid()), 2886);
    QCOMPARE(points.oudlers, 3);
    QCOMPARE(points.handlePoints, 40);
    QCOMPARE(points.slamDone, true);
    QCOMPARE(points.GetLittleEndianPoints(), 10); // won by the attack last trick

    // --------------------------------------------------------------------
    // Test 8: Slam announced but not realized, with a little oudler played at the last trick
    // 3 oudlers
    // 81 points realized by the attack

    actual_bool = deal.LoadGameDealLog(Util::ExecutablePath() + "/../../tests/played_deals/slam_announced_failure.json");
    QCOMPARE(actual_bool, true);

    // Launch score calculation
    points.Clear();
    deal.AnalyzeGame(points, 4U);

    QCOMPARE(points.Winner().Value(), Team::ATTACK);
    QCOMPARE(points.GetPoints(Team::ATTACK, deal.GetBid()), 360);
    QCOMPARE(points.oudlers, 3);
    QCOMPARE(points.handlePoints, 0);
    QCOMPARE(points.slamDone, false);
    QCOMPARE(points.GetLittleEndianPoints(), 10); // won by the attack last trick

    // --------------------------------------------------------------------
    // Test 9: Slam not announced but realized by the defense
    // 0 oudlers
    // 81 points realized by the attack

    // FIXME : deal to be generated
/*
    actual_bool = deal.LoadGameDealLog(Util::ExecutablePath() + "/../../tests/played_deals/slam_not_announed_by_defense.json");
    QCOMPARE(actual_bool, true);

    // Launch score calculation
    points.Clear();
    deal.AnalyzeGame(points, 4U);

    QCOMPARE(points.Winner().Value(), Team::DEFENSE);
    QCOMPARE(points.GetPoints(Team::ATTACK, deal.GetBid()), -686 * 3);
    QCOMPARE(points.oudlers, 0);
    QCOMPARE(points.handlePoints, 0);
    QCOMPARE(points.slamDone, true);
    QCOMPARE(points.GetLittleEndianPoints(), 0); // won by the attack last trick
*/
}

