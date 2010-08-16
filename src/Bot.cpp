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
#include "LuaBot.h"

#include <iostream>
using namespace std;

/*****************************************************************************/
Bot::Bot() : Client()
{
   connect( this, SIGNAL(sgnlMessage(const QString &)), this, SLOT(slotMessage(const QString &)));
   connect( this, SIGNAL(sgnlReceptionCartes()), this, SLOT(slotReceptionCartes()));
   connect( this, SIGNAL(sgnlAfficheSelection(Place)), this, SLOT(slotAfficheSelection(Place)));
   connect( this, SIGNAL(sgnlChoixEnchere(Contrat)), this, SLOT(slotChoixEnchere(Contrat)));
   connect( this, SIGNAL(sgnlAfficheEnchere(Place,Contrat)), this, SLOT(slotAfficheEnchere(Place,Contrat)));
   connect( this, SIGNAL(sgnlRedist()), this, SLOT(slotRedist()));
   connect( this, SIGNAL(sgnlAfficheChien()), this, SLOT(slotAfficheChien()));
   connect( this, SIGNAL(sgnlPrepareChien()), this, SLOT(slotPrepareChien()));
   connect( this, SIGNAL(sgnlDepartDonne(Place,Contrat)), this, SLOT(slotDepartDonne(Place,Contrat)));
   connect( this, SIGNAL(sgnlJoueCarte()), this, SLOT(slotJoueCarte()));
   connect( this, SIGNAL(sgnlAfficheCarte(int, Place)), this, SLOT(slotAfficheCarte(int, Place)));
   connect( this, SIGNAL(sgnlFinDonne(Place, float, bool)), this, SLOT(slotFinDonne(Place, float, bool)));
   connect( this, SIGNAL(sgnlWaitPli(Place, float)), this, SLOT(slotWaitPli(Place, float)));

   timeBeforeSend.setSingleShot(true);
   timeBeforeSend.setInterval(0);
   connect(&timeBeforeSend, SIGNAL(timeout()), this, SLOT(slotTimeBeforeSend()));

   L = lua_open();
   luaL_openlibs(L); // open standard Lua libraries

   // Register the LuaGameObject data type with Lua
   Luna<LuaBot>::Register(L);

   // Push a pointer to this Bot to the Lua stack
   lua_pushlightuserdata(L, (void*)this);
   // And set the global name of this pointer
   lua_setglobal(L,"BotObject");
}
/*****************************************************************************/
Bot::~Bot()
{
   lua_close(L);
}
/*****************************************************************************/
void Bot::setTimeBeforeSend(int t)
{
   timeBeforeSend.setInterval(t);
}
/*****************************************************************************/
void Bot::slotTimeBeforeSend()
{
   Card *c;
   c = play();
   removeCard(c);
   sendCard(c);
}
/*****************************************************************************/
void Bot::slotMessage( const QString &text )
{
   Q_UNUSED(text);
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
   Q_UNUSED(p);
   // nada aussi
}
/*****************************************************************************/
void Bot::slotChoixEnchere( Contrat c )
{
   int ret;
   Contrat mon_contrat;

   // on lance le script lua
   ret = luaL_dofile(L,"bid.lua");
   if( ret ) {
      // on a eu une erreur
      cerr << "Error in Lua script : " << lua_tostring(L, -1) << endl;
      mon_contrat = calculEnchere();
   } else {
      lua_getglobal(L, "CalculateBid");
      if (!lua_pcall(L, 0, 1, 0)) {
         if(lua_isnumber(L, -1)) {
            ret = (int)lua_tonumber(L, -1);
            cerr << "Bid: " << ret;
            // security test
            if (ret<PASSE || ret>GARDE_CONTRE) {
               mon_contrat = calculEnchere();
            } else {
               mon_contrat = (Contrat)ret;
            }
         }
         lua_pop(L, 1);
      } else {
         mon_contrat = calculEnchere();
      }
   }

   // only bid over previous one is allowed
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
   sendReady();
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
   Q_UNUSED(p);
   Q_UNUSED(c);
   // nada
}
/*****************************************************************************/
void Bot::slotJoueCarte()
{
   timeBeforeSend.start();
}
/*****************************************************************************/
void Bot::slotAfficheCarte(int id, Place p)
{
   Q_UNUSED(id);
   Q_UNUSED(p);
   // nada
}
/*****************************************************************************/
void Bot::slotFinDonne(Place winner, float pointsTaker, bool lastDeal)
{
   Q_UNUSED(winner);
   Q_UNUSED(pointsTaker);
   Q_UNUSED(lastDeal);
   if (lastDeal == false) {
      sendReady();
   }
}
/*****************************************************************************/
void Bot::slotWaitPli(Place winner, float pointsTaker)
{
   Q_UNUSED(winner);
   Q_UNUSED(pointsTaker);
   sendVuPli();
}

//=============================================================================
// End of file Bot.cpp
//=============================================================================
