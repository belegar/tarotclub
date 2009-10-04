/*=============================================================================
 * Tarot Club - Game.cpp
 *=============================================================================
 * Derived class from MainWindow class. Central class for the game engine.
 *=============================================================================
 * Tarot Club est un jeu de Tarot fran�ais
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

#include <QStatusBar>
#include <QInputDialog>
#include <QMessageBox>
#include "Game.h"
#include "base/Jeu.h"

#define CARD_NORMAL		522
#define CARD_SELECTED	502

/*****************************************************************************/
Game::Game( ConfigFile *conf ) : MainWindow()
{
   config = conf;

   Jeu::init();
   optionsWindow->setPath( config->getPath() );
   if( setTheme( config->getPath() ) ) {
      exit(-1);
   }
   applyOptions();

   connect( tapis, SIGNAL(sgnlViewportClicked()), this, SLOT(slotClickTapis()) );
   connect( tapis, SIGNAL(sgnlClickCard(GfxCard *)), this, SLOT(slotClickCard(GfxCard *)) );
   connect( tapis, SIGNAL(sgnlMoveCursor(GfxCard *)), this, SLOT(slotMoveCursor(GfxCard *)) );

   connect( boutonPasse, SIGNAL(clicked()), this, SLOT(slotBoutton1()) );
   connect( boutonPrise, SIGNAL(clicked()), this, SLOT(slotBoutton2()) );
   connect( boutonGarde, SIGNAL(clicked()), this, SLOT(slotBoutton3()) );
   connect( boutonGardeSans, SIGNAL(clicked()), this, SLOT(slotBoutton4()) );
   connect( boutonGardeContre, SIGNAL(clicked()), this, SLOT(slotBoutton5()) );
   connect( boutonAccepterChien, SIGNAL(clicked()), this, SLOT(slotAccepteChien()) );

/*
   // Fenêtre réseau (client)
   connect( clientWindow, SIGNAL(sgnlExit()), this, SLOT(slotClientWndExit()));
   connect( clientWindow, SIGNAL(sgnlSendMessage(const QString &)), this, SLOT(slotClientMessage(const QString &)));
   connect( clientWindow, SIGNAL(sgnlConnection(const QString &)), this, SLOT(slotClientConnexion(const QString &)));
   connect( clientWindow, SIGNAL(sgnlDeconnection()), this, SLOT(slotClientDeconnexion()));
*/
   // Connexion des événements sur le client
   connect( &client, SIGNAL(sgnlMessage(const QString &)), chatDock, SLOT(message(const QString &)));
   connect( &client, SIGNAL(sgnlReceptionCartes()), this, SLOT(slotReceptionCartes()));
   connect( &client, SIGNAL(sgnlAfficheSelection(Place)), this, SLOT(slotAfficheSelection(Place)));
   connect( &client, SIGNAL(sgnlChoixEnchere(Contrat)), this, SLOT(slotChoixEnchere(Contrat)));
   connect( &client, SIGNAL(sgnlAfficheEnchere(Place,Contrat)), this, SLOT(slotAfficheEnchere(Place,Contrat)));
   connect( &client, SIGNAL(sgnlAfficheChien()), this, SLOT(slotAfficheChien()));
   connect( &client, SIGNAL(sgnlRedist()), this, SLOT(slotRedist()));
   connect( &client, SIGNAL(sgnlPrepareChien()), this, SLOT(slotPrepareChien()));
   connect( &client, SIGNAL(sgnlDepartDonne(Place,Contrat)), this, SLOT(slotDepartDonne(Place,Contrat)));
   connect( &client, SIGNAL(sgnlJoueCarte()), this, SLOT(slotJoueCarte()));
   connect( &client, SIGNAL(sgnlAfficheCarte(int)), this, SLOT(slotAfficheCarte(int)));
   connect( &client, SIGNAL(sgnlFinDonne()), this, SLOT(slotFinDonne()));
   connect( &client, SIGNAL(sgnlWaitPli()), this, SLOT(slotWaitPli()));

   // Menu Fichier
   connect(newLocalGameAct, SIGNAL(triggered()), this, SLOT(slotNewLocalGame()));
//   connect(netGameServerAct, SIGNAL(triggered()), this, SLOT(slotServerWndShow()));
//   connect(netGameClientAct, SIGNAL(triggered()), this, SLOT(slotClientWndShow()));
//   connect(newDonneAct, SIGNAL(triggered()), this, SLOT(slotDonneAuto()));
//   connect(newDonneManuAct, SIGNAL(triggered()), this, SLOT(slotDonneManu()));

   // Menu Tarot
   connect(optionsAct, SIGNAL(triggered()), this, SLOT(showOptions()));
//   connect(pliPrecAct, SIGNAL(triggered()), this, SLOT(slotAffichePliPrecedent()));

   newDonneAct->setEnabled(false);
   newDonneManuAct->setEnabled(false);
   pliPrecAct->setEnabled(false);
}
/*****************************************************************************/
Game::~Game()
{

}
/*****************************************************************************/
void Game::slotNewLocalGame()
{
   int i;

   GameOptions *options = config->getGameOptions();

   newDonneAct->setEnabled(true);
   newDonneManuAct->setEnabled(true);

   btOuest->show();
   btSud->show();
   btEst->show();
   if( options->nbPlayers == 4 ) {
      btNord->show();
   }
   if( options->nbPlayers == 5 ) {
      btNordOuest->show();
   }
   
   scoresDock->clear();
   infosDock->clear();
   client.connectToHost( "127.0.0.1", options->port ); 
   qApp->processEvents(QEventLoop::AllEvents,300);
   for( i=0; i<(options->nbPlayers-1); i++ ) {
      bots[i].connectToHost( "127.0.0.1", options->port ); 
      qApp->processEvents(QEventLoop::AllEvents,300);
   }
   tapis->setFilter(AUCUN);
   sequence = DISTRIBUTION;
   statusBar()->showMessage( "Cliquez sur le tapis pour démarrer le tour." );
}
/*****************************************************************************/
int Game::setTheme( QString gamePath )
{
   int i,j,n;
   QString varImg;
   QString image;
   QFile f;

   //----- 4 couleurs
   for( i=0; i<4; i++ ){
      if( i==0 ) {
         varImg = "pic";
      } else if( i==1 ) {
         varImg = "coeur";
      } else if( i==2 ) {
         varImg = "trefle";
      } else {
         varImg = "carreau";
      }

      // de l'as au roi (14 cartes)
      for( j=0; j<14; j++ ) {
         n = i*14+j;
         image = gamePath + QString("/data/cartes/Classique/") + varImg + QString("-") + QString().setNum(j+1) + QString(".png");
         f.setFileName(image);

         if( f.exists() ) {
            // On assigne une image � la carte
            cardsPics[n].setPixmap( QPixmap( image ));
         } else {
            return(1);
         }
         cardsPics[n].hide();
         canvas->addItem( &cardsPics[n] );
      }
   }

   //----- 21 atouts
   for( i=56; i<77; i++) {
      image = gamePath + QString("/data/cartes/Classique/atout-") + QString().setNum(i-55) + QString(".png");
      f.setFileName(image);
      if( f.exists() ) {
         cardsPics[i].setPixmap( QPixmap( image ));
      } else {
         return(2);
      }
      cardsPics[i].hide();
      canvas->addItem( &cardsPics[i] );
   }

   //----- L'excuse
   image = gamePath + QString("/data/cartes/Classique/excuse.png");
   f.setFileName(image);
   if( f.exists() ) {
      cardsPics[77].setPixmap( QPixmap( image ));
   } else {
      return(3);
   }
   cardsPics[77].hide();
   canvas->addItem( &cardsPics[77] );

   return 0;
}
/*****************************************************************************/
void Game::applyOptions()
{
   int i;

   GameOptions *options = config->getGameOptions();
   QBrush background;

   showAvatars( options->showAvatars, options->nbPlayers );
   scoresDock->setOptions( options );

   for(i=0; i<4; i++ ) {
      bots[i].setIdentity( &options->identities[1+i] );
   }
   client.setIdentity( &options->identities[0] );
   printNames(options->identities, SUD);

   // On teste si on a un arriere plan
   QPixmap pm;
   QString fichier = config->getPath() + QString("/data/" + options->tapis );
   if( pm.load( fichier ) == false ) {
      pm.fill( Qt::darkGreen );
   }
   background.setTexture( pm );
   canvas->setBackgroundBrush( background );
}
/*****************************************************************************/
void Game::showOptions()
{
   optionsWindow->setOptions( config->getGameOptions() );

   if( optionsWindow->exec() == QDialog::Accepted ) {
      config->setGameOptions( optionsWindow->getOptions() );
      config->save(CONFIG_FILE);
      applyOptions();
   }
}
/*****************************************************************************/
void Game::slotClickTapis()
{
   if( sequence == DISTRIBUTION ) {
      client.sendStart();
      sequence = GAME;
   } else if( sequence == MONTRE_CHIEN ) {
      statusBar()->clearMessage();
      sequence = VIDE;
      client.sendVuChien();
   } else if( sequence == SEQ_WAIT_PLI ) {
      hidePli();
      statusBar()->clearMessage();
      sequence = GAME;
      client.sendVuPli();
   }
}
/*****************************************************************************/
void Game::hidePli()
{
   int i;
   Card *c;
   GfxCard *gc;
   GameInfos *inf = client.getGameInfos();

   for( i = inf->gameCounter-inf->nbJoueurs; i<inf->gameCounter; i++ ) {
      c = client.getCardMainDeck(i);
      gc = &cardsPics[c->getId()];
      gc->hide();
   }
}
/*****************************************************************************/
/**
 * Cette fonction est appel�e � chaque fois que l'utilisateur d�place son
 * curseur au dessus d'une carte
 */
