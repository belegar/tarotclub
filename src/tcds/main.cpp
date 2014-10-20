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
#include "System.h"
#include "GetOptions.h"

/*****************************************************************************/
/**
 * @brief Entry point of the dedicated game server
 */
int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    if(CmdOptionExists(argv, argv + argc, "-h"))
    {
        // Print help and exit:
        std::cout << std::endl << "Usage: " << argv[0] << "[option] [argument] ..." << std::endl;
        std::cout << "Options are:" << std::endl;
        std::cout << "\t" << "-h" << "\tPrints this help" << std::endl;
        std::cout << "\t" << "-f" << "\tSpecifies a JSON server configuration file" << std::endl;
        return 0;
    }

    std::string fileName(GetCmdOption(argv, argv + argc, "-f"));
    if (fileName.size() == 0)
    {
        // No specific configuration file, try to open the default one
        fileName = System::HomePath() + ServerConfig::DEFAULT_SERVER_CONFIG_FILE;
    }
    std::cout << "Using configuration file: " << fileName << std::endl;
    ServerConfig conf;
    conf.Load(fileName);

    System::Initialize();
    Log::SetLogPath(System::LogPath());

    std::cout << "TarotClub server " << TAROT_VERSION << " is ready." << std::endl;

    Lobby lobby;
    lobby.Initialize(conf.GetOptions());
    lobby.WaitForEnd();

    return 0;
}

//=============================================================================
// End of file main.cpp
//=============================================================================
