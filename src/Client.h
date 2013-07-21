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
#include "Deal.h"

/*****************************************************************************/
class Client : public Protocol
{
    Q_OBJECT

public:
    Client();

    // Helpers
    void Initialize();
    bool TestHandle();
    Contract CalculateBid();
    void UpdateStatistics();
    Card *Play();
    bool IsValid(Card *c);
    void BuildDogDeck();

    // Getters
    Deck::Statistics &GetStatistics();
    Deck &GetCurrentTrick();
    Deck &GetDogDeck();
    Deck &GetHandleDeck();
    Deck &GetMyDeck();
    Identity &GetMyIdentity();
    Game &GetGameInfo();
    Score &GetScore();

    // Setters
    void SetMyIdentity(const Identity &ident);

    // Network
    void ConnectToHost(const QString &hostName, quint16 port);
    void Close();

    // Protocol methods
    void SendBid(Contract c);
    void SendSyncDog();
    void SendDog();
    void SendReady();
    void SendError();
    void SendCard(Card *c);
    void SendSyncTrick();
    void SendHandle();
    void SendSyncStart();
    void SendSyncCard();
    void SendSyncBid();
    void SendSyncHandle();

public slots:
    void slotSocketReadData();
    void slotSocketConnected();
    void slotSocketHostFound();
    void slotSocketClosed();
    void slotSocketError(QAbstractSocket::SocketError code);

signals:

    void sigMessage(const QString &message);
    void sigAssignedPlace(Place p);
    void sigPlayersList(QMap<Place, Identity> &players);
    void sigReceiveCards();
    void sigSelectPlayer(Place);
    void sigRequestBid(Contract);
    void sigShowBid(Place, Contract);
    void sigStartDeal(Place, Contract);
    void sigShowDog();
    void sigShowHandle();
    void sigBuildDiscard();
    void sigDealAgain();
    void sigPlayCard();
    void sigShowCard(int, Place);
    void sigWaitTrick(Place);
    void sigEndOfDeal();
    void sigEndOfGame();

private:

    Player      player;
    QTcpSocket  socket;
    Deck::Statistics   stats;      // statistics on player's cards
    Game        info;       // Helper class to store various game information
    Score       score;
    Deck        dogDeck;
    Deck        handleDeck;    // declared poignee by a player
    Deck        currentTrick;

    // TarotClub Protocol methods
    void SendIdentity();
    void SendChatMessage(const QString &message);
    bool DoAction(QDataStream &in);

};

#endif // _CLIENT_H

//=============================================================================
// End of file Client.h
//=============================================================================
