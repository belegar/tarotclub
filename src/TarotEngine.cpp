/*=============================================================================
 * TarotClub - TarotEngine.cpp
 *=============================================================================
 * Moteur de jeu principal + serveur de jeu réseau
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

#include <QtNetwork>
#include <QCoreApplication>
#include "TarotEngine.h"
#include "DealEditorFile.h"
#ifndef QT_NO_DEBUG
   #include <iostream>
   #include <fstream>
using namespace std;
#endif // QT_NO_DEBUG

/*****************************************************************************/
TarotEngine::TarotEngine()
   : QThread()
{
   qsrand(QTime(0,0,0).secsTo(QTime::currentTime())); // seed init

   QDir b = QCoreApplication::applicationDirPath();
   b.cdUp();

   gamePath = b.path();
   infos.gameCounter = 0;
   sequence = VIDE;
   newGame = false;
   dealNumber = 0;
   dealType = RANDOM_DEAL;
   tcpPort = DEFAULTPORT;

   connect(&server, SIGNAL(newConnection()), this, SLOT(slotNewConnection()));
}
/*****************************************************************************/
TarotEngine::~TarotEngine()
{

}
/*****************************************************************************/
void TarotEngine::run()
{
   exec();
}
/*****************************************************************************/
void TarotEngine::setGameType(GameType type)
{
   gameType = type;
}
/*****************************************************************************/
void TarotEngine::setDealNumber(int deal)
{
   dealNumber = deal;
}
/*****************************************************************************/
void TarotEngine::setDealType(DealType type)
{
   dealType = type;
}
/*****************************************************************************/
void TarotEngine::setDealFile(QString file)
{
   dealFile = file;
}
/*****************************************************************************/
void TarotEngine::setOptions(GameOptions *options)
{
   int i;

   tcpPort = options->port;
   for(i=0; i<3; i++ ) {
      bots[i].setIdentity( &options->bots[i] );
      bots[i].setTimeBeforeSend(options->timer);
   }

}
/*****************************************************************************/
void TarotEngine::customEvent( QEvent *e )
{

   if( (int)e->type() == MsgStartGame ) {
      if( newGame == true ) {
         emit sigPrintMessage("Game already started.");
         return;
      }
      newServerGame();
   } else if( (int)e->type() == MsgStopGame ) {
      if( newGame == false ) {
         emit sigPrintMessage("Game not started.");
         return;
      }
      closeServerGame();
   } else if( (int)e->type() == MsgExitGame ) {
      closeServerGame();
     // QCoreApplication::exit(0);
   }

}
/*****************************************************************************/
Place TarotEngine::nextPlayer( Place j )
{
   Place p;

   if( j == SUD ) {
      p = EST;
   } else if( j == EST ) {
      p = NORD;
   } else if( j == NORD ) {
      p = OUEST;
   } else { // OUEST
      p = SUD;
   }
   return(p);
}
/*****************************************************************************/
void TarotEngine::newServerGame()
{
   int i;

   closeClients();
   server.close();

   // 4 joueurs max + une connexion en plus pour avertir aux nouveaux arrivants
   // que le serveur est plein
   server.setMaxPendingConnections(5);
   server.listen( QHostAddress::LocalHost, tcpPort );

   // On initialise toutes les variables locales et on choisit le donneur
   i = qrand()%4;
   donneur = (Place)i;
   newGame = true;
   score.init();
   dealCounter = 0;
   emit sigPrintMessage("Server started.\r\n");
}
/*****************************************************************************/
void TarotEngine::connectBots()
{
   int i;

   qApp->processEvents(QEventLoop::AllEvents,100);
   for( i=0; i<3; i++ ) {
      bots[i].connectToHost( "127.0.0.1", tcpPort );
      qApp->processEvents(QEventLoop::AllEvents,100);
   }
}
/*****************************************************************************/
void TarotEngine::closeServerGame()
{
   newGame = false;
   sequence = VIDE;

   closeClients();
   server.close();
}
/*****************************************************************************/
void TarotEngine::closeClients()
{
   QMapIterator<QTcpSocket*, Player*> i(players);
   while (i.hasNext()) {
     i.next();
     QTcpSocket *cnx = i.key();
     connect(cnx, SIGNAL(disconnected()), cnx, SLOT(deleteLater()));
     cnx->close();
     players.remove(cnx);
   }
}
/*****************************************************************************/
Player *TarotEngine::getPlayer(Place p)
{
   QMapIterator<QTcpSocket*, Player*> i(players);
   while (i.hasNext()) {
     i.next();
     if(i.value()->getPlace() == p) {
        return i.value();
     }
   }
   return NULL;
}
/*****************************************************************************/
QTcpSocket *TarotEngine::getConnection(Place p)
{
   QMapIterator<QTcpSocket*, Player*> i(players);
   while (i.hasNext()) {
     i.next();
     if(i.value()->getPlace() == p) {
        return i.key();
     }
   }
   return NULL;
}
/*****************************************************************************/
int TarotEngine::getConnectedPlayers( Identity *idents )
{
   int j = 0;

   QMapIterator<QTcpSocket*, Player*> i(players);
   while (i.hasNext()) {
     i.next();
     idents[j] = *i.value()->getIdentity();
   }
   return players.size();
}
/*****************************************************************************/
int TarotEngine::getNumberOfConnectedPlayers()
{
   return players.size();
}
/*****************************************************************************/
Score *TarotEngine::getScore()
{
   return &score;
}
/*****************************************************************************/
int TarotEngine::getDealNumber()
{
   return dealNumber;
}
/*****************************************************************************/
/**
 * Teste si une carte du joueur local peut être jouée
 */
