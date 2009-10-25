/*=============================================================================
 * TarotClub - Jeu.h
 *=============================================================================
 * Static class in memory to store cards of the game.
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

#ifndef _JEU_H
#define _JEU_H

#include "Card.h"

/*****************************************************************************/
class Jeu
{
   static Card cards[78];
public:
   Jeu();
   static void init();
   static Card *getCard(int id);
};

#endif // _JEU_H

//=============================================================================
// End of file Jeu.h
//=============================================================================
