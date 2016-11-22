/*=============================================================================
 * TarotClub - Log.h
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

#ifndef LOG_H
#define LOG_H

#include <cstdint>
#include <mutex>
#include <map>
#include "Observer.h"

/*****************************************************************************/
/**
 * @brief The Log class
 *
 * File format is CSV, each new action will append a new entry at the end of the file.
 *
 * "Category", "Date", "File", "Message"
 *
 * Example:
 * Error, 19-04-2013 09:17:30, "Protocol.cpp", "Buffer length too small!",
 *
 */
class Log
{
public:
    static const std::uint8_t Error;
    static const std::uint8_t Info;
    static const std::uint8_t Network;
    static const std::uint8_t Script;
    static const std::uint8_t Server;

    static const std::uint32_t SizeLimit = 50 * (1024U * 1024U); // Filesize limit to 50MB per log

    Log();

    static void AddEntry(std::uint8_t event, const std::string &file, const int line, const std::string &message);
    static void RegisterListener(Observer<std::string> &listener);
    static void SetLogPath(const std::string &path)
    {
        mLogPath = path;
    }

    static void EnableLog(bool enable) { mEnableFileOutput = enable; }

private:
    static void Save(const std::string &line);

    static std::mutex mMutex;
    static Subject<std::string> mSubject;
    static std::string mLogPath;
    static bool mEnableFileOutput;
};

// Macros definitions
#define TLogInfo(message)       Log::AddEntry(Log::Info, __FILE__, __LINE__, (message))
#define TLogError(message)      Log::AddEntry(Log::Error, __FILE__, __LINE__, (message))
#define TLogNetwork(message)    Log::AddEntry(Log::Network, __FILE__, __LINE__, (message))
#define TLogScript(message)     Log::AddEntry(Log::Script, __FILE__, __LINE__, (message))
#define TLogServer(message)     Log::AddEntry(Log::Server, __FILE__, __LINE__, (message))

#endif // LOG_H

//=============================================================================
// End of file Log.h
//=============================================================================