bool TarotEngine::cardIsValid( Card *c, Place p )
{
   bool ret = false;

   if( sequence == CHIEN ) {
      if( c->getType() == ATOUT || c->getType() == EXCUSE ||
            (c->getType() == CARTE && c->getValue()==14 )) {
         ret = false;
      } else {
         ret = true;
      }
   } else if( sequence == GAME ) {
      ret =  getPlayer(p)->canPlayCard( &mainDeck, c, infos.gameCounter, NB_PLAYERS );
   }

   return (ret);
}
/*****************************************************************************/
/**
 * Teste si une carte du joueur local existe dans sa main
 */
bool TarotEngine::cardExists( Card *c, Place p )
{
   return (getPlayer(p)->cardExists(c));
}
/*****************************************************************************/
/**
 * Calcule qui a remporté le pli
 */
Place TarotEngine::calculGagnantPli()
{
   Card *c;

   // première carte
   CardType type;
   CardColor coul;
   Place pl = NORD;

   bool flag=false; // vrai si l'entame est l'excuse
   int i, debut=0;
   int leader = 0; // 0 est le premier joueur a joué etc. jusqu'à 5
   // par défaut, celui qui entame est le leader (car couleur demandée)

   // étape 1 : on cherche la couleur demandée

   c = mainDeck.at( infos.gameCounter - NB_PLAYERS ); // première carte jouée à ce tour

   type = c->getType();
   coul = c->getColor();

    // aïe, le joueur a commencé avec une excuse
   if( type == EXCUSE ) {
      c = mainDeck.at( infos.gameCounter - NB_PLAYERS + 1 ); // la couleur demandée est donc la seconde carte
      flag = true;
      debut = 1;
      type = c->getType();
      coul = c->getColor();
      leader = 2;
   }

   debut = debut + infos.gameCounter - NB_PLAYERS + 1 ; // le début est décalé si le premier pli est l'excuse

   int  drapeau; // indique une coupe, donc le leader est forcément un atout
   int  valLeader;
   valLeader = c->getValue();

   if( type == ATOUT ) {
      drapeau = 1;   // on cherche une éventuelle surcoupe
   } else {
      drapeau = 0;   // on cherche une coupe
   }

   for( i = debut; i< infos.gameCounter; i++) {
      c = mainDeck.at( i );

      // coupe !
      if( c->getType() == ATOUT ) {
         if( c->getValue() > valLeader && drapeau == 1 ) { // surcoupe ??
            valLeader = c->getValue();
            leader = i - (infos.gameCounter - NB_PLAYERS);
         } else if( drapeau == 0 ) {
            valLeader = c->getValue();
            leader = i - (infos.gameCounter - NB_PLAYERS);
            drapeau = 1;
         }

      // pas de coupe détectée
      } else if( c->getType() == CARTE && drapeau == 0 ) {
         if( c->getColor() == coul) { // ahah, même couleur !
            if( c->getValue() > valLeader ) {
               valLeader = c->getValue();
               leader = i - (infos.gameCounter - NB_PLAYERS);
            }
         }
      }
   }

   // place du leader

   if( tour == SUD) {
      Place tab[] = {EST, NORD, OUEST, SUD};
      pl = tab[leader];
   } else if( tour == EST) {
      Place tab[] = {NORD, OUEST, SUD, EST};
      pl = tab[leader];
   } else if( tour == NORD) {
      Place tab[] = {OUEST, SUD, EST, NORD};
      pl = tab[leader];
   } else {
      Place tab[] = {SUD, EST, NORD, OUEST};
      pl = tab[leader];
   }

   return pl;
}
/*****************************************************************************/
void TarotEngine::nouvelleDonne()
{
   if(dealType == CUSTOM_DEAL) {
      customDeal();
   } else {
      randomDeal();
   }
   score.reset();
   donneur = nextPlayer(donneur);
   infos.contrat = PASSE;
   infos.gameCounter = 0;
   tour = nextPlayer(donneur); // Le joueur à la droite du donneur commence les enchères
   selectPlayer(tour);
   askBid(infos.contrat);
}
/*****************************************************************************/
#ifndef QT_NO_DEBUG
void TarotEngine::generateLog()
{
   // generate a file with cards of all players
   ofstream f("round_cards.txt");
   QString l1, l2, l3, l4;

   // Card type
   for (int j=0; j<mainDeck.size(); j++) {
      QString n;
      Card *c = mainDeck.at(j);
      l1 += c->getCardName() + "\t";
      l2 += n.setNum(c->getPoints()) + "\t";
      l3 += n.setNum((int)c->getOwner()) + "\t";
      l4 += n.setNum(score.getPli(j)) + "\t";
   }
   f << l1.toStdString() << "\n" << l2.toStdString() << "\n" << l3.toStdString() << l4.toStdString() << "\n";

   l1 = l2 = l3 = l4 = "";

   // Card type
   for (int j=0; j<deckChien.size(); j++) {
      QString n;
      Card *c = deckChien.at(j);
      l1 += c->getCardName() + "\t";
      l2 += n.setNum(c->getPoints()) + "\t";
      l3 += n.setNum((int)c->getOwner()) + "\t";
      l3 += n.setNum(score.getPli(j)) + "\t";
   }
   f << l1.toStdString() << "\n" << l2.toStdString() << "\n" << l3.toStdString() << l4.toStdString() << "\n";

   f.close();
}
#endif // QT_NO_DEBUG
/*****************************************************************************/
/**
 * Automate servant à séquencer le jeu selon l'état courant des variables
 */
