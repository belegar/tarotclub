/*=============================================================================
 * TarotClub - Engine.cpp
 *=============================================================================
 * Main Tarot engine
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

#include <chrono>
#include <random>
#include <sstream>
#include <iostream>

#include "Engine.h"
#include "DealGenerator.h"
#include "Identity.h"
#include "Util.h"
#include "Log.h"
#include "System.h"
#include "JsonReader.h"

static const std::string DEAL_RESULT_FILE_VERSION  = "3";

/*****************************************************************************/
Engine::Engine()
    : mNbPlayers(4U)
    , mSequence(STOPPED)
    , mPosition(0U)
    , mTrickCounter(0U)
{
    std::chrono::system_clock::rep seed = std::chrono::system_clock::now().time_since_epoch().count(); // rep is long long
    mSeed = static_cast<std::uint32_t>(seed);

    NewDeal();

    for (std::uint8_t i = 0U; i < 5U; i++)
    {
        mHandleAsked[i] = false;
    }
}
/*****************************************************************************/
Engine::~Engine()
{

}
/*****************************************************************************/
/**
 * @brief Engine::Initialize
 * Call this method before clients connections
 */
void Engine::Initialize()
{
    mSequence = STOPPED;
}
/*****************************************************************************/
void Engine::CreateTable(std::uint8_t nbPlayers)
{
    // Save parameters
    mNbPlayers = nbPlayers;

    // 1. Initialize internal states
    mBid.Initialize();

    // Choose the dealer
    mDealer = DealGenerator::RandomPlace(mNbPlayers);

    // Wait for ready
    mSequence = WAIT_FOR_PLAYERS;
}
/*****************************************************************************/
void Engine::NewGame()
{
    mSequence = WAIT_FOR_READY;
}
/*****************************************************************************/
Tarot::Distribution Engine::NewDeal(const Tarot::Distribution &shuffle)
{
    Tarot::Distribution shReturned = shuffle;

    // 1. Initialize internal states
    NewDeal();
    mBid.Initialize();
    mPosition = 0U;
    currentTrick.Clear();

    // 2. Choose the dealer and the first player to start the bid
    mDealer = mDealer.Next(mNbPlayers);
    mCurrentPlayer = mDealer.Next(mNbPlayers); // The first player on the dealer's right begins the bid

    // 3. Give cards to all players
    CreateDeal(shReturned);

    // 4. Prepare the wait for ack
    mSequence = WAIT_FOR_CARDS;

    return shReturned;
}
/*****************************************************************************/
/**
 * @brief Engine::StartDeal
 * @return The first player to play
 */
Place Engine::StartDeal()
{
    mTrickCounter = 0U;
    mPosition = 0U;

    for (std::uint8_t i = 0U; i < 5U; i++)
    {
        mHandleAsked[i] = false;
    }

    // In case of slam, the first player to play is the taker.
    // Otherwise, it is the player on the right of the dealer
    if (mBid.slam == true)
    {
        mCurrentPlayer = mBid.taker;
    }
    else
    {
        mCurrentPlayer = mDealer.Next(mNbPlayers); // The first player on the dealer's right
    }

    std::stringstream ss;
    ss << "Taker: " << mBid.taker.ToString() << " / ";
    ss << "Contract: " << mBid.contract.ToString();
    TLogInfo(ss.str());

    mFirstPlayer = mCurrentPlayer;
    return mCurrentPlayer;
}
/*****************************************************************************/
bool Engine::SetDiscard(const Deck &discard)
{
    bool valid = mPlayers[mBid.taker.Value()].TestDiscard(discard, mDog, mNbPlayers);

    if (valid)
    {
        // Add the dog to the player's deck, and then filter the discard
        mPlayers[mBid.taker.Value()] += mDog;
        mPlayers[mBid.taker.Value()].RemoveDuplicates(discard);

        std::stringstream ss;
        ss << "Received discard: " << discard.ToString() << " / ";
        ss << "Taker's deck after the discard: " << mPlayers[mBid.taker.Value()].ToString();
        TLogInfo(ss.str());

        mDiscard = discard;
        mDiscard.SetOwner(Team(Team::ATTACK));
        mSequence = WAIT_FOR_START_DEAL;
    }
    return valid;
}
/*****************************************************************************/
/**
 * @brief Engine::SetHandle
 * @param handle
 * @param p
 * @return  true if the handle is valid, otherwise false
 */
