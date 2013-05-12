/*=============================================================================
 * TarotClub - Card.cpp
 *=============================================================================
 * Abstract class representing Tarot cards caracteristics
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

#include "Card.h"

/*****************************************************************************/
QString Card::getCardName()
{
   QString name;
   QTextStream stream(&name);

   stream << value;
   if (cardType == ATOUT || (cardType == EXCUSE))
   {
      stream << "-A";
   }
   else
   {
      // Color
      if (color == PIC)
      {
         stream << "-S"; // spades
      }
      else if (color == COEUR)
      {
         stream << "-H"; // heart
      }
      else if (color == TREFLE)
      {
         stream << "-C"; // club
      }
      else
      {
         stream << "-D"; // diamond
      }
   }
   return name;
}
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