void TarotEngine::jeu()
{
   bool ret;
   float points;
   bool lastDeal;

   if( !(infos.gameCounter%NB_PLAYERS) && infos.gameCounter ) {
      ret = finLevee(points);

      cptVu = 0;
      sendWaitPli(points);

      if( ret == false ) {
  #ifndef QT_NO_DEBUG
         generateLog();
  #endif
         dealCounter++;
         // end of deal, send score to all players
         if (dealCounter<MAX_ROUNDS && gameType == LOC_TOURNAMENT )
            lastDeal = false;
         else
            lastDeal = true;

         sendFinDonne( score.getScoreInfos(), lastDeal );

         if(lastDeal == true) {
            // stop the server
            closeServerGame();
         } else {
            sequence = SEQ_WAIT_PLAYER;
         }
      }
   } else {
      tour = nextPlayer(tour);
      jeuNext();
   }
}
/*****************************************************************************/
void TarotEngine::jeuNext()
{
   infos.gameCounter++;
   selectPlayer(tour);
   sendJoueCarte();
}
/*****************************************************************************/
void TarotEngine::sequenceEncheres()
{
   if( tour == donneur ) {
      if( infos.contrat == PASSE ) {
         // si tous les joueurs ont passé on s'arrête et on prévient tout le monde
         sequence = VIDE;
         sendRedist();
      } else {
         montreChien();
      }
   } else {
      tour = nextPlayer(tour);
      selectPlayer(tour);
      askBid(infos.contrat);
   }
}
/*****************************************************************************/
void TarotEngine::montreChien()
{
   int i;
   Card *c;

   // On précise quel joueur possède le chien en fonction du contrat
   for( i=0; i<deckChien.count(); i++) {
      c = deckChien.at( i );
      if( infos.contrat != GARDE_CONTRE ) {
         c->setOwner( infos.preneur );
      } else {
         // En cas de garde contre, le chien appartient à la défense
         c->setOwner( HYPERSPACE );
      }
   }

   if( infos.contrat == GARDE_SANS || infos.contrat == GARDE_CONTRE ) {
      // On n'affiche pas le chien et on commence la partie immédiatement
      sendDepartDonne();
      jeu();
      return;
   }
   sendShowChien(); // Prise ou garde, on affiche le chien chez tout le monde
   cptVu = 0;
   sequence = WAIT_CHIEN; // et on attend le chien écarté par le preneur
}
/*****************************************************************************/
/**
 * Fin d'un tour, on calcule le gagnant du pli et on prépare la suite
 * On retourne également le nombre de points réalisés par le preneur
 */
