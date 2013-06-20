/*=============================================================================
 * TarotClub - Table.cpp
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
#include "Table.h"

/*****************************************************************************/
Table::Table()
{
}
/*****************************************************************************/
void Table::Initialize()
{
    // 1. Create the server thread
    server.moveToThread(&thread);

    // 2. Load server configuration
    LoadConfiguration();

    // 3. Start the thread
    thread.start();
}
/*****************************************************************************/
void Table::LoadConfiguration()
{
    serverConfig.Load();

    // 3. Apply configuration
    for (int i = 0; i < 3; i++)
    {
        bots[i].SetMyIdentity(serverConfig.GetOptions().bots[i]);
        bots[i].SetTimeBeforeSend(serverConfig.GetOptions().timer);
    }
}
/*****************************************************************************/
void Table::SaveConfiguration(ServerOptions &opt)
{
    serverConfig.SetOptions(opt);
    serverConfig.Save();
}
/*****************************************************************************/
void Table::CreateGame(Game::Mode gameMode, Table::Mode tableMode)
{
    mode = tableMode;
    server.NewServerGame(gameMode);
}
/*****************************************************************************/
void Table::Start()
{
    if (mode == USE_BOTS)
    {
        ConnectBots();
    }
}
/*****************************************************************************/
void Table::Stop()
{
    server.StopServer();
    thread.exit();
    thread.wait(); // block until thread has finished execution
}
/*****************************************************************************/
Server &Table::GetServer()
{
    return server;
}
/*****************************************************************************/
ServerOptions &Table::GetOptions()
{
    return serverConfig.GetOptions();
}
/*****************************************************************************/
void Table::SetShuffle(TarotEngine::Shuffle &s)
{
    server.GetEngine().SetShuffle(s);
}
/*****************************************************************************/
void Table::ConnectBots()
{
    int i;

    qApp->processEvents(QEventLoop::AllEvents, 100);
    for (i = 0; i < 3; i++)
    {
        bots[i].ConnectToHost("127.0.0.1", GetOptions().port);
        qApp->processEvents(QEventLoop::AllEvents, 100);
    }
}

//=============================================================================
// End of file Table.cpp
//=============================================================================

