/*=============================================================================
 * TarotClub - Deck.cpp
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

#include "Deck.h"
// Standard includes
#ifndef QT_NO_DEBUG
#include <iostream>
#include <fstream>
#endif // QT_NO_DEBUG

#include <QtAlgorithms>

#include <cstdlib>
#include <ctime>
using namespace std;


/*****************************************************************************/
Deck::Deck()
{

}
/*****************************************************************************/
QString Deck::GetCardList()
{
    QString list;

    for(int i=0; i<size(); i++)
    {
        list += at(i)->getCardName();
        if (i < (size()-1))
        {
            list += ";";
        }
    }
    return list;
}
/*****************************************************************************/
void Deck::shuffle(int seed)
{
    for ( int i = size(); i > 0; --i )
    {
        // pseudorandom number generation algorithm
        // taken from KDE game KPat, thanks !
        seed = 214013 * seed + 2531011;
        int rand = ( seed >> 16 ) & 0x7fff;
        int z = rand % i;
        swap(z, i-1);
    }
}
/*****************************************************************************/
Card *Deck::getCardById( int id )
{
    for( int i = 0; i<this->size(); ++i) {
        if( this->at(i)->getId() == id )
            return this->at(i);
    }
    return (NULL);
}
/*****************************************************************************/
void Deck::sort()
{
    qSort( this->begin(), this->end(), lessThanCards );
}
/*****************************************************************************/
bool Deck::lessThanCards(Card *carte1, Card *carte2 )
{
    return carte1->getId() > carte2->getId();
}
//=============================================================================
// End of file Deck.cpp
//=============================================================================