bool TarotEngine::finLevee(float &points)
{
   int i;
   Card *c;

#ifndef QT_NO_DEBUG
   QFile f("debug.txt");
   QTextStream fout(&f);
   f.open(QIODevice::Append | QIODevice::Text);
   fout << "----- tour avant : " << tour << endl;
#endif

   // On détecte qui a remporté le pli, c'est lui qui entame le prochain tour
   tour = calculGagnantPli();

   points = 0;
   for( i = (infos.gameCounter-NB_PLAYERS); i<infos.gameCounter; i++ ) {
      c = mainDeck.at(i);
      score.setPli( i, tour );
      // calculate points won by the taker, with the special case of the Excuse
      if(tour == infos.preneur) {
          // here, the taker won the round but the Excuse stays to the Defenders
          if( c->getType() != EXCUSE ) {
              points += c->getPoints();
          } else {
              points += 0.5;
          }
      } else {
          // here, the taker lost the round, he can keep his Excuse
          if( c->getType() == EXCUSE && c->getOwner() == tour) {
              points += c->getPoints() + 0.5;
          }
      }

#ifndef QT_NO_DEBUG
    fout << c->getId() << " i=" << i << ", tour=" << tour << " ";
#endif
    }

#ifndef QT_NO_DEBUG
    fout << endl << "-----" << endl;
    f.close();
#endif

   if( infos.gameCounter < 72 ) {
      return true;
   } else { // fin du jeu
      score.calcul( mainDeck, deckChien, infos );
      return false;
   }

}
/*****************************************************************************/
/**
 * Use a custom deal for the game
 */
void TarotEngine::customDeal()
{
   int j;
   Card *c;
   quint8 params[18] = {0};   // cartes du joueur (3 joueurs: 24 cartes, 4j:18, 5j:15)

   DealEditorFile editor;
   editor.loadFile(dealFile);

   QMapIterator<QTcpSocket*, Player*> i(players);
   while (i.hasNext()) {
      i.next();
      i.value()->emptyDeck();
      Place p = i.value()->getPlace();
      Deck *d;

      if (p == SUD) {
         d = &editor.southDeck;
      } else if (p == EST) {
         d = &editor.eastDeck;
      } else if (p == OUEST) {
         d = &editor.westDeck;
      } else {
         d = &editor.northDeck;
      }

      for( j=0; j<18; j++ ) {
         c = d->at(j);
         if( c->getType() == EXCUSE ) {
            score.setExcuse(p);
         }
         c->setOwner(p);
         i.value()->addCard( c );
         params[j] = c->getId();
      }
      sendCards( p, params );
   }

   deckChien = editor.chienDeck;
   mainDeck.clear();
}
/*****************************************************************************/
/**
 * On distribue les cartes entre les joueurs et le chien
 */
void TarotEngine::randomDeal()
{
   int n, j;
   Card *c;
   int index;
   int nb_cartes_chien;
   int nb_cartes_joueur;
   Place p;
   quint8 params[18] = {0};   // cartes du joueur (3 joueurs: 24 cartes, 4j:18, 5j:15)


   // on ajoute les cartes dans le deck
   mainDeck.clear();
   for( int i=0; i<78; i++ ) {
      mainDeck.append( Jeu::getCard(i) );
   }
   if(dealType == RANDOM_DEAL) {
      dealNumber = qrand()%RAND_MAX;
   }
   mainDeck.shuffle(dealNumber);
   deckChien.clear();

   if( NB_PLAYERS == 3 ) {
      nb_cartes_chien = 6;
   } else if( NB_PLAYERS == 4 ) {
      nb_cartes_chien = 6;
   } else { // 5 joueurs
      nb_cartes_chien = 3;
   }
   nb_cartes_joueur = (78-nb_cartes_chien)/NB_PLAYERS;
   n = 0;

   QMapIterator<QTcpSocket*, Player*> i(players);
   while (i.hasNext()) {
     i.next();
     i.value()->emptyDeck();
     p = i.value()->getPlace();

     for( j=0; j<nb_cartes_joueur; j++ ) {
        index = n*nb_cartes_joueur+j;
        c = mainDeck.at(index);
        if( c->getType() == EXCUSE ) {
           score.setExcuse(p);
        }
        c->setOwner(p);
        i.value()->addCard( c );
        params[j] = c->getId();
     }
     sendCards( p, params );
     n++;
   }

   // Les cartes restantes vont au chien
   for( j=78-nb_cartes_chien; j<78; j++) {
      c = mainDeck.at(j);
      deckChien.append( c );
   }
   mainDeck.clear();
}
/*****************************************************************************/
void TarotEngine::slotNewConnection()
{
   QTcpSocket *cnx = server.nextPendingConnection();
   int n = players.size();

   if( n == NB_PLAYERS ) {
      QString message = "Le serveur est complet.";
      QByteArray block;
      QDataStream out( &block, QIODevice::WriteOnly );
      out.setVersion(QT_STREAMVER);
      out << (quint16)0 << (quint8)NET_MESSAGE
          << message
          << (quint16)0xFFFF;
      out.device()->seek(0);
      out << (quint16)( block.size() - sizeof(quint16) );

      connect(cnx, SIGNAL(disconnected()), cnx, SLOT(deleteLater()));
      cnx->write(block);
      cnx->flush();
      cnx->close();
   } else {
      Player *player = new Player();
      Place p;

      if (n == 0) {
         p = SUD;
      } else if (n == 1) {
         p = EST;
      } else if (n == 2) {
         p = NORD;
      } else {
         p = OUEST;
      }
      player->setPlace(p);

      connect( cnx, SIGNAL(disconnected()), this, SLOT(slotClientClosed()));
      connect( cnx, SIGNAL(readyRead()), this, SLOT(slotReadData()));

      players[cnx] = player; // on ajoute ce client à la liste

      // on envoie une demande d'infos personnelles
      QByteArray block;
      QDataStream out( &block, QIODevice::WriteOnly );
      out.setVersion(QT_STREAMVER);
      out << (quint16)0 << (quint8)NET_IDENTIFICATION
          << (quint16)0xFFFF;

      out.device()->seek(0);
      out << (quint16)( block.size() - sizeof(quint16) );

      cnx->write(block);
   }
}
/*****************************************************************************/
void TarotEngine::slotClientClosed()
{
   // retrieve our sender QTcpSocket
   QTcpSocket* cnx = qobject_cast<QTcpSocket *>(sender());
   Player *player = players[cnx];
   if (player == NULL) {
      return;
   }
   QString message = "Le joueur " + player->getName() + " a quitté la partie.";
   connect(cnx, SIGNAL(disconnected()), cnx, SLOT(deleteLater()));
   cnx->close();
   players.remove(cnx);
   sendMessage( message, BROADCAST );
   sendPlayersList();

   // TODO: if a player has quit during a game, replace it by a bot
}
/*****************************************************************************/
/**
 * Décodage de la trame reçue
 */
