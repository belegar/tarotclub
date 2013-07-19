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

#include <QObject>
#include <QMap>
#include "Card.h"
#include "Deck.h"
#include "Deal.h"
#include "Player.h"
#include "TarotDeck.h"
#include "defines.h"
#include "Game.h"
#include "ServerConfig.h"

/*****************************************************************************/
class TarotEngine : public QObject
{
    Q_OBJECT

public:
    enum ShuffleType
    {
        RANDOM_DEAL,
        CUSTOM_DEAL,
        NUMBERED_DEAL
    };

    struct Shuffle
    {
        ShuffleType type;
        QString     file;
        int         seed;
    };

    TarotEngine();
    ~TarotEngine();

    // Helpers
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
    void BidSequence();

    // Getters
    Player &GetPlayer(Place p);
    Score &GetScore();
    int GetDealNumber();
    Game &GetGameInfo();
    Deal &GetDeal();

    // Setters
    void SetShuffle(Shuffle &s);
    void SetDiscard(Deck &discard);
    void SetHandle(Deck &handle, Place p);
    void SetCard(Card *c, Place p);
    Contract SetBid(Contract c, Place p);

signals:
    void sigRequestBid(Contract c, Place p);
    void sigDealAgain();
    void sigPlayCard(Place p);
    void sigEndOfTrick(Place p);
    void sigEndOfDeal();
    void sigSendCards();
    void sigShowDog();
    void sigStartDeal();

private:
    Player      players[5];     // [3..5] players
    Deck        currentTrick;   // the main deck of cards
    Deal        deal;
    Game        gameState;
    Shuffle     shuffle;
    int         dealCounter;    // number of deals for the tournament game

    // synchonization counters
    int         cntSyncDog;     // players saw the dog
    int         cntSyncBid;     // players saw the bid
    int         cntSyncTrick;   // end of a round
    int         cntSyncReady;   // end of a deal
    int         cntSyncStart;   // start of a deal
    int         cntSyncCard;    // players saw the played card

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
