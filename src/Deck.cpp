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

    for (int i = 0; i < size(); i++)
    {
        list += at(i)->GetName();
        if (i < (size() - 1))
        {
            list += ";";
        }
    }
    return list;
}
/*****************************************************************************/
void Deck::Shuffle(int seed)
{
    for (int i = size(); i > 0; --i)
    {
        // pseudorandom number generation algorithm
        // taken from KDE game KPat, thanks !
        seed = 214013 * seed + 2531011;
        int rand = (seed >> 16) & 0x7fff;
        int z = rand % i;
        swap(z, i - 1);
    }
}
/*****************************************************************************/
Card *Deck::GetCardById(int id)
{
    for (int i = 0; i < this->size(); i++)
    {
        if (this->at(i)->GetId() == id)
        {
            return this->at(i);
        }
    }
    return NULL;
}
/*****************************************************************************/
Card *Deck::GetCardByName(const QString &i_name)
{
    for (int i = 0; i < this->size(); i++)
    {
        if (this->at(i)->GetName() == i_name)
        {
            return this->at(i);
        }
    }
    return NULL;
}
/*****************************************************************************/
bool Deck::HasCard(Card *c)
{
    if (this->indexOf(c) == -1)
    {
        return false;
    }
    else
    {
        return true;
    }
}
/*****************************************************************************/
void Deck::Sort()
{
    qSort(this->begin(), this->end(), LessThanCards);
}
/*****************************************************************************/
bool Deck::LessThanCards(Card *c1, Card *c2)
{
    return c1->GetId() > c2->GetId();
}
/*****************************************************************************/
void Deck::SetOwner(Team team)
{
    owner = team;
}
/*****************************************************************************/
Team Deck::GetOwner()
{
    return owner;
}

//=============================================================================
// End of file Deck.cpp
//=============================================================================
