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
#include "../Client.h"
#include "../TarotEngine.h"
#include "MainWindow.h"
#include "ClientConfig.h"
#include "ServerConfig.h"

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

private:
    TarotEngine server;         // built-in server, to run local games or to host network games
    ClientConfig  clientConfig;
    ServerConfig  serverConfig;
    Client      client; // The human player
    Sequence    sequence;
    bool        firstTurn;
    int         turnCounter;
    QList<Identity> players;
    GameType    gameType;

    Card *getCardFromPix(GfxCard *gc);

public:
    TarotClub();
    ~TarotClub();

    void applyOptions();
    int setTheme();
    void afficheCartesJoueur(int pos);
    void hidePli();
    void hideChien();
    void newLocalGame();
    void showVictoryWindow();

public slots:

    // Menus
    void slotNewTournamentGame();
    void slotNewNumberedDeal();
    void slotNewCustomDeal();
    void slotNewQuickGame();
    void slotJoinNetworkGame();
    void slotShowOptions();
    void slotQuitTarotClub();

    // client events
    void slotListeDesJoueurs(QList<Identity> pl);
    void slotReceptionCartes();
    void slotAfficheSelection(Place p);
    void slotAfficheChien();
    void slotRedist();
    void slotPrepareChien();
    void slotDepartDonne(Place p, Contrat c);
    void slotJoueCarte();
    void slotAfficheCarte(int id, Place tour);
    void slotFinDonne(Place winner, float pointsTaker, bool);
    void slotWaitPli(Place winner, float pointsTaker);
    void slotSetEnchere(Contrat cont);

    // Board events
    void slotPresenterPoignee();
    void slotAccepteChien();
    void slotClickCard(GfxCard *c);
    void slotClickTapis();
    void slotMoveCursor(GfxCard *c);

};

#endif // _TAROTCLUB_H

//=============================================================================
// End of file TarotClub.h
//=============================================================================
