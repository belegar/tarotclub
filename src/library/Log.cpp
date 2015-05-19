/*=============================================================================
 * TarotClub - Log.cpp
 *=============================================================================
 * Log utility class: store events and data in a chronological way
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

// C++ / STL
#include <iostream>
#include <fstream>
#include <sstream>

// TarotClub
#include "Log.h"
#include "Util.h"

std::mutex Log::mMutex;
Subject<std::string> Log::mSubject;
std::string Log::mLogPath;

const std::uint8_t Log::Error   = 1U;
const std::uint8_t Log::Info    = 2U;
const std::uint8_t Log::Network = 4U;
const std::uint8_t Log::Script  = 8U;
const std::uint8_t Log::Server  = 16U;

static std::map<std::uint8_t, std::string> LogInit();
static std::map<std::uint8_t, std::string> eventString = LogInit();

/*****************************************************************************/
/**
 * @brief One time initialization of a global variable
 * @return
 */
std::map<uint8_t, std::string> LogInit()
{
    std::map<std::uint8_t, std::string> evt;

    evt[Log::Error]      = "Error";
    evt[Log::Info]       = "Info";
    evt[Log::Network]    = "Network";
    evt[Log::Script]     = "Script";
    evt[Log::Server]     = "Server";

    return evt;
}

/*****************************************************************************/
Log::Log()
{
}
/*****************************************************************************/
void Log::RegisterListener(Observer<std::string> &listener)
{
    mSubject.Attach(listener);
}
/*****************************************************************************/
void Log::AddEntry(uint8_t event, const std::string &file, const int line, const std::string &message)
{
    std::stringstream ss;

    ss << eventString[event] << ", " <<
       Util::CurrentDateTime("%Y-%m-%d.%X") << ", " <<
       file << ", " <<
       line << ", " <<
       message;

    mSubject.Notify(ss.str(), event);    // send message to all the listeners
    Save(ss.str());                     // save message to a file
}
/*****************************************************************************/
void Log::Save(const std::string &line)
{
    std::fstream f;
    std::string fileName;

    mMutex.lock();
    // One log file per day should be enough!

    fileName = mLogPath + "/log_" + Util::CurrentDateTime("%Y-%m-%d") + ".csv";

    f.open(fileName, std::ios_base::out | std::ios_base::binary  | std::ios_base::app);

    if (f.is_open())
    {
        f << line << std::endl;
        f.close();
    }
    mMutex.unlock();
}

//=============================================================================
// End of file Log.cpp
//=============================================================================
