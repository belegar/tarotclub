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

static const std::string DEAL_RESULT_FILE_VERSION  = "3";

/*****************************************************************************/
Deal::Deal()
{
    NewDeal();
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
    mTricksWon = 0;
    statsAttack.Reset();
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

    if (turn == 0U)
    {
        firstPlayer = mFirstPlayer; // First trick, we take the first player after the dealer
    }
    else
    {
        firstPlayer = mWinner[turn - 1U];
    }

    // Get the number of tricks we must play, it depends of the number of players
    int numberOfTricks = Tarot::NumberOfCardsInHand(numberOfPlayers);

    if (turn < 24U)
    {
        mTricks[turn] = trick;
        Card cLeader;

        // Each trick is won by the highest trump in it, or the highest card
        // of the suit led if no trumps were played.
        cLeader = trick.HighestTrump();
        if (!cLeader.IsValid())
        {
            // lead color is the first one, except if the first card is a fool. In that case, the second player plays the lead color
            cLeader = trick.HighestSuit();
        }

        if (!cLeader.IsValid())
        {
            TLogError("cLeader cannot be invalid!");
        }
        else
        {
            // The trick winner is the card leader owner
            winner = GetOwner(firstPlayer, cLeader, turn);

            if (mTricks[turn].HasFool())
            {
                Card cFool = mTricks[turn].GetCard("00-T");
                if (!cFool.IsValid())
                {
                    TLogError("Card cannot be invalid");
                }
                else
                {
                    Place foolPlace = GetOwner(firstPlayer, cFool, turn);
                    Team winnerTeam = (winner == mBid.taker) ? Team::ATTACK : Team::DEFENSE;
                    Team foolTeam = (foolPlace == mBid.taker) ? Team::ATTACK : Team::DEFENSE;

                    if (Tarot::IsDealFinished(trickCounter, numberOfPlayers))
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
                            if (winnerTeam == foolTeam)
                            {
                                foolSwap = true;
                            }
                        }
                    }
                    else
                    {
                        // In all other cases, the fool is kept by the owner. If the trick is won by a
                        // different team than the fool owner, they must exchange 1 low card with the fool.
                        if (winnerTeam != foolTeam)
                        {
                            foolSwap = true;
                        }
                    }
                }
            }
        }

        if (winner == mBid.taker)
        {
            mTricks[turn].SetOwner(Team::ATTACK);
            mTricks[turn].AnalyzeTrumps(statsAttack);
            mTricksWon++;

            if (foolSwap)
            {
                statsAttack.points -= 4; // defense keeps its points
                statsAttack.oudlers--; // attack looses an oudler! what a pity!
            }
        }
        else
        {
            mTricks[turn].SetOwner(Team::DEFENSE);
            if (foolSwap)
            {
                statsAttack.points += 4; // get back the points
                statsAttack.oudlers++; // hey, it was MY oudler!
            }
        }
    }
    else
    {
        TLogError("Index out of scope!");
    }

    // Save the current winner for the next turn
    mWinner[turn] = winner;
    return winner;
}
/*****************************************************************************/
Place Deal::GetOwner(Place firstPlayer,const Card &c, int turn)
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
/**
 * @brief Deal::GetTrick
 * @param turn
 * @return
 */
Deck Deal::GetTrick(std::uint8_t turn, std::uint8_t numberOfPlayers)
{
    if (turn >= Tarot::NumberOfCardsInHand(numberOfPlayers))
    {
        turn = 0U;
    }
    return mTricks[turn];
}
/*****************************************************************************/
Place Deal::GetWinner(std::uint8_t turn, std::uint8_t numberOfPlayers)
{
    if (turn >= Tarot::NumberOfCardsInHand(numberOfPlayers))
    {
        turn = 0U;
    }
    return mWinner[turn];
}
/*****************************************************************************/
void Deal::SetHandle(const Deck &handle, Team team)
{
    if (team == Team::ATTACK)
    {
        mAttackHandle = handle;
    }
    else
    {
        mDefenseHandle = handle;
    }
}
/*****************************************************************************/
Deck Deal::GetDog()
{
    return mDog;
}
/*****************************************************************************/
Deck Deal::GetDiscard()
{
    return mDiscard;
}
/*****************************************************************************/
void Deal::SetDiscard(const Deck &discard, Team owner)
{
    mDiscard = discard;
    mDiscard.SetOwner(owner);
}
/*****************************************************************************/
void Deal::SetDog(const Deck &dog)
{
    mDog = dog;
}
/*****************************************************************************/
void Deal::AnalyzeGame(Points &points, std::uint8_t numberOfPlayers)
{
    std::uint8_t numberOfTricks = Tarot::NumberOfCardsInHand(numberOfPlayers);
    std::uint8_t lastTrick = numberOfTricks - 1U;

    // 1. Slam detection
    if ((mTricksWon == numberOfTricks) || (mTricksWon == 0U))
    {
        points.slamDone = true;
    }
    else
    {
        points.slamDone = false;
    }

    // 2. Attacker points, we add the dog if needed
    if (mDiscard.GetOwner() == Team::ATTACK)
    {
        mDiscard.AnalyzeTrumps(statsAttack);
    }

    // 3. One of trumps in the last trick bonus detection
    if (points.slamDone)
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
        points.littleEndianOwner = mTricks[lastTrick].GetOwner();
    }
    else
    {
        points.littleEndianOwner = Team::NO_TEAM;
    }

    // 4. The number of oudler(s) decides the points to do
    points.oudlers = statsAttack.oudlers;

    // 5. We save the points done by the attacker
    points.pointsAttack = static_cast<int>(statsAttack.points); // voluntary ignore digits after the coma

    // 6. Handle bonus: Ces primes gardent la même valeur quel que soit le contrat.
    // La prime est acquise au camp vainqueur de la donne.
    points.handlePoints += Tarot::GetHandlePoints(Tarot::GetHandleType(mAttackHandle.Size()));
    points.handlePoints += Tarot::GetHandlePoints(Tarot::GetHandleType(mDefenseHandle.Size()));
}
/*****************************************************************************/
/**
 * @brief Generate a file with all played cards of the deal
 */
