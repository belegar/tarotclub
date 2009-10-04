/*=============================================================================
 * Tarot Club - TarotEngine.cpp
 *=============================================================================
 * Moteur de jeu principal + serveur de jeu r�seau
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

#include <QtNetwork>
#include <QCoreApplication>
#include "TarotEngine.h"
#include "ServerConsole.h"

/*****************************************************************************/
TarotEngine::TarotEngine()
   : QThread()
{
   qsrand(QTime(0,0,0).secsTo(QTime::currentTime())); // seed init
   
   Jeu::init();
   QDir b = QCoreApplication::applicationDirPath();
   b.cdUp();

   gamePath = b.path();
   infos.nbJoueurs = 4;
   infos.gameCounter = 0;
   sequence = VIDE;
   newGame = false;
   dealNumber = 0;

   connect( &server, SIGNAL(newConnection()), this, SLOT(newConnection()));
}
/*****************************************************************************/
TarotEngine::~TarotEngine()
{
    closeClients();
}
/*****************************************************************************/
void TarotEngine::run()
{
   exec();
}
/*****************************************************************************/
void TarotEngine::customEvent( QEvent *e )
{
   if( (int)e->type() == MsgStartGame ) {
      if( newGame == true ) {
         cout << "Game already started.";
         return;
      }
      newServerGame( DEFAULTPORT );
   } else if( (int)e->type() == MsgStopGame ) {
      if( newGame == false ) {
         cout << "Game not started.";
         return;
      }
      closeServerGame();
   } else if( (int)e->type() == MsgExitGame ) {
      closeServerGame();
      QCoreApplication::exit(0);
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
   cout << endl << "Server started" << endl;
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
/**
 * Teste si une carte du joueur local peut �tre jou�e
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
* Calcule qui a remport� le pli
*/
Place TarotEngine::calculGagnantPli()
{
   Card *c;

   // premi�re carte
   CardType type;
   CardColor coul;
   Place pl = NORD;

   bool flag=false; // vrai si l'entame est l'excuse
   int i, debut=0;
   int leader = 0; // 0 est le premier joueur a jou� etc. jusqu'� 5
   // par d�faut, celui qui entame est le leader (car couleur demand�e)

   // �tape 1 : on cherche la couleur demand�e

   c = mainDeck.at( infos.gameCounter - infos.nbJoueurs ); // premi�re carte jou�e � ce tour

   type = c->getType();
   coul = c->getColor();

    // a�e, le joueur a commenc� avec une excuse
   if( type == EXCUSE ) {
      c = mainDeck.at( infos.gameCounter - infos.nbJoueurs + 1 ); // la couleur demand�e est donc la seconde carte
      flag = true;
      debut = 1;
      type = c->getType();
      coul = c->getColor();
      leader = 2;
   }

   debut = debut + infos.gameCounter - infos.nbJoueurs + 1 ; // le d�but est d�cal� si le premier pli est l'excuse

   int  drapeau; // indique une coupe, donc le leader est forc�ment un atout
   int  valLeader;
   valLeader = c->getValue();

   if( type == ATOUT ) {
      drapeau = 1;   // on cherche une �ventuelle surcoupe
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

      // pas de coupe d�tect�e
      } else if( c->getType() == CARTE && drapeau == 0 ) {
         if( c->getColor() == coul) { // ahah, m�me couleur !
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
   tour = nextPlayer(donneur); // Le joueur � la droite du donneur commence les ench�res
   selectPlayer(tour);
   askBid( tour, infos.contrat );
}
/*****************************************************************************/
/**
 * Automate servant � s�quencer le jeu selon l'�tat courant des variables
 */
void TarotEngine::jeu()
{  
   bool ret;

   if( !(infos.gameCounter%infos.nbJoueurs) && infos.gameCounter ) {
      ret = finLevee();
      if( ret == false ) {
          sendFinDonne( score.getScoreInfos() );
      } else {
         cptVu = 0;
         sendWaitPli();
      }
      return;
   }
   tour = nextPlayer(tour);
   jeuNext();
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
         // si tous les joueurs ont pass� on s'arr�te et on pr�vient tout le monde
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

   // On pr�cise quel joueur poss�de le chien en fonction du contrat
   for( i=0; i<deckChien.count(); i++) {
      c = deckChien.at( i );
      if( infos.contrat != GARDE_CONTRE ) {
         c->setOwner( infos.preneur );
      } else {
         // En cas de garde contre, le chien appartient � la d�fense
         c->setOwner( HYPERSPACE );
      }
   }

   if( infos.contrat == GARDE_SANS || infos.contrat == GARDE_CONTRE ) {
      // On n'affiche pas le chien et on commence la partie imm�diatement
      sendDepartDonne();
      jeu();
      return;
   }
   sendShowChien(); // Prise ou garde, on affiche le chien chez tout le monde
   cptVu = 0;
   sequence = WAIT_CHIEN; // et on attend le chien �cart� par le preneur
}
/*****************************************************************************/
/**
 * Fin d'un tour, on calcule le gagnant du pli et on pr�pare la suite
 */
bool TarotEngine::finLevee()
{
   int i;
   Card *c;

#ifdef _DEBUG
   QFile f("debug.txt");
   QTextStream fout(&f);
   f.open(QIODevice::Append | QIODevice::Text);
   fout << "----- tour avant : " << tour << endl;
#endif

   // On d�tecte qui a remport� le pli, c'est lui qui entame le prochain tour
   tour = calculGagnantPli();

   for( i = infos.gameCounter-infos.nbJoueurs; i<infos.gameCounter; i++ ) {
      c = mainDeck.at(i);
      score.setPli( i, tour );

#ifdef _DEBUG
    fout << c->getId() << " i=" << i << ", tour=" << tour << " ";
#endif
    }

#ifdef _DEBUG
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
         clients[SUD] = client; // on ajoute ce client � la liste
         connect( client, SIGNAL(disconnected()), this, SLOT(clientClosed1()));
         connect( client, SIGNAL(readyRead()), this, SLOT( readData1()));
      } else if( clients.contains(EST) == false ) {
         clients[EST] = client; // on ajoute ce client � la liste
         connect( client, SIGNAL(disconnected()), this, SLOT(clientClosed2()));
         connect( client, SIGNAL(readyRead()), this, SLOT( readData2()));
      } else if( clients.contains(NORD) == false ) {
         clients[NORD] = client; // on ajoute ce client � la liste
         connect( client, SIGNAL(disconnected()), this, SLOT(clientClosed3()));
         connect( client, SIGNAL(readyRead()), this, SLOT( readData3()));
      } else if( clients.contains(OUEST) == false ) {
         clients[OUEST] = client; // on ajoute ce client � la liste
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
   QString message = "Le joueur " + players[p].getName() + " a quitt� la partie.";
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
 * D�codage de la trame re�ue
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
      return; // � d�faut d'autre chose
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
      return; // � d�faut d'autre chose
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
         cout << "Client message: " << message.toLatin1().data() << endl;
         break;
      }
      
      /**
       * Identit� d'un client
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
         // On cherche si le nick n'existe pas d�j�
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
            // On pr�vient le joueur que son nick est d�j� pris
            QString message = "Votre pseudonyme \"" + ident.name + "\" existe d�j� sur le serveur."+
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
            cout << m.toLatin1().data() << endl;
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
#ifdef _DEBUG
         tour = OUEST;
#endif
         sendDepartDonne();
         jeu();
         break;
      }

      /**
       * Carte d'un client
       */
      case NET_CLIENT_CARTE:
      {
         quint8 id;
         Card *c;

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
 * Broadcast des cartes � tous les clients
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
       << (quint8)c   // le contrat pr�c�dent
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
       << (quint8)p   // la place de celui qui vient d'ench�rir
       << (quint8)c   // le contrat � afficher
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
 * Affiche un message dans la fen�tre de dialogue et l'envoi
 * � tous les clients connect�s
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
         return; // � d�faut d'autre chose
      }
      client->write(block);
      client->flush();
   }
}
/*****************************************************************************/
/**
 * Envoie la liste des joueurs connect�s
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
   broadcast( block ); // clients connect�s
}
/*****************************************************************************/
/**
 * On demande � tous les clients de montrer le Chien
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
   broadcast( block ); // clients connect�s
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
 * On envoie les param�tres de jeu pour commencer la nouvelle partie
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
   broadcast( block ); // clients connect�s
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
 * On broadcast la carte jou�e par un joueur
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
   broadcast( block ); // clients connect�s
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
   broadcast( block ); // clients connect�s
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
   broadcast( block ); // clients connect�s
}
/*****************************************************************************/
/**
 * On demande aux clients d'acquitter le pli avant d'effectuer la lev�e
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
   broadcast( block ); // clients connect�s
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
