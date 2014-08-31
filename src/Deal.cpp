/*=============================================================================
 * TarotClub - Deal.cpp
 *=============================================================================
 * This class stores a complete deal history and calculate final points
 *=============================================================================
 * TarotClub ( http://www.tarotclub.fr ) - This file is part of TarotClub
 * Copyright (C) 2003-2999 - Anthony Rabine
 * anthony@tarotclub.fr
 *
 * TarotClub is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * TarotClub is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with TarotClub.  If not, see <http://www.gnu.org/licenses/>.
 *
 *=============================================================================
 */

#include <sstream>
#include "JsonReader.h"
#include "JsonWriter.h"
#include "Deal.h"
#include "Common.h"
#include "Log.h"
#include "System.h"

static const std::string DEAL_RESULT_FILE_VERSION  = "2.0";

/*****************************************************************************/
Deal::Deal()
{
    NewGame();
    NewDeal();
}
/*****************************************************************************/
void Deal::NewGame()
{
    for (std::uint32_t i = 0U; i < MAX_ROUNDS; i++)
    {
        for (std::uint32_t j = 0U; j < 5U; j++)
        {
            scores[i][j] = 0;
        }
    }
    dealCounter = 0U;
}
/*****************************************************************************/
void Deal::NewDeal()
{
    mDiscard.Clear();
    mAttackHandle.Clear();
    mDefenseHandle.Clear();

    for (std::uint32_t i = 0U; i < 24U; i++)
    {
        mTricks[i].Clear();
    }

    littleEndianOudler = false;
    littleEndianOwner = NO_TEAM;
    slamDone = false;
    slamOwner = NO_TEAM;
    mTricksWon = 0;
    statsAttack.Reset();
    score.Reset();
}
/*****************************************************************************/
void Deal::StartDeal(Place firstPlayer, const Tarot::Bid &bid)
{
    mFirstPlayer = firstPlayer;
    mBid = bid;
}
/*****************************************************************************/
/**
 * @brief SetTrick
 *
 * Store the current trick into memory and calculate the trick winner.
 *
 * This methods does not verify if the trick is consistent with the rules; this
 * is supposed to have been previously verified.
 *
 * @param[in] trickCounter Must begin at 1 (first trick of the deal)
 * @return The place of the winner of this trick
 */
