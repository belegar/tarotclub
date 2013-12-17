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

    // Client events
    virtual void Message(const std::string &message);
    virtual void AssignedPlace(Place p);
    virtual void PlayersList(std::map<Place, Identity> &players);
    virtual void ReceiveCards();
    virtual void SelectPlayer(Place p);
    virtual void RequestBid(Contract highestBid);
    virtual void ShowBid(Place p, bool slam, Contract c);
    virtual void StartDeal(Place taker, Contract c, const Game::Shuffle &sh);
    virtual void ShowDog();
    virtual void ShowHandle();
    virtual void BuildDiscard();
    virtual void DealAgain();
    virtual void PlayCard();
    virtual void ShowCard(Place p, const std::string &name);
    virtual void WaitTrick(Place winner);
    virtual void EndOfDeal();
    virtual void EndOfGame();

private slots:
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
