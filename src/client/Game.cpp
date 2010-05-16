/*=============================================================================
 * TarotClub - Game.cpp
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

#include <QStatusBar>
#include <QInputDialog>
#include <QMessageBox>
#include "Game.h"
#include "../Jeu.h"

#define CARD_NORMAL	522
#define CARD_SELECTED	502

/*****************************************************************************/
Game::Game( ConfigFile *conf ) : MainWindow()
{
   config = conf;

   Jeu::init();
   if( tapis->loadCards(config->getGameOptions()) ) {
      exit(-1);
   }
   applyOptions();

   // Board clic events
   connect( tapis, SIGNAL(sgnlViewportClicked()), this, SLOT(slotClickTapis()) );
   connect( tapis, SIGNAL(sgnlClickCard(GfxCard *)), this, SLOT(slotClickCard(GfxCard *)) );
   connect( tapis, SIGNAL(sgnlMoveCursor(GfxCard *)), this, SLOT(slotMoveCursor(GfxCard *)) );
   connect( tapis, SIGNAL(sgnlContrat(Contrat)), this, SLOT(slotSetEnchere(Contrat)) );
   connect( tapis, SIGNAL(sgnlAccepteChien()), this, SLOT(slotAccepteChien()) );

/*
   // Network window (client)
   connect( clientWindow, SIGNAL(sgnlExit()), this, SLOT(slotClientWndExit()));
   connect( clientWindow, SIGNAL(sgnlSendMessage(const QString &)), this, SLOT(slotClientMessage(const QString &)));
   connect( clientWindow, SIGNAL(sgnlConnection(const QString &)), this, SLOT(slotClientConnexion(const QString &)));
   connect( clientWindow, SIGNAL(sgnlDeconnection()), this, SLOT(slotClientDeconnexion()));
*/
   // Network window (server)
   connect( &server, SIGNAL(sigPrintMessage(const QString &)), serverUI.textBrowser, SLOT(append(const QString &)) );

   // Client events connection
   connect( &client, SIGNAL(sgnlMessage(const QString &)), chatDock, SLOT(message(const QString &)));
   connect( &client, SIGNAL(sgnlReceptionCartes()), this, SLOT(slotReceptionCartes()));
   connect( &client, SIGNAL(sgnlAfficheSelection(Place)), this, SLOT(slotAfficheSelection(Place)));
   connect( &client, SIGNAL(sgnlChoixEnchere(Contrat)), tapis, SLOT(slotAfficheBoutons(Contrat)));
   connect( &client, SIGNAL(sgnlAfficheEnchere(Place,Contrat)), tapis, SLOT(slotAfficheEnchere(Place,Contrat)));
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

   connect(qApp, SIGNAL(aboutToQuit()), this, SLOT(slotQuitTarotClub()));

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
/**
 * This method allows a proper cleanup before closing the application
 */
void Game::slotQuitTarotClub()
{
   server.closeServerGame();
   server.exit();
}
/*****************************************************************************/
void Game::slotNewLocalGame()
{
   int i;
   GameOptions *options = config->getGameOptions();

   // Lancement du thread serveur
   if (server.isRunning() == false) {
      server.start();
   }
   server.newServerGame(options->port);

   newDonneAct->setEnabled(true);
   newDonneManuAct->setEnabled(true);
   scoresDock->clear();
   infosDock->clear();
   client.connectToHost( "127.0.0.1", options->port );
   qApp->processEvents(QEventLoop::AllEvents,100);
   for( i=0; i<(options->nbPlayers-1); i++ ) {
      bots[i].connectToHost( "127.0.0.1", options->port );
      qApp->processEvents(QEventLoop::AllEvents,100);
   }
   tapis->setNbPlayers(options->nbPlayers);
   tapis->setFilter(AUCUN);
   sequence = DISTRIBUTION;
   statusBar()->showMessage( "Cliquez sur le tapis pour démarrer le tour." );
}
/*****************************************************************************/
void Game::applyOptions()
{
   int i;
   GameOptions *options = config->getGameOptions();

   scoresDock->setOptions( options );

   for(i=0; i<4; i++ ) {
      bots[i].setIdentity( &options->identities[1+i] );
   }
   client.setIdentity( &options->identities[0] );

   tapis->showAvatars( options->showAvatars, options->nbPlayers );
   tapis->printNames(options->identities, SUD);
   tapis->setBackground(options->tapis);
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
      hideChien();
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
      gc = tapis->getGfxCard(c->getId());
      gc->hide();
   }
}
/*****************************************************************************/
/**
 * Cette fonction est appelée à chaque fois que l'utilisateur déplace son
 * curseur au dessus d'une carte
 */
