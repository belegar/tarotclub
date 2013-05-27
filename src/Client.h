/*=============================================================================
 * TarotClub - Client.h
 *=============================================================================
 * This class manages the network protocol and is used to join a online game
 *=============================================================================
 * TarotClub ( http://www.tarotclub.fr ) - This file is part of TarotClub
 * Copyright (C) 2003-2999 - Anthony Rabine
 * anthony@tarotclub.fr
 *
 * TarotClub is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * TarotClub is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with TarotClub.  If not, see <http://www.gnu.org/licenses/>.
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
#include "defines.h"
#include "Score.h"
#include "GameState.h"

class Client : public Player
{
    Q_OBJECT

protected:
    QTcpSocket  socket;
    DeckStats   stats;      // statistics on player's cards
    GameState    info;       // Helper class to store various game information
    Score       score;
    Deck        chien;
    Deck        mainDeck;
    Deck        poignee;    // declared poignee by the player

public:
    Client();

    void        init()
    {
        score.init();
    }
    Contrat     calculEnchere();
    void        razStats();
    void        updateStats();
    Card        *play();
    bool        isValid(Card *c);
    DeckStats   *getStats();
    GameState    &GetGameState();
    Score       &GetScore();

    // opérations sur le deck principal
    Card *GetMainDeckCard(int i);
    int GetMainDeckSize();

    // opérations sur le deck Chien
    void emptyChien();
    Card *getCardChien(int i);
    void addCardChien(Card *c);
    void removeCardChien(Card *c);
    int  getTailleChien();
    void choixChien(Deck *);   // méthode qui génère un chien valide au hasard

    // opération sur la poignée
    void emptyPoignee();
    void addCardPoignee(Card *c);
    void removeCardPoignee(Card *c);
    int  getTaillePoignee();
    bool testPoignee();

    // Réseau
    void connectToHost(const QString &hostName, quint16 port);
    void close();
    void sendIdentity();
    void sendMessage(const QString &message);
    void sendEnchere(Contrat c);
    void sendChien();
    void sendPoignee();
    void sendCard(Card *c);
    void sendReady();
    void sendError();
    void sendVuChien();
    void sendVuPli();
    void doAction(QDataStream &in);

public slots:
    void socketReadData();
    void socketConnected();
    void socketHostFound();
    void socketClosed();
    void socketError(QAbstractSocket::SocketError code);

signals:

   /**
    * @brief Emitted when a chat message has been broadcasted by one player or the server
    * @param message
    */
    void sgnlMessage(const QString &message);

   /**
    * @brief Emitted when the server has assigned a place around the table
    * @param p Place assigned by the server
    */
   void sgnlAssignedPlace(Place p);
    void sgnlListeDesJoueurs(QList<Identity> players);
    void sgnlReceptionCartes();
    void sgnlAfficheSelection(Place);
    void sgnlChoixEnchere(Contrat);
    void sgnlAfficheEnchere(Place, Contrat); // Affiche le 'Contrat' du joueur 'Place'
    void sgnlDepartDonne(Place, Contrat); // Place indique le preneur, Contrat indique le contrat finalement adopté
    void sgnlAfficheChien();
    void sgnlPrepareChien();
    void sgnlRedist();
    void sgnlJoueCarte();
    void sgnlAfficheCarte(int, Place);
    void sgnlFinDonne(Place, float, bool lastDeal);
    void sgnlWaitPli(Place, float);

};

#endif // _CLIENT_H

//=============================================================================
// End of file Client.h
//=============================================================================
