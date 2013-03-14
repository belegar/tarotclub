/*=============================================================================
 * TarotClub - Player.h
 *=============================================================================
 * Stores the identity of a player
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
