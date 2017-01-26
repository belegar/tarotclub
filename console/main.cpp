/*=============================================================================
 * TarotClub - main.cpp
 *=============================================================================
 * Entry point of TarotClub console client
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

#include "Lobby.h"
#include "Server.h"
#include "TournamentConfig.h"
#include "System.h"
#include "GetOptions.h"
#include "ConsoleClient.h"

#include <windows.h>

// You can fill-in your own behavior, this is just an example handler that
//   covers most of the most common events.
BOOL
ControlHandler (DWORD dwControlEvent)
{
  switch (dwControlEvent)
  {
    // User wants to shutdown
    case CTRL_SHUTDOWN_EVENT:
      return FALSE;

    // User wants to logoff
    case CTRL_LOGOFF_EVENT:
      return FALSE;

    // Ctrl + C
    case CTRL_C_EVENT:
      return TRUE;

    // User wants to exit the "normal" way
    case CTRL_CLOSE_EVENT:
      return TRUE;

    // Everything else, just ignore it...
    default:
      return FALSE;
  }
}

/*****************************************************************************/
/**
 * @brief Entry point of the dedicated game server
 */
int main(int /*argc*/, char ** /*argv[]*/)
{
    std::string homePath = System::HomePath();
    System::Initialize(); // default home path

    TLogInfo("Using home path: " + homePath);

    // Init lobby
    Lobby lobby;
    lobby.CreateTable("Console");
    Server server(lobby);

    ConsoleClient client;

    Log::SetLogPath(System::LogPath());
    Log::RegisterListener(client);

    ServerOptions options = ServerConfig::GetDefault();
    options.localHostOnly = true;

    server.Start(options); // Blocking call. On exit, quit the executable


    // Set the control handler so the app will be notified upon any special
    //   termination event.
    SetConsoleCtrlHandler ((PHANDLER_ROUTINE) ControlHandler, TRUE);


    client.Run(options.game_tcp_port);

    server.Stop();
    client.Stop();

    return 0;
}

//=============================================================================
// End of file main.cpp
//=============================================================================
