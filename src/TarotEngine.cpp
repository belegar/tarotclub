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
   infos.nbJoueurs = 4;
   infos.gameCounter = 0;
   sequence = VIDE;
   newGame = false;
   dealNumber = 0;

   timerBetweenPlayers.setSingleShot(true);
   timerBetweenPlayers.setInterval(0);

   timerBetweenTurns.setSingleShot(true);
   timerBetweenTurns.setInterval(0);

   connect(&timerBetweenPlayers, SIGNAL(timeout()), this, SLOT(slotTimerBetweenPlayers()));
   connect(&timerBetweenTurns, SIGNAL(timeout()), this, SLOT(slotTimerBetweenTurns()));
   connect(&server, SIGNAL(newConnection()), this, SLOT(newConnection()));
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
void TarotEngine::slotTimerBetweenPlayers()
{
    infos.gameCounter++;
    selectPlayer(tour);
    sendJoueCarte();
}
/*****************************************************************************/
void TarotEngine::slotTimerBetweenTurns()
{
    bool ret;

    ret = finLevee();
    if( ret == false ) {
#ifndef QT_NO_DEBUG
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
#endif

       // end of round, send score to all players
       sendFinDonne( score.getScoreInfos() );
    } else {
       cptVu = 0;
       sendWaitPli();
    }
}
/*****************************************************************************/
void TarotEngine::setTimerBetweenPlayers(int t)
{
    timerBetweenPlayers.setInterval(t);
}
/*****************************************************************************/
void TarotEngine::setTimerBetweenTurns(int t)
{
    timerBetweenTurns.setInterval(t);
}
/*****************************************************************************/
void TarotEngine::customEvent( QEvent *e )
{

   if( (int)e->type() == MsgStartGame ) {
      if( newGame == true ) {
         emit sigPrintMessage("Game already started.");
         return;
      }
      newServerGame( DEFAULTPORT );
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
void TarotEngine::newServerGame( int port )
{
   int i;

   closeClients();
   // 5 joueurs max + une connexion en plus pour avertir aux nouveaux arrivants
   // que le serveur est plein
   server.setMaxPendingConnections(6);
   server.listen( QHostAddress::LocalHost, port );

   // On initialise toutes les variables locales et on choisit le donneur
   infos.nbJoueurs = 4;
   i = qrand()%infos.nbJoueurs;
   donneur = (Place)i;
   newGame = true;
   score.init();
   emit sigPrintMessage("Server started.\r\n");
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
   for( int i=0; i<5; i++ ) {
      if( clients.contains((Place)i) == true ) {
         if( clients.value((Place)i)->state() == QAbstractSocket::ConnectedState ) {
            connect(clients.value((Place)i), SIGNAL(disconnected()), clients.value((Place)i), SLOT(deleteLater()));
            clients.value((Place)i)->close();
         }
      }
   }
}
/*****************************************************************************/
void TarotEngine::addPlayer( Place p, Identity *ident )
{
   ident->avatar = gamePath+"/pics/avatars/"+ident->avatar;
   players[p].setIdentity( ident );
}
/*****************************************************************************/
int TarotEngine::getConnectedPlayers( Identity *idents )
{
   int n = 0;

   for( int i=0; i<5; i++ ) {
      if( clients.contains((Place)i) == true ) {
         if( clients.value((Place)i)->state() == QAbstractSocket::ConnectedState ) {
            idents[i] = *players[i].getIdentity();
            n++;
         }
      }
   }
   return (n);
}
/*****************************************************************************/
int TarotEngine::getConnectedNumber()
{
   int n = 0;

   for( int i=0; i<5; i++ ) {
      if( clients.contains((Place)i) == true ) {
         if( clients.value((Place)i)->state() == QAbstractSocket::ConnectedState ) {
            n++;
         }
      }
   }
   return (n);
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
      ret =  players[p].canPlayCard( &mainDeck, c, infos.gameCounter, infos.nbJoueurs );
   }

   return (ret);
}
/*****************************************************************************/
/**
 * Teste si une carte du joueur local existe dans sa main
 */
bool TarotEngine::cardExists( Card *c, Place p )
{
   return (players[p].cardExists(c));
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

   c = mainDeck.at( infos.gameCounter - infos.nbJoueurs ); // première carte jouée à ce tour

   type = c->getType();
   coul = c->getColor();

    // aïe, le joueur a commencé avec une excuse
   if( type == EXCUSE ) {
      c = mainDeck.at( infos.gameCounter - infos.nbJoueurs + 1 ); // la couleur demandée est donc la seconde carte
      flag = true;
      debut = 1;
      type = c->getType();
      coul = c->getColor();
      leader = 2;
   }

   debut = debut + infos.gameCounter - infos.nbJoueurs + 1 ; // le début est décalé si le premier pli est l'excuse

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
            leader = i - (infos.gameCounter - infos.nbJoueurs);
         } else if( drapeau == 0 ) {
            valLeader = c->getValue();
            leader = i - (infos.gameCounter - infos.nbJoueurs);
            drapeau = 1;
         }

      // pas de coupe détectée
      } else if( c->getType() == CARTE && drapeau == 0 ) {
         if( c->getColor() == coul) { // ahah, même couleur !
            if( c->getValue() > valLeader ) {
               valLeader = c->getValue();
               leader = i - (infos.gameCounter - infos.nbJoueurs);
            }
         }
      }
   }

   // place du leader
   if( infos.nbJoueurs == 4 ) {
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
   }

   return pl;
}
/*****************************************************************************/
void TarotEngine::nouvelleDonne()
{
   // on ajoute les cartes dans le deck
   mainDeck.clear();
   for( int i=0; i<78; i++ ) {
      mainDeck.append( Jeu::getCard(i) );
   }

   dealNumber = mainDeck.shuffle( true, 0 );
   donneur = nextPlayer(donneur);
   infos.contrat = PASSE;
   infos.gameCounter = 0;
   distribution();
   tour = nextPlayer(donneur); // Le joueur à la droite du donneur commence les enchères
   selectPlayer(tour);
   askBid( tour, infos.contrat );
}
/*****************************************************************************/
/**
 * Automate servant à séquencer le jeu selon l'état courant des variables
 */
