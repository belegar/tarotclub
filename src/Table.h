/*=============================================================================
 * TarotClub - Table.h
 *=============================================================================
 * The table manages the Tarot thread server
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
#ifndef TABLE_H
#define TABLE_H

#include <map>
#include "TcpServer.h"
#include "Controller.h"
#include "Bot.h"
#include "UserId.h"

/*****************************************************************************/
class Table : public Observer<TcpServer::Signal>
{

public:
    Table();

    // Helpers
    void LoadConfiguration(int port = DEFAULT_PORT);
    void CreateGame(Game::Mode gameMode, int nbPlayers, const Game::Shuffle &shuffle);
    void Initialize();
    void Stop();
    void ConnectBots();

    // Getters
    ServerOptions &GetOptions();

    // Setters
    void SaveConfiguration(const ServerOptions &opt);

    // Handle incomming connections and data
    void Update(const TcpServer::Signal &info);

private:
    class ControllerListener : public Observer<Controller::Signal>
    {
    public:
        ControllerListener(Table &i_table)
            : mTable(i_table)
        {

        }

        void Update(const Controller::Signal &info)
        {
            mTable.SendToSocket(info.data);
        }

    private:
        Table &mTable;
    };

    friend class ControllerListener;

    void SendToSocket(const ByteArray &packet);
    void StopServer();
    void CloseClients();

    ControllerListener mControllerListener;
    int maximumPlayers;
    int mTcpPort;
    Controller mController;
    ServerConfig serverConfig;
    Bot mBots[3];
    UserId mIdManager;
    TcpServer mTcpServer;

    // Pair of UUID and socket
    std::map<std::uint32_t, std::int32_t> mUsers;
};

#endif // TABLE_H

//=============================================================================
// End of file Table.h
//=============================================================================
