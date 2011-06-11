/*=============================================================================
 * TarotClub - LuaBot.h
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

#ifndef _LUABOT_H_
#define _LUABOT_H_

// Need to include lua headers this way
extern "C" {
   #include "lua.h"
   #include "lauxlib.h"
   #include "lualib.h"
}

// I am using luna
#include "luna.h"
#include "Bot.h"

/*****************************************************************************/
class LuaBot
{
public:
  // Constants
  static const char className[];
  static Luna<LuaBot>::RegType methods[];

  // Initialize the pointer
  LuaBot(lua_State *L);
  ~LuaBot();

  // Methods we will need to use
  int getCardNumber(lua_State *L);


private:
   // The pointer to the 'real object' defined in Bot.cpp
   Bot *bot;
};

#endif // _LUABOT_H_

//=============================================================================
// End of file LuaBot.h
//=============================================================================