void TarotEngine::slotReadData()
{
   quint16 blockSize = 0;
   bool trameEnPlus = false;
   unsigned int total=0;
   unsigned int bytes;

   // retrieve our sender QTcpSocket
   QTcpSocket* cnx = qobject_cast<QTcpSocket *>(sender());
   bytes = cnx->bytesAvailable();

   QDataStream in( cnx );
   for( ;; ) {
      if( blockSize == 0 ) {
         if( bytes < sizeof(quint16) ) {
            break;
         }
         in >> blockSize;
      }

      // fin de la trame ?
      if( blockSize == 0xFFFF ) {
         if( trameEnPlus == true ) {
            blockSize = 0;
            trameEnPlus = false;
            continue;
         } else {
            break;
         }
      }

      total += blockSize + 2;

      if( bytes < total ) {
         break;
      } else if( bytes > total ) {
         trameEnPlus = true;
      } else {
         trameEnPlus = false;
      }

      doAction( in, cnx );
      blockSize = 0;
   }
}
/*****************************************************************************/
/**
 * ALL data *must*  be read !! TODO, add robustness here
 */
void TarotEngine::doAction( QDataStream &in, QTcpSocket* cnx )
{
   quint8 type;   // type de trame
   in >> type;

   switch( type ) {

      /**
       * Message en provenance d'un client
       */
      case NET_CLIENT_MSG:
      {
         // TODO: REFUSER SI MAUVAISE SEQUENCE EN COURS

         QString message;
         in >> message;
         sendMessage( message, BROADCAST );
         emit sigPrintMessage(QString("Client message: ") + message);
         break;
      }

      /**
       * Identité d'un client
       */
      case NET_CLIENT_INFOS:
      {
         Identity ident;
         QString version;
         quint8 sex;

         // TODO: REFUSER SI MAUVAISE SEQUENCE EN COURS

         in >> version;
         in >> ident.name;
         in >> ident.avatar;
         in >> ident.quote;
         in >> sex;
         ident.sex = (SexType)sex;

#ifndef QT_NO_DEBUG
        QFile f("avatars.txt");
        QTextStream fout(&f);
        f.open(QIODevice::Append | QIODevice::Text);
        fout << ident.avatar.toLatin1().data() << endl;
        f.close();
#endif

         bool ok = false;
         // On cherche si le nick n'existe pas déjà
         QMapIterator<QTcpSocket*, Player*> i(players);
         while (i.hasNext()) {
            i.next();
            if (i.value()->getName() == ident.name) {
               ok = true;
               break;
            }
         }

         if( ok == true ) {
            // On prévient le joueur que son nick est déjà pris
            QString message = "Votre pseudonyme \"" + ident.name + "\" existe déjà sur le serveur."+
               " Changez le et reconnectez-vous.";

            // et on annule sa connexion
            sendMessage( message, players[cnx]->getPlace() );
            connect(cnx, SIGNAL(disconnected()), cnx, SLOT(deleteLater()));
            cnx->close();
            players.remove(cnx);

         } else {
            ident.avatar = ":/images/avatars/"+ident.avatar;
            players[cnx]->setIdentity( &ident );
            QString m = "Le joueur " + ident.name + " a rejoint la partie.";
            sendMessage( m , BROADCAST );
            emit sigPrintMessage(m);
            sendPlayersList();

            if (getNumberOfConnectedPlayers() == NB_PLAYERS) {
               nouvelleDonne();
            }
         }
         break;
      }

      /**
       * Contrat d'un joueur
       */
      case NET_CLIENT_ENCHERE:
      {
         quint8 c;
         // TODO: REFUSER SI MAUVAISE SEQUENCE EN COURS
         in >> c;
         if( (Contrat)c > infos.contrat) {
            infos.contrat = (Contrat)c;
            infos.preneur = tour;
         }
         sendBid(tour, (Contrat)c);
         sequenceEncheres();
         break;
      }

      case NET_CLIENT_VU_CHIEN:
         // TODO: REFUSER SI MAUVAISE SEQUENCE EN COURS
         cptVu++;
         if( cptVu == NB_PLAYERS ) {
            sequence = CHIEN;
            sendDoChien(); // On ordonne au preneur de faire le chien et on attend
         }
         break;

      case NET_CLIENT_VU_PLI:
         // TODO: REFUSER SI MAUVAISE SEQUENCE EN COURS
         cptVu++;
         if( cptVu == NB_PLAYERS ) {
            jeuNext();
         }
         break;

      /**
       * Chien d'un client
       */
      case NET_CLIENT_CHIEN:
      {
         quint8 id;
         int nb_cartes_chien;
         int i;
         Card *c;
         // TODO: REFUSER SI MAUVAISE SEQUENCE EN COURS
         if( NB_PLAYERS == 3 ) {
            nb_cartes_chien = 6;
         } else if( NB_PLAYERS == 4 ) {
            nb_cartes_chien = 6;
         } else { // 5 joueurs
            nb_cartes_chien = 3;
         }
         deckChien.clear();
         for( i=0; i<nb_cartes_chien; i++ ) {
            in >> id;
            c = Jeu::getCard( id );
            deckChien.append( c );
         }
         sequence = GAME;
         tour = donneur;
         sendDepartDonne();
         jeu();
         break;
      }

      /**
       * Un client a déclaré une poignée
       */
      case NET_CLIENT_POIGNEE:
      {
         quint8 dummy;
         in >> dummy;
         quint8 id;
         Poignee p;
         // TODO: REFUSER SI MAUVAISE SEQUENCE EN COURS
         // TODO: add protection, limits ...
         poigneeDeck.clear();
         for(int i=0; i<dummy;i++) {
            in >> id;
            poigneeDeck.append(Jeu::getCard(id));
         }
         // TODO: add robustness here!!
         if(dummy == 10)
            p = P_SIMPLE;
         else if (dummy == 13)
            p = P_DOUBLE;
         else
            p = P_TRIPLE;

         if(players[cnx]->getPlace() == infos.preneur) {
            score.setPoigneeAttaque(p);
         } else {
            score.setPoigneeDefense(p);
         }
         /*
          Tester :
          1) L'origine du client (sud, est ... ip ??)
          2) La validité de la poignée (présence dans le deck du joueur, utilisation de l'excuse)
          3) La poignée doit être déclarée avant de jouer la première carte
          4) Puis envoyer la poignée à tout le monde ...
         */
         break;
      }

      /**
       * Carte d'un client
       */
      case NET_CLIENT_CARTE:
      {
         quint8 id;
         Card *c;
         // TODO: tester la validité de la carte (ID + présence dans le jeu du joueur)
         // si erreur : logguer et prévenir quelqu'un ??
         // TODO: REFUSER SI MAUVAISE SEQUENCE EN COURS
         in >> id;
         c = Jeu::getCard( id );
         mainDeck.append(c);
         players[cnx]->removeCard(c);
         sendCard(c);
         jeu();
         break;
      }

      case NET_CLIENT_READY:
         // TODO: REFUSER SI MAUVAISE SEQUENCE EN COURS
         if (sequence == SEQ_WAIT_PLAYER) {
            cptVu++;
            if( cptVu == NB_PLAYERS ) {
               nouvelleDonne();
            }
         }
         break;

      default:
         break;
   }
}
/*****************************************************************************/
/**
 * Broadcast des cartes à tous les clients
 */