bool Engine::SetHandle(const Deck &handle, Place p)
{
    bool valid = mPlayers[p.Value()].TestHandle(handle);

    if (valid)
    {
        Team team;

        if (p == mBid.taker)
        {
            team = Team::ATTACK;
        }
        else
        {
            team = Team::DEFENSE;
        }

        mSequence = WAIT_FOR_SHOW_HANDLE;
        SetHandle(handle, team);
    }
    return valid;
}
/*****************************************************************************/
bool Engine::SetCard(const Card &c, Place p)
{
    bool ret = false;

    if (mPlayers[p.Value()].CanPlayCard(c, currentTrick))
    {
        currentTrick.Append(c);
        mPlayers[p.Value()].Remove(c);

        std::stringstream ss;
        ss << "Player " << p.ToString() << " played " << c.ToString() << " Engine player deck is: " << mPlayers[p.Value()].ToString();
        TLogInfo(ss.str());

        // ------- PREPARE NEXT ONE
        mPosition++; // done for this player
        mCurrentPlayer = mCurrentPlayer.Next(mNbPlayers); // next player!
        mSequence = WAIT_FOR_SHOW_CARD;
        ret = true;
    }
    else
    {
        std::stringstream ss;
        ss << "The player " << p.ToString() << " cannot play the card: " << c.ToString()
           << " on turn " << (int)mTrickCounter + 1 << " Engine deck is: " << mPlayers[p.Value()].ToString();

        TLogError(ss.str());
    }
    return ret;
}
/*****************************************************************************/
Contract Engine::SetBid(Contract c, bool slam, Place p)
{
    if (c > mBid.contract)
    {
        mBid.contract = c;
        mBid.taker = p;
        mBid.slam = slam;
    }
    else
    {
        c = Contract::PASS;
    }

    // ------- PREPARE NEXT ONE
    mPosition++; // done for this player
    mCurrentPlayer = mCurrentPlayer.Next(mNbPlayers); // next player!
    mSequence = WAIT_FOR_SHOW_BID;
    return c;
}
/*****************************************************************************/
bool Engine::SetKingCalled(const Card &c)
{
    bool success = false;

    // On l'appelle King mais en fait le preneur peut appeler une dame (ou cavalier, etc) s'il
    // possède 4 rois ou 4 dames. On va vérifier cela.
    // On analyse toujours le jeu de l'attaquant avant de continuer
    mPlayers[mBid.taker.Value()].AnalyzeSuits(statsAttack);

    bool cardIsValid = true;
    if (c.GetValue() != Card::KING)
    {
        if (statsAttack.kings == 4)
        {
            if (c.GetValue() != Card::QUEEN)
            {
                if (statsAttack.queens == 4)
                {
                    if (c.GetValue() != Card::KNIGHT)
                    {
                        if (statsAttack.knights == 4)
                        {
                            if (c.GetValue() != Card::JACK)
                            {
                                if (statsAttack.jacks == 4)
                                {
                                    cardIsValid = true;
                                }
                                else
                                {
                                    cardIsValid = false; // nah, il n'a pas 4 valets
                                }
                            }
                        }
                        else
                        {
                            cardIsValid = false; // nah, il n'a pas 4 cavaliers
                        }
                    }
                }
                else
                {
                    cardIsValid = false; // nah, il n'a pas 4 dames
                }
            }
        }
        else
        {
            cardIsValid = false; // nah, il n'a pas 4 rois
        }
    }


    if (cardIsValid)
    {
        // Appel au roi dans le chien ou dans le deck du preneur ?
        if (mDog.HasCard(c) || mPlayers[mBid.taker.Value()].HasCard(c))
        {
            // Il est tout seul car il a appelé une carte à lui ou au chien
            mBid.partner = mBid.taker;
            success = true;
        }
        else
        {
            // On recherche son partenaire
            Place partner = mBid.taker.Next(5);
            for (uint32_t i = 0; i < 4; i++)
            {
                if (mPlayers[partner.Value()].HasCard(c))
                {
                    mBid.partner = partner;
                    success = true;
                    break;
                }
                partner = partner.Next(5);
            }
        }

        mKingCalled = c; // sauvegarde du roi appelé
    }

    if (success)
    {
        mSequence = Engine::WAIT_FOR_SHOW_KING_CALL;
    }

    return success;
}
/*****************************************************************************/
void Engine::Stop()
{
    mSequence = STOPPED;
}
/*****************************************************************************/
Deck Engine::GetDeck(Place p)
{
    Deck deck;

    if (p < Place(Place::NOWHERE))
    {
        deck = mPlayers[p.Value()];
    }

    return deck;
}
/*****************************************************************************/
Points Engine::GetCurrentGamePoints()
{
    return mCurrentPoints;
}
/*****************************************************************************/
/**
 * @brief Engine::GameSequence
 * @return true if the trick is finished
 */
