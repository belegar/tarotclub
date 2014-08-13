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
void Log::AddEntry(Event event, const std::string &file, const int line, const std::string &message)
{
    std::vector<std::string> eventString;
    eventString.push_back("Error");
    eventString.push_back("Info");
    eventString.push_back("Engine");
    eventString.push_back("Bot");
    eventString.push_back("Protocol");
    eventString.push_back("Error");

    std::stringstream ss;
    ss << eventString[event] << ", " <<
       Util::CurrentDateTime("%Y-%m-%d.%X") << ", " <<
       file << ", " <<
       line << ", " <<
       message;

    std::cout << ss.str() << std::endl; // print to local std output
    mSubject.Notify(ss.str());          // send message to all the listeners
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
        f << line;
        f.close();
    }
    mMutex.unlock();
}

//=============================================================================
// End of file Log.cpp
//=============================================================================
