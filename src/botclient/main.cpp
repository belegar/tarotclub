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

#include "System.h"
#include "Log.h"
#include "Bot.h"
#include "GetOptions.h"
#include "ClientConfig.h"

/*****************************************************************************/
/**
 * @brief Entry point of the dedicated bot client
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
        std::cout << "\t" << "-f" << "\tSpecifies a JSON client configuration file" << std::endl;
        std::cout << "\t" << "-c" << "\tConnect to the specified server IP address" << std::endl;
        std::cout << "\t" << "-p" << "\tSpecifies the server TCP port number" << std::endl;
        std::cout << "\t" << "-t" << "\tSpecifies the table number to join" << std::endl;
        return 0;
    }

    // Parse arguments
    std::string fileName(GetCmdOption(argv, argv + argc, "-f"));
    std::string address(GetCmdOption(argv, argv + argc, "-c"));
    std::string port(GetCmdOption(argv, argv + argc, "-p"));
    std::string table(GetCmdOption(argv, argv + argc, "-t"));

    std::cout << "Using configuration file: " << fileName << std::endl;

    std::uint32_t tableId = 1U;
    if (table.size() > 0)
    {
        tableId = std::stoi(table);
    }
    else
    {
         std::cout << "No table specified, using default one (1)" << std::endl;
    }
    std::cout << "Join table ID: " << tableId << std::endl;

    ClientConfig conf;
    conf.Load(fileName); // will load default settings if fileName is not valid

    System::Initialize();
    Log::SetLogPath(System::LogPath());

    std::cout << "TarotClub bot client " << TAROT_VERSION << " is starting." << std::endl;

    Bot bot;

    bot.SetTableToJoin(tableId);
    bot.SetIdentity(conf.GetOptions().identity);
    bot.SetTimeBeforeSend(0U);
    bot.Initialize();
    // Connect the bot to the server
    std::cout << "Connecting to " << address << ":" << port << std::endl;
    bot.ConnectToHost(address, std::stoi(port));
    if (bot.IsConnected())
    {
        std::cout << "Bot is connected to the server" << std::endl;
    }

    std::cout << "Hit the enter button to exit the client" << std::endl;

    std::cin.get();
    bot.Close();

    std::cout << "Bot client is closed, exiting..." << std::endl;
    return 0;
}

//=============================================================================
// End of file main.cpp
//=============================================================================
