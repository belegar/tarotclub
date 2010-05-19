/*=============================================================================
 * TarotClub - Card.h
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
