/*=============================================================================
 * Tarot Club - Client.cpp
 *=============================================================================
 * Classe modélisant un client connecté à une partie serveur.
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

#ifndef QT_NO_DEBUG
   #include <iostream>
   #include <fstream>
using namespace std;
#endif // _DEBUG

#include "Client.h"
#include "defines.h"

/*****************************************************************************/
Client::Client() : Player()
{
   // Événements sur le socket
   connect( &socket, SIGNAL(readyRead()),this, SLOT(socketReadData()));
   connect( &socket, SIGNAL(disconnected()),this, SLOT(socketClosed()));
   connect( &socket, SIGNAL(connected()),this, SLOT(socketConnected()));
   connect( &socket, SIGNAL(error(QAbstractSocket::SocketError)),this, SLOT(socketError(QAbstractSocket::SocketError)));
   connect( &socket, SIGNAL(hostFound()),this, SLOT(socketHostFound()));
}
/*****************************************************************************/
void Client::setType( PlayerType t )
{
   type = t;
}
/*****************************************************************************/
PlayerType Client::getType()
{
   return (type);
}
/*****************************************************************************/
DECK_STATS *Client::getStats()
{
   return (&stats);
}
/*****************************************************************************/
Card *Client::getCardMainDeck(int i)
{
   return(mainDeck.at(i));
}
/*****************************************************************************/
Card *Client::getCardChien(int i)
{
   return(chien.at(i));
}
/*****************************************************************************/
int Client::getTailleChien()
{
   return(chien.count());
}
/*****************************************************************************/
GameInfos *Client::getGameInfos()
{
   return &infos;
}
/*****************************************************************************/
ScoreInfos *Client::getScoreInfos()
{
   return(&score_inf);
}
/*****************************************************************************/
void Client::emptyChien()
{
   chien.clear();
}
/*****************************************************************************/
void Client::removeCardChien(Card *c)
{
   if( chien.contains(c) == true ) {
      chien.removeAll(c);
   }
}
/*****************************************************************************/
void Client::addCardChien(Card *c)
{
   chien.append(c);
}
/*****************************************************************************/
/**
 * Reset statistics
 */
void Client::razStats()
{
   stats.atouts = 0;
   stats.bouts = 0;
   stats.atoutsMajeurs = 0;
   
   stats.rois = 0;
   stats.dames = 0;
   stats.cavaliers = 0;
   stats.valets = 0;

   stats.mariages = 0;
   stats.longues = 0;
   stats.coupes = 0;
   stats.singletons = 0;
   stats.sequences = 0;

   stats.trefles = 0;
   stats.pics = 0;
   stats.coeurs = 0;
   stats.carreaux = 0;

   stats.petit = false;
   stats.vingtEtUn = false;
   stats.excuse = false;

}
/*****************************************************************************/
/**
 * Décide de l'enchère
 */
