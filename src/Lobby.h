/*=============================================================================
 * TarotClub - Lobby.h
 *=============================================================================
 * Manage temporary connections to join free game tables
 *=============================================================================
 * TarotClub ( http://www.tarotclub.fr ) - This file is part of TarotClub
 * Copyright (C) 2003-2999 - Anthony Rabine
 * anthony@tarotclub.fr
 *
 * This file must be used under the terms of the CeCILL.
 * This source file is licensed as described in the file COPYING, which
 * you should have received as part of this distribution.  The terms
 * are also available at
 * http://www.cecill.info/licences/Licence_CeCILL_V2-en.txt
 *
 *=============================================================================
 */

#ifndef LOBBY_H
#define LOBBY_H

#include "TcpServer.h"
#include "Table.h"
#include "ServerConfig.h"

/*****************************************************************************/
class Lobby : public TcpServer::IEvent
{

public:
    Lobby();
    ~Lobby();

    void Initialize();
    void WaitForEnd();

private:
    struct TableInstance
    {
        std::string name;
        Table table;   // A Tarot table, owns a thread, bots and a Tarot network engine game
    };

    struct Saloon
    {
        std::string name; // name of the saloon
        TableInstance tables[SERVER_MAX_TABLES]; // available game tables in this saloon
    };

    // TcpServer class events
    virtual void NewConnection(int socket);
    virtual void ReadData(int socket, const std::string &data);
    virtual void ClientClosed(int socket);
    virtual void ServerTerminated(CloseType type);

    TcpServer mTcpServer;
    Saloon  saloons[SERVER_MAX_SALOONS];

};

#endif // LOBBY_H

//=============================================================================
// End of file Lobby.h
//=============================================================================
