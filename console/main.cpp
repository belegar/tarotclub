/*=============================================================================
 * TarotClub - main.cpp
 *=============================================================================
 * Entry point of TarotClub dedicated server
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
#include "Log.h"

/*****************************************************************************/
class Logger : public Observer<std::string>
{
public:
    Logger()
        : Observer(Log::Error | Log::Server)
    {

    }

    void Update(const std::string &info)
    {
        std::cout << info << std::endl;
    }
};

static const char* TAROTCLUB_CONSOLE_VERSION = "1.0";


/*****************************************************************************/
/**
 * @brief Entry point of the dedicated game server
 */
int main(int argc, char *argv[])
{
    std::cout << "TarotClub  " << TAROTCLUB_CONSOLE_VERSION << std::endl;
    std::string homePath = System::HomePath();
    System::Initialize(); // default home path

    std::cout << "Using home path: " << homePath << std::endl;

    // Init lobby
    Lobby lobby;
    Server server(lobby);

    Logger logger;

    Log::SetLogPath(System::LogPath());
    Log::RegisterListener(logger);

    ServerOptions options = ServerConfig::GetDefault();

    std::cout << "Starting lobby on TCP port: " << options.game_tcp_port << std::endl;
    server.Start(options); // Blocking call. On exit, quit the executable

    std::cout << "'q' to exit:" << std::endl;

    char input;
    do
    {
        std::cin >> input;
        std::cout << input;
    } while (input != 'q');

    server.Stop();

    return 0;
}

//=============================================================================
// End of file main.cpp
//=============================================================================