void Engine::GameSequence()
{
    // If end of trick, prepare next one
    if (IsEndOfTrick())
    {
        TLogInfo("----------------------------------------------------\n");

        // The current trick winner will begin the next trick
        mCurrentPlayer = SetTrick(currentTrick, mTrickCounter);
        currentTrick.Clear();
        mSequence = WAIT_FOR_END_OF_TRICK;
    }
    // Special case of first round: players can declare a handle
    else if ((mTrickCounter == 0U) &&
             (!mHandleAsked[mCurrentPlayer.Value()]))
    {
        mHandleAsked[mCurrentPlayer.Value()] = true;
        mSequence = WAIT_FOR_HANDLE;
    }
    else
    {
        std::stringstream message;

        message << "Turn: " << (std::uint32_t)mTrickCounter << " player: " << mCurrentPlayer.ToString();
        TLogInfo(message.str());

        mSequence = WAIT_FOR_PLAYED_CARD;
    }
}
/*****************************************************************************/
void Engine::EndOfDeal(JsonObject &json)
{
    mCurrentPoints.Clear();
    AnalyzeGame(mCurrentPoints, mNbPlayers);
    GenerateEndDealLog(mNbPlayers, json);

    mSequence = WAIT_FOR_END_OF_DEAL;
}
/*****************************************************************************/
void Engine::SetAfterBidSequence()
{
    if ((mBid.contract == Contract::GUARD_WITHOUT) || (mBid.contract == Contract::GUARD_AGAINST))
    {
        // No discard is made, set the owner of the dog
        if (mBid.contract != Contract(Contract::GUARD_AGAINST))
        {
            mDiscard = mDog;
            mDiscard.SetOwner(Team(Team::ATTACK));
        }
        else
        {
            // Guard _against_, the dog belongs to the defense
            mDiscard = mDog;
            mDiscard.SetOwner(Team(Team::DEFENSE));
        }

         // We do not display the dog and start the deal immediatly
         mSequence = WAIT_FOR_START_DEAL;
    }
    else
    {
        // Show the dog to all the players
        mSequence = WAIT_FOR_SHOW_DOG;
    }
}
/*****************************************************************************/

/**
 * @brief Engine::BidSequence
 * @return The next sequence to go
 */
