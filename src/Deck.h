/*=============================================================================
 * TarotClub - Deck.h
 *=============================================================================
 * A deck composed of a (dynamic) list of cards.
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
