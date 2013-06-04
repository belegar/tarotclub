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
    /**
     * @brief The GameMode enum
     */
    enum GameMode
    {
        LOCAL_ONEDEAL,
        LOCAL_TOURNAMENT,
        NET_GAME_SERVER,
        NET_GAME_CLIENT
    };

    /**
     * @brief The DealType enum
     */
    enum DealType
    {
        RANDOM_DEAL,
        CUSTOM_DEAL,
        NUMBERED_DEAL
    };

    TarotEngine();
    ~TarotEngine();

    Player &GetPlayer(Place p);
    Score &GetScore();
    int GetDealNumber();

    void SetDealType(DealType type);
    void SetDealNumber(int deal);
    void SetDealFile(QString file);
    void SetGameMode(GameMode mode);
    void SetOptions(ServerOptions &opt);

private:
    Player      players[5];     // [3..5] players
    Deck        currentTrick;   // the main deck of cards
    Deal        deal;
    Game        gameState;
    DealType    dealType;
    int         dealNumber;
    int         dealCounter;    // number of deals for the tournament game
    QString     dealFile;
    GameMode    gameMode;

    // synchonization counters
    int         cptVuChien; // players saw the dog
    int         cptVuPli;   // end of a round
    int         cptVuDonne; // end of a deal

    void NewGame();
    void StopGame();
    void NewDeal();
    void StartDeal();

    bool IsCardValid(Card *c, Place p);
    bool HasCard(Card *c, Place p);
    void CreateDeal();

    /**
     * @brief CalculateTrickWinner
     * Each trick is won by the highest trump in it, or the highest card
     * of the suit led if no trumps were played.
     *
     * This methods does not verify if the trick is consistent with the rules; this
     * is supposed to have been previously verified.
     *
     * @return The place of the winner of this trick
     */
    Place CalculateTrickWinner();
    void BidSequence();
    void ShowDog();
    void GameSateMachine();

    /**
     * @brief EndOfTrick
     *
     * Fin d'un tour, on calcule le gagnant du pli et on prépare la suite
     * On retourne également le nombre de points réalisés par le preneur
     * retourne true si la partie est terminée, sinon false
     *
     * @param points
     * @return
     */
    bool EndOfTrick(float &points);

signals:
    void sigSelectPlayer(Place p);
    void sigAskBid(Contract p);
    void sigDealAgain();
    void sigPlayCard(Place p);
    void sigEndOfTrick();
    void sigEndOfDeal();
    void sigSendCards();
    void sigShowDog();
    void sigStartDeal();

};

#endif // _TAROTENGINE_H

//=============================================================================
// End of file TarotEngine.h
//=============================================================================