void Engine::BidSequence()
{
    // If a slam has been announced, we start immediately the deal
    if (IsEndOfTrick() || mBid.slam)
    {
        if (mBid.contract == Contract::PASS)
        {
            // All the players have passed, deal again new cards
            mSequence = WAIT_FOR_ALL_PASSED;
        }
        // On a terminé les enchères, on bascule sur le choix du roi
        // dans le cas du jeu à 5 joueurs
        else if ((mNbPlayers == 5) && (mSequence == WAIT_FOR_SHOW_BID))
        {
            mBid.partner = mBid.taker; // par défaut, le partenaire est le preneur (cas à 5 joueurs que l'on tente
            // de généraliser pour tous les autres modes de jeu
            mSequence = WAIT_FOR_KING_CALL;
        }
        else
        {
            SetAfterBidSequence();
        }
    }
    else
    {
        mSequence = WAIT_FOR_BID;
    }
}
/*****************************************************************************/
void Engine::DiscardSequence()
{
    mSequence = WAIT_FOR_DISCARD;
}
/*****************************************************************************/
bool Engine::IsEndOfTrick()
{
    bool endOfTrick = false;
    if (mPosition >= mNbPlayers)
    {
        // Trick as ended, all the players have played
        mPosition = 0U;
        mTrickCounter++;
        endOfTrick = true;
    }
    return endOfTrick;
}
/*****************************************************************************/
void Engine::CreateDeal(Tarot::Distribution &shuffle)
{
    DealGenerator editor;
    bool random = true;

    if (shuffle.mType == Tarot::Distribution::CUSTOM_DEAL)
    {
        std::string fullPath;

        // If not an absolute path, then it is a path relative to the home directory
        if (!Util::FileExists(shuffle.mFile))
        {
            fullPath = System::HomePath() + shuffle.mFile;
        }
        else
        {
            fullPath = shuffle.mFile;
        }

        if (!editor.LoadFile(fullPath))
        {
            // Fall back to default mode
            TLogError("Cannot load custom deal file: " + fullPath);
        }
        else if (editor.IsValid(mNbPlayers))
        {
            random = false;
            // Override the current player
            mCurrentPlayer = editor.GetFirstPlayer();
            mDealer = mCurrentPlayer.Previous(mNbPlayers);
        }
        else
        {
            // Fall back to default mode
            TLogError("Invalid deal file");
        }
    }

    if (random)
    {
        bool valid = true;
        do
        {
            if (shuffle.mType == Tarot::Distribution::NUMBERED_DEAL)
            {
                valid = editor.CreateRandomDeal(mNbPlayers, shuffle.mSeed);
                if (!valid)
                {
                    // The provided seed does dot generate a valid deal, so switch to a random one
                    shuffle.mType = Tarot::Distribution::RANDOM_DEAL;
                }
            }
            else
            {
                valid = editor.CreateRandomDeal(mNbPlayers);
            }
        }
        while (!valid);

        // Save the seed
        shuffle.mSeed = editor.GetSeed();
    }

#ifdef UNIT_TEST
    editor.SaveFile("unit_test_current_deal.json");
#endif

    // Copy deal editor cards to engine
    for (std::uint32_t i = 0U; i < mNbPlayers; i++)
    {
        Place p(i);
        mPlayers[i].Clear();
        mPlayers[i].Append(editor.GetPlayerDeck(p));

        TLogInfo( "Player " + p.ToString() + " deck: " + mPlayers[i].ToString());

#ifdef UNIT_TEST
    std::cout << "Player " + p.ToString() + " deck: " + mPlayers[i].ToString() << std::endl;
#endif
    }
    mDog = editor.GetDogDeck();

    TLogInfo("Dog deck: " + editor.GetDogDeck().ToString());
}
/*****************************************************************************/
bool Engine::HasDecimal(float f)
{
    return (f - static_cast<int>(f) != 0);
}
/*****************************************************************************/
void Engine::NewDeal()
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
Place Engine::SetTrick(const Deck &trick, std::uint8_t trickCounter)
{
    std::uint8_t turn = trickCounter - 1U;
    Place winner;
    std::uint8_t numberOfPlayers = trick.Size();
    Place firstPlayer;

    if (turn == 0U)
    {
        firstPlayer = mFirstPlayer; // First trick, we take the first player after the dealer
    }
    else
    {
        // trick counter 2 and beyond
        firstPlayer = mWinner[turn - 1U];
    }

    // Get the number of tricks we must play, it depends of the number of players
    int numberOfTricks = Tarot::NumberOfCardsInHand(numberOfPlayers);

    if (turn < 24U)
    {
        // Bonus: Fool
        bool foolSwap = false;  // true if the fool has been swaped of teams
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
                    Team winnerTeam = ((winner == mBid.taker) || (winner == mBid.partner)) ? Team(Team::ATTACK) : Team(Team::DEFENSE);
                    Team foolTeam = (foolPlace == mBid.taker) ? Team(Team::ATTACK) : Team(Team::DEFENSE);

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

        if ((winner == mBid.taker) || (winner == mBid.partner))
        {
            mTricks[turn].SetOwner(Team(Team::ATTACK));
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
            mTricks[turn].SetOwner(Team(Team::DEFENSE));
            if (foolSwap)
            {
                statsAttack.points += 4; // get back the points
                statsAttack.oudlers++; // hey, it was MY oudler!
            }
        }

        // Save the current winner for the next turn
        mWinner[turn] = winner;
    }
    else
    {
        TLogError("Index out of scope!");
    }

    return winner;
}
/*****************************************************************************/
Place Engine::GetOwner(Place firstPlayer,const Card &card, int turn)
{
    Place p = firstPlayer;
    std::uint8_t numberOfPlayers = mTricks[turn].Size();

    for (const auto &c : mTricks[turn])
    {
        if (card == c)
        {
            break;
        }
        p = p.Next(numberOfPlayers); // max before rollover is the number of players
    }
    return p;
}
/*****************************************************************************/
/**
 * @brief Engine::GetTrick
 * @param turn
 * @return
 */
