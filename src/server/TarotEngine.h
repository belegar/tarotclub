/*=============================================================================
 * Tarot Club - TarotEngine.h
 *=============================================================================
 * Moteur de jeu principal + serveur de jeu réseau
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
#ifndef _TAROTENGINE_H
#define _TAROTENGINE_H

#include <QtNetwork>
#include <QMap>
#include "../base/Card.h"
#include "../base/Deck.h"
#include "../base/Player.h"
#include "../base/Score.h"
#include "../base/Jeu.h"
#include "../defines.h"

/*****************************************************************************/
class TarotEngine : public QThread
{
   Q_OBJECT
   
private:
   Player      players[5]; // Données des joueurs
   QMap<Place, QTcpSocket *> clients; // les clients connectés
   QTcpServer  server;

   QString     gamePath;
   GameInfos   infos;
   Score       score;
   Deck        deckChien;     // le chien
   Deck        mainDeck;      // le tas principal
   Place       donneur;       // qui est le donneur ?
   Place       tour;          // A qui le tour (d'enchérir ou de jouer)
   Sequence    sequence;      // indique la séquence de jeu actuelle
   bool        newGame;       // vrai si une nouvelle partie a été commencée
   int         dealNumber;
   int         cptVu;

protected:
   void customEvent( QEvent *e );

public:
   TarotEngine();
    ~TarotEngine();

   void run();

   int   getConnectedPlayers( Identity *idents );
   int   getConnectedNumber();
   void  newServerGame( int port );
   void  closeServerGame();
   void  closeClients();
   bool  cardIsValid( Card *c, Place p );
   bool  cardExists( Card *c, Place p );
   void  distribution();
   void  addPlayer( Place p, Identity *ident );
   Place calculGagnantPli();
   void  nouvelleDonne();
   void  jeu();
   void  jeuNext();
   Place nextPlayer( Place j );
   bool  finLevee();
   void  sequenceEncheres();
   void  montreChien();

   // Fonctions réseau
   void  sendCards( Place p, quint8 *params );
   void  askBid( Place p, Contrat c );
   void  sendBid( Place p, Contrat c );
   void  broadcast( QByteArray &block );
   void  doAction( QDataStream &in, Place p );
   void  sendMessage( const QString &message, Place p );
   void  readData( Place p );
   void  clientClosed(Place p );
   void  sendPlayersList();
   void  sendShowChien();
   void  sendDoChien();
   void  sendJoueCarte();
   void  sendCard( Card *c );
   void  sendDepartDonne();
   void  sendRedist();
   void  sendFinDonne( ScoreInfos *score_inf );
   void  sendWaitPli();
   void  selectPlayer( Place p );

public slots:
   void  newConnection();
   void  clientClosed1();
   void  clientClosed2();
   void  clientClosed3();
   void  clientClosed4();
   void  readData1();
   void  readData2();
   void  readData3();
   void  readData4();

};

#endif // _TAROTENGINE_H

//=============================================================================
// End of file TarotEngine.h
//=============================================================================