void TarotEngine::sendCards( Place p, quint8 *params )
{
   int j;
   QByteArray block;

   QDataStream out( &block, QIODevice::WriteOnly );
   out.setVersion(QT_STREAMVER);
   out << (quint16)0 << (quint8)NET_RECEPTION_CARTES
       << (quint8)p
       << (quint8)NB_PLAYERS;
   for(j=0; j<24; j++ ) {
      out << (quint8)params[j];
   }
   out << (quint16)0xFFFF;
   out.device()->seek(0);
   out << (quint16)( block.size() - sizeof(quint16) );

   QTcpSocket *s = getConnection(p);
   s->write(block);
   s->flush();
}
/*****************************************************************************/
/**
 * Ask a client for his bid
 */
void TarotEngine::askBid(Contrat c)
{
   QByteArray block;
   QDataStream out( &block, QIODevice::WriteOnly );
   out.setVersion(QT_STREAMVER);
   out << (quint16)0 << (quint8)NET_DEMANDE_ENCHERE
       << (quint8)c   // le contrat précédent
       << (quint16)0xFFFF;
   out.device()->seek(0);
   out << (quint16)( block.size() - sizeof(quint16) );

   QTcpSocket *s = getConnection(tour);
   s->write(block);
   s->flush();
}
/*****************************************************************************/
void TarotEngine::sendBid( Place p, Contrat c )
{
   QByteArray block;
   QDataStream out( &block, QIODevice::WriteOnly );
   out.setVersion(QT_STREAMVER);
   out << (quint16)0 << (quint8)NET_ENCHERE_JOUEUR
       << (quint8)p   // la place de celui qui vient d'enchérir
       << (quint8)c   // le contrat à afficher
       << (quint16)0xFFFF;
   out.device()->seek(0);
   out << (quint16)( block.size() - sizeof(quint16) );
   broadcast( block );
}
/*****************************************************************************/
void TarotEngine::selectPlayer( Place p )
{
   QByteArray block;
   QDataStream out( &block, QIODevice::WriteOnly );
   out.setVersion(QT_STREAMVER);
   out << (quint16)0 << (quint8)NET_SELECTION_JOUEUR
       << (quint8)p   // la place du joueur qui est en train de jouer
       << (quint16)0xFFFF;
   out.device()->seek(0);
   out << (quint16)( block.size() - sizeof(quint16) );
   broadcast( block );
}
/*****************************************************************************/
/**
 * Affiche un message dans la fenêtre de dialogue et l'envoi
 * à tous les clients connectés
 */