Deck Engine::GetTrick(std::uint8_t turn, std::uint8_t numberOfPlayers)
{
    if (turn >= Tarot::NumberOfCardsInHand(numberOfPlayers))
    {
        turn = 0U;
    }
    return mTricks[turn];
}
/*****************************************************************************/
Place Engine::GetWinner(std::uint8_t turn, std::uint8_t numberOfPlayers)
{
    if (turn >= Tarot::NumberOfCardsInHand(numberOfPlayers))
    {
        turn = 0U;
    }
    return mWinner[turn];
}
/*****************************************************************************/
void Engine::SetHandle(const Deck &handle, Team team)
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
Deck Engine::GetDog()
{
    return mDog;
}
/*****************************************************************************/
Deck Engine::GetDiscard()
{
    return mDiscard;
}
/*****************************************************************************/
void Engine::AnalyzeGame(Points &points, std::uint8_t numberOfPlayers)
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

    // 4.1 On sauvegarde les points bruts de l'attaque avant ajustements
    // Peut éventuellement servir pour analyser le jeu (ou le montrer)
    points.cardsPointsAttack = statsAttack.points;

    // 4.2 : à 3 ou 5 joueurs, le demi-point va au gagnant
    if (HasDecimal(statsAttack.points))
    {
        if (statsAttack.points >= Tarot::PointsToDo(points.oudlers))
        {
            // le demi-point lui-revient
            statsAttack.points += 0.5;
        }
        else
        {
            // il perd le demi-point
            statsAttack.points -= 0.5;
        }
    }

    // 5. We save the points done by the attacker
    points.pointsAttack = static_cast<int>(statsAttack.points); // voluntary ignore digits after the coma

    // 6. Handle bonus: Ces primes gardent la même valeur quel que soit le contrat.
    // La prime est acquise au camp vainqueur de la donne.
    points.handlePoints += Tarot::GetHandlePoints(numberOfPlayers, Tarot::GetHandleType(mAttackHandle.Size()));
    points.handlePoints += Tarot::GetHandlePoints(numberOfPlayers, Tarot::GetHandleType(mDefenseHandle.Size()));
}
/*****************************************************************************/
/**
 * @brief Generate a file with all played cards of the deal
 */
void Engine::GenerateEndDealLog(std::uint8_t numberOfPlayers, JsonObject &json)
{
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
}
/*****************************************************************************/
bool Engine::LoadGameDealLog(const std::string &fileName)
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
bool Engine::LoadGameDeal(const std::string &buffer)
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
bool Engine::DecodeJsonDeal(const JsonValue &json)
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
#ifdef UNIT_TEST
            std::cout << "First player: " << str_value << std::endl;
#endif
            mFirstPlayer = Place(str_value);
            mBid = bid;

            // Load played cards
            JsonValue tricks = json.FindValue("tricks");
            if (tricks.GetArray().Size() == Tarot::NumberOfCardsInHand(numberOfPlayers))
            {
                std::uint8_t trickCounter = 1U;
                mDiscard.CreateTarotDeck();

                for (JsonArray::iterator iter = tricks.GetArray().begin(); iter != tricks.GetArray().end(); ++iter)
                {
                    Deck trick(iter->GetString());

                    if (trick.Size() == numberOfPlayers)
                    {
                        Place winner = SetTrick(trick, trickCounter);
#ifdef UNIT_TEST
                        (void) winner;
            //            std::cout << "Trick: " << (int)trickCounter << ", Cards: " << trick.ToString() << ", Winner: " << winner.ToString() << std::endl;
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
#ifdef UNIT_TEST
                    std::cout << "Discard: " << mDiscard.ToString() << std::endl;
#endif

                    // Give the cards to the right team owner
                    if (bid.contract == Contract::GUARD_AGAINST)
                    {
                        mDiscard.SetOwner(Team(Team::DEFENSE));
                    }
                    else
                    {
                        mDiscard.SetOwner(Team(Team::ATTACK));
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
// End of file Engine.cpp
//=============================================================================