std::string Deal::GenerateEndDealLog(std::uint8_t numberOfPlayers)
{
    JsonObject json;
    json.AddValue("version", DEAL_RESULT_FILE_VERSION);

    // ========================== Game information ==========================
    JsonObject dealInfo;

    // Players are sorted from south to north-west, anti-clockwise (see Place class)
    dealInfo.AddValue("number_of_players", numberOfPlayers);
    dealInfo.AddValue("taker", mBid.taker.ToString());
    dealInfo.AddValue("contract", mBid.contract.ToString());
    dealInfo.AddValue("slam", mBid.slam);
    dealInfo.AddValue("first_trick_lead", mFirstPlayer.ToString());
    dealInfo.AddValue("dog", mDog.ToString());
    dealInfo.AddValue("attack_handle", mAttackHandle.ToString());
    dealInfo.AddValue("defense_handle", mDefenseHandle.ToString());
    json.AddValue("deal_info", dealInfo);

    // ========================== Played cards ==========================
    JsonArray tricks;

    for (std::uint32_t i = 0U; i < Tarot::NumberOfCardsInHand(numberOfPlayers); i++)
    {
        tricks.AddValue(mTricks[i].ToString());
    }
    json.AddValue("tricks", tricks);

    return json.ToString(0U);
}
/*****************************************************************************/
bool Deal::LoadGameDealLog(const std::string &fileName)
{
    bool ret = false;
    JsonValue json;

#ifdef TAROT_DEBUG
    std::cout << "File: " << fileName << std::endl;
#endif
    if (JsonReader::ParseFile(json, fileName))
    {
        ret = DecodeJsonDeal(json);
    }
    else
    {
        TLogError("Cannot open Json deal file");
    }
    return ret;
}
/*****************************************************************************/
bool Deal::LoadGameDeal(const std::string &buffer)
{
    bool ret = false;
    JsonValue json;

    if (JsonReader::ParseString(json, buffer))
    {
        ret = DecodeJsonDeal(json);
    }
    else
    {
        TLogError("Cannot analyze JSON buffer");
    }
    return ret;
}
/*****************************************************************************/
bool Deal::DecodeJsonDeal(const JsonValue &json)
{
    bool ret = true;

    NewDeal();

    std::uint32_t numberOfPlayers;
    JsonValue players = json.FindValue("deal_info:number_of_players");
    numberOfPlayers = static_cast<std::uint32_t>(players.GetInteger());
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

        if (json.GetValue("deal_info:dog", str_value))
        {
            mDog.SetCards(str_value);
        }
        else
        {
            ret = false;
        }

        if (json.GetValue("deal_info:attack_handle", str_value))
        {
            mAttackHandle.SetCards(str_value);
        }
        else
        {
            ret = false;
        }

        if (json.GetValue("deal_info:defense_handle", str_value))
        {
            mDefenseHandle.SetCards(str_value);
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
#ifdef DEAL_TEST
            std::cout << "First player: " << str_value << std::endl;
#endif
            StartDeal(str_value, bid);

            // Load played cards
            JsonValue tricks = json.FindValue("tricks");
            if (tricks.GetArray().Size() == Tarot::NumberOfCardsInHand(numberOfPlayers))
            {
                std::uint8_t trickCounter = 1U;
                mDiscard.CreateTarotDeck();

                for (JsonArray::Iterator iter = tricks.GetArray().Begin(); iter != tricks.GetArray().End(); ++iter)
                {
                    Deck trick(iter->GetString());

                    if (trick.Size() == numberOfPlayers)
                    {
                        Place winner = SetTrick(trick, trickCounter);
#ifdef DEAL_TEST
                        std::cout << "Trick: " << (int)trickCounter << ", Cards: " << trick.ToString() << ", Winner: " << winner.ToString() << std::endl;
#else
                        (void) winner;
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
#ifdef DEAL_TEST
                    std::cout << "Discard: " << mDiscard.ToString() << std::endl;
#endif

                    // Give the cards to the right team owner
                    if (bid.contract == Contract::GUARD_AGAINST)
                    {
                        mDiscard.SetOwner(Team::DEFENSE);
                    }
                    else
                    {
                        mDiscard.SetOwner(Team::ATTACK);
                    }
                }
                else
                {
                    std::stringstream msg;
                    msg << "Bad discard size: " << (int)mDiscard.Size() << ", contents: " << mDiscard.ToString();
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

    return ret;
}

//=============================================================================
// End of file Deal.h
//=============================================================================