Place Deal::SetTrick(const Deck &trick, std::uint8_t trickCounter)
{
    std::uint8_t turn = trickCounter - 1U;
    Place winner;
    std::uint8_t numberOfPlayers = trick.Size();
    Place firstPlayer;
    // Bonus: Fool
    bool foolSwap = false;  // true if the fool has been swaped of teams
    Team foolOwner = NO_TEAM; // the final owner of the fool

    if (turn == 0U)
    {
        firstPlayer = mFirstPlayer;
    }
    else
    {
        firstPlayer = mPreviousWinner;
    }

    // Get the number of tricks we must play, it depends of the number of players
    int numberOfTricks = Tarot::NumberOfCardsInHand(numberOfPlayers);

    if (turn < 24U)
    {
        mTricks[turn] = trick;
        Card *cLeader = NULL;

        // Each trick is won by the highest trump in it, or the highest card
        // of the suit led if no trumps were played.
        cLeader = trick.HighestTrump();
        if (cLeader == NULL)
        {
            // lead color is the first one, except if the first card is a fool. In that case, the second player plays the lead color
            cLeader = trick.HighestSuit();
        }

        if (cLeader == NULL)
        {
            TLogError("cLeader cannot be null!");
        }
        else
        {
            // The trick winner is the card leader owner
            winner = GetOwner(firstPlayer, cLeader, turn);

            if (mTricks[turn].HasFool())
            {
                Card *cFool = mTricks[turn].GetCardByName("00-T");
                if (cFool == NULL)
                {
                    TLogError("Card pointer cannot be NULL");
                }
                else
                {
                    Place foolPlace = GetOwner(firstPlayer, cFool, turn);
                    if (Tarot::IsDealFinished(trickCounter, numberOfPlayers) == true)
                    {
                        // Special case of the fool: if played at last turn with a slam realized, it wins the trick
                        if ((mTricksWon == (numberOfTricks - 1)) &&
                                (foolPlace == mBid.taker))
                        {
                            winner = mBid.taker;
                        }
                        // Otherwise, the fool is _always_ lost if played at the last trick, even if the
                        // fool belongs to the same team than the winner of the trick.
                        else
                        {
                            foolSwap = true;
                            if (foolPlace == mBid.taker)
                            {
                                foolOwner = DEFENSE;
                            }
                            else
                            {
                                foolOwner = ATTACK;
                            }
                        }
                    }
                    else
                    {
                        // In all other cases, the fool is kept by the owner. If the trick is won by a
                        // different team than the fool owner, they must exchange 1 low card with the fool.
                        if (winner != foolPlace)
                        {
                            foolSwap = true;
                            if (winner == mBid.taker)
                            {
                                foolOwner = DEFENSE;
                            }
                            else
                            {
                                foolOwner = ATTACK;
                            }
                        }
                        // else, the trick is won by the attacker, it keeps the fool
                    }
                }
            }
        }

        if (winner == mBid.taker)
        {
            mTricks[turn].SetOwner(ATTACK);
            mTricks[turn].AnalyzeTrumps(statsAttack);
            mTricksWon++;

            if (foolSwap == true)
            {
                if (foolOwner == DEFENSE)
                {
                    statsAttack.points -= 4; // defense keeps its points
                    statsAttack.oudlers--; // attack looses an oudler! what a pity!
                }
                else
                {
                    statsAttack.points += 4; // get back the points
                    statsAttack.oudlers++; // hey, it was MY oudler!
                }
            }
        }
        else
        {
            mTricks[turn].SetOwner(DEFENSE);
        }
    }
    else
    {
        TLogError("Index out of scope!");
    }

    // Save the current winner for the next turn
    mPreviousWinner = winner;
    return winner;
}
/*****************************************************************************/
Place Deal::GetOwner(Place firstPlayer, Card *c, int turn)
{
    Place p = firstPlayer;
    std::uint8_t numberOfPlayers = mTricks[turn].Size();

    for (Deck::Iterator it = mTricks[turn].Begin(); it != mTricks[turn].End(); ++it)
    {
        if ((*it) == c)
        {
            break;
        }
        p = p.Next(numberOfPlayers); // max before rollover is the number of players
    }
    return p;
}
/*****************************************************************************/
Deck Deal::GetTrick(int turn)
{
    if ((turn < 0) || (turn >= 24))
    {
        turn = 0;
    }
    return mTricks[turn];
}
/*****************************************************************************/
void Deal::SetHandle(const Deck &handle, Team team)
{
    if (team == ATTACK)
    {
        mAttackHandle = handle;
    }
    else
    {
        mDefenseHandle = handle;
    }
}
/*****************************************************************************/
int Deal::GetTotalPoints(Place p) const
{
    int total;

    total = 0;
    for (std::uint32_t i = 0U; i < dealCounter; i++)
    {
        total += scores[i][p.Value()];
    }
    return (total);
}
/*****************************************************************************/
/**
 * @brief Deal::GetPodium
 *
 * Gets the podium of the tournament. The first player has the highest number of points.
 *
 * @return The sorted list of players, by points
 */
std::map<int, Place> Deal::GetPodium()
{
    std::map<int, Place> podium;

    if (dealCounter > 0U)
    {
        // init podium
        for (std::uint32_t i = 0U; i < 5U; i++)
        {
            int points = scores[dealCounter - 1U][i];
            podium[points] = (Place)i;
        }
    }

    // Since the map naturally sorts the list in the key order (points)
    // there is no need to sort anything
    return podium;
}
/*****************************************************************************/
Deck Deal::GetDog()
{
    return mDiscard;
}
/*****************************************************************************/
Score &Deal::GetScore()
{
    return score;
}
/*****************************************************************************/
void Deal::SetScore(const Score &s)
{
    score = s;
}
/*****************************************************************************/
/**
 * @brief Deal::AddScore
 * @param info
 * @return true if the tournament must continue, false if it is finished
 */
