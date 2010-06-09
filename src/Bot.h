/*=============================================================================
 * TarotClub - Bot.h
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

#ifndef _BOT_H
#define _BOT_H

#include "Client.h"

/*****************************************************************************/
class Bot : public Client
{
   Q_OBJECT

private:
   QTimer  timeBeforeSend;

public:
   Bot();

   void setTimeBeforeSend(int t);

public slots:
   void slotTimeBeforeSend();

   // client events
   void slotMessage( const QString &text );
   void slotReceptionCartes();
   void slotAfficheSelection( Place p );
   void slotChoixEnchere( Contrat c );
   void slotAfficheEnchere( Place, Contrat );
   void slotRedist();
   void slotAfficheChien();
   void slotPrepareChien();
   void slotDepartDonne(Place p,Contrat c);
   void slotJoueCarte();
   void slotAfficheCarte(int id, Place p);
   void slotFinDonne();
   void slotWaitPli(Place);
};

#endif // _BOT_H

//=============================================================================
// End of file Bot.h
//=============================================================================
