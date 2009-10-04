/*=============================================================================
 * Tarot Club - Deck.h
 *=============================================================================
 * A deck composed of a (dynamic) list of cards.
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

#ifndef _DECK_H
#define _DECK_H

// Qt includes
#include <QList>

// Game includes
#include "Card.h"
#include "defines.h"


/*****************************************************************************/
class Deck : public QList<Card *>
{

public:
   // Constructeurs et destructeur
   Deck();

   unsigned int   shuffle( bool , unsigned int );  // Mélange le jeu
   Card           *getCardById( int );             // Renvoi un pointeur vers une carte
   void           sort();
   static bool    lessThanCards(Card *carte1, Card *carte2 );
};

#endif // _DECK_H

//=============================================================================
// End of file Deck.h
//=============================================================================