Contrat Client::calculEnchere()
{
   int total = 0;
   Contrat cont;

   updateStats();

   // On distribue des points en fonction des stats
   if( stats.vingtEtUn == true ) {
      total += 9;
   }
   if( stats.excuse == true ) {
      total += 7;
   }
   if( stats.petit == true ) {
      if( stats.atouts == 5 ) {
         total += 5;
      } else if( stats.atouts == 6 || stats.atouts == 7 ) {
         total += 7;
      } else if( stats.atouts > 7 ) {
         total += 8;
      }
   }

   // Chaque atout vaut deux points
   // Chaque atout majeur vaut 1 point supplémentaire
   total += stats.atouts * 2;
   total += stats.atoutsMajeurs * 2;
   total += stats.rois * 6;
   total += stats.dames * 3;
   total += stats.cavaliers * 2;
   total += stats.valets;
   total += stats.mariages;
   total += stats.longues * 5; 
   total += stats.coupes * 5;
   total += stats.singletons * 3;
   total += stats.sequences * 4;

   // on décide de l'enchère :
   if( total <= 35 ) {
      cont = PASSE;
   } else if( total >= 36  && total <= 50 ) {
      cont = PRISE;
   } else if( total >= 51  && total <= 65 ) {
      cont = GARDE;
   } else if( total >= 66  && total <= 75 ) {
      cont = GARDE_SANS;
   } else {
      cont = GARDE_CONTRE;
   }
   return cont;
}
/*****************************************************************************/
void Client::choixChien( Deck *deckChien )
{
   int i, j, k;
   Card *c, *cdeck;

#ifndef QT_NO_DEBUG
// contenu du chien avant
   ofstream f("chien_avant.txt");

   for( i=0; i<deckChien->count(); i++) {
      c = deckChien->at(i);
   
      if (f.is_open()) {
         f << "Couleur : " << c->getColor() << endl;
         f << "Type : " << c->getType() << endl;
         f << "Valeur : " << c->getValue() << endl;
         f << "Id : " << c->getId() << endl;
         f << endl;
      }
   }
   f.close();
#endif

   bool ok=false;
   i=0;

   // on cherche si il y a un atout, une excuse, ou un roi dans le chien
   // et on les remplace par des cartes valides
   while( ok == false ) {

      c = deckChien->at( i );
      if( c->getType() == ATOUT || c->getType() == EXCUSE || ( c->getType() == CARTE && c->getValue() == 14 ) ) {
         // on cherche une carte de remplacement
         k = myDeck.count();
         for( j=0; j<k; j++ ) {
            cdeck = myDeck.at( j );
            if( cdeck->getType() == CARTE && cdeck->getValue() < 14 ) {
               // ok, on procède à l'échange
               myDeck.removeAll( cdeck );
               myDeck.append( c );
               deckChien->removeAll( c );
               deckChien->append( cdeck );
               break;
            }
         }
         i=0;
      } else {
         i++;
      }
      
      if( i == 6 ) {
         ok = true;
      }
   }


#ifndef QT_NO_DEBUG
// contenu du chien après
   f.open("chien_apres.txt");

   for( i=0; i<deckChien->count(); i++) {
      c = deckChien->at(i);
   
      if (f.is_open()) {
         f << "Couleur : " << c->getColor() << endl;
         f << "Type : " << c->getType() << endl;
         f << "Valeur : " << c->getValue() << endl;
         f << "Id : " << c->getId() << endl;
         f << endl;
      }
   }
   f.close();
#endif


}
/*****************************************************************************/
/**
 * Update statistics by analyzing the deck
 */
