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

#include <iostream>
#include <vector>
#include <fstream>
#include "Log.h"
#include "Tools.h"

std::mutex Log::mMutex;
Subject<std::string> Log::mSubject;

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
void Log::AddEntry(Event event, const std::string &file, const std::string &message)
{
    std::vector<std::string> eventString;
    eventString.push_back("Error");
    eventString.push_back("Info");
    eventString.push_back("Engine");
    eventString.push_back("Bot");
    eventString.push_back("Protocol");
    eventString.push_back("Error");

    std::string line = eventString[event] + ", " + Util::CurrentDateTime("%Y-%m-%d.%X") + ", " + file + ", " + message + "\r\n";

    std::cout << line;      // print to local std output
    mSubject.Notify(line);  // send message to all the listeners
    Save(line);             // save message to a file
}
/*****************************************************************************/
void Log::Save(const std::string &line)
{
    std::fstream f;
    std::string fileName;

    mMutex.lock();
    // One log file per day should be enough!

    fileName = Config::LogPath + "/log_" + Util::CurrentDateTime("%Y-%m-%d") + ".csv";

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
