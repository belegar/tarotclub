/*=============================================================================
 * TarotClub - defines.h
 *=============================================================================
 * Global types
 *=============================================================================
 * TarotClub ( http://www.tarotclub.fr ) - This file is part of TarotClub
 * Copyright (C) 2003-2999 - Anthony Rabine
 * anthony@tarotclub.fr
 *
 * TarotClub is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * TarotClub is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with TarotClub.  If not, see <http://www.gnu.org/licenses/>.
 *
 *=============================================================================
 */

#ifndef _DEFINES_H
#define _DEFINES_H

#include <string>
#include <vector>
#include "Common.h"
#include "Util.h"

/*****************************************************************************/
// Game definitions

/**
 * @brief The version string uses Semantic Versioning format
 * @see http://semver.org
 */
#define TAROT_VERSION   "2.3.0-alpha.1"
#define TAROT_TITRE     "TarotClub"
#define TAROT_VNAME     "Christmas"

#define NB_LANGUAGE     2
#define MAX_ROUNDS      5

/*****************************************************************************/
enum Team       { ATTACK = 0, DEFENSE = 1, NO_TEAM = 0xFF };
enum Handle     { SIMPLE_HANDLE = 0, DOUBLE_HANDLE = 1, TRIPLE_HANDLE = 2 };
/*****************************************************************************/

namespace Config
{

#ifdef TAROT_DEBUG
const std::string HomePath  = "./";
#else
const std::string HomePath  = Util::HomePath() + "/.tarotclub/";
#endif
const std::string GamePath  = HomePath + "/games/";
const std::string LogPath   = HomePath + "/logs/";

} // namespace config


#endif // _DEFINES_H

//=============================================================================
// End of file defines.h
//=============================================================================
