/*=============================================================================
 * TarotClub - Deck.cpp
 *=============================================================================
 * A deck composed of a (dynamic) list of cards.
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