void TarotEngine::jeu()
{
   if( !(infos.gameCounter%infos.nbJoueurs) && infos.gameCounter ) {
       timerBetweenTurns.start();
   } else {
      tour = nextPlayer(tour);
      jeuNext();
   }
}
/*****************************************************************************/
void TarotEngine::jeuNext()
{
   timerBetweenPlayers.start();
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
      askBid( tour, infos.contrat );
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
 */
bool TarotEngine::finLevee()
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

   for( i = (infos.gameCounter-infos.nbJoueurs); i<infos.gameCounter; i++ ) {
      c = mainDeck.at(i);
      score.setPli( i, tour );

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
      score.calcul( mainDeck, deckChien, &infos );
      return false;
   }

}
/*****************************************************************************/
/**
 * On distribue les cartes entre les joueurs et le chien
 */
void TarotEngine::distribution()
{
   int i, j;
   Card *c;
   int index;
   int nb_cartes_chien;
   int nb_cartes_joueur;
   quint8 params[24] = {0};   // cartes du joueur (3 joueurs: 24, 4j: 18, 5j: 15)

   // On vide tous les decks
   for(i=0; i<infos.nbJoueurs; i++ ) {
      players[i].emptyDeck();
   }
   deckChien.clear();

   if( infos.nbJoueurs == 3 ) {
      nb_cartes_chien = 6;
   } else if( infos.nbJoueurs == 4 ) {
      nb_cartes_chien = 6;
   } else { // 5 joueurs
      nb_cartes_chien = 3;
   }
   nb_cartes_joueur = (78-nb_cartes_chien)/infos.nbJoueurs;

   for( i=0; i<infos.nbJoueurs; i++ ) {
      for( j=0; j<nb_cartes_joueur; j++ ) {
         index = i*nb_cartes_joueur+j;
         c = mainDeck.at(index);
         if( c->getType() == EXCUSE ) {
            score.setExcuse((Place)i);
         }
         c->setOwner((Place)i);
         players[i].addCard( c );
         params[j] = c->getId();
      }
      sendCards( (Place)i, params );
   }

   // Les cartes restantes vont au chien
   for( i=78-nb_cartes_chien; i<78; i++) {
      c = mainDeck.at(i);
      deckChien.append( c );
   }
   mainDeck.clear();
}
/*****************************************************************************/
void TarotEngine::newConnection()
{
   QTcpSocket *client = server.nextPendingConnection();

    // Place disponible ?
   if( getConnectedNumber() == infos.nbJoueurs ) {
      QString message = "Le serveur est complet.";
      QByteArray block;
      QDataStream out( &block, QIODevice::WriteOnly );
      out.setVersion(QT_STREAMVER);
      out << (quint16)0 << (quint8)NET_MESSAGE
          << message
          << (quint16)0xFFFF;
      out.device()->seek(0);
      out << (quint16)( block.size() - sizeof(quint16) );

      connect(client, SIGNAL(disconnected()), client, SLOT(deleteLater()));
      client->write(block);
      client->close();
   } else {
      // Ok, il reste de la place, on connecte ce client
      if( clients.contains(SUD) == false ) {
         clients[SUD] = client; // on ajoute ce client à la liste
         connect( client, SIGNAL(disconnected()), this, SLOT(clientClosed1()));
         connect( client, SIGNAL(readyRead()), this, SLOT( readData1()));
      } else if( clients.contains(EST) == false ) {
         clients[EST] = client; // on ajoute ce client à la liste
         connect( client, SIGNAL(disconnected()), this, SLOT(clientClosed2()));
         connect( client, SIGNAL(readyRead()), this, SLOT( readData2()));
      } else if( clients.contains(NORD) == false ) {
         clients[NORD] = client; // on ajoute ce client à la liste
         connect( client, SIGNAL(disconnected()), this, SLOT(clientClosed3()));
         connect( client, SIGNAL(readyRead()), this, SLOT( readData3()));
      } else if( clients.contains(OUEST) == false ) {
         clients[OUEST] = client; // on ajoute ce client à la liste
         connect( client, SIGNAL(disconnected()), this, SLOT(clientClosed4()));
         connect( client, SIGNAL(readyRead()), this, SLOT( readData4()));
      }

      // on envoie une demande d'infos personnelles
      QByteArray block;
      QDataStream out( &block, QIODevice::WriteOnly );
      out.setVersion(QT_STREAMVER);
      out << (quint16)0 << (quint8)NET_IDENTIFICATION
          << (quint16)0xFFFF;

      out.device()->seek(0);
      out << (quint16)( block.size() - sizeof(quint16) );

      client->write(block);
   }
}
/*****************************************************************************/
void TarotEngine::clientClosed1()
{
   clientClosed( SUD );
}
/*****************************************************************************/
void TarotEngine::clientClosed2()
{
   clientClosed( EST );
}
/*****************************************************************************/
void TarotEngine::clientClosed3()
{
   clientClosed( NORD );
}
/*****************************************************************************/
void TarotEngine::clientClosed4()
{
   clientClosed( OUEST );
}
/*****************************************************************************/
void TarotEngine::clientClosed( Place p )
{
   QString message = "Le joueur " + players[p].getName() + " a quitté la partie.";
   sendMessage( message, BROADCAST );
   sendPlayersList();
   clients.remove(p);
}
/*****************************************************************************/
void TarotEngine::readData1()
{
   readData( SUD );
}
/*****************************************************************************/
void TarotEngine::readData2()
{
   readData( EST );
}
/*****************************************************************************/
void TarotEngine::readData3()
{
   readData( NORD );
}
/*****************************************************************************/
void TarotEngine::readData4()
{
   readData( OUEST );
}
/*****************************************************************************/
/**
 * Décodage de la trame reçue
 */