bool Deal::AddScore(const Tarot::Bid &bid, std::uint8_t numberOfPlayers)
{
    for (std::uint32_t i = 0U; i < numberOfPlayers; i++)
    {
        if (Place(i) == bid.taker)
        {
            scores[dealCounter][i] = score.GetAttackScore();
        }
        else
        {
            scores[dealCounter][i] = score.GetDefenseScore();
        }
    }
    dealCounter++;
    if (dealCounter < MAX_ROUNDS)
    {
        return true;
    }
    return false;
}
/*****************************************************************************/
void Deal::AnalyzeGame(std::uint8_t numberOfPlayers)
{
    std::uint8_t numberOfTricks = Tarot::NumberOfCardsInHand(numberOfPlayers);
    std::uint8_t lastTrick = numberOfTricks - 1U;

    // 1. Slam detection
    if (mTricksWon == numberOfTricks)
    {
        slamDone = true;
        slamOwner = ATTACK;
    }
    else if (mTricksWon == 0)
    {
        slamDone = true;
        slamOwner = DEFENSE;
    }
    else
    {
        slamDone = false;
    }

    // 2. Attacker points, we add the dog if needed
    if (mDiscard.GetOwner() == ATTACK)
    {
        mDiscard.AnalyzeTrumps(statsAttack);
    }

    // 3. One of trumps in the last trick bonus detection
    if (slamDone)
    {
        // With a slam, the 1 of Trump bonus is valid if played
        // in the penultimate trick AND if the fool is played in the last trick
        if (mTricks[lastTrick].HasFool())
        {
            lastTrick--;
        }
    }
    if (mTricks[lastTrick].HasOneOfTrump())
    {
        littleEndianOudler = true;
        littleEndianOwner = mTricks[lastTrick].GetOwner();
    }

    // 4. The number of oudler(s) decides the points to do
    score.oudlers = statsAttack.oudlers;

    // 5. We save the points done by the attacker
    score.pointsAttack = static_cast<int>(statsAttack.points); // voluntary ignore digits after the coma
}
/*****************************************************************************/
/**
 * @brief Calculate the final score of all players
 *
 * We calculate the points with the following formula:
 *   points = ((25 + pt + pb) * mu) + pg + ch
 *
 * pt = difference between the card points the taker actually won and the minimum number of points he needed
 * pb = the petit au bout bonus is added or subtracted if applicable
 * mu = factor (mu) depending on the bid (1, 2, 4 or 6)
 * pg = the handle bonus (20, 30 or 40)
 * ch = the slam bonus (200 or 400)
 *
 */
void Deal::CalculateScore()
{
    // Handle bonus: Ces primes gardent la même valeur quel que soit le contrat.
    // La prime est acquise au camp vainqueur de la donne.
    score.handlePoints += Tarot::GetHandlePoints(Tarot::GetHandleType(mAttackHandle.Size()));
    score.handlePoints += Tarot::GetHandlePoints(Tarot::GetHandleType(mDefenseHandle.Size()));

    // Little endian bonus:
    // Le camp qui réalise la dernière levée, à condition que cette levée
    // comprenne le Petit, bénéficie d'une prime de 10 points,
    // multipliable selon le contrat, quel que soit le résultat de la donne.
    if (littleEndianOudler == true)
    {
        if (littleEndianOwner == ATTACK)
        {
            score.littleEndianPoints = 10;
        }
        else
        {
            score.littleEndianPoints = -10;
        }
    }

    // Slam bonus
    if (slamDone)
    {
        if (mBid.slam == true)
        {
            score.slamPoints = 400;
        }
        else
        {
            score.slamPoints = 200;
        }
    }
    else
    {
        // announced but not realized
        if (mBid.slam == true)
        {
            score.slamPoints = -200;
        }
    }

    // Final scoring
    score.scoreAttack = (25 + abs(score.Difference()) + score.littleEndianPoints) * Tarot::GetMultiplier(mBid.contract) + score.handlePoints + score.slamPoints;
}
/*****************************************************************************/
void Deal::SetDiscard(const Deck &discard, Team owner)
{
    mDiscard = discard;
    mDiscard.SetOwner(owner);
}
/*****************************************************************************/
/**
 * @brief Generate a file with all played cards of the deal
 */
