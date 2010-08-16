/*=============================================================================
 * TarotClub - Game.h
 *=============================================================================
 * Derived class from MainWindow class. Central class for the game engine.
 *=============================================================================
 * TarotClub ( http://www.tarotclub.fr ) - This file is part of TarotClub
 * Copyright (C) 2003-2999 - Anthony Rabine
 * anthony@tarotclub.fr
 *
 * This file must be used under the terms of the CeCILL.
 * This source file is licensed as described in the file COPYING, which
 * you should have received as part of this distribution.  The terms
 * are also available at
 * http://www.cecill.info/licences/Licence_CeCILL_V2-en.txt
 *
 *=============================================================================
 */

#ifndef _GAME_H
#define _GAME_H

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
class Game : public MainWindow
{
   Q_OBJECT

private:
   TarotEngine server;
   ClientConfig  clientConfig;
   ServerConfig  serverConfig;
   Client      client; // The human player
   Sequence    sequence;
   bool        firstTurn;
   int         turnCounter;
   QList<Identity> players;
   GameType    gameType;

   Card *getCardFromPix( GfxCard *gc );

public:
   Game();
   ~Game();

   void applyOptions();
   int setTheme();
   void afficheCartesJoueur( int pos );
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
   void slotShowOptions();
   void slotQuitTarotClub();

   // client events
   void slotListeDesJoueurs(QList<Identity> pl);
   void slotReceptionCartes();
   void slotAfficheSelection(Place p);
   void slotAfficheChien();
   void slotRedist();
   void slotPrepareChien();
   void slotDepartDonne(Place p,Contrat c);
   void slotJoueCarte();
   void slotAfficheCarte(int id, Place tour);
   void slotFinDonne(Place winner, float pointsTaker, bool);
   void slotWaitPli(Place winner, float pointsTaker);
   void slotSetEnchere( Contrat cont );

   // événements du tapis
   void slotPresenterPoignee();
   void slotAccepteChien();
   void slotClickCard(GfxCard *c);
   void slotClickTapis();
   void slotMoveCursor( GfxCard *c );

};

#endif // _GAME_H

//=============================================================================
// End of file Game.h
//=============================================================================
