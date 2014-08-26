
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
    Card *c = TarotDeck::GetCard(card);
    bool actual = player.CanPlayCard(c, currentTrick);
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
    Tarot::Handle attack;
    Tarot::Handle defense;
    Tarot::Bid bid;
    Score score;

    // --------------------------------------------------------------------
    // Test 1: Load the TarotMag issue 17 example deal
    bool actual_bool = deal.LoadGameDealLog(Util::ExecutablePath() + "/../../prj/testu/deals/tarotmag_17.json");
    QCOMPARE(actual_bool, true);

    // Launch score calculation
    attack = Tarot::NO_HANDLE;
    defense = Tarot::NO_HANDLE;
    bid.contract = Contract::TAKE;
    bid.taker = Place::SOUTH;
    deal.AnalyzeGame(4U);
    deal.CalculateScore(bid, attack, defense);

    score = deal.GetScore();

    QCOMPARE(score.Winner(), DEFENSE);
    QCOMPARE(score.GetAttackScore(), -102); // -34 multiple 3 players
    QCOMPARE(score.oudlers, 1U);
    QCOMPARE(score.handlePoints, 0U);
    QCOMPARE(score.slamPoints, 0U);
    QCOMPARE(score.littleEndianPoints, 0U);

    // --------------------------------------------------------------------
    // Test 2: The taker loses some oudlers
    // On a pris le petit du preneur, et le preneur joue son excuse au dernier pli.
    // Malgré cela, au calcul des points, le logiciel lui conserve les 3 bouts

    actual_bool = deal.LoadGameDealLog(Util::ExecutablePath() + "/../../prj/testu/deals/taker_loses_two_oudlers.json");
    QCOMPARE(actual_bool, true);

    // Launch score calculation
    attack = Tarot::TRIPLE_HANDLE;
    defense = Tarot::NO_HANDLE;
    bid.contract = Contract::GUARD_WITHOUT;
    bid.taker = Place::SOUTH;
    deal.AnalyzeGame(4U);
    deal.CalculateScore(bid, attack, defense);

    score = deal.GetScore();

    QCOMPARE(score.Winner(), ATTACK);
    QCOMPARE(score.pointsAttack, 67U);
    QCOMPARE(score.GetAttackScore(), 612);
    QCOMPARE(score.oudlers, 1U);
    QCOMPARE(score.handlePoints, 40U);
    QCOMPARE(score.slamPoints, 0U);
    QCOMPARE(score.littleEndianPoints, 0U);

    // --------------------------------------------------------------------
    // Test 3:  Load the TarotMag issue 18 example deal

    actual_bool = deal.LoadGameDealLog(Util::ExecutablePath() + "/../../prj/testu/deals/tarotmag_18.json");
    QCOMPARE(actual_bool, true);

    // Launch score calculation
    attack = Tarot::NO_HANDLE;
    defense = Tarot::NO_HANDLE;
    bid.contract = Contract::GUARD;
    bid.taker = Place::SOUTH;
    deal.AnalyzeGame(4U);
    deal.CalculateScore(bid, attack, defense);

    score = deal.GetScore();

    QCOMPARE(score.Winner(), ATTACK);
    QCOMPARE(score.GetAttackScore(), 54 * 3);
    QCOMPARE(score.oudlers, 2U);
    QCOMPARE(score.handlePoints, 0U);
    QCOMPARE(score.slamPoints, 0U);
    QCOMPARE(score.littleEndianPoints, 0U);


}

