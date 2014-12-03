/*=============================================================================
 * TarotClub - DataBase.h
 *=============================================================================
 * A simple DataBase class abstraction layer
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

#ifndef DATABASE_H
#define DATABASE_H

#include <vector>
#include "Value.h"
#include "sqlite3.h"
#include "IDataBase.h"
#include <mutex>
#include <thread>

/*****************************************************************************/
class DataBase : public IDataBase
{
public:
    class IWorkItem
    {
    public:
        virtual void DoAction() = 0;
    };

    DataBase();
    ~DataBase();

    void Stop() { mStopRequested = true; mThread.join(); }

    // From IDataBase
    void Initialize();
    void AddPlayer();
    void DecPlayer();

    // Database Helpers
    bool Open(const std::string &fileName);
    void Close();
    std::vector<std::vector<Value> > Query(const std::string &query);

private:
    sqlite3 *mDb;
    bool mValid;
    std::thread mThread;
    bool mInitialized;
    bool mStopRequested;
    std::mutex mMutex;

    // Server statistics
    IDataBase::Stats mStats;

    // Thread
    static void EntryPoint(void *pthis);
    void Run();

};


#endif // DATABASE_H

//=============================================================================
// End of file DataBase.h
//=============================================================================