void TarotEngine::sendMessage( const QString &message, Place p )
{
   QByteArray block;
   QDataStream out( &block, QIODevice::WriteOnly );
   out.setVersion(QT_STREAMVER);
   out << (quint16)0 << (quint8)NET_MESSAGE
       << message
       << (quint16)0xFFFF;
   out.device()->seek(0);
   out << (quint16)( block.size() - sizeof(quint16) );

   if( p == BROADCAST ) {
      broadcast( block );
   } else {
      QTcpSocket *s = getConnection(p);
      s->write(block);
      s->flush();
   }
}
/*****************************************************************************/
/**
 * Envoie la liste des joueurs connectés
 */
void TarotEngine::sendPlayersList()
{
   QByteArray block;
   quint8 n;
   Identity idents[5];

   n = getConnectedPlayers( idents );
   QDataStream out( &block, QIODevice::WriteOnly );
   out.setVersion(QT_STREAMVER);
   out << (quint16)0 << (quint8)NET_LISTE_JOUEURS
       << (quint8)n; // nombre de joueurs

   for( int i=0; i<n; i++ ) {
      QFileInfo fi(idents[i].avatar);
      out << idents[i].name
          << fi.fileName()
          << idents[i].quote
          << (quint8)idents[i].sex;
   }
   out << (quint16)0xFFFF;
   out.device()->seek(0);
   out << (quint16)( block.size() - sizeof(quint16) );
   broadcast( block ); // clients connectés
}
/*****************************************************************************/
/**
 * On demande à tous les clients de montrer le Chien
 */
void TarotEngine::sendShowChien()
{
   int i;
   QByteArray block;
   QDataStream out( &block, QIODevice::WriteOnly );

   out.setVersion(QT_STREAMVER);
   out << (quint16)0 << (quint8)NET_MONTRE_CHIEN;
   for(i=0; i<deckChien.count(); i++ ) {
       out << (quint8)deckChien.at(i)->getId();
   }
   out << (quint16)0xFFFF;
   out.device()->seek(0);
   out << (quint16)( block.size() - sizeof(quint16) );
   broadcast( block ); // clients connectés
}
/*****************************************************************************/
/**
 * On demande au client de faire le Chien (ou d'attendre pour les autres)
 */