void Client::updateStats()
{
   int i, k, n, val, count, flag, longue;
   Card *c;
   CardColor coul;
   
   razStats();
   n = myDeck.count();
   count = 0; // compteur
   flag = 0;  // indicateur de petit

   // recherche des atouts
   for( i=0; i<n; i++) {
      c = myDeck.at(i);
      if( c->getType() == ATOUT ) {
         stats.atouts++;
         val = c->getValue();
         if( val >= 15 ) {
            stats.atoutsMajeurs++; // atout majeur
         }
         if( val == 21 ) {
            stats.vingtEtUn = true;
         }
         if( val == 1 ) {
            stats.petit = true;
         }
      }

      if( c->getType() == EXCUSE ) {
         stats.atouts++;
         stats.excuse = true;
      }
   }

   // calcul du nombre de bouts
   if( stats.excuse == true ) {
      stats.bouts++;
   }
   if( stats.vingtEtUn == true ) {
      stats.bouts++;
   }
   if( stats.petit == true ) {
      stats.bouts++;
   }

   int distr[14] = {0}; // teste une distribution

   // on s'occupe maintenant des couleurs
   for( i=0; i<4; i++ ) {
      if( i==0 ) {
         coul = PIC;
      } else if( i==1 ) {
         coul = COEUR;
      } else if( i==2 ) {
         coul = TREFLE;
      } else {
         coul = CARREAU;
      }

      for( k=0; k<14; k++ ) {
         distr[k] = 0;
      }
      count = 0;

      for( k=0; k<n; k++ ) {
         c = myDeck.at(k);
         if( c->getType() == CARTE && c->getColor() == coul ) {
            count++;
            val = c->getValue();
            distr[val-1] = 1;
            if( val == 11 ) {
               stats.valets++; // valet
            }
            if( val == 12 ) {
               stats.cavaliers++; // cavalier
            }
         }
      }

      if( count == 1 ) {
         stats.singletons++;     // singleton
      }
      if( count == 0 ) {
         stats.coupes++;         // coupe
      }

      // Nombre de cartes dans chaque couleur
      if( i==0 ) {
         stats.pics = count;
      } else if( i==1 ) {
         stats.coeurs = count;
      } else if( i==2 ) {
         stats.trefles = count;
      } else {
         stats.carreaux = count;
      }

      if( distr[13] == 1 && distr[12] == 1 ) {
         stats.mariages++; // mariage (dame+roi)
      }
      if( distr[13] == 1 ) {
         stats.rois++;     // roi sans dame
      }
      if( distr[12] == 1 ) {
         stats.dames++;    // dame sans roi
      }

      // test des séquences :
      count = 0;  // longueur de la séquence
      flag = 0;   // couleur trouvée : on est dans la séquence
      longue = 0;

      for( k=0; k<14; k++ ) {
         if( distr[k] == 1 ) {
            longue++;
            // début d'une séquence
            if( flag == 0 ) {
               flag = 1;
               count++;
            } else {
               count++;
            }
         } else if( flag == 1 ) {
            if( count >= 5 ) {
               stats.sequences++;
            }
            count = 0;
            flag = 0;
         }
      }
      if( longue >=5 ) {
         stats.longues++;
      }
   }


#ifndef QT_NO_DEBUG
   // Affichage avant le mélange
   ofstream f("stats.txt");
   
   if (f.is_open())
   {
   
      f << "stats.atouts : " << stats.atouts << endl;
      f << "stats.bouts : " << stats.bouts << endl;
      f << "stats.atoutsMajeurs : " << stats.atoutsMajeurs << endl;
      
      f << "stats.rois : " << stats.rois << endl;
      f << "stats.dames : " << stats.dames << endl;
      f << "stats.cavaliers : " << stats.cavaliers << endl;
      f << "stats.valets : " << stats.valets << endl;

      f << "stats.mariages : " << stats.mariages << endl;
      f << "stats.longues : " << stats.longues << endl;
      f << "stats.coupes : " << stats.coupes << endl;
      f << "stats.singletons : " << stats.singletons << endl;
      f << "stats.sequences : " << stats.sequences << endl;

      f << "stats.petit : " << stats.petit << endl;
      f << "stats.vingtEtUn : " << stats.vingtEtUn << endl;
      f << "stats.excuse : " << stats.excuse << endl;

      f << "-----------------------------------" << endl;
   
   }
   
   f.close();

#endif // _DEBUG

}
/*****************************************************************************/
/**
 * Retourne la première carte valide dans la main du joueur
 */
Card *Client::play()
{
   Card *c = NULL;
   int i, n;

   n = myDeck.count();
   for( i=0; i<n; i++ ) {
      c = myDeck.at( i );
      if( canPlayCard( &mainDeck, c, infos.gameCounter, infos.nbJoueurs ) == true ) {
         break;
      }
   }
   return (c);
}
/*****************************************************************************/
bool Client::isValid( Card *c )
{
   return(canPlayCard( &mainDeck, c, infos.gameCounter, infos.nbJoueurs ));
}
/*****************************************************************************/
void Client::sendMessage( const QString &message )
{
   QByteArray block;
   QString msg;

   // On ajoute le nick avant le message
   msg = identity.name + "> " + message;
   
   // Préparation de la trame
   QDataStream out( &block, QIODevice::WriteOnly );
   out.setVersion(QT_STREAMVER);
   out << (quint16)0 << (quint8)NET_CLIENT_MSG
       << msg
       << (quint16)0xFFFF;

   out.device()->seek(0);
   out << (quint16)( block.size() - sizeof(quint16) );

   // On envoie la trame au serveur
   socket.write(block);
}
/*****************************************************************************/
/**
 * Demande au serveur de démarrer une nouvelle donne
 */
