

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
