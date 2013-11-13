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

#include "Log.h"

Log::Log()
{
}

QMutex Log::mMutex;

void Log::AddEntry(Event event, const QString &key, const QString &value)
{
    QStringList eventString;
    eventString << "Error" << "Info" << "Engine" << "Bot" << "Protocol" << "Message";

    QString line = eventString[event] + ", " + QDateTime::currentDateTime().toString("ddMMyyyy_hhmmss") + ", " + key + ", " + value;
    Save(line);
}

void Log::Save(const QString &line)
{
    mMutex.lock();
    // One log file per day should be enough!
    QString fileName = Config::LogPath + "/log_" + QDateTime::currentDateTime().toString("ddMMyyyy") + ".csv" ;
    QFile f(fileName);
    if (f.open(QIODevice::Append) == false)
    {
        return;
    }
    QTextStream fout(&f);
    fout << line;
    f.close();
    mMutex.unlock();
}

//=============================================================================
// End of file Log.cpp
//=============================================================================
