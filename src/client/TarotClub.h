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

// Qt includes
#include <QtNetwork>

// Game includes
#include "../defines.h"
#include "../Table.h"
#include "MainWindow.h"
#include "ClientConfig.h"
#include "../ServerConfig.h"
#include <QThread>

/*****************************************************************************/
/**
 * joueur 0 : humain SUD
 * joueur 1 : ordi EST
 * joueur 2 : ordi NORD (ou NORD-EST)
 * joueur 3 : ordi OUEST
 */
class TarotClub : public MainWindow
{
    Q_OBJECT

public:
    TarotClub();

    // Helpers
    void Initialize(); // First time initialization
    void ApplyOptions();
    void afficheCartesJoueur(int pos);
    void hidePli();
    void hideChien();
    void NewGame(const QString &address, int port);
    void showVictoryWindow();

    // Setters
    int setTheme();

private:
    Table table;    // A Tarot table, owns a thread, bots and a Tarot network engine game
    QThread thread;
    ClientConfig clientConfig;
    Client      client; // The human player
    bool        firstTurn;
    QMap<Place, Identity> players;
    Deal        deal;

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

    // client events
    void slotMessage(const QString &message);
    void slotAssignedPlace(Place p);
    void slotPlayersList(QMap<Place, Identity> &pl);
    void slotReceiveCards();
    void slotSelectPlayer(Place p);
    void slotRequestBid(Contract highestBid);
    void slotShowBid(Place p, Contract c);
    void slotStartDeal(Place p, Contract c);
    void slotShowDog();
    void slotBuildDiscard();
    void slotDealAgain();
    void slotPlayCard();
    void slotShowCard(int id, Place p);
    void slotShowHandle();
    void slotWaitTrick(Place winner);
    void slotEndOfDeal();
    void slotEndOfGame();

    // Board events
    void slotPresenterPoignee();
    void slotAccepteChien();
    void slotSetEnchere(Contract cont);
    void slotClickCard(GfxCard *c);
    void slotClickTapis();
    void slotMoveCursor(GfxCard *c);
};

#endif // _TAROTCLUB_H

//=============================================================================
// End of file TarotClub.h
//=============================================================================
