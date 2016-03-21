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

#ifndef SERVER_H
#define SERVER_H

#include <vector>
#include "Value.h"
#include "sqlite3.h"
#include "LobbyServer.h"
#include "Protocol.h"
#include "DataBase.h"
#include "JsonValue.h"
#include "Console.h"
#include "BotManager.h"
#include <mutex>
#include <thread>
#include <chrono>

/*****************************************************************************/
class Server : public Observer<LobbyServer::Event>, public Protocol::IWorkItem
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

    struct BotMatch
    {
        std::uint32_t tableId;
        Identity identity;
        std::chrono::high_resolution_clock::time_point startTime;
        JsonArray deals;
        Score score;
        std::vector<std::uint32_t> botIds;
        bool finished;
    };

    Server();

    void Start(const ServerOptions &options);
    void Stop() { mStopRequested = true; mThread.join(); }

    // From Lobby::Event
    virtual void Update(const LobbyServer::Event &event);

    // From Protocol::IWorkItem
    virtual bool DoAction(std::uint8_t cmd, std::uint32_t src_uuid, std::uint32_t dest_uuid, const ByteArray &data);

private:
    Lobby mLobby;
    LobbyServer mLobbyServer;
    Console mConsole;
    BotManager mBotManager;
    DataBase mDb;
    std::thread mThread;
    bool mInitialized;
    bool mStopRequested;
    std::uint16_t mGamePort;

    // AI Contest variables
    std::vector<BotMatch> mPendingAi; // Pending matches
    std::mutex mAiMutex;
    std::vector<Tarot::Distribution> mAiContest;

    // Server statistics
    Stats mStats;
    std::mutex mStatsMutex;

    // Thread
    static void EntryPoint(void *pthis);
    void Run();
    void IncPlayer();
    void DecPlayer();
    void StoreStats(time_t currTime);
    void CheckNewAIBots();
    bool IsPending(const std::string &username);
    bool StoreBotScore(const BotMatch &match, const std::string &docId);
    void CheckFinishedGames();
};


#endif // SERVER_H

//=============================================================================
// End of file Server.h
//=============================================================================