void TarotEngine::readData( Place p )
{
   QTcpSocket *client;
   quint16 blockSize = 0;
   bool trameEnPlus = false;
   unsigned int total=0;
   unsigned int bytes;

   if( clients.contains(p) ) {
      client = clients.value(p);
   } else {
      return; // à défaut d'autre chose
   }

   bytes = client->bytesAvailable();
   QDataStream in( client );
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

      doAction( in, p );
      blockSize = 0;
   }
}
/*****************************************************************************/
void TarotEngine::doAction( QDataStream &in, Place p )
{
   quint8 type;   // type de trame
   QTcpSocket *client;
   in >> type;

   if( clients.contains(p) ) {
      client = clients.value(p);
   } else {
      return; // à défaut d'autre chose
   }

   switch( type ) {

      /**
       * Message en provenance d'un client
       */
      case NET_CLIENT_MSG:
      {
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
         for( int i=0; i<5; i++ ) {
            if( clients.contains((Place)i) == true ) {
               if( clients.value((Place)i)->state() == QAbstractSocket::ConnectedState ) {
                  if( players[i].getName() == ident.name ) {
                     ok = true;
                     break;
                  }
               }
            }
         }

         if( ok == true ) {
            // On prévient le joueur que son nick est déjà pris
            QString message = "Votre pseudonyme \"" + ident.name + "\" existe déjà sur le serveur."+
               " Changez le et reconnectez-vous.";

            // et on annule sa connexion
            connect(client, SIGNAL(disconnected()), client, SLOT(deleteLater()));
            sendMessage( message, p );
            client->close();

         // TODO : tester s'il reste encore une place libre

         } else {
            addPlayer( p, &ident );
            QString m = "Le joueur " + ident.name + " a rejoint la partie.";
            sendMessage( m , BROADCAST );
            emit sigPrintMessage(m);
            sendPlayersList();
         }
         break;
      }

      /**
       * On lance une nouvelle donne
       */
      case NET_CLIENT_DONNE:
      {
         nouvelleDonne();
         break;
      }

      /**
       * Contrat d'un joueur
       */
      case NET_CLIENT_ENCHERE:
      {
         quint8 c;

         in >> c;
         if( (Contrat)c > infos.contrat) {
            infos.contrat = (Contrat)c;
            infos.preneur = tour;
         }
         sendBid( tour, (Contrat)c );
         sequenceEncheres();
         break;
      }

      case NET_CLIENT_VU_CHIEN:
         cptVu++;
         if( cptVu == infos.nbJoueurs ) {
            sequence = CHIEN;
            sendDoChien(); // On ordonne au preneur de faire le chien et on attend
         }
         break;

      case NET_CLIENT_VU_PLI:
         cptVu++;
         if( cptVu == infos.nbJoueurs ) {
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

         if( infos.nbJoueurs == 3 ) {
            nb_cartes_chien = 6;
         } else if( infos.nbJoueurs == 4 ) {
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
         /*
          Tester :
          1) L'origine du client (sud, est ... ip ??)
          2) La validité de la poignée (présence dans le deck du joueur, utilisation de l'excuse)
          3) La poignée doit être déclarée avant de jouer la première carte
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
         in >> id;
         c = Jeu::getCard( id );
         mainDeck.append(c);
         players[p].removeCard(c);
         sendCard(c);
         jeu();
         break;
      }

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
       << (quint8)infos.nbJoueurs;
   for(j=0; j<24; j++ ) {
      out << (quint8)params[j];
   }
   out << (quint16)0xFFFF;
   out.device()->seek(0);
   out << (quint16)( block.size() - sizeof(quint16) );
   clients[p]->write(block);
   clients[p]->flush();
}
/*****************************************************************************/
/**
 * Ask a client for his bid
 */
void TarotEngine::askBid( Place p, Contrat c )
{
   QByteArray block;
   QDataStream out( &block, QIODevice::WriteOnly );
   out.setVersion(QT_STREAMVER);
   out << (quint16)0 << (quint8)NET_DEMANDE_ENCHERE
       << (quint8)c   // le contrat précédent
       << (quint16)0xFFFF;
   out.device()->seek(0);
   out << (quint16)( block.size() - sizeof(quint16) );
   clients[p]->write(block);
   clients[p]->flush();
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
      QTcpSocket *client;
      if( clients.contains(p) ) {
         client = clients.value(p);
      } else {
         return; // à défaut d'autre chose
      }
      client->write(block);
      client->flush();
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
   clients[infos.preneur]->write(block);
   clients[infos.preneur]->flush();
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
   clients[tour]->write(block);
   clients[tour]->flush();
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
void TarotEngine::sendFinDonne( ScoreInfos *score_inf )
{
   QByteArray block;
   QDataStream out( &block, QIODevice::WriteOnly );

   out.setVersion(QT_STREAMVER);
   out << (quint16)0 << (quint8)NET_FIN_DONNE
       << (quint32)score_inf->attaque
       << (quint32)score_inf->defense
       << (quint32)score_inf->bouts
       << (quint32)score_inf->pointsAFaire
       << (quint32)score_inf->difference
       << (quint32)score_inf->points_petit_au_bout
       << (quint32)score_inf->multiplicateur
       << (quint32)score_inf->points_poignee
       << (quint32)score_inf->points_chelem
       << (quint32)score_inf->points_defense
       << (quint16)0xFFFF;
   out.device()->seek(0);
   out << (quint16)( block.size() - sizeof(quint16) );
   broadcast( block ); // clients connectés
}
/*****************************************************************************/
/**
 * On demande aux clients d'acquitter le pli avant d'effectuer la levée
 */
void TarotEngine::sendWaitPli()
{
   QByteArray block;
   QDataStream out( &block, QIODevice::WriteOnly );

   out.setVersion(QT_STREAMVER);
   out << (quint16)0 << (quint8)NET_SERVER_WAIT_PLI
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
   QMapIterator<Place, QTcpSocket*> i(clients);
   while (i.hasNext()) {
      i.next();
      QTcpSocket *client = i.value();
      client->write(block);
      client->flush();
   }
}


//=============================================================================
// End of file TarotEngine.cpp
//=============================================================================
