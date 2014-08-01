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
#include "UniqueId.h"

/*****************************************************************************/
class Table : public TcpServer::IEvent, Controller::IEvent
{

public:
    Table();

    // Helpers
    void SetBotParameters(std::map<Place, Identity> &ident, std::uint16_t delay);
    void CreateTable(std::uint8_t nbPlayers);
    void Initialize();
    void Stop();
    void ConnectBots();

    // Getters
    std::uint16_t GetTcpPort();
    bool IsCreated() { return mCreated; }

    // Setters
    void SetTcpPort(std::uint16_t port);

    // Fom TcpServer interface
    virtual void NewConnection(int socket);
    virtual void ReadData(int socket, const std::string &data);
    virtual void ClientClosed(int socket);

    // From Controller
    virtual void SendData(const ByteArray &block);

private:
    void CloseClients();

    int             mTcpPort;
    Controller      mController;
    Bot             mBots[3];
    UniqueId        mIdManager;
    TcpServer       mTcpServer;
    bool            mCreated;   ///< True if the table has been created
    bool            mIsFull;

    // Pair of UUID and socket
    std::map<std::uint32_t, std::int32_t> mUsers;
    std::mutex mUsersMutex;
};

#endif // TABLE_H

//=============================================================================
// End of file Table.h
//=============================================================================
