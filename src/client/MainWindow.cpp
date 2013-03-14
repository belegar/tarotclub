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

#include "MainWindow.h"
#include <QStatusBar>
#include <QMenuBar>
#include <QDateTime>
#include <QDir>
#include "textes.h"

/*****************************************************************************/
MainWindow::MainWindow(QWidget* parent)
   : QMainWindow( parent)
{
   //==============================================================
   //       FENETRES DU JEU
   //==============================================================

   // About Window
   about = new AboutWindow(this);
   about->hide();

   // Scores
   resultWindow = new ResultWindow(this);
   resultWindow->setAttribute(Qt::WA_ShowModal, true);
   resultWindow->hide();

   // Options
   optionsWindow = new OptionsWindow(this);
   optionsWindow->setAttribute(Qt::WA_ShowModal, true);
   optionsWindow->hide();

   // Joignage d'une partie réseau
   joinWizard = new JoinWizard(this);
   joinWizard->hide();

   // Aide en ligne // TODO: use QtHelp system
 //  rulesWindow = new QDialog( this, Qt::WindowMinMaxButtonsHint );
 //  rulesUI.setupUi(rulesWindow);
 //  rulesWindow->hide();

   // Deal editor
   editorWindow = new EditorWindow(this);
   editorWindow->setAttribute(Qt::WA_ShowModal, true);
   editorWindow->hide();

   // ----------  Docks de droite ------------------------

   // Dock window : scores
   scoresDock = new ScoresDock(this);
   addDockWidget(Qt::RightDockWidgetArea, scoresDock);
   scoresDock->show();
   connect( scoresDock,SIGNAL(sgnlClose()),this, SLOT(slotCloseScores()) );

   // Dock window : informations
   infosDock = new InfosDock(this);
   addDockWidget(Qt::RightDockWidgetArea, infosDock);
   infosDock->show();

   // Dock window : donne
   roundDock = new RoundDock(this);
   addDockWidget(Qt::RightDockWidgetArea, roundDock);
   roundDock->show();



   // ----------  Docks du bas ------------------------

   // Dock window : discussion
   chatDock = new ChatDock(this);
   addDockWidget(Qt::BottomDockWidgetArea, chatDock);
   chatDock->show();
   connect( chatDock,SIGNAL(sgnlClose()),this, SLOT(slotCloseChat()) );

   // Dock window : serveur
   serverDock = new QDockWidget(this);
   serverUI.setupUi(serverDock);
   addDockWidget(Qt::BottomDockWidgetArea, serverDock);
   serverDock->show();

   // Debug window
   debugDock = new DebugDock(this);
   addDockWidget(Qt::BottomDockWidgetArea, debugDock);
   debugDock->show();

   tabifyDockWidget(roundDock, debugDock);

   //==============================================================
   //       MENUS
   //==============================================================

   //----------
   // Menu Jeu
   //----------
   //----- Local
   newQuickGameAct = new QAction("&Nouvelle partie rapide", this);
   newQuickGameAct->setShortcut(QString("Ctrl+N"));
   newQuickGameAct->setStatusTip("Lance une partie solitaire contre l'ordinateur");

   newTournamentAct = new QAction(trUtf8("Nouveau &tournoi"), this);
   newTournamentAct->setShortcut(trUtf8("Ctrl+T"));
   newTournamentAct->setStatusTip(trUtf8("Enchaînez plusieurs donne contre l'ordinateur"));

   newNumberedDealAct = new QAction(trUtf8("Nouvelle donne nu&mérotée"), this);
   newNumberedDealAct->setShortcut(trUtf8("Ctrl+M"));
   newNumberedDealAct->setStatusTip(trUtf8("Redistribue les cartes avec une donne précise"));

   newCustomDealAct = new QAction(trUtf8("Nouvelle donne manuelle"), this);
   newCustomDealAct->setShortcut(trUtf8("Ctrl+C"));
   newCustomDealAct->setStatusTip(trUtf8("Redistribue les cartes avec une donne construite avec l'éditeur"));

   //----- Network
   netGameServerAct = new QAction(trUtf8("Créer une partie en &réseau (serveur)"), this);
   netGameServerAct->setShortcut(trUtf8("Ctrl+R"));
   netGameServerAct->setStatusTip(trUtf8("Crée une partie en réseau en tant que serveur"));
   netGameServerAct->setEnabled(false);

   netGameClientAct = new QAction(trUtf8("Re&joindre une partie en réseau"), this);
   netGameClientAct->setShortcut(trUtf8("Ctrl+J"));
   netGameClientAct->setStatusTip(trUtf8("Connexion à un serveur sur Internet ou en réseau local"));

   //----- Misc
   pliPrecAct = new QAction(trUtf8("&Pli précédent"), this);
   pliPrecAct->setShortcut(trUtf8("Ctrl+P"));
   pliPrecAct->setStatusTip(trUtf8("Affiche le pli précédent"));
   pliPrecAct->setEnabled(false);

   QAction *exitAct = new QAction(trUtf8("&Quitter"), this);
   exitAct->setShortcut(trUtf8("Ctrl+Q"));
   exitAct->setStatusTip(trUtf8("Quitte le jeu"));
   connect(exitAct, SIGNAL(triggered()), qApp, SLOT(quit()));

   // On ajoute les actions au menu 'Jeu'
   gameMenu = menuBar()->addMenu(trUtf8("Jeu"));
   gameMenu->addAction(newQuickGameAct);
   gameMenu->addAction(newTournamentAct);
   gameMenu->addAction(newNumberedDealAct);
   gameMenu->addAction(newCustomDealAct);
   gameMenu->addSeparator();
   gameMenu->addAction(netGameServerAct);
   gameMenu->addAction(netGameClientAct);
   gameMenu->addSeparator();
   gameMenu->addAction(pliPrecAct);
   gameMenu->addSeparator();
   gameMenu->addAction(exitAct);

   //---------------
   // Menu Paramètres
   //---------------
   optionsAct = new QAction(trUtf8("&Options"), this);
   optionsAct->setShortcut(trUtf8("Ctrl+O"));
   optionsAct->setStatusTip(trUtf8("Options du jeu"));

   dealEditorAct = new QAction(trUtf8("É&diteur de donne"), this);
   dealEditorAct->setShortcut(trUtf8("Ctrl+D"));
   dealEditorAct->setStatusTip(trUtf8("Choisissez les cartes de chaque joueurs pour une donne particulière."));
   connect(dealEditorAct, SIGNAL(triggered()), this, SLOT(slotDealEditor()));

   scoresAct = new QAction(trUtf8("Scores"), this);
   scoresAct->setCheckable(true);
   scoresAct->setStatusTip(trUtf8("Montre/cache la fenêtre des scores"));
   connect(scoresAct, SIGNAL(triggered()), this, SLOT(slotScoresDock()));
   scoresAct->setChecked(true);

   infosAct = new QAction(trUtf8("Informations"), this);
   infosAct->setCheckable(true);
   infosAct->setStatusTip(trUtf8("Montre/cache la fenêtre d'informations"));
   connect(infosAct, SIGNAL(triggered()), this, SLOT(slotInfosDock()));
   infosAct->setChecked(true);

   chatAct = new QAction(trUtf8("Discussion"), this);
   chatAct->setCheckable(true);
   chatAct->setStatusTip(trUtf8("Montre/cache la fenêtre de discussion"));
   connect(chatAct, SIGNAL(triggered()), this, SLOT(slotChatDock()));
   chatAct->setChecked(false);

   serverAct = new QAction(trUtf8("Serveur"), this);
   serverAct->setCheckable(true);
   serverAct->setStatusTip(trUtf8("Montre/cache la fenêtre de serveur"));
   connect(serverAct, SIGNAL(triggered()), this, SLOT(slotServerDock()));
   serverAct->setChecked(false);

   paramsMenu = menuBar()->addMenu(trUtf8("Paramètres"));
   paramsMenu->addAction(optionsAct);
   paramsMenu->addAction(dealEditorAct);
   paramsMenu->addSeparator();
   paramsMenu->addAction(scoresAct);
   paramsMenu->addAction(infosAct);
   paramsMenu->addAction(chatAct);
   paramsMenu->addAction(serverAct);

   //-----------
   // Menu Help
   //-----------
   QAction *aboutAct = new QAction(trUtf8("&À propos..."), this);
   aboutAct->setShortcut(trUtf8("Ctrl+A"));
   aboutAct->setStatusTip(trUtf8("À propos de TarotClub"));
   connect(aboutAct, SIGNAL(triggered()), about, SLOT(show()));

//   QAction *rulesAct = new QAction(trUtf8("&Règles du Tarot"), this);
//   rulesAct->setShortcut(trUtf8("Ctrl+R"));
//   rulesAct->setStatusTip(trUtf8("Règles du Tarot"));
//   connect(rulesAct, SIGNAL(triggered()), rulesWindow, SLOT(show()));

   // On ajoute les actions au menu 'Help'
   helpMenu = menuBar()->addMenu(trUtf8("Aide"));
   helpMenu->addAction(aboutAct);
//   helpMenu->addAction(rulesAct);

   //==============================================================
   //       FENETRE PRINCIPALE
   //==============================================================
   setWindowTitle(QString(TAROT_TITRE) + " " + QString(TAROT_VERSION));
   tapis = new Tapis( this );
   setCentralWidget( tapis );
   tapis->show();
}
/*****************************************************************************/
void  MainWindow::slotDealEditor()
{
   editorWindow->init();
   editorWindow->exec();
}
/*****************************************************************************/
void MainWindow::slotCloseChat()
{
   chatAct->setChecked(false);
}
/*****************************************************************************/
void MainWindow::slotCloseScores()
{
   scoresAct->setChecked(false);
}
/*****************************************************************************/
void MainWindow::slotScoresDock(void)
{
   if( scoresDock->isVisible() == true ) {
     scoresAct->setChecked(false);
     scoresDock->hide();
   } else {
     scoresAct->setChecked(true);
     scoresDock->show();
   }
}
/*****************************************************************************/
void MainWindow::slotInfosDock(void)
{
   if( infosDock->isVisible() == true ) {
     infosAct->setChecked(false);
     infosDock->hide();
   } else {
     infosAct->setChecked(true);
     infosDock->show();
   }
}
/*****************************************************************************/
void MainWindow::slotChatDock(void)
{
   if( chatDock->isVisible() == true ) {
     chatAct->setChecked(false);
     chatDock->hide();
   } else {
     chatAct->setChecked(true);
     chatDock->show();
   }
}
/*****************************************************************************/
void MainWindow::slotServerDock(void)
{
   if( serverDock->isVisible() == true ) {
     serverAct->setChecked(false);
     serverDock->hide();
   } else {
     serverAct->setChecked(true);
     serverDock->show();
   }
}

//=============================================================================
// End of file MainWindow.cpp
//=============================================================================
