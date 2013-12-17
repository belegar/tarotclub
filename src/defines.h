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

#ifdef USE_WINDOWS_OS
#include <windows.h>
#endif

#include <ctime>
#include <sstream>
#include <iomanip>
#include <string>
#include <vector>

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

static const std::string STR_PASS           = "Pass";
static const std::string STR_TAKE           = "Take";
static const std::string STR_GUARD          = "Guard";
static const std::string STR_GUARD_WITHOUT  = "Guard without";
static const std::string STR_GUARD_AGAINST  = "Guard against";

static const std::string STR_SUD      = "South";
static const std::string STR_EST      = "East";
static const std::string STR_NORD     = "North";
static const std::string STR_OUEST    = "West";


#define STR_WIN      QObject::tr("Contract succeded by ")
#define STR_LOSE     QObject::tr("Contract failed by ")
#define STR_POINTS   QObject::tr(" points")


/*****************************************************************************/
enum Place      { SOUTH = 0, EAST = 1, NORTH = 2, WEST = 3, FIFTH = 4, NOWHERE = 0xFF };
enum Contract   { PASS = 0, TAKE = 1, GUARD = 2, GUARD_WITHOUT = 3, GUARD_AGAINST = 4 };
enum Team       { ATTACK = 0, DEFENSE = 1, NO_TEAM = 0xFF };
enum Handle     { SIMPLE_HANDLE = 0, DOUBLE_HANDLE = 1, TRIPLE_HANDLE = 2 };
/*****************************************************************************/

class Util
{
public:
    static std::string ToString(Place p)
    {
        std::vector<std::string> places;
        places.push_back(STR_SUD);
        places.push_back(STR_EST);
        places.push_back(STR_NORD);
        places.push_back(STR_OUEST);
        return places[p];
    }

    static std::string ToString(Contract c)
    {
        std::vector<std::string> contracts;
        contracts.push_back(STR_PASS);
        contracts.push_back(STR_TAKE);
        contracts.push_back(STR_GUARD);
        contracts.push_back(STR_GUARD_WITHOUT);
        contracts.push_back(STR_GUARD_AGAINST);
        return contracts[c];
}

    // Get current date/time, format is YYYY-MM-DD.HH:mm:ss
    static std::string CurrentDateTime(const std::string &format)
    {
        std::stringstream datetime;

        time_t rawtime;
        struct tm * timeinfo;
        char buffer [80];

        time (&rawtime);
        timeinfo = localtime(&rawtime);

        strftime (buffer, sizeof(buffer), format.c_str(), timeinfo);

        datetime << buffer;
    /*
     * This code if the C++0x11 way of formating date, but GCC does not support it yet :(
        std::stringstream datetime;
        std::time_t t = std::time(nullptr);
        std::tm tm = *std::localtime(&t);

        datetime << std::put_time(&tm, format);
    */
        return datetime.str();
    }

    static std::string ExecutablePath()
    {
        std::string path;
        std::uint32_t found;

#ifdef USE_WINDOWS_OS
        wchar_t buf[MAX_PATH];

        // Will contain exe path
        HMODULE hModule = GetModuleHandle(NULL);
        if (hModule != NULL)
        {
            // When passing NULL to GetModuleHandle, it returns handle of exe itself
            GetModuleFileName(hModule, buf, (sizeof(buf)));
        }
        std::wstring wstr(buf);
        path = std::string(wstr.begin(), wstr.end());
        found = path.find_last_of("\\");

#elif defined(USE_UNIX_OS)
        char buf[MAX_PATH];
        readlink("/proc/self/exe", buf, sizeof(buf))
        path = buf;
        found = path.find_last_of("/");
#elif defined(USE_APPLE_OS)
        _NSGetExecutablePath(path, &size); // make it compile
#else
    #error "A portable code is needed here"
#endif
        return(path.substr(0, found));
    }

    static std::string HomePath()
    {
        std::string homedir;

#if defined(USE_WINDOWS_OS)
        homedir = getenv("HOMEDRIVE");
        homedir += getenv("HOMEPATH");
#else
        homedir = getenv("HOME");
#endif
        return homedir;
    }
};

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