void Game::slotMoveCursor( GfxCard *gc )
{
   Card *c = tapis->getObjectCard(gc);

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
   Card *c = tapis->getObjectCard(gc);

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

      // sélection de la carte
      if( gc->y() == CARD_NORMAL ) {
         if( client.getTailleChien() == 6 ) {
            return;
         }

         gc->setPos( gc->x(), CARD_SELECTED );
         client.addCardChien(c);

         if( client.getTailleChien() == 6 ) {
            tapis->setAccepterChienVisible(true);
         }

      // désélection de la carte
      } else if( gc->y() == CARD_SELECTED ) {
         if( client.getTailleChien() == 0 ) {
            return;
         }
         gc->setPos( gc->x(), CARD_NORMAL );
         client.removeCardChien( c );
         tapis->setAccepterChienVisible(false);
      }
   }
}
/*****************************************************************************/
void Game::slotReceptionCartes()
{
   int i;

   for( i=0; i<78; i++ ) {
      tapis->getGfxCard(i)->hide();
   }
   afficheCartesJoueur(0);
}
/*****************************************************************************/
void Game::slotAfficheSelection(Place p)
{
   tour = p;
   tapis->afficheSelection(p);
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
      gc = tapis->getGfxCard(c->getId());
      gc->hide();
   }
   tapis->setAccepterChienVisible(false);
   sequence = VIDE;
   tapis->setFilter( AUCUN );
   afficheCartesJoueur(0);
   client.sendChien();
}
/*****************************************************************************/
void Game::slotSetEnchere( Contrat cont )
{
   client.sendEnchere( cont );
   tapis->cacheBoutons();
}
/*****************************************************************************/
/**
 * Pos : décale ou non l'affichage des cartes
 * 0 : normal pour 18 cartes
 * 1 : serrées car il y a le chien en plus
 */
void Game::afficheCartesJoueur( int pos )
{
   qreal i, x, y, pas;
   Card *c;
   GfxCard *cgfx;

   client.tidyDeck();

   if( pos == 1 ) {
      x = 20.0;
      pas = 35.0;
   } else {
      x = 50.0;
      pas = 40.0;
   }
   y = CARD_NORMAL;

   for( i=0; i<client.getCardNumber(); i++ ) {
      c = client.getCard(i);
      cgfx = tapis->getGfxCard(c->getId());
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
      cgfx = tapis->getGfxCard(c->getId());
      cgfx->setPos(x, y);
      cgfx->setZValue(i);
      cgfx->show();
      x = x + 40;
   }
   sequence = MONTRE_CHIEN;
   statusBar()->showMessage(trUtf8("Cliquez sur le tapis une fois que vous avez vu le Chien.") );
}
/*****************************************************************************/
void Game::hideChien()
{
   int i;
   Card *c;
   GfxCard *cgfx;

   for( i=0; i<client.getTailleChien(); i++ ) {
      c = client.getCardChien(i);
      cgfx = tapis->getGfxCard(c->getId());
      cgfx->hide();
   }
}
/*****************************************************************************/
void Game::slotRedist()
{
   QMessageBox::information(this, trUtf8("Information"),
                   trUtf8("Tous les joueurs ont passï¿œ.\n"
                      "Nouvelle distribution des cartes.") );

   sequence = GAME;
   infosDock->clear();
   tapis->setFilter(AUCUN);
   tapis->razTapis();
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
   statusBar()->showMessage(trUtf8("Sï¿œlectionnez des cartes pour construire votre chien.") );
}
/*****************************************************************************/
void Game::slotDepartDonne(Place p,Contrat c)
{
   infosDock->setContrat(c);
   infosDock->setPreneur( config->getGameOptions()->identities[p].name );
   sequence = GAME;
   tapis->setFilter( AUCUN );
   tapis->razTapis();
}
/*****************************************************************************/
void Game::slotJoueCarte()
{
   tapis->setFilter( JEU );
   statusBar()->showMessage(trUtf8("A votre tour de jouer une carte.") );
}
/*****************************************************************************/
void Game::slotAfficheCarte(int id)
{
   GfxCard *gc = tapis->getGfxCard(id);
   tapis->afficheCarte(gc,tour);
}
/*****************************************************************************/
void Game::slotFinDonne()
{
   resultWindow->setCalcul( client.getScoreInfos(), client.getGameInfos() );
   if( resultWindow->exec() == QDialog::Accepted ) {
      // Add current turn result to total score
      scoresDock->setNewScore(server.getScore()->getLastTurnScore(), config->getGameOptions()->nbPlayers);
      client.sendStart();
   }
}
/*****************************************************************************/
void Game::slotWaitPli()
{
   sequence = SEQ_WAIT_PLI;
   tapis->setFilter( AUCUN );
   statusBar()->showMessage(trUtf8("Cliquez sur le tapis pour continuer.") );
}



//=============================================================================
// End of file Game.cpp
//=============================================================================
