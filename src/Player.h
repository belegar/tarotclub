/*=============================================================================
 * Tarot Club - Player.h
 *=============================================================================
 * Stores the identity of a player
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

#ifndef _PLAYER_H
#define _PLAYER_H

#include <QObject>
#include "../defines.h"
#include "../base/Deck.h"

/*****************************************************************************/
class Player : public QObject
{

protected:
   Identity    identity;   // identité du joueur
   Deck        myDeck;     // cartes en main

public:
   Player();

   // Opérations sur l'identité
   void setName( const QString & );
   void setQuote( const QString & );
   void setSex( SexType );
   void setAvatar( const QString & );
   void setIdentity( Identity *ident );
   QString getName();
   QString getQuote();
   SexType getSex();
   QString getAvatar();
   Identity *getIdentity();

   // Opérations sur les cartes du joueur
   void tidyDeck();
   void addCard( Card * );
   void removeCard( Card * );
   void emptyDeck();
   bool cardExists( Card * );
   bool canPlayCard( Deck *, Card *, int, int);
   int getCardNumber();
   Card *getCard( int );
   Card *getCardById( int );
};

#endif // _PLAYER_H

//=============================================================================
// End of file Player.h
//=============================================================================
