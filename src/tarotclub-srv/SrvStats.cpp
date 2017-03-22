/*=============================================================================
 * TarotClub - Server.cpp
 *=============================================================================
 * Server, game modes, database and statistics management
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

#include <cctype>
#include <iostream>
#include <string>
#include <sstream>
#include "Util.h"
#include "System.h"
#include "SrvStats.h"
#include "Log.h"
#include "Version.h"

static const std::string cDbFileName = "tcds.sqlite";

#ifdef TAROT_DEBUG
    static const std::uint32_t cRefreshPeriodInSeconds = 60U;
#else
    static const std::uint32_t cRefreshPeriodInSeconds = 600U;
#endif

/*****************************************************************************/
SrvStats::SrvStats(IScriptEngine &jsEngine, IEventLoop &ev, Lobby &lobby)
    : mScriptEngine(jsEngine)
    , mEventLoop(ev)
    , mLobby(lobby)
{

}
/*****************************************************************************/
void SrvStats::Update(const JsonValue &info)
{
    std::string cmd = info.FindValue("cmd").GetString();

    if (cmd == "Event")
    {
        std::string type = info.FindValue("type").GetString();
        if (type == "New")
        {
            IncPlayer();
        }
        else if (type == "Quit")
        {
            DecPlayer();
        }
    }
}
/*****************************************************************************/
std::string SrvStats::GetName()
{
    return "Server statistics";
}
/*****************************************************************************/
void SrvStats::Initialize()
{
    mEventLoop.AddTimer(cRefreshPeriodInSeconds, std::bind(&SrvStats::FireTimer, this, std::placeholders::_1));
}
/*****************************************************************************/
void SrvStats::Stop()
{
    // Nothing to do for now FIXME: remove the timer from the event loop?
}
/*****************************************************************************/
void SrvStats::FireTimer(IEventLoop::Event event)
{
    if (event == IEventLoop::EvTimer)
    {
#ifdef TAROT_DEBUG
        std::cout << "Top event" << std::endl;
#endif

        time_t rawtime;

        time(&rawtime);
        StoreStats(rawtime);

        std::stringstream ss;

        ss << "var Server = {"
           << "name: \"" << mLobby.GetName() << "\""
           << ", version: \"" << TCDS_VERSION << "\""
           << ", players: " << (std::int32_t)mLobby.GetNumberOfPlayers()
           << ", min: " << (std::int32_t)mStats.min
           << ", max: " << (std::int32_t)mStats.max
           << ", total: " << (std::int32_t)mStats.total
           << ", current_mem: " << (std::int32_t) Util::GetCurrentMemoryUsage()
           << ", max_mem: " << (std::int32_t)Util::GetMaximumMemoryUsage()

           << "};";

        std::string output;
        mScriptEngine.EvaluateString(ss.str(), output);
    }
}
/*****************************************************************************/
void SrvStats::IncPlayer()
{
    mStats.current++;
    mStats.total++;
    if (mStats.current >= mStats.max)
    {
        mStats.max = mStats.current;
    }
}
/*****************************************************************************/
void SrvStats::DecPlayer()
{
    if (mStats.current > 0U)
    {
        mStats.current--;
        if (mStats.current <= mStats.min)
        {
            mStats.min = mStats.current;
        }
    }
}
/*****************************************************************************/
void SrvStats::StoreStats(time_t currTime)
{
    // Store statistics
    if (mDb.Open(System::HomePath() + cDbFileName))
    {
        mDb.Query("CREATE TABLE IF NOT EXISTS stats(date_time INTEGER, min INTEGER, max INTEGER, current INTEGER, total INTEGER, mem_current INTEGER, mem_max INTEGER)");
        std::stringstream query;
        query << "INSERT INTO stats VALUES("
              << currTime << ","
              << mStats.min << ","
              << mStats.max << ","
              << mStats.current << ","
              << mStats.total << ","
              << Util::GetCurrentMemoryUsage() << ","
              << Util::GetMaximumMemoryUsage()
              << ");";

        TLogServer("Storing stats: " + query.str());

        mDb.Query(query.str());
        mDb.Close();
    }
    mStats.total = 0U;
}


//=============================================================================
// End of file Server.cpp
//=============================================================================
