/*=============================================================================
 * TarotClub - TarotEngine.h
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
#ifndef _TAROTENGINE_H
#define _TAROTENGINE_H

#include <QtNetwork>
#include <QMap>
#include <QThread>
#include "Card.h"
#include "Deck.h"
#include "Player.h"
#include "Score.h"
#include "Jeu.h"
#include "defines.h"

enum {
   MsgStartGame = QEvent::User+1,
   MsgRandomDeal,
   MsgStopGame,
   MsgExitGame
};


/*****************************************************************************/
class EvStartGame : public QEvent
{
public:
   EvStartGame() : QEvent( (QEvent::Type)MsgStartGame ) {}
};
/*****************************************************************************/
class EvStopGame : public QEvent
{
public:
   EvStopGame() : QEvent( (QEvent::Type)MsgStopGame ) {}
};
/*****************************************************************************/
class EvExitGame : public QEvent
{
public:
   EvExitGame() : QEvent( (QEvent::Type)MsgExitGame ) {}
};

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
   int         cptVu;         // counter of chien seen by clients

protected:
   void customEvent( QEvent *e );

public:
   TarotEngine();
    ~TarotEngine();

   void run();

   int   getConnectedPlayers( Identity *idents );
   int   getConnectedNumber();
   Score *getScore();
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

signals:
   void  sigPrintMessage(const QString &);

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
