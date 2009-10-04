/*=============================================================================
 * Tarot Club - Bot.h
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

#ifndef _BOT_H
#define _BOT_H

#include "Client.h"

/*****************************************************************************/
class Bot : public Client
{
   Q_OBJECT

private:

   
public:
   Bot();

public slots:
   void message( const QString &text );
   void slotReceptionCartes();
   void slotAfficheSelection( Place p );
   void slotChoixEnchere( Contrat c );
   void slotAfficheEnchere( Place, Contrat );
   void slotRedist();
   void slotAfficheChien();
   void slotPrepareChien();
   void slotDepartDonne(Place p,Contrat c);
   void slotJoueCarte();
   void slotAfficheCarte(int id);
   void slotFinDonne();
   void slotWaitPli();
};

#endif // _BOT_H

//=============================================================================
// End of file Bot.h
//=============================================================================