void Game::slotMoveCursor( GfxCard *gc )
{
   Card *c = getCardFromPix(gc);

   if( client.cardExists( c ) == false ) {
      return;
   }

   if( sequence == CHIEN ) {
      if( c->getType() == ATOUT || c->getType() == EXCUSE || 
            (c->getType() == CARTE && c->getValue()==14 )) {
         tapis->setCursorType( FORBIDDEN );
      } else {
         tapis->setCursorType( ARROW );
      }
   } else if( sequence == GAME ) {
      if( client.isValid( c ) == true ) {
         tapis->setCursorType( ARROW );
      } else {
         tapis->setCursorType( FORBIDDEN );
      }
   }
}
/*****************************************************************************/
void Game::slotClickCard(GfxCard *gc)
{
   Card *c = getCardFromPix(gc);
   
   if( client.cardExists( c ) == false ) {
      return;
   }
   
   if( sequence == GAME ) {
      // On teste si la carte est valide
      if( client.isValid( c ) == false ) {
         return;
      }
      tapis->setFilter( AUCUN );
      statusBar()->clearMessage();
      client.removeCard(c);
      afficheCartesJoueur(0);
      client.sendCard(c);

   } else if( sequence == CHIEN ) {

      if( c->getType() == ATOUT || c->getType() == EXCUSE || 
         (c->getType() == CARTE && c->getValue()==14 )) {
         return;
      }

      // s�lection de la carte
      if( gc->y() == CARD_NORMAL ) {
         if( client.getTailleChien() == 6 ) {
            return;
         }

         gc->setPos( gc->x(), CARD_SELECTED );
         client.addCardChien(c);

         if( client.getTailleChien() == 6 ) {
            boutonAccepterChien->show();
         }

      // d�selection de la carte
      } else if( gc->y() == CARD_SELECTED ) {
         if( client.getTailleChien() == 0 ) {
            return;
         }
         gc->setPos( gc->x(), CARD_NORMAL );
         client.removeCardChien( c );
         boutonAccepterChien->hide();
      }
   }
}
/*****************************************************************************/
void Game::slotReceptionCartes()
{
   int i;

   for( i=0; i<78; i++ ) {
      cardsPics[i].hide();
   }
   afficheCartesJoueur(0);
}
/*****************************************************************************/
void Game::slotAfficheSelection(Place p)
{
   tour = p;
   afficheSelection(p);
}
/*****************************************************************************/
void Game::slotChoixEnchere(Contrat c)
{
   afficheBoutons(c);
}
/*****************************************************************************/
void Game::slotAfficheEnchere(Place p, Contrat c)
{
   afficheEnchere(p,c);
}
/*****************************************************************************/
void Game::slotBoutton1()
{
   setEnchere( PASSE );
}
/*****************************************************************************/
void Game::slotBoutton2()
{
   setEnchere( PRISE );
}
/*****************************************************************************/
void Game::slotBoutton3()
{
   setEnchere( GARDE );
}
/*****************************************************************************/
void Game::slotBoutton4()
{
   setEnchere( GARDE_SANS );
}
/*****************************************************************************/
void Game::slotBoutton5()
{
   setEnchere( GARDE_CONTRE );
}
/*****************************************************************************/
void Game::slotAccepteChien()
{
   Card *c;
   GfxCard *gc;
   int i;

   for( i=0; i<client.getTailleChien(); i++) {
      c = client.getCardChien(i);
      client.removeCard(c);
      gc = &cardsPics[c->getId()];
      gc->hide();
   }
   boutonAccepterChien->hide();
   sequence = VIDE;
   tapis->setFilter( AUCUN );
   afficheCartesJoueur(0);
   client.sendChien();
}
/*****************************************************************************/
void Game::setEnchere( Contrat cont )
{
   client.sendEnchere( cont );
   groupBoutons->hide();
}
/*****************************************************************************/
/**
 * Pos : d�cale ou non l'affichage des cartes
 * 0 : normal pour 18 cartes
 * 1 : serr�es car il y a le chien en plus
 */
