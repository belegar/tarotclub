/*=============================================================================
 * TarotClub - Client.h
 *=============================================================================
 * This class manages the network protocol and is used to join a online game
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

#ifndef _CLIENT_H
#define _CLIENT_H

#include <QTcpSocket>
#include "Card.h"
#include "Deck.h"
#include "Player.h"
#include "TarotDeck.h"
#include "Protocol.h"
#include "defines.h"
#include "Score.h"
#include "Game.h"

class Client : public Protocol
{
    Q_OBJECT

public:
    Client();

    // Helpers
    void Initialize();

    /**
     * @brief A fool in an handle means that the player has no more trumps
     * return true if it is the case, otherwise false
     */
    bool TestHandle();
    Contract CalculateBid();
    void UpdateStatistics();

    /**
     * @brief Return the first valid card in the player's hand deck
     */
    Card *Play();
    bool IsValid(Card *c);

    /**
     * @brief Build a valid discard at random
     */
    void BuildDogDeck(Deck &deck);

    // Getters
    Deck::Statistics &GetStatistics();
    Deck &GetMainDeck();
    Deck &GetDogDeck();
    Deck &GetHandleDeck();
    Game &GetGameInfo();
    Score &GetScore();

public slots:
    void SocketReadData();
    void SocketConnected();
    void SocketHostFound();
    void SocketClosed();
    void SocketError(QAbstractSocket::SocketError code);

signals:

    /**
     * @brief Emitted when a chat message has been broadcasted by one player or the server
     * @param message
     */
    void sigMessage(const QString &message);

    /**
     * @brief Emitted when the server has assigned a place around the table
     * @param p Place assigned by the server
     */
    void sigAssignedPlace(Place p);

    /**
     * @brief Emitted when the client has received the identity of the players in game
     * @param players
     */
    void sigPlayersList(QList<Identity> players);
    void sigReceivedCards();
    void sigSelectPlayer(Place);
    void sigRequestBid(Contract);
    void sigShowBid(Place, Contract);
    void sigStartDeal(Place, Contract);
    void sigShowDog();
    void sigBuildDiscard();
    void sigDealAgain();
    void sigPlayCard();
    void sigShowCard(int, Place);
    void sigWaitTrick(Place, float);
    void sigEndOfDeal();
    void sigEndOfGame();

private:

    Player      player;
    QTcpSocket  socket;
    Deck::Statistics   stats;      // statistics on player's cards
    Game        info;       // Helper class to store various game information
    Score       score;
    Deck        dogDeck;
    Deck        mainDeck;
    Deck        handleDeck;    // declared poignee by the player
    Deck        currentTrick;

    // Network methods
    void ConnectToHost(const QString &hostName, quint16 port);
    void Close();
    void SendIdentity();
    void SendChatMessage(const QString &message);
    void SendBid(Contract c);
    void SendDog();
    void SendHandle();
    void SendCard(Card *c);
    void SendReady();
    void SendError();
    void SendSyncDog();
    void SendSyncTrick();
    void SendSyncHandle();
    void DoAction(QDataStream &in);  

};

#endif // _CLIENT_H

//=============================================================================
// End of file Client.h
//=============================================================================
