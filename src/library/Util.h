/*=============================================================================
 * TarotClub - Util.h
 *=============================================================================
 * General purpose system methods
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

#ifndef UTIL_H
#define UTIL_H

#include <string>
#include <vector>
#include <cstdint>

/*****************************************************************************/
class Util
{
public:
#ifdef USE_WINDOWS_OS
    static const char DIR_SEPARATOR = '\\';
#else
    static const char DIR_SEPARATOR = '/';
#endif

    static std::string CurrentDateTime(const std::string &format);
    static std::string ExecutablePath();
    static std::string HomePath();
    static bool FolderExists(const std::string &foldername);
    static bool Mkdir(const std::string &fullPath);
    static void ReplaceCharacter(std::string &theString, const std::string &toFind, const std::string &toReplace);
    static std::vector<std::string> Split(const std::string &theString, const std::string &delimiter);
    static std::string Join(const std::vector<std::string> &tokens, const std::string &delimiter);
    static std::int32_t GetCurrentMemoryUsage();
    static std::int32_t GetMaximumMemoryUsage();
    static std::string GetFileName(const std::string &path);
    static std::string GetDirectoryPath(const std::string &path);
};

#endif // UTIL_H

//=============================================================================
// End of file Util.h
//=============================================================================