void TarotEngine::sendDoChien()
{
   QByteArray block;
   QDataStream out( &block, QIODevice::WriteOnly );

   out.setVersion(QT_STREAMVER);
   out << (quint16)0 << (quint8)NET_FAIT_CHIEN
       << (quint16)0xFFFF;
   out.device()->seek(0);
   out << (quint16)( block.size() - sizeof(quint16) );

   QTcpSocket *s = getConnection(infos.preneur);
   s->write(block);
   s->flush();
}
/*****************************************************************************/
/**
 * On envoie les paramètres de jeu pour commencer la nouvelle partie
 */
void TarotEngine::sendDepartDonne()
{
   QByteArray block;
   QDataStream out( &block, QIODevice::WriteOnly );

   out.setVersion(QT_STREAMVER);
   out << (quint16)0 << (quint8)NET_DEPART_DONNE
       << (quint8)infos.preneur
       << (quint8)infos.contrat
       << (quint16)0xFFFF;
   out.device()->seek(0);
   out << (quint16)( block.size() - sizeof(quint16) );
   broadcast( block ); // clients connectés
}
/*****************************************************************************/
/**
 * On demande au joueur dont c'est le tour de jouer
 */
void TarotEngine::sendJoueCarte()
{
   QByteArray block;
   QDataStream out( &block, QIODevice::WriteOnly );

   out.setVersion(QT_STREAMVER);
   out << (quint16)0 << (quint8)NET_JOUE_CARTE
       << (quint16)0xFFFF;
   out.device()->seek(0);
   out << (quint16)( block.size() - sizeof(quint16) );

   QTcpSocket *s = getConnection(tour);
   s->write(block);
   s->flush();
}
/*****************************************************************************/
/**
 * On broadcast la carte jouée par un joueur
 */
void TarotEngine::sendCard( Card *c )
{
   QByteArray block;
   QDataStream out( &block, QIODevice::WriteOnly );

   out.setVersion(QT_STREAMVER);
   out << (quint16)0 << (quint8)NET_MONTRE_CARTE
       << (quint8)c->getId()
       << (quint8)tour
       << (quint16)0xFFFF;
   out.device()->seek(0);
   out << (quint16)( block.size() - sizeof(quint16) );
   broadcast( block ); // clients connectés
}
/*****************************************************************************/
/**
 * On broadcast l'information d'un nouveau tour qui commence
 */
void TarotEngine::sendRedist()
{
   QByteArray block;
   QDataStream out( &block, QIODevice::WriteOnly );

   out.setVersion(QT_STREAMVER);
   out << (quint16)0 << (quint8)NET_SERVER_REDIST
       << (quint16)0xFFFF;
   out.device()->seek(0);
   out << (quint16)( block.size() - sizeof(quint16) );
   broadcast( block ); // clients connectés
}
/*****************************************************************************/
/**
 * On termine ce tour de jeu et on affiche les scores
 */
void TarotEngine::sendFinDonne( ScoreInfos *score_inf, bool lastDeal )
{
   QByteArray block;
   QDataStream out( &block, QIODevice::WriteOnly );

   out.setVersion(QT_STREAMVER);
   out << (quint16)0 << (quint8)NET_FIN_DONNE
       << (qint32)score_inf->attaque
       << (qint32)score_inf->defense
       << (qint32)score_inf->bouts
       << (qint32)score_inf->pointsAFaire
       << (qint32)score_inf->difference
       << (qint32)score_inf->points_petit_au_bout
       << (qint32)score_inf->multiplicateur
       << (qint32)score_inf->points_poignee
       << (qint32)score_inf->points_chelem
       << (qint32)score_inf->points_defense;

   if (lastDeal == true)
       out << (quint8)1;
   else
       out << (quint8)0;

   out << (quint16)0xFFFF;
   out.device()->seek(0);
   out << (quint16)( block.size() - sizeof(quint16) );
   broadcast( block ); // clients connectés
}
/*****************************************************************************/
/**
 * On demande aux clients d'acquitter le pli avant d'effectuer la levée
 */
void TarotEngine::sendWaitPli(float pointsTour)
{
   QByteArray block;
   QDataStream out( &block, QIODevice::WriteOnly );

   out.setVersion(QT_STREAMVER);
   out << (quint16)0 << (quint8)NET_SERVER_WAIT_PLI
       << (quint8)tour // winner of the current turn
       << (quint32)pointsTour // points won by the taker
       << (quint16)0xFFFF;
   out.device()->seek(0);
   out << (quint16)( block.size() - sizeof(quint16) );
   broadcast( block ); // clients connectés
}
/*****************************************************************************/
/**
 * Send a message to all connected clients
 */
void TarotEngine::broadcast( QByteArray &block )
{
   QMapIterator<QTcpSocket*, Player*> i(players);
   while (i.hasNext()) {
     i.next();
     i.key()->write(block);
     i.key()->flush();
   }
}


//=============================================================================
// End of file TarotEngine.cpp
//=============================================================================