void Client::sendStart()
{
   QByteArray block;
   QDataStream out( &block, QIODevice::WriteOnly );
   out.setVersion(QT_STREAMVER);
   out << (quint16)0 << (quint8)NET_CLIENT_DONNE
       << (quint16)0xFFFF;

   out.device()->seek(0);
   out << (quint16)( block.size() - sizeof(quint16) );

   // On envoie la trame au serveur
   socket.write(block);
}
/*****************************************************************************/
/**
 * Le client envoie son choix d'enchère
 */
void Client::sendEnchere( Contrat c )
{
   QByteArray block;
   QDataStream out( &block, QIODevice::WriteOnly );
   out.setVersion(QT_STREAMVER);
   out << (quint16)0 << (quint8)NET_CLIENT_ENCHERE
       << (quint8)c
       << (quint16)0xFFFF;

   out.device()->seek(0);
   out << (quint16)( block.size() - sizeof(quint16) );

   // On envoie la trame au serveur
   socket.write(block);
}
/*****************************************************************************/
/**
 * On envoie le Chien au serveur
 */
void Client::sendChien()
{
   int i;

   QByteArray block;
   QDataStream out( &block, QIODevice::WriteOnly );
   out.setVersion(QT_STREAMVER);
   out << (quint16)0 << (quint8)NET_CLIENT_CHIEN;
   
   for( i=0; i<chien.count(); i++ ) {
      out << (quint8)chien.at(i)->getId();
   }
   
   out << (quint16)0xFFFF;
   out.device()->seek(0);
   out << (quint16)( block.size() - sizeof(quint16) );

   // On envoie la trame au serveur
   socket.write(block);
}
/*****************************************************************************/
/**
 * On envoie la carte jouée
 */
void Client::sendCard( Card *c )
{
   QByteArray block;
   QDataStream out( &block, QIODevice::WriteOnly );
   out.setVersion(QT_STREAMVER);
   out << (quint16)0 << (quint8)NET_CLIENT_CARTE
       << (quint8)c->getId()
       << (quint16)0xFFFF;

   out.device()->seek(0);
   out << (quint16)( block.size() - sizeof(quint16) );

   // On envoie la trame au serveur
   socket.write(block);
}
/*****************************************************************************/
/**
 * Demande au serveur de démarrer une nouvelle donne
 */
void Client::sendVuChien()
{
   QByteArray block;
   QDataStream out( &block, QIODevice::WriteOnly );
   out.setVersion(QT_STREAMVER);
   out << (quint16)0 << (quint8)NET_CLIENT_VU_CHIEN
       << (quint16)0xFFFF;

   out.device()->seek(0);
   out << (quint16)( block.size() - sizeof(quint16) );

   // On envoie la trame au serveur
   socket.write(block);
}
/*****************************************************************************/
/**
 * On prévient le serveur qu'on a bien vu toutes les cartes du pli
 */
void Client::sendVuPli()
{
   QByteArray block;
   QDataStream out( &block, QIODevice::WriteOnly );
   out.setVersion(QT_STREAMVER);
   out << (quint16)0 << (quint8)NET_CLIENT_VU_PLI
       << (quint16)0xFFFF;

   out.device()->seek(0);
   out << (quint16)( block.size() - sizeof(quint16) );

   // On envoie la trame au serveur
   socket.write(block);
}
/*****************************************************************************/
void Client::socketReadData()
{
   QDataStream in( &socket );
   quint16 blockSize = 0;
   bool trameEnPlus = false;
   unsigned int total=0;
   qint64 bytes = socket.bytesAvailable();

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

      // On décode le trame reçue
      doAction( in );
      blockSize = 0;
   }

}
/*****************************************************************************/
/**
 * On agit en fonction du type de bloc reçu
 */
