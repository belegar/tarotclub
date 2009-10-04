/*=============================================================================
 * Tarot Club - Game.h
 *=============================================================================
 * Derived class from MainWindow class. Central class for the game engine.
 *=============================================================================
 * Tarot Club est un jeu de Tarot français
 * Copyright (C) 2003-2005  Anthony Rabine
 * anthony@ooso.org
 * http://tarotclub.ooso.org
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 *=============================================================================
 */

#ifndef _GAME_H
#define _GAME_H

// Qt includes
#include <QGraphicsScene>
#include <QtNetwork>

// Game includes
#include "defines.h"
#include "windows/MainWindow.h"
#include "base/Client.h"
#include "base/Bot.h"
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
   ConfigFile  *config;
   GfxCard     cardsPics[78];
   Bot         bots[4];
   Client      client;
   Sequence    sequence;
   Place       tour;

   Card *getCardFromPix( GfxCard *gc );

public:
   Game( ConfigFile *conf );
   ~Game();

   void applyOptions();
   int setTheme( QString gamePath );
   void afficheCartesJoueur( int pos );
   void hidePli();

public slots:
   void slotNewLocalGame();
   void showOptions();
   void slotClickCard(GfxCard *c);
   void slotClickTapis();
   void slotMoveCursor( GfxCard *c );
   
   // événements du serveur
   void slotReceptionCartes();
   void slotAfficheSelection(Place p);
   void slotChoixEnchere(Contrat c);
   void slotAfficheEnchere(Place p, Contrat c);
   void slotAfficheChien();
   void slotRedist();
   void slotPrepareChien();
   void slotDepartDonne(Place p,Contrat c);
   void slotJoueCarte();
   void slotAfficheCarte(int id);
   void slotFinDonne();
   void slotWaitPli();

   // boutons sur le tapis
   void slotBoutton1();
   void slotBoutton2();
   void slotBoutton3();
   void slotBoutton4();
   void slotBoutton5();
   void slotAccepteChien();
   void setEnchere( Contrat cont );
   
};

#endif // _GAME_H

//=============================================================================
// End of file Game.h
//=============================================================================
