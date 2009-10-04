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
   //       ELEMENTS DU CANVAS
   //==============================================================
   canvas = new QGraphicsScene( this );   
   tapis = new Tapis( canvas, this );

   btNord = new PlayerBox(440, 10, QBrush(Qt::yellow), canvas);
   btOuest = new PlayerBox(210, 113, QBrush(Qt::yellow), canvas);
   btSud = new PlayerBox(440, 462, QBrush(Qt::yellow), canvas);
   btEst = new PlayerBox(677, 113, QBrush(Qt::yellow), canvas);
   btNordOuest = new PlayerBox(677, 236, QBrush(Qt::yellow), canvas);

   btNord->show();
   btOuest->show();
   btSud->show();
   btEst->show();

   enchNord = new TextBox(440, 50, QBrush(Qt::red), canvas);
   enchOuest = new TextBox(210, 153, QBrush(Qt::red), canvas);
   enchSud = new TextBox(440, 422, QBrush(Qt::red), canvas);
   enchEst = new TextBox(677, 153, QBrush(Qt::red), canvas);
   enchNordOuest = new TextBox(555, 236, QBrush(Qt::red), canvas);

   //==============================================================
   //       BOUTONS ENCHERES
   //==============================================================
   groupBoutons = new QGroupBox( tr("Enchères"), tapis );
   groupBoutons->setGeometry( 20, 250, 120, 230 );
   groupBoutons->hide();

   boutonPasse = new QPushButton(STR_PASSE, groupBoutons);
   boutonPasse->move(10,20);
   boutonPrise = new QPushButton(STR_PRISE, groupBoutons);
   boutonPrise->move(10,55);
   boutonGarde = new QPushButton(STR_GARDE, groupBoutons);
   boutonGarde->move(10,90);
   boutonGardeSans = new QPushButton(STR_GARDE_SANS, groupBoutons);
   boutonGardeSans->move(10,125);
   boutonGardeContre = new QPushButton(STR_GARDE_CONTRE, groupBoutons);
   boutonGardeContre->move(10,160);
   chelem = new QCheckBox( "Chelem", groupBoutons );
   chelem->move(10,195);
   //==============================================================

   boutonAccepterChien = new QPushButton(tr("Accepter le chien"), tapis);
   boutonAccepterChien->move(800, 462);
   boutonAccepterChien->setMinimumSize( boutonAccepterChien->sizeHint() );
   boutonAccepterChien->hide();
   
   //==============================================================
   //       FENETRE PRINCIPALE
   //==============================================================
   setWindowTitle(QString(TAROT_TITRE) + " " + QString(TAROT_VERSION));
   setCentralWidget( tapis ) ;

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
/**
 * Change la place absolue (vue du serveur, qui est toujours SUD) en relatif
 * au joueur
 */
Place MainWindow::retournePlace( Place origine, Place place_absolue )
{
   Place pl = SUD;

   if( origine == OUEST) {
      Place tab[] = {EST, NORD, OUEST, SUD};
      pl = tab[place_absolue];
   } else if( origine == NORD) {
      Place tab[] = {NORD, OUEST, SUD, EST};
      pl = tab[place_absolue];
   } else if( origine == EST) {
      Place tab[] = {OUEST, SUD, EST, NORD};
      pl = tab[place_absolue];
   } else {
      Place tab[] = {SUD, EST, NORD, OUEST};
      pl = tab[place_absolue];
   }

   return (pl);
}
/*****************************************************************************/
/**
 * Affiche les noms sur le tapis selon la position relative du joueur
 */
void MainWindow::printNames( Identity *identities, Place place )
{
   Place serveur; // place relative du serveur

   serveur = retournePlace( place, SUD );

   btSud->setText( identities[retournePlace( serveur, SUD )].name );
   btEst->setText( identities[retournePlace( serveur,EST )].name  );
   btNord->setText( identities[retournePlace( serveur,NORD )].name  );
   btOuest->setText( identities[retournePlace( serveur,OUEST )].name  );

   btSud->setAvatar( identities[retournePlace( serveur,SUD )].avatar );
   btEst->setAvatar( identities[retournePlace( serveur,EST )].avatar );
   btNord->setAvatar( identities[retournePlace( serveur,NORD )].avatar );
   btOuest->setAvatar( identities[retournePlace( serveur,OUEST )].avatar );

}
/*****************************************************************************/
void MainWindow::colorisePreneur( Place preneur )
{
   if( preneur == SUD ) {
      btSud->setCouleur( QBrush(Qt::green) );
   } else if( preneur == EST ) {
      btEst->setCouleur( QBrush(Qt::green) );
   } else if( preneur == NORD ) {
      btNord->setCouleur( QBrush(Qt::green) );
   } else { // OUEST
      btOuest->setCouleur( QBrush(Qt::green) );
   }
}
/*****************************************************************************/
/**
 * Affiche une carte "c" à l'emplacement "p" de la table.
 * p = NORD, OUEST, SUD, EST
 */