void Client::doAction( QDataStream &in )
{
   quint8 type;  // type de trame

   in >> type;
   switch( type ) {

      /**
       * On reçoit un message de quelqu'un
       */
      case NET_MESSAGE:
      {
         QString message;
         in >> message;
         emit sgnlMessage(message);
         break;
      }

      /**
       * Le serveur nous demande notre identité
       */
      case NET_IDENTIFICATION:
      {
         QByteArray block;
         QFileInfo fi(identity.avatar);
         QDataStream out( &block, QIODevice::WriteOnly );
         out.setVersion( 5 );
         out << (quint16)0 << (quint8)NET_CLIENT_INFOS
             << QString( TAROT_VERSION ) // version de TarotClub pour de futurs tests de compatibilité
             << identity.name
             << fi.fileName()
             << identity.quote
             << (quint8)identity.sex
             << (quint16)0xFFFF;

         out.device()->seek(0);
         out << (quint16)( block.size() - sizeof(quint16) );
         socket.write(block);
         break;
      }

      /**
       * On reçoit la liste des joueurs qui sont connectés
       */
      case NET_LISTE_JOUEURS:
      {
         quint8 nombre;
         Identity idents[5];
         quint8 sex;

         in >> nombre;
         for( int i=0; i< nombre; i++) {
            in >> idents[i].name;
            in >> idents[i].avatar;
            in >> idents[i].quote;
            in >> sex;
            idents[i].sex = (SexType)sex;
         }
         emit sgnlListeDesJoueurs( nombre, idents );
         break;
      }

      /**
       * On reçoit ses cartes et les paramètres du jeu
       */
      case NET_RECEPTION_CARTES:
      {
         int i;
         quint8 n, p;
         int nb_cartes;

         in >> p;
         in >> n;
         if( n == 3 ) {
            nb_cartes = 24;
         } else if( n == 4 ) {
            nb_cartes = 18;
         } else {
            nb_cartes = 15;
         }
         infos.nbJoueurs = n;
         infos.place = (Place)p;
         myDeck.clear();
         for( i=0; i<nb_cartes; i++ ) {
            in >> n;
            myDeck.append(Jeu::getCard( n ));
         }
         // on vide le stream
         for( i=0; i<(24-nb_cartes); i++ ) {
            in >> n;
         }
         emit sgnlReceptionCartes();
         break;
      }

      /**
       * Le serveur nous indique à qui est le tour
       */
      case NET_SELECTION_JOUEUR:
      {
         quint8 p;

         in >> p;
         infos.gameCounter++;
         emit sgnlAfficheSelection((Place)p);
         break;
      }

      /**
       * Le serveur nous demande notre enchère
       */
      case NET_DEMANDE_ENCHERE:
      {
         quint8 c;
         
         in >> c; // contrat le plus élevé annoncé précédemment
         emit sgnlChoixEnchere((Contrat)c);
         break;
      }

      /**
       * On reçoit l'enchère proposée par un joueur
       */
      case NET_ENCHERE_JOUEUR:
      {
         qint8 c, p;
         in >> p;
         in >> c;
         emit sgnlAfficheEnchere( (Place)p, (Contrat)c );
         break;
      }

      
      /**
       * Le serveur nous montre le chien
       */
      case NET_MONTRE_CHIEN:
      {
         int nb_cartes_chien, i;
         qint8 carte_id;

         if( infos.nbJoueurs == 3 ) {
            nb_cartes_chien = 6;
         } else if( infos.nbJoueurs == 4 ) {
            nb_cartes_chien = 6;
         } else { // 5 joueurs
            nb_cartes_chien = 3;
         }
         chien.clear();
         for( i=0; i<nb_cartes_chien; i++ ) {
            in >> carte_id;
            chien.append( Jeu::getCard(carte_id) );
         }
         // on vide le stream
         if( nb_cartes_chien == 3 ) {
            in >> carte_id;in >> carte_id;in >> carte_id;
         }
         chien.sort();
         emit sgnlAfficheChien();
         break;
      }
      
      /**
       * Le serveur nous demande de faire notre chien
       */
      case NET_FAIT_CHIEN:
      {
         emit sgnlPrepareChien();
         break;
      }

      /**
       * On commence à jouer cette donne
       */
      case NET_DEPART_DONNE:
      {
         qint8 preneur;
         qint8 contrat;

         in >> preneur;
         in >> contrat;
         infos.preneur = (Place)preneur;
         infos.contrat = (Contrat)contrat;
         mainDeck.clear();
         infos.gameCounter = 0;
         emit sgnlDepartDonne((Place)preneur,(Contrat)contrat);
         break;
      }

      /**
       * C'est à notre tour de jouer
       */
      case NET_JOUE_CARTE:
      {
         emit sgnlJoueCarte();
         break;
      }

      /**
       * On reçoit la carte jouée par un joueur
       */
      case NET_MONTRE_CARTE:
      {
         quint8 id;
         quint8 tour;
         
         in >> id;
         in >> tour;
         mainDeck.append(Jeu::getCard(id));
         emit sgnlAfficheCarte(id);
         break;
      }

      /**
       * Le serveur nous demande si on a bien vu les cartes de ce tour
       */
      case NET_SERVER_WAIT_PLI:
      {
         emit sgnlWaitPli();
         break;
      }

      /**
       * On prévient la fin de cette donne, on affiche les scores
       */
      case NET_FIN_DONNE:
      {
         quint32 tmp;

         in >> tmp;
         score_inf.attaque = (float)tmp;
         in >> tmp;
         score_inf.defense = (float)tmp;
         in >> tmp;
         score_inf.bouts = (int)tmp;
         in >> tmp;
         score_inf.pointsAFaire = (int)tmp;
         in >> tmp;
         score_inf.difference = (int)tmp;
         in >> tmp;
         score_inf.points_petit_au_bout = (int)tmp;
         in >> tmp;
         score_inf.multiplicateur = (int)tmp;
         in >> tmp;
         score_inf.points_poignee = (int)tmp;
         in >> tmp;
         score_inf.points_chelem = (int)tmp;
         in >> tmp;
         score_inf.points_defense = (int)tmp;

         emit sgnlFinDonne();
         break;
      }

      /**
       * Tous les joueurs ont passé, le serveur nous demande si on veut redistribuer
       */
      case NET_SERVER_REDIST:
      {
         emit sgnlRedist();
         break;
      }

      default:
         emit sgnlMessage( "Paquet reçu non valide." );
         break;
   }
}
/*****************************************************************************/
void Client::connectToHost( const QString &hostName, quint16 port )
{
   socket.connectToHost(hostName,port);
}
/*****************************************************************************/
void Client::socketConnected()
{
   emit sgnlMessage( identity.name + tr(" est connecté.") );
}
/*****************************************************************************/
void Client::socketHostFound()
{
   emit sgnlMessage( identity.name + tr(" se connecte au serveur ...") );
}
/*****************************************************************************/
void Client::socketClosed()
{
   emit sgnlMessage( tr("Le serveur a mis fin à la connexion.") );
}
/*****************************************************************************/
void Client::socketError( QAbstractSocket::SocketError code )
{
   QString message;

   switch( code ) {
      case QAbstractSocket::ConnectionRefusedError:
         message = tr("Erreur réseau : connexion refusée.");
         break;
      case QAbstractSocket::HostNotFoundError:
         message = tr("Erreur réseau : serveur introuvable.");
         break;
      default:
         message = tr("Erreur réseau : la transmission de données a échoué.");
         break;
   }
   emit sgnlMessage( message );
}

//=============================================================================
// End of file Client.cpp
//=============================================================================
