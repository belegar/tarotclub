/*=============================================================================
 * TarotClub - LuaBot.cpp
 *=============================================================================
 * C++ class interface used in Lua scripts for AI
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

// Define the Lua ClassName
const char LuaBot::className[] = "Bot";

// Define the methods we will expose to Lua
#define method(class, name) {#name, &class::name}
Luna<LuaBot>::RegType LuaBot::methods[] = {
   method(LuaBot, getCardNumber),
   {0,0}
};

/*****************************************************************************/
LuaBot::LuaBot(lua_State *L)
{
  bot = (Bot*)lua_touserdata(L, 1);
}
/*****************************************************************************/
LuaBot::~LuaBot()
{
  cerr << "Deleted Lua object " << (void *)this;
}
/*****************************************************************************/
int LuaBot::getCardNumber(lua_State *L)
{
  lua_pushinteger(L, bot->getCardNumber() );
  return 1; // number of results
}

//=============================================================================
// End of file LuaBot.cpp
//=============================================================================
