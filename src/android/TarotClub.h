/*=============================================================================
 * TarotClub - TarotClub.h
 *=============================================================================
 * Graphical TarotClub client class, contains an embedded server.
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

#ifndef _TAROTCLUB_H
#define _TAROTCLUB_H

#include <QWidget>

// Game includes
#include "Client.h"
#include "Common.h"
#include "Table.h"
#include "ClientConfig.h"
#include "ServerConfig.h"
#include "GfxCard.h"
#include "Canvas.h"

/*****************************************************************************/
/**
 * joueur 0 : humain SUD
 * joueur 1 : ordi EST
 * joueur 2 : ordi NORD (ou NORD-EST)
 * joueur 3 : ordi OUEST
 */
class TarotClub : public QWidget, public Client::IEvent
{
    Q_OBJECT

public:
    enum ConnectionType
    {
        NO_CONNECTION,
        LOCAL,
        REMOTE
    };

    TarotClub();

    /**
     * @brief Initialize
     * First time initialization
     */
    void Initialize();

private:
    Table           table;    // A Tarot table, owns a thread, bots and a Tarot network engine game
    ClientConfig    clientConfig;
    ServerConfig    mServerConfig;
    Client          mClient; // The human player
    bool            firstTurn;
    QMap<Place, Identity> players;
    Deal            deal;
    Deck            discard;
    std::list<std::string> mMessages;
    ConnectionType  mConnectionType;
    Canvas *tapis;      // QCanvasView

    // Helpers
    void ApplyOptions();
    void ShowSouthCards();
    void HideTrick();
    void LaunchLocalGame(Game::Mode, const Game::Shuffle &sh);
    void LaunchRemoteGame(const std::string &ip, std::uint16_t port);
    void InitScreen();
    void showVictoryWindow();
    bool HasLocalConnection();

    // Client events
    virtual void Message(const std::string &message)
    {
        mMessages.push_back(message);
        emit sigMessage();
    }
    virtual void AssignedPlace() { emit sigAssignedPlace(); }
    virtual void PlayersList(std::map<Place, Identity> &pl)
    {
        std::map<Place, Identity>::iterator iter;

        players.clear();
        for(iter = pl.begin(); iter != pl.end(); iter++)
        {
            players[iter->first] = iter->second;
        }

        emit sigPlayersList();
    }
    virtual void ReceiveCards() { emit sigReceiveCards(); }
    virtual void SelectPlayer(Place p) { emit sigSelectPlayer(p); }
    virtual void RequestBid(Contract highestBid) { emit sigRequestBid(highestBid); }
    virtual void ShowBid(Place p, bool slam, Contract c) { emit sigShowBid(p, slam, c); }
    virtual void StartDeal(Place taker, Contract c, const Game::Shuffle &sh) { emit sigStartDeal(taker, c, sh); }
    virtual void ShowDog() { emit sigShowDog(); }
    virtual void ShowHandle() { emit sigShowHandle(); }
    virtual void BuildDiscard() { emit sigBuildDiscard(); }
    virtual void DealAgain() { emit sigDealAgain(); }
    virtual void PlayCard()  { emit sigPlayCard(); }
    virtual void ShowCard(Place p, const std::string &name)  { emit sigShowCard(p, name); }
    virtual void WaitTrick(Place winner) { emit sigWaitTrick(winner); }
    virtual void EndOfDeal() { emit sigEndOfDeal(); }
    virtual void EndOfGame() { emit sigEndOfGame(); }

signals:
   void sigReceiveCards();
   void sigAssignedPlace();
   void sigPlayersList();
   void sigMessage();
   void sigSelectPlayer(Place);
   void sigRequestBid(Contract);
   void sigShowBid(Place, bool, Contract);
   void sigShowDog();
   void sigStartDeal(Place, Contract, Game::Shuffle);
   void sigShowHandle();
   void sigBuildDiscard();
   void sigDealAgain();
   void sigPlayCard();
   void sigShowCard(Place, std::string);
   void sigWaitTrick(Place);
   void sigEndOfDeal();
   void sigEndOfGame();

private slots:
    // Client events
    void slotReceiveCards();
    void slotAssignedPlace();
    void slotPlayersList();
    void slotMessage();
    void slotSelectPlayer(Place p);
    void slotRequestBid(Contract highestBid);
    void slotShowBid(Place p, bool slam, Contract c);
    void slotShowDog();
    void slotStartDeal(Place taker, Contract c, Game::Shuffle sh);
    void slotShowHandle();
    void slotBuildDiscard();
    void slotDealAgain();
    void slotPlayCard();
    void slotShowCard(Place p, std::string name);
    void slotWaitTrick(Place p);
    void slotEndOfDeal();
    void slotEndOfGame();

    // Menus
    void slotNewTournamentGame();
    void slotNewQuickGame();
    void slotCreateNetworkGame();
    void slotQuitTarotClub();

    // Board events
    void slotAcceptHandle();
    void slotAcceptDiscard();
    void slotSetEnchere(Contract cont);
    void slotClickCard(GfxCard *c);
    void slotClickTapis();
    void slotMoveCursor(GfxCard *c);
    void slotSendChatMessage(const QString &message);
};

#endif // _TAROTCLUB_H

//=============================================================================
// End of file TarotClub.h
//=============================================================================
