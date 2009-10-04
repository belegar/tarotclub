/*=============================================================================
 * Tarot Club - Client.h
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

#ifndef _CLIENT_H
#define _CLIENT_H

#include <QTcpSocket>
#include "Card.h"
#include "Deck.h"
#include "Player.h"
#include "Jeu.h"
#include "../defines.h"

class Client : public Player
{
   Q_OBJECT

protected:
   QTcpSocket  socket;
   PlayerType  type;       // Type du joueur (humain, IA, ou réseau)
   DECK_STATS  stats;      // statistiques sur les cartes en main du joueur
   GameInfos   infos;      // Informations sur le jeu en cours
   ScoreInfos  score_inf;
   Deck        chien;
   Deck        mainDeck;
   
public:
   Client();

   Contrat  calculEnchere();
   void     razStats();
   void     updateStats();
   Card     *play();
   bool     isValid( Card *c );
   void     setType( PlayerType );
   PlayerType  getType();
   DECK_STATS  *getStats();
   GameInfos *getGameInfos();
   ScoreInfos *getScoreInfos();

   // opérations sur le deck principal
   Card *getCardMainDeck(int i);

   // opérations sur le deck Chien
   void  emptyChien();
   Card  *getCardChien(int i);
   void  addCardChien(Card *c);
   void  removeCardChien(Card *c);
   int   getTailleChien();
   void  choixChien( Deck * ); // méthode qui génère un chien valide au hasard

   // Réseau
   void connectToHost( const QString &hostName, quint16 port );
   void sendMessage( const QString &message );
   void sendEnchere( Contrat c );
   void sendChien();
   void sendCard( Card *c );
   void sendStart();
   void sendVuChien();
   void sendVuPli();
   void doAction( QDataStream &in );

public slots:
   void socketReadData();
   void socketConnected();
   void socketHostFound();
   void socketClosed();
   void socketError( QAbstractSocket::SocketError code );

signals:
   void sgnlMessage(const QString &message);
   void sgnlListeDesJoueurs( quint8 nombre, Identity *idents );
   void sgnlReceptionCartes();
   void sgnlAfficheSelection(Place);
   void sgnlChoixEnchere(Contrat);
   void sgnlAfficheEnchere(Place,Contrat); // Affiche le 'Contrat' du joueur 'Place'
   void sgnlDepartDonne(Place,Contrat); // Place indique le preneur, Contrat indique le contrat finalement adopté
   void sgnlAfficheChien();
   void sgnlPrepareChien();
   void sgnlRedist();
   void sgnlJoueCarte();
   void sgnlAfficheCarte(int);  
   void sgnlFinDonne();
   void sgnlWaitPli();

};

#endif // _CLIENT_H

//=============================================================================
// End of file Client.h
//=============================================================================
