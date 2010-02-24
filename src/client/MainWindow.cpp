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
MainWindow::MainWindow( QWidget* parent, Qt::WFlags f )
   : QMainWindow( parent, f )
{
   //==============================================================
   //       FENETRES DU JEU
   //==============================================================

   // About Window
   about = new AboutWindow( this );
   about->hide();

   // Scores
   resultWindow = new ResultWindow( this );
   resultWindow->setAttribute(Qt::WA_ShowModal, true);
   resultWindow->hide();

   // Options
   optionsWindow = new OptionsWindow( this );
   optionsWindow->setAttribute(Qt::WA_ShowModal, true);
   optionsWindow->hide();

   // Joignage d'une partie réseau
   clientWindow = new QDialog( this, Qt::WindowMinMaxButtonsHint );
   clientUI.setupUi(clientWindow);
   clientWindow->hide();

   // Dock window : scores
   scoresDock = new ScoresDock(this);
   addDockWidget(Qt::RightDockWidgetArea, scoresDock);
   scoresDock->show();
   connect( scoresDock,SIGNAL(sgnlClose()),this, SLOT(closeScores()) );

   // Dock window : informations
   infosDock = new InfosDock(this);
   addDockWidget(Qt::RightDockWidgetArea, infosDock);
   infosDock->show();

   // Dock window : discussion
   chatDock = new ChatDock(this);
   addDockWidget(Qt::BottomDockWidgetArea, chatDock);
   chatDock->hide();
   connect( chatDock,SIGNAL(sgnlClose()),this, SLOT(closeChat()) );

   // Dock window : serveur
   serverDock = new QDockWidget(this);
   serverUI.setupUi(serverDock);
   addDockWidget(Qt::RightDockWidgetArea, serverDock);
   serverDock->show();

   //==============================================================
   //       MENUS
   //==============================================================

   //----------
   // Menu Jeu
   //----------
   newLocalGameAct = new QAction("&Nouveau jeu solitaire", this);
   newLocalGameAct->setShortcut(QString("Ctrl+N"));
   newLocalGameAct->setStatusTip("Lance une partie solitaire contre l'ordinateur");

   netGameServerAct = new QAction(trUtf8("Créer une partie en &réseau (serveur)"), this);
   netGameServerAct->setShortcut(trUtf8("Ctrl+R"));
   netGameServerAct->setStatusTip(trUtf8("Crée une partie en réseau en tant que serveur"));

   netGameClientAct = new QAction(trUtf8("Re&joindre une partie en réseau"), this);
   netGameClientAct->setShortcut(trUtf8("Ctrl+J"));
   netGameClientAct->setStatusTip(trUtf8("Connexion à un serveur sur Internet ou en réseau local"));

   newDonneAct = new QAction(trUtf8("Nouvelle &donne"), this);
   newDonneAct->setShortcut(trUtf8("Ctrl+D"));
   newDonneAct->setStatusTip(trUtf8("Redistribue les cartes pour ce tour de jeu"));

   newDonneManuAct = new QAction(trUtf8("Nouvelle donne &manuelle"), this);
   newDonneManuAct->setShortcut(trUtf8("Ctrl+M"));
   newDonneManuAct->setStatusTip(trUtf8("Redistribue les cartes avec une donne précise"));

   QAction *exitAct = new QAction(trUtf8("&Quitter"), this);
   exitAct->setShortcut(trUtf8("Ctrl+Q"));
   exitAct->setStatusTip(trUtf8("Quitte le jeu"));
   connect(exitAct, SIGNAL(triggered()), qApp, SLOT(quit()));

   // On ajoute les actions au menu 'Jeu'
   jeu = menuBar()->addMenu(trUtf8("Jeu"));
   jeu->addAction(newLocalGameAct);
   jeu->addAction(netGameServerAct);
   jeu->addAction(netGameClientAct);
   jeu->addSeparator();
   jeu->addAction(newDonneAct);
   jeu->addAction(newDonneManuAct);
   jeu->addSeparator();
   jeu->addAction(exitAct);

   //------------
   // Menu Tarot
   //------------
   optionsAct = new QAction(trUtf8("&Options"), this);
   optionsAct->setShortcut(trUtf8("Ctrl+O"));
   optionsAct->setStatusTip(trUtf8("Options du jeu"));

   pliPrecAct = new QAction(trUtf8("&Pli précédent"), this);
   pliPrecAct->setShortcut(trUtf8("Ctrl+P"));
   pliPrecAct->setStatusTip(trUtf8("Affiche le pli précédent"));

   // On ajoute les actions au menu 'Tarot'
   tarot = menuBar()->addMenu(trUtf8("Tarot"));
   tarot->addAction(optionsAct);
   tarot->addAction(pliPrecAct);

   //---------------
   // Menu Fenêtres
   //---------------
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
   serverAct->setChecked(true);

   fenetres = menuBar()->addMenu(trUtf8("Fenêtres"));
   fenetres->addAction(scoresAct);
   fenetres->addAction(infosAct);
   fenetres->addAction(chatAct);
   fenetres->addAction(serverAct);

   //-----------
   // Menu Help
   //-----------
   QAction *aboutAct = new QAction(trUtf8("&À propos..."), this);
   aboutAct->setShortcut(trUtf8("Ctrl+A"));
   aboutAct->setStatusTip(trUtf8("À propos de Tarot Club"));
   connect(aboutAct, SIGNAL(triggered()), about, SLOT(show()));

   // On ajoute les actions au menu '?'
   help = menuBar()->addMenu(trUtf8("?"));
   help->addAction(aboutAct);

   //==============================================================
   //       FENETRE PRINCIPALE
   //==============================================================
   setWindowTitle(QString(TAROT_TITRE) + " " + QString(TAROT_VERSION));
   tapis = new Tapis( this );
   setCentralWidget( tapis );
   tapis->show();
}
/*****************************************************************************/
void MainWindow::closeChat()
{
   chatAct->setChecked(false);
}
/*****************************************************************************/
void MainWindow::closeScores()
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
