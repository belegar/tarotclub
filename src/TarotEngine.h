/*=============================================================================
 * TarotClub - TarotEngine.h
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
#ifndef _TAROTENGINE_H
#define _TAROTENGINE_H

#include "Card.h"
#include "Deck.h"
#include "Deal.h"
#include "Player.h"
#include "TarotDeck.h"
#include "defines.h"
#include "Game.h"
#include "ServerConfig.h"
#include "Observer.h"

/*****************************************************************************/
class TarotEngine
{
public:
    class IEvent
    {
    public:
        virtual void RequestBid(Contract c, Place p) = 0;
        virtual void DealAgain() = 0;
        virtual void PlayCard(Place p) = 0;
        virtual void EndOfTrick(Place p) = 0;
        virtual void EndOfDeal() = 0;
        virtual void SendCards() = 0;
        virtual void ShowDog() = 0;
        virtual void StartDeal() = 0;
    };

    TarotEngine(IEvent &handler);
    ~TarotEngine();

    // Helpers
    void Initialize();
    void NewGame(Game::Mode mode);
    void StopGame();
    void NewDeal();
    void StartDeal();
    bool SyncDog();
    void SyncTrick();
    void SyncReady();
    void SyncCard();
    void SyncStart();
    void SyncBid();
    void SyncHandle();
    void BidSequence();

    // Getters
    Player &GetPlayer(Place p);
    Player *GetPlayer(std::uint32_t uuid);
    Place GetPlayerPlace(std::uint32_t uuid);
    Score &GetScore();
    Game &GetGameInfo();
    Deal &GetDeal();
    Game::Shuffle GetShuffle();
    Place GetFreePlayer();

    // Setters
    bool SetIdentity(std::uint32_t uuid, const Identity &ident);
    void SetShuffle(const Game::Shuffle &s);
    void SetDiscard(Deck &discard);
    void SetHandle(Deck &handle, Place p);
    void SetCard(Card *c, Place p);
    Contract SetBid(Contract c, bool slam, Place p);

private:
    Player  players[5];     // [3..5] players
    Deck    currentTrick;   // the main deck of cards
    Deal    deal;
    Game    gameState;
    Game::Shuffle       shuffle;
    IEvent& mEventHandler;

    // synchonization counters
    std::uint32_t     cntSyncIdentity;    // player sent his identity
    std::uint32_t     cntSyncDog;         // players saw the dog
    std::uint32_t     cntSyncBid;         // players saw the bid
    std::uint32_t     cntSyncTrick;       // end of a round
    std::uint32_t     cntSyncReady;       // end of a deal
    std::uint32_t     cntSyncHandle;      // show a declared handle to all players
    std::uint32_t     cntSyncStart;       // start of a deal
    std::uint32_t     cntSyncCard;        // players saw the played card

    bool IsCardValid(Card *c, Place p);
    bool HasCard(Card *c, Place p);
    void CreateDeal();
    void ShowDog();
    void GameSateMachine();
    void EndOfDeal();
};

#endif // _TAROTENGINE_H

//=============================================================================
// End of file TarotEngine.h
//=============================================================================
