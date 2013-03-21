/*=============================================================================
 * TarotClub - Card.cpp
 *=============================================================================
 * Abstract class representing Tarot cards caracteristics
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
