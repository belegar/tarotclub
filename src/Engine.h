/*=============================================================================
 * TarotClub - Engine.h
 *=============================================================================
 * Moteur de jeu principal + serveur de jeu réseau
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
#ifndef TAROT_ENGINE_H
#define TAROT_ENGINE_H

#include "Card.h"
#include "Deck.h"
#include "Common.h"
#include "ServerConfig.h"
#include "Observer.h"
#include "JsonValue.h"
#include "Score.h"

/*****************************************************************************/
/**
 * @brief Tarot state engine
 *
 * This class stores information about one deal. It also manages the game states
 * and transitions.
 *
 */
class Engine
{
public:
    enum Sequence
    {
        BAD_STEP,
        STOPPED,
        WAIT_FOR_PLAYERS,
        WAIT_FOR_READY,
        WAIT_FOR_CARDS,
        WAIT_FOR_BID,
        WAIT_FOR_SHOW_BID,
        WAIT_FOR_ALL_PASSED,
        WAIT_FOR_KING_CALL, // étape spécifique pour le jeu à 5 joueurs
        WAIT_FOR_SHOW_KING_CALL, // étape spécifique pour le jeu à 5 joueurs
        WAIT_FOR_SHOW_DOG,
        WAIT_FOR_DISCARD,
        WAIT_FOR_START_DEAL,
        WAIT_FOR_HANDLE,
        WAIT_FOR_SHOW_HANDLE,
        WAIT_FOR_PLAYED_CARD,
        WAIT_FOR_SHOW_CARD,
        WAIT_FOR_END_OF_TRICK,
        WAIT_FOR_END_OF_DEAL
    };

    Engine();
    ~Engine();

    // Helpers
    void Initialize();
    void Stop();
    void CreateTable(std::uint8_t nbPlayers);
    void NewGame();
    Tarot::Distribution NewDeal(const Tarot::Distribution &shuffle);
    Place StartDeal();
    void SetAfterBidSequence();
    void EndOfDeal(JsonObject &json);
    void BidSequence();
    void DiscardSequence();
    void GameSequence();

    // Getters
    Deck GetDeck(Place p);
    Place GetCurrentPlayer()
    {
        return mCurrentPlayer;
    }
    Sequence GetSequence()
    {
        return mSequence;
    }
    Points GetCurrentGamePoints();

    std::uint8_t    GetNbPlayers()
    {
        return mNbPlayers;
    }

    bool IsLastTrick()
    {
        return Tarot::IsDealFinished(mTrickCounter, mNbPlayers);
    }

    // Setters
    bool SetDiscard(const Deck &discard);
    bool SetHandle(const Deck &handle, Place p);
    bool SetCard(const Card &c, Place p);
    Contract SetBid(Contract c, bool slam, Place p);
    bool SetKingCalled(const Card &c);
    void SetHandle(const Deck &handle, Team team);
    Place SetTrick(const Deck &trick, std::uint8_t trickCounter);

    // Helpers
    void NewDeal();
    void AnalyzeGame(Points &points, std::uint8_t numberOfPlayers);
    void GenerateEndDealLog(std::uint8_t numberOfPlayers, JsonObject &json);
    bool LoadGameDealLog(const std::string &fileName);
    bool LoadGameDeal(const std::string &buffer);
    bool DecodeJsonDeal(const JsonValue &json);

    // Getters
    Deck GetTrick(std::uint8_t turn, std::uint8_t numberOfPlayers);
    Place GetWinner(std::uint8_t turn, std::uint8_t numberOfPlayers);
    std::map<int, Place> GetPodium();
    Deck GetDog();
    Deck GetDiscard();
    Tarot::Bid GetBid() { return mBid; }

    static bool HasDecimal(float f);


private:
    Deck    mPlayers[5];     // [3..5] deck of players with their UUID, index = Place
    Deck    currentTrick;   // store the current trick cards played
    Points  mCurrentPoints;

    // Game state variables
    std::uint8_t    mNbPlayers;
    Sequence        mSequence;
    Tarot::Bid      mBid;
    std::uint8_t    mPosition;          // Current position, [1..numberOfPlayers]
    Place           mDealer;            // who has dealt the cards
    std::uint8_t    mTrickCounter;       // number of tricks played [1..18] for 4 players
    Place           mCurrentPlayer;
    unsigned        mSeed;
    bool            mHandleAsked[5U];

    // Gestion de la donne en cours
    Deck mDiscard;
    Deck mDog;
    Deck mAttackHandle;
    Deck mDefenseHandle;
    Deck mTricks[24];    // 24 tricks max with 3 players
    Place mWinner[24];
    Place mPreviousWinner;
    Place mFirstPlayer;
    Card mKingCalled;
    int mTricksWon;
    Deck::Statistics statsAttack;

    void CreateDeal(Tarot::Distribution &shuffle);
    bool IsEndOfTrick();
    Place GetOwner(Place firstPlayer, const Card &card, int turn);

};

#endif // TAROT_ENGINE_H

//=============================================================================
// End of file Engine.h
//=============================================================================