void Deal::GenerateEndDealLog(const std::map<Place, Identity> &players)
{
    JsonWriter json;
    std::uint8_t numberOfPlayers = players.size();

    std::string fileName = System::GamePath() + "deal_result_" + Util::CurrentDateTime("%Y-%m-%d.%H%M%S") + ".json";

    json.CreateValuePair("version", DEAL_RESULT_FILE_VERSION);
    json.CreateValuePair("generator", "Generated by TarotClub " + TAROT_VERSION);

    // ========================== Game information ==========================
    JsonObject *obj = json.CreateObjectPair("deal_info");

    // Players are sorted from south to north-west, anti-clockwise (see Place class)
    JsonArray *obj2 = obj->CreateArrayPair("players");
    std::map<Place, Identity>::const_iterator it;
    for (it = players.begin(); it != players.end(); ++it)
    {
        obj2->CreateValue((it->second).name);
    }

    obj->CreateValuePair("taker", mBid.taker.ToString());
    obj->CreateValuePair("contract", mBid.contract.ToString());
    obj->CreateValuePair("slam", mBid.slam);
    obj->CreateValuePair("first_trick_lead", mFirstPlayer.ToString());

    // ========================== Score calculation ==========================
    JsonObject *scoreObj = obj->CreateObjectPair("score");
    scoreObj->CreateValuePair("attacker_points", static_cast<std::int32_t>(score.pointsAttack));
    scoreObj->CreateValuePair("attacker_score", static_cast<std::int32_t>(score.scoreAttack));
    scoreObj->CreateValuePair("oudlers", static_cast<std::int32_t>(score.oudlers));
    scoreObj->CreateValuePair("one_of_trump_bonus", static_cast<std::int32_t>(score.littleEndianPoints));
    scoreObj->CreateValuePair("handle_bonus", static_cast<std::int32_t>(score.handlePoints));
    scoreObj->CreateValuePair("slam_bonus", static_cast<std::int32_t>(score.slamPoints));

    // ========================== Played cards ==========================
    JsonArray *obj3 = json.CreateArrayPair("tricks");

    for (std::uint32_t i = 0U; i < Tarot::NumberOfCardsInHand(numberOfPlayers); i++)
    {
        obj3->CreateValue(mTricks[i].GetCardList());
    }

    if (!json.SaveToFile(fileName))
    {
        TLogError("Saving deal game result failed.");
    }
}
/*****************************************************************************/
bool Deal::LoadGameDealLog(const std::string &fileName)
{
    bool ret = true;
    JsonReader json;

    NewGame();
    NewDeal();

    if (json.Open(fileName))
    {
        std::uint32_t numberOfPlayers;
        std::vector<JsonValue> players = json.GetArray("deal_info:players", JsonValue::STRING);
        numberOfPlayers = players.size();
        if ((numberOfPlayers == 3U) ||
                (numberOfPlayers == 4U) ||
                (numberOfPlayers == 5U))
        {
            Tarot::Bid bid;

            std::string str_value;
            if (json.GetValue("deal_info:taker", str_value))
            {
                bid.taker = str_value;
            }
            else
            {
                ret = false;
            }
            if (json.GetValue("deal_info:contract", str_value))
            {
                bid.contract = str_value;
            }
            else
            {
                ret = false;
            }
            bool slam;
            if (json.GetValue("deal_info:slam", slam))
            {
                bid.slam = slam;
            }
            else
            {
                ret = false;
            }
            if (!json.GetValue("deal_info:first_trick_lead", str_value))
            {
                ret = false;
            }

            if (ret)
            {
#ifdef TAROT_DEBUG
                std::cout << "First player: " << str_value << std::endl;
#endif
                StartDeal(str_value, bid);

                // Load played cards
                std::vector<JsonValue> tricks = json.GetArray("tricks", JsonValue::STRING);
                if (tricks.size() == Tarot::NumberOfCardsInHand(numberOfPlayers))
                {
                    std::uint8_t trickCounter = 1U;
                    mDiscard.CreateTarotDeck();

                    for (std::uint32_t i = 0U; i < tricks.size(); i++)
                    {
                        Deck trick(tricks[i].GetString());

                        if (trick.Size() == numberOfPlayers)
                        {
                            Place winner = SetTrick(trick, trickCounter);
    #ifdef TAROT_DEBUG
                            std::cout << "Trick: " << (int)trickCounter << ", Cards: " << trick.GetCardList() << ", Winner: " << winner.ToString() << std::endl;
    #endif
                            // Remove played cards from this deck
                            if (mDiscard.RemoveDuplicates(trick) != numberOfPlayers)
                            {
                                std::stringstream msg;
                                msg << "Bad deal contents, trick: " << (int)trickCounter;
                                TLogError(msg.str());
                                ret = false;
                            }
                            trickCounter++;
                        }
                        else
                        {
                            std::stringstream msg;
                            msg << "Bad deal contents at trick: " << (int)trickCounter;
                            TLogError(msg.str());
                            ret = false;
                        }
                    }

                    // Now that we have removed all the played cards from the mDiscard deck,
                    // it should contains only the discard cards
                    if (mDiscard.Size() == Tarot::NumberOfDogCards(numberOfPlayers))
                    {
                        // Give the cards to the right team owner
                        if (bid.contract == Contract::GUARD_AGAINST)
                        {
                            mDiscard.SetOwner(DEFENSE);
                        }
                        else
                        {
                            mDiscard.SetOwner(ATTACK);
                        }
                    }
                    else
                    {
                        std::stringstream msg;
                        msg << "Bad discard size: " << (int)mDiscard.Size() << ", contents: " << mDiscard.GetCardList();
                        TLogError(msg.str());
                        ret = false;
                    }
                }
                else
                {
                    TLogError("Bad deal contents");
                    ret = false;
                }
            }
        }
        else
        {
            TLogError("Bad number of players in the array");
            ret = false;
        }
    }
    else
    {
        TLogError("Cannot open deal JSON file");
        ret = false;
    }
    return ret;
}

//=============================================================================
// End of file Deal.h
//=============================================================================
