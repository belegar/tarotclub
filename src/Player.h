/*=============================================================================
 * TarotClub - Player.h
 *=============================================================================
 * Stores the identity of a player
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

#ifndef _PLAYER_H
#define _PLAYER_H

#include <QObject>
#include "defines.h"
#include "Deck.h"
#include "Identity.h"

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
   void setIdentity( Identity &ident );
   void setPlace(Place p);

   QString getName();
   QString getQuote();
   SexType getSex();
   QString getAvatar();
   Identity *getIdentity();
   Place getPlace();

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
   Card *GetCardByName(const QString &i_name);
};

#endif // _PLAYER_H

//=============================================================================
// End of file Player.h
//=============================================================================
