/*=============================================================================
 * Tarot Club - MainWindow.h
 *=============================================================================
 * Main window of the game. Contains all the widgets.
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

#ifndef _MAINWINDOW_H
#define _MAINWINDOW_H

// Includes Qt
#include <QMainWindow>
#include <QMenu>
#include <QGraphicsScene>
#include <QAction>

// Includes locales
#include "defines.h"
#include "Tapis.h"
#include "TextBox.h"
#include "AboutWindow.h"
#include "ResultWindow.h"
#include "OptionsWindow.h"
#include "ui_NetClientUI.h"
#include "ScoresDock.h"
#include "InfosDock.h"
#include "ChatDock.h"

/*****************************************************************************/
class MainWindow : public QMainWindow
{
   Q_OBJECT

protected:

   // Menus
   QMenu *jeu;
   QMenu *tarot;
   QMenu *fenetres;
   QMenu *help;

   // Menu Jeu
   QAction    *newLocalGameAct;
   QAction    *netGameServerAct;
   QAction    *netGameClientAct;
   QAction    *newDonneAct;
   QAction    *newDonneManuAct;
   
   // Menu Tarot
   QAction    *optionsAct;
   QAction    *pliPrecAct;

   // Menu Fenêtres
   QAction    *scoresAct;
   QAction    *infosAct;
   QAction    *chatAct;

   // Canvas et CanvasView
   QGraphicsScene   *canvas;
   Tapis            *tapis;      // QCanvasView

   // Autres fenêtres
   AboutWindow    *about;           // A propos
   ResultWindow   *resultWindow;    // Résultat d'une donne
   OptionsWindow  *optionsWindow;   // Options
   QDialog        *clientWindow;    // fenêtre pour joindre une partie réseau

   // Dock windows
   ScoresDock     *scoresDock;
   InfosDock      *infosDock;
   ChatDock       *chatDock;

   // Graphiques
   PlayerBox      *btNord;
   PlayerBox      *btOuest;
   PlayerBox      *btSud;
   PlayerBox      *btEst;
   PlayerBox      *btNordOuest;

   TextBox        *enchNord;
   TextBox        *enchOuest;
   TextBox        *enchSud;
   TextBox        *enchEst;
   TextBox        *enchNordOuest;
   
   QGroupBox      *groupBoutons;
   QPushButton    *boutonPasse;
   QPushButton    *boutonPrise;
   QPushButton    *boutonGarde;
   QPushButton    *boutonGardeSans;
   QPushButton    *boutonGardeContre;
   QCheckBox      *chelem;
   QPushButton    *boutonAccepterChien;
   QPushButton    *boutonPresenterPoignee;

public:
   MainWindow( QWidget* parent = 0, Qt::WFlags f = 0 );

   // Méthodes générales de gestion des Widgets du tapis de jeu
   Place retournePlace( Place origine, Place place_absolue );
   void  printNames( Identity *identities, Place place );
   void  colorisePreneur( Place );
   void  afficheSelection( Place );
   void  afficheCarte( GfxCard *, Place );
   void  afficheBoutons( Contrat contrat );
   void  afficheEnchere(Place enchereur,Contrat cont);
   void  cacheEncheres();
   void  showAvatars( bool b, int nb_players );
   void  razTapis();

public slots:
   void  slotScoresDock(void);
   void  slotInfosDock(void);
   void  slotChatDock(void);
   void  closeChat();
   void  closeScores();
};

#endif // _MAINWINDOW_H

//=============================================================================
// End of file TarotWindow.h
//=============================================================================
