/*=============================================================================
 * TarotClub - Card.h
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

#ifndef _CARD_H
#define _CARD_H

// Game includes
#include "defines.h"
#include <QString>

/*****************************************************************************/
class Card
{

private:
   CardType    cardType;   // Type de carte (voir enum correspondant)
   CardColor   color;      // insignifiant si la carte est un atout ou l'excuse
   float       points;     // points de la carte
   int         id;         // identificateur unique, usage multiple (classement, etc.)
   Place       owner;      // A qui appartient cette carte (OUEST, EST ...)
   int         value;

   // valeur :
   // si excuse : insignifiant
   // Si atout : [1..21]
   // Pour les autres cartes : [1..14]
   //    avec :
   //          Roi         = 14
   //          Dame     = 13
   //          Cavalier = 12
   //          Valet    = 11

public:
   // Mutateurs
   void  setType( CardType t );
   void  setColor( CardColor );
   void  setPoints( float);
   void  setId( int );
   void  setOwner( Place );
   void  setValue( int );

   // Accesseurs
   CardType    getType();
   CardColor   getColor();
   float       getPoints();
   int         getId();
   Place       getOwner();
   int         getValue();
   QString     getCardName();

};

#endif // _CARD_H

//=============================================================================
// End of file Card.h
//=============================================================================
