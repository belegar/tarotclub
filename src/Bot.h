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
#include <QScriptEngine>
#include <QtScriptTools>

class StatsWrapper: public DeckStats, protected QScriptable
{
   Q_OBJECT
public:

    StatsWrapper() {}

public slots:
     QScriptValue qscript_call(QWidget *parent = 0)
     {
      DeckStats * const iface = new DeckStats();
      return engine()->newQObject(iface, QScriptEngine::AutoOwnership);
     }

};

/*****************************************************************************/
class Bot : public Client
{
   Q_OBJECT

private:
   QTimer  timeBeforeSend;
   QScriptEngine botEngine;
   QScriptEngineDebugger debugger;

   QScriptValue m_thisObject;

public:
   Bot();
   ~Bot();



   void setTimeBeforeSend(int t);

// Private slots are not visible in a QtScript
private slots:
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
   void slotFinDonne(Place, float, bool lastDeal);
   void slotWaitPli(Place p, float points);


// public slots ARE visible in a QtScript
public slots:

    int getToto() { return 9; }


};

#endif // _BOT_H

//=============================================================================
// End of file Bot.h
//=============================================================================
