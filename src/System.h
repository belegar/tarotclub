/*=============================================================================
 * TarotClub - System.h
 *=============================================================================
 * Manages system path for resources and generated files
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
#ifndef SYSTEM_H_
#define SYSTEM_H_

#include <string>
#include "Util.h"

/*****************************************************************************/
class System
{
public:
    static std::string HomePath()
    {
        return mHomePath;
    }
    static std::string GamePath()
    {
        return mHomePath + "games" + Util::DIR_SEPARATOR;
    }
    static std::string LogPath()
    {
        return mHomePath + "logs" + Util::DIR_SEPARATOR;
    }
    static std::string LocalePath()
    {
        return mLocalePath;
    }

    /**
     * @brief DeckPath, where the card images are located
     * @return The path
     */
    static std::string DeckPath()
    {
        return mDeckPath;
    }
    static std::string ScriptPath()
    {
        return mScriptPath;
    }

    static void Initialize();

private:
    System(); // forbid any instance

    static std::string mHomePath;
    static std::string mDeckPath;
    static std::string mScriptPath;
    static std::string mLocalePath;
};

#endif // SYSTEM_H_

//=============================================================================
// End of file System.h
//=============================================================================
