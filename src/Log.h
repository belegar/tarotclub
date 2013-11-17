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

#include <QFile>
#include <QMutex>
#include "defines.h"
#include "Observer.h"


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
    enum Event
    {
        Error = 0,
        Info = 1,
        Engine = 2,
        Bot = 3,
        Protocol = 4,
        Message = 5
    };

    Log();

    static void AddEntry(Event event, const QString &file, const QString &message);
    static void RegisterListener(Observer<QString> &listener);

private:
    static void Save(const QString &line);

    static QMutex mMutex;
    static Subject<QString> mSubject;
};

// Macros definitions
#define TLogInfo(message)       Log::AddEntry(Log::Info, __FILE__, message)
#define TLogError(message)       Log::AddEntry(Log::Error, __FILE__, message)


#endif // LOG_H

//=============================================================================
// End of file Log.h
//=============================================================================