void Game::afficheCartesJoueur( int pos )
{
   int i, x, y, pas;
   Card *c;
   GfxCard *cgfx;

   client.tidyDeck();

   if( pos == 1 ) {
      x = 20;
      pas = 35;
   } else {
      x = 50;
      pas = 40;
   }
   y = CARD_NORMAL;

   for( i=0; i<client.getCardNumber(); i++ ) {
      c = client.getCard(i);
      cgfx = &cardsPics[c->getId()];
      cgfx->setPos(x, y);
      cgfx->setZValue( i );
      cgfx->show();
      x = x + pas;
   }
}
/*****************************************************************************/
void Game::slotAfficheChien()
{
   int i, n, x, y;
   Card *c;
   GfxCard *cgfx;

   n = client.getTailleChien();
   x = 350;
   y = 150;

   for( i=0; i<n; i++ ) {
      c = client.getCardChien(i);
      cgfx = &cardsPics[c->getId()];
      cgfx->setPos(x, y);
      cgfx->setZValue(i);
      cgfx->show();
      x = x + 40;
   }
   sequence = MONTRE_CHIEN;
   statusBar()->showMessage(tr("Cliquez sur le tapis une fois que vous avez vu le Chien.") );
}
/*****************************************************************************/
void Game::slotRedist()
{
   QMessageBox::information(this, tr("Information"),
                   tr("Tous les joueurs ont pass�.\n"
                      "Nouvelle distribution des cartes.") );

   razTapis();
   sequence = GAME;
   infosDock->clear();
   tapis->setFilter(AUCUN);
   client.sendStart();
}
/*****************************************************************************/
void Game::slotPrepareChien()
{
   Card *c;
   
   // On ajoute le chien au deck du joueur
   for( int i=0; i<client.getTailleChien(); i++ ) {
      c = client.getCardChien( i );
      client.addCard( c );
   }
   client.emptyChien();
   sequence = CHIEN;
   tapis->setFilter( JEU );
   // on affiche le deck du joueur + le contenu du chien
   afficheCartesJoueur(1);
   statusBar()->showMessage(tr("S�lectionnez des cartes pour construire votre chien.") );
}
/*****************************************************************************/
void Game::slotDepartDonne(Place p,Contrat c)
{
   infosDock->setContrat(c);
   infosDock->setPreneur( config->getGameOptions()->identities[p].name );
   razTapis();
   sequence = GAME;
   tapis->setFilter( AUCUN );
}
/*****************************************************************************/
void Game::slotJoueCarte()
{
   tapis->setFilter( JEU );
   statusBar()->showMessage(tr("A votre tour de jouer une carte.") );
}
/*****************************************************************************/
void Game::slotAfficheCarte(int id)
{
   GfxCard *gc = &cardsPics[id];
   afficheCarte(gc,tour);
}
/*****************************************************************************/
void Game::slotFinDonne()
{
   resultWindow->setCalcul( client.getScoreInfos(), client.getGameInfos() );
   if( resultWindow->exec() == QDialog::Accepted ) {
      client.sendStart();
   }
}
/*****************************************************************************/
void Game::slotWaitPli()
{
   sequence = SEQ_WAIT_PLI;
   tapis->setFilter( AUCUN );
   statusBar()->showMessage(tr("Cliquez sur le tapis pour continuer.") );
}
/*****************************************************************************/
Card *Game::getCardFromPix( GfxCard *gc )
{
   Card *c = NULL;
   for( int i=0; i<78; i++ ) {
      if( &cardsPics[i] == gc ) {
         c = Jeu::getCard(i);
         break;
      }
   }
   return(c);
}


//=============================================================================
// End of file Game.cpp
//=============================================================================