void MainWindow::afficheCarte( GfxCard *c, Place p )
{
   int x, y;

   if( p == NORD ) {
      x = btNord->x()+5;
      y = btNord->y()+40;
   } else if( p == OUEST ) {
      x = btOuest->x()+5;
      y = btOuest->y()+40;
   } else if( p == SUD ) {
      x = btSud->x()+5;
      y = btSud->y()-206;
   } else if( p == EST ) {
      x = btEst->x()+5;
      y = btEst->y()+40;
   } else {// Nord Ouest
      x = btNordOuest->x()+5;
      y = btNordOuest->y()+40;
   }

   c->setPos(x,y);
   c->show();
}
/*****************************************************************************/
void MainWindow::afficheSelection( Place p )
{
   if( p == NORD ) {
      btNord->setPen( 1 );
      btOuest->setPen( 0 );
      btEst->setPen( 0 );
      btSud->setPen( 0 );
      btNordOuest->setPen( 0 );
   } else if( p == OUEST ) {
      btNord->setPen( 0 );
      btOuest->setPen( 1 );
      btEst->setPen( 0 );
      btSud->setPen( 0 );
      btNordOuest->setPen( 0 );
   } else if( p == SUD ) {
      btNord->setPen( 0 );
      btOuest->setPen( 0 );
      btEst->setPen( 0 );
      btSud->setPen( 1 );
      btNordOuest->setPen( 0 );
   } else if( p == EST ) {
      btNord->setPen( 0 );
      btOuest->setPen( 0 );
      btEst->setPen( 1 );
      btSud->setPen( 0 );
      btNordOuest->setPen( 0 );
   } else {// Nord ouest
      btNord->setPen( 0 );
      btOuest->setPen( 0 );
      btEst->setPen( 0 );
      btSud->setPen( 0 );
      btNordOuest->setPen( 1 );
   }
}
/*****************************************************************************/
void MainWindow::afficheEnchere( Place enchereur, Contrat cont )
{
   QString mot;

   if( cont == GARDE_CONTRE ) {
      mot = STR_GARDE_CONTRE;
   } else if( cont == GARDE_SANS ) {
      mot = STR_GARDE_SANS;
   } else if( cont == GARDE ) {
      mot = STR_GARDE;
   } else if( cont == PRISE ) {
      mot = STR_PRISE;
   } else {
      mot = STR_PASSE;
   }

   if( enchereur == NORD ) {
      enchNord->setText(mot);
      enchNord->show();
   } else if( enchereur == OUEST ) {
      enchOuest->setText(mot);
      enchOuest->show();
   } else if( enchereur == SUD ) {
      enchSud->setText(mot);
      enchSud->show();
   } else { // EST
      enchEst->setText(mot);
      enchEst->show();
   }
}
/*****************************************************************************/
void MainWindow::cacheEncheres()
{
   enchNord->hide();
   enchOuest->hide();
   enchSud->hide();
   enchEst->hide();
}
/*****************************************************************************/
void MainWindow::afficheBoutons( Contrat contrat )
{
   boutonPrise->show();
   boutonGarde->show();
   boutonGardeSans->show();
   boutonGardeContre->show();

   if( contrat >= PRISE ) {
      boutonPrise->hide();
   }
   if( contrat >= GARDE ) {
      boutonGarde->hide();
   }
   if( contrat >= GARDE_SANS ) {
      boutonGardeSans->hide();
   }
   if( contrat >= GARDE_CONTRE ) {
      boutonGardeContre->hide();
   }
   groupBoutons->show();
}
/*****************************************************************************/
void MainWindow::showAvatars( bool b, int nb_players )
{
   btSud->enableAvatar(b);
   btEst->enableAvatar(b);
   btOuest->enableAvatar(b);

   if( nb_players > 3 ) {
      btNord->enableAvatar(b);
   }
   if( nb_players > 4 ) {
      btNordOuest->enableAvatar(b);
   }
}
/*****************************************************************************/
void MainWindow::razTapis()
{
   cacheEncheres();
   groupBoutons->hide();

   btSud->setCouleur( QBrush(Qt::yellow) );
   btEst->setCouleur( QBrush(Qt::yellow) );
   btNord->setCouleur( QBrush(Qt::yellow) );
   btOuest->setCouleur( QBrush(Qt::yellow) );

   btNord->setPen( 0 );
   btOuest->setPen( 0 );
   btEst->setPen( 0 );
   btSud->setPen( 0 );
   btNordOuest->setPen( 0 );
}

//=============================================================================
// End of file MainWindow.cpp
//=============================================================================
