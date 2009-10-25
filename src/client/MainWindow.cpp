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
   Ui::NetClientUI  clientUI;
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

   //==============================================================
   //       MENUS
   //==============================================================

   //----------
   // Menu Jeu
   //----------
   newLocalGameAct = new QAction( STR_NEW_GAME, this);
   newLocalGameAct->setShortcut(QString("Ctrl+N"));
   newLocalGameAct->setStatusTip(STR_STATUS1);

   netGameServerAct = new QAction(tr("Nouveau jeu en &réseau (serveur)"), this);
   netGameServerAct->setShortcut(tr("Ctrl+R"));
   netGameServerAct->setStatusTip(tr("Lance une partie en réseau (création du serveur)"));

   netGameClientAct = new QAction(tr("Nouveau &jeu en réseau (client)"), this);
   netGameClientAct->setShortcut(tr("Ctrl+J"));
   netGameClientAct->setStatusTip(tr("Lance une partie en réseau (connexion à un serveur)"));

   newDonneAct = new QAction(tr("Nouvelle &donne"), this);
   newDonneAct->setShortcut(tr("Ctrl+D"));
   newDonneAct->setStatusTip(tr("Redistribue les cartes pour ce tour de jeu"));

   newDonneManuAct = new QAction(tr("Nouvelle donne &manuelle"), this);
   newDonneManuAct->setShortcut(tr("Ctrl+M"));
   newDonneManuAct->setStatusTip(tr("Redistribue les cartes avec une donne précise"));

   QAction *exitAct = new QAction(tr("&Quitter"), this);
   exitAct->setShortcut(tr("Ctrl+Q"));
   exitAct->setStatusTip(tr("Quitte le jeu"));
   connect(exitAct, SIGNAL(triggered()), qApp, SLOT(quit()));

   // On ajoute les actions au menu 'Jeu'
   jeu = menuBar()->addMenu(tr("Jeu"));
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
   optionsAct = new QAction(tr("&Options"), this);
   optionsAct->setShortcut(tr("Ctrl+O"));
   optionsAct->setStatusTip(tr("Options du jeu"));

   pliPrecAct = new QAction(tr("&Pli précédent"), this);
   pliPrecAct->setShortcut(tr("Ctrl+P"));
   pliPrecAct->setStatusTip(tr("Affiche le pli précédent"));

   // On ajoute les actions au menu 'Tarot'
   tarot = menuBar()->addMenu(tr("Tarot"));
   tarot->addAction(optionsAct);
   tarot->addAction(pliPrecAct);

   //---------------
   // Menu Fenêtres
   //---------------
   scoresAct = new QAction(tr("Scores"), this);
   scoresAct->setCheckable(true);
   scoresAct->setStatusTip(tr("Montre/cache la fenêtre des scores"));
   connect(scoresAct, SIGNAL(triggered()), this, SLOT(slotScoresDock()));
   scoresAct->setChecked(true);

   infosAct = new QAction(tr("Informations"), this);
   infosAct->setCheckable(true);
   infosAct->setStatusTip(tr("Montre/cache la fenêtre d'informations"));
   connect(infosAct, SIGNAL(triggered()), this, SLOT(slotInfosDock()));
   infosAct->setChecked(true);

   chatAct = new QAction(tr("Discussion"), this);
   chatAct->setCheckable(true);
   chatAct->setStatusTip(tr("Montre/cache la fenêtre de discussion"));
   connect(chatAct, SIGNAL(triggered()), this, SLOT(slotChatDock()));
   chatAct->setChecked(false);

   fenetres = menuBar()->addMenu(tr("Fenêtres"));
   fenetres->addAction(scoresAct);
   fenetres->addAction(infosAct);
   fenetres->addAction(chatAct);

   //-----------
   // Menu Help
   //-----------
   QAction *aboutAct = new QAction(tr("&À propos..."), this);
   aboutAct->setShortcut(tr("Ctrl+A"));
   aboutAct->setStatusTip(tr("À propos de Tarot Club"));
   connect(aboutAct, SIGNAL(triggered()), about, SLOT(show()));

   // On ajoute les actions au menu '?'
   help = menuBar()->addMenu(tr("?"));
   help->addAction(aboutAct);

   //==============================================================
   //       FENETRE PRINCIPALE
   //==============================================================
   setWindowTitle(QString(TAROT_TITRE) + " " + QString(TAROT_VERSION));
   tapis = new Tapis( this );
   setCentralWidget( tapis );
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


//=============================================================================
// End of file MainWindow.cpp
//=============================================================================
