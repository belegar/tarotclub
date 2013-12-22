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

// Game includes
#include "Client.h"
#include "defines.h"
#include "Table.h"
#include "MainWindow.h"
#include "ClientConfig.h"
#include "ServerConfig.h"

// Qt includes
#include <QtNetwork>
#include <QThread>


/*****************************************************************************/
/**
 * joueur 0 : humain SUD
 * joueur 1 : ordi EST
 * joueur 2 : ordi NORD (ou NORD-EST)
 * joueur 3 : ordi OUEST
 */
class TarotClub : public MainWindow, public Client::IEvent
{
    Q_OBJECT

public:
    TarotClub();

    // Helpers
    void Initialize(); // First time initialization
    void ApplyOptions();
    void ShowSouthCards();
    void HideTrick();
    void NewGame(const QString &address, int port);
    void showVictoryWindow();

    // Setters
    int setTheme();

private:
    Table           table;    // A Tarot table, owns a thread, bots and a Tarot network engine game
    ClientConfig    clientConfig;
    Client          mClient; // The human player
    bool            firstTurn;
    QMap<Place, Identity> players;
    Deal            deal;
    Deck            discard;
    std::list<std::string> mMessages;

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
    virtual void ShowHandle();
    virtual void BuildDiscard();
    virtual void DealAgain();
    virtual void PlayCard();
    virtual void ShowCard(Place p, const std::string &name);
    virtual void WaitTrick(Place winner);
    virtual void EndOfDeal();
    virtual void EndOfGame();

signals:
   void sigReceiveCards();
   void sigAssignedPlace();
   void sigPlayersList();
   void sigMessage();
   void sigSelectPlayer(Place);
   void sigRequestBid(Contract);
   void sigShowBid(Place, bool, Contract);
   void sigShowDog();
   void sigStartDeal(Place, Contract, const Game::Shuffle &);

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
    void slotStartDeal(Place taker, Contract c, const Game::Shuffle &sh);

    // Menus
    void slotNewTournamentGame();
    void slotNewNumberedDeal();
    void slotNewCustomDeal();
    void slotNewQuickGame();
    void slotJoinNetworkGame();
    void slotQuickJoinNetworkGame();
    void slotCreateNetworkGame();
    void slotShowOptions();
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
