/*=============================================================================
 * Tarot Club - Card.cpp
 *=============================================================================
 * Represents a Tarot cards, derivated from a Qt sprite
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

#include "Card.h"

/*****************************************************************************/
void Card::setType( CardType t )
{
   cardType = t;
}
/*****************************************************************************/
void Card::setColor( CardColor cardColor )
{
   color = cardColor;
}
/*****************************************************************************/
void Card::setPoints( float cardPoints )
{
   points = cardPoints;
}
/*****************************************************************************/
void Card::setId( int i )
{
   id = i;
}
/*****************************************************************************/
void Card::setOwner( Place p )
{
   owner = p;
}
/*****************************************************************************/
void Card::setValue( int cardValue )
{
   value = cardValue;
}
/*****************************************************************************/
CardType Card::getType()
{
   return cardType;
}
/*****************************************************************************/
CardColor Card::getColor()
{
   return color;
}
/*****************************************************************************/
float Card::getPoints()
{
   return points;
}
/*****************************************************************************/
int Card::getId()
{
   return id;
}
/*****************************************************************************/
Place Card::getOwner()
{
   return owner;
}
/*****************************************************************************/
int Card::getValue()
{
   return value;
}

//=============================================================================
// End of file Card.h
//=============================================================================
