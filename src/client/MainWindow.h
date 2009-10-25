/*=============================================================================
 * TarotClub - MainWindow.h
 *=============================================================================
 * Main window of the game. Contains all the widgets.
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

#ifndef _MAINWINDOW_H
#define _MAINWINDOW_H

// Includes Qt
#include <QMainWindow>
#include <QMenu>
#include <QAction>

// Includes locales
#include "../defines.h"
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

   Tapis      *tapis;      // QCanvasView

   // Autres fenêtres
   AboutWindow    *about;           // A propos
   ResultWindow   *resultWindow;    // Résultat d'une donne
   OptionsWindow  *optionsWindow;   // Options
   QDialog        *clientWindow;    // fenêtre pour joindre une partie réseau

   // Dock windows
   ScoresDock     *scoresDock;
   InfosDock      *infosDock;
   ChatDock       *chatDock;

public:
   MainWindow( QWidget* parent = 0, Qt::WFlags f = 0 );

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
