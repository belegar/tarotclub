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
#include "ui_NumberedDealUI.h"

#define SOUTH_CARDS_POS     522

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
   connect( tapis, SIGNAL(sgnlPresenterPoignee()), this, SLOT(slotPresenterPoignee()) );

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

   // Game Menu
   connect(newQuickGameAct, SIGNAL(triggered()), this, SLOT(slotNewQuickGame()));
   connect(newTournamentAct, SIGNAL(triggered()), this, SLOT(slotNewTournamentGame()));
   connect(newNumberedDealAct, SIGNAL(triggered()), this, SLOT(slotNewNumberedDeal()));
   connect(newCustomDealAct, SIGNAL(triggered()), this, SLOT(slotNewCustomDeal()));
//   connect(netGameServerAct, SIGNAL(triggered()), this, SLOT(slotServerWndShow()));
//   connect(netGameClientAct, SIGNAL(triggered()), this, SLOT(slotClientWndShow()));
//   connect(pliPrecAct, SIGNAL(triggered()), this, SLOT(slotAffichePliPrecedent()));

   // Parameters Menu
   connect(optionsAct, SIGNAL(triggered()), this, SLOT(slotShowOptions()));

   // Exit catching to terminate the game properly
   connect(qApp, SIGNAL(aboutToQuit()), this, SLOT(slotQuitTarotClub()));

   // Lancement du thread serveur
   server.start();
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
   server.wait(); // block until thread has finished execution
}
/*****************************************************************************/
void Game::slotNewTournamentGame()
{
   gameType = LOC_TOURNAMENT;
   rounds = 0;
   server.setDealType(RANDOM_DEAL);
   newLocalGame();
}
/*****************************************************************************/
void Game::slotNewNumberedDeal()
{
   QDialog *widget = new QDialog;
   Ui::NumberedDeal ui;
   ui.setupUi(widget);

   if( widget->exec() == QDialog::Accepted ) {
      gameType = LOC_ONEDEAL;
      server.setDealType(NUMBERED_DEAL);
      server.setDealNumber(ui.dealNumber->value());
      newLocalGame();
   }
}
/*****************************************************************************/
void Game::slotNewCustomDeal()
{
   QString fileName = QFileDialog::getOpenFileName(this);

   gameType = LOC_ONEDEAL;
   server.setDealType(CUSTOM_DEAL);
   server.setDealFile(fileName);
   newLocalGame();
}
/*****************************************************************************/
void Game::slotNewQuickGame()
{
   gameType = LOC_ONEDEAL;
   server.setDealType(RANDOM_DEAL);
   newLocalGame();
}
/*****************************************************************************/
void Game::newLocalGame()
{
   int i;
   GameOptions *options = config->getGameOptions();

   scoresDock->clear();
   infosDock->clear();
   tapis->razTapis();
   tapis->resetCards();

   if (server.getNumberOfConnectedPlayers() == options->nbPlayers) {
      client.sendStart();
   } else {
      server.newServerGame(options->port);
      client.connectToHost( "127.0.0.1", options->port );
      qApp->processEvents(QEventLoop::AllEvents,100);
      for( i=0; i<(options->nbPlayers-1); i++ ) {
         bots[i].connectToHost( "127.0.0.1", options->port );
         qApp->processEvents(QEventLoop::AllEvents,100);
      }
   }
   tapis->setNbPlayers(options->nbPlayers);
   tapis->setFilter(AUCUN);
   sequence = DISTRIBUTION;
   statusBar()->showMessage( trUtf8("Cliquez sur le tapis pour démarrer le tour.") );
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

   server.setTimerBetweenPlayers(options->timer1);
   server.setTimerBetweenTurns(options->timer2);
}
/*****************************************************************************/
void Game::slotShowOptions()
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
   } else if( sequence == BUILD_POIGNEE ) {
      if(c->getType() == ATOUT)
         tapis->setCursorType( ARROW );
      else
         tapis->setCursorType( FORBIDDEN );
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
      if( gc->getStatus() == CARD_NORMAL ) {
         if( client.getTailleChien() == 6 ) {
            return;
         }
         client.addCardChien(c);
         if( client.getTailleChien() == 6 ) {
            tapis->setAccepterChienVisible(true);
         }
      // désélection de la carte
      } else if( gc->getStatus() == CARD_SELECTED ) {
         if( client.getTailleChien() == 0 ) {
            return;
         }
         client.removeCardChien( c );
         tapis->setAccepterChienVisible(false);
      }
      gc->toggleStatus();
   } else if( sequence == BUILD_POIGNEE ) {
      if(c->getType() == ATOUT) {
         // sélection de la carte
         if( gc->getStatus() == CARD_NORMAL ) {
            client.addCardPoignee(c);
            if( client.getTaillePoignee() == 10 ||
                client.getTaillePoignee() == 13 ||
                client.getTaillePoignee() == 15
                ) {
               tapis->setAccepterChienVisible(true);
            }
         // désélection de la carte
         } else if( gc->getStatus() == CARD_SELECTED ) {

            client.removeCardChien( c );
            if( !(client.getTaillePoignee() == 10 ||
                client.getTaillePoignee() == 13 ||
                client.getTaillePoignee() == 15)
                ) {
               tapis->setAccepterChienVisible(false);
            }
         }
         gc->toggleStatus();
      }
   }
}
/*****************************************************************************/
void Game::slotReceptionCartes()
{
   client.updateStats();
   tapis->resetCards();
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
void Game::slotPresenterPoignee()
{
   /*
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
   */
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
   y = SOUTH_CARDS_POS;

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
   x = 260;
   y = 160;

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
                   trUtf8("Tous les joueurs ont passé.\n"
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
void Game::slotDepartDonne(Place p, Contrat c)
{
   firstTurn = true;
   infosDock->setContrat(c);
   infosDock->setPreneur( config->getGameOptions()->identities[p].name );
   infosDock->setDonne(server.getDealNumber());
   sequence = GAME;
   tapis->setFilter( AUCUN );
   tapis->razTapis();
   tapis->colorisePreneur(p);
}
/*****************************************************************************/
void Game::slotJoueCarte()
{
   tapis->setFilter( JEU );

   // If we're about to play the first card the Player can declare a Poignée
   if (firstTurn == true) {
      firstTurn = false;
      // TODO: test if a Poignée exists in the player's deck
      if(client.getStats()->atouts >=10 ) {
         int ret = QMessageBox::question(this, trUtf8("Poignée"),
                                        trUtf8("Vous possédez une poignée.\n"
                                           "Voulez-vous la déclarer ?"),
                                        QMessageBox::Yes | QMessageBox::No);
         if (ret == QMessageBox::YesRole) {
            sequence = BUILD_POIGNEE;
            statusBar()->showMessage(trUtf8("Constituez votre poignée.") );
         }
      }
   } else {
      statusBar()->showMessage(trUtf8("À votre tour de jouer une carte.") );
   }
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
   rounds++;
   resultWindow->setCalcul( client.getScoreInfos(), client.getGameInfos() );
   resultWindow->exec();

   if( (gameType == LOC_TOURNAMENT) &&
       (rounds<MAX_ROUNDS) ) {
      // Add current turn result to total score
      scoresDock->setNewScore(server.getScore()->getLastTurnScore(), config->getGameOptions()->nbPlayers);
      client.sendStart();
   } else {
      sequence = VIDE;
      tapis->setFilter( AUCUN );
      tapis->razTapis();
      tapis->resetCards();
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
