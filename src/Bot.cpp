/*=============================================================================
 * Tarot Club - Bot.cpp
 *=============================================================================
 * Modélise un robot joueur. Interface vers les scripts d'IA
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
#ifdef _DEBUG
   mon_contrat = PASSE; // TODO : à effacer
#endif
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
