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
#include "LobbyServer.h"
#include "System.h"
#include "GetOptions.h"
#include "Console.h"
#include "Server.h"
#include "Version.h"
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

/*****************************************************************************/
/**
 * @brief Entry point of the dedicated game server
 */
int main(int argc, char *argv[])
{
    CommandLine cmd(argc, argv);

    std::cout << "TarotClub dedicated server " << TCDS_VERSION << std::endl;

    if(cmd.Exists("-h"))
    {
        // Print help and exit:
        std::cout << std::endl << "Usage: " << argv[0] << "[option] [argument] ..." << std::endl;
        std::cout << "Options are:" << std::endl;
        std::cout << "\t" << "-h" << "\tPrints this help" << std::endl;
        std::cout << "\t" << "-w" << "\tSpecifies a workspace path where generated/configuration files are located" << std::endl;
        return 0;
    }

    std::string homePath;
    if(cmd.Exists("-w"))
    {
        homePath = cmd.GetOption("-w");
        System::Initialize(homePath);
    }
    else
    {
        homePath = System::HomePath();
        System::Initialize(); // default home path
    }
    std::cout << "Using home path: " << homePath << std::endl;

    Logger logger;
    Log::SetLogPath(System::LogPath());
    Log::RegisterListener(logger);

    ServerConfig conf;
    conf.Load(System::HomePath() + ServerConfig::DEFAULT_SERVER_CONFIG_FILE);
    ServerOptions options = conf.GetOptions();
    std::cout << "Starting lobby on TCP port: " << options.game_tcp_port << std::endl;

    Protocol::GetInstance().Initialize();

    Server server;
    server.Start(options); // Blocking call. On exit, quit the executable

    Protocol::GetInstance().Stop();
    return 0;
}

//=============================================================================
// End of file main.cpp
//=============================================================================
