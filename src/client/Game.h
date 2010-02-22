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
#include "../Bot.h"
#include "../TarotEngine.h"
#include "MainWindow.h"
#include "ConfigFile.h"


/*****************************************************************************/
/**
 * joueur 0 : humain SUD
 * joueur 1 : ordi EST
 * joueur 2 : ordi NORD (ou NORD-EST)
 * joueur 3 : ordi OUEST
 * joueur 4 : ordi NORD-OUEST
 */
class Game : public MainWindow
{
   Q_OBJECT

private:
   TarotEngine server;
   ConfigFile  *config;
   Bot         bots[4];
   Client      client;
   Sequence    sequence;
   Place       tour;

   Card *getCardFromPix( GfxCard *gc );

public:
   Game( ConfigFile *conf );
   ~Game();

   void applyOptions();
   int setTheme();
   void afficheCartesJoueur( int pos );
   void hidePli();
   void hideChien();

public slots:
   void slotNewLocalGame();
   void showOptions();
   void slotClickCard(GfxCard *c);
   void slotClickTapis();
   void slotMoveCursor( GfxCard *c );

   // événements du serveur
   void slotReceptionCartes();
   void slotAfficheSelection(Place p);
   void slotAfficheChien();
   void slotAccepteChien();
   void slotRedist();
   void slotPrepareChien();
   void slotDepartDonne(Place p,Contrat c);
   void slotJoueCarte();
   void slotAfficheCarte(int id);
   void slotFinDonne();
   void slotWaitPli();
   void slotSetEnchere( Contrat cont );

};

#endif // _GAME_H

//=============================================================================
// End of file Game.h
//=============================================================================
