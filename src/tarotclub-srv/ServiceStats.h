/*=============================================================================
 * TarotClub - Server.h
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

#ifndef SERVICE_STATS_H
#define SERVICE_STATS_H

#include <vector>
#include "Value.h"
#include "DataBase.h"
#include "JsonValue.h"
#include "IScriptEngine.h"
#include "IService.h"
#include "Lobby.h"

/*****************************************************************************/
class ServiceStats : public Observer<JsonValue>, public IService
{
public:
    struct Stats
    {
        Stats() { Reset(); }

        void Reset()
        {
            min = 0U;
            max = 0U;
            current = 0U;
            total = 0U;
        }

        std::uint32_t min;
        std::uint32_t max;
        std::uint32_t current;
        std::uint32_t total;
    };

    ServiceStats(IScriptEngine &jsEngine, Lobby &lobby);

    // From Observer<JsonValue>
    void Update(const JsonValue &info);

    // From IService
    virtual std::string GetName();
    virtual void Initialize();
    virtual void Stop();

    // From SrvStats
    void FireTimer();

private:
    IScriptEngine &mScriptEngine;
    Lobby &mLobby;

    DataBase mDb;
    Stats mStats;

    void IncPlayer();
    void DecPlayer();
    void StoreStats(time_t currTime);
};


#endif // SERVICE_STATS_H

//=============================================================================
// End of file SrvStats.h
//=============================================================================
