/*=============================================================================
 * TarotClub - Bot.cpp
 *=============================================================================
 * Modélise un robot joueur. Interface vers les scripts d'IA
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

#include "Bot.h"

/*****************************************************************************/
Bot::Bot() : Client()
{
   type = BOT;

   connect( this, SIGNAL(sgnlMessage(const QString &)), this, SLOT(message(const QString &)));
   connect( this, SIGNAL(sgnlReceptionCartes()), this, SLOT(slotReceptionCartes()));
   connect( this, SIGNAL(sgnlAfficheSelection(Place)), this, SLOT(slotAfficheSelection(Place)));
   connect( this, SIGNAL(sgnlChoixEnchere(Contrat)), this, SLOT(slotChoixEnchere(Contrat)));
   connect( this, SIGNAL(sgnlAfficheEnchere(Place,Contrat)), this, SLOT(slotAfficheEnchere(Place,Contrat)));
   connect( this, SIGNAL(sgnlRedist()), this, SLOT(slotRedist()));
   connect( this, SIGNAL(sgnlAfficheChien()), this, SLOT(slotAfficheChien()));
   connect( this, SIGNAL(sgnlPrepareChien()), this, SLOT(slotPrepareChien()));
   connect( this, SIGNAL(sgnlDepartDonne(Place,Contrat)), this, SLOT(slotDepartDonne(Place,Contrat)));
   connect( this, SIGNAL(sgnlJoueCarte()), this, SLOT(slotJoueCarte()));
   connect( this, SIGNAL(sgnlAfficheCarte(int)), this, SLOT(slotAfficheCarte(int)));
   connect( this, SIGNAL(sgnlFinDonne()), this, SLOT(slotFinDonne()));
   connect( this, SIGNAL(sgnlWaitPli()), this, SLOT(slotWaitPli()));
}
/*****************************************************************************/
void Bot::message( const QString &text )
{
   // un bot ne sait pas répondre (encore :)
}
/*****************************************************************************/
void Bot::slotReceptionCartes()
{
   updateStats();
}
/*****************************************************************************/
void Bot::slotAfficheSelection( Place p )
{
   // nada aussi
}
/*****************************************************************************/
void Bot::slotChoixEnchere( Contrat c )
{
   Contrat mon_contrat = calculEnchere();
   if( mon_contrat <= c ) {
      mon_contrat = PASSE;
   }
   sendEnchere(mon_contrat);
}
/*****************************************************************************/
void Bot::slotAfficheEnchere( Place, Contrat )
{
   // nada
}
/*****************************************************************************/
void Bot::slotRedist()
{
   // nada
}
/*****************************************************************************/
void Bot::slotAfficheChien()
{
   sendVuChien();
}
/*****************************************************************************/
void Bot::slotPrepareChien()
{
   choixChien(&chien);
   sendChien();
}
/*****************************************************************************/
void Bot::slotDepartDonne(Place p,Contrat c)
{
   // nada
}
/*****************************************************************************/
void Bot::slotJoueCarte()
{
   Card *c;
   c = play();
   removeCard(c);
   sendCard(c);
}
/*****************************************************************************/
void Bot::slotAfficheCarte(int id)
{
   // nada
}
/*****************************************************************************/
void Bot::slotFinDonne()
{
   // nada
}
/*****************************************************************************/
void Bot::slotWaitPli()
{
   sendVuPli();
}

//=============================================================================
// End of file Bot.cpp
//=============================================================================
