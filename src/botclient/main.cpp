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
#include "ServerConfig.h"
#include "Util.h"
#include "JsonReader.h"

static const char *cTarotClientVersion = "1.0.0";

void PrintHelp(char *executable)
{
    std::cout << std::endl << "Usage: " << Util::GetFileName(executable) << " [options] -c xx.xx.xx.xx config.json" << std::endl;
    std::cout << "Options are:" << std::endl;
    std::cout << "\t" << "-h" << "\tPrint this help" << std::endl;
    std::cout << "\t" << "-p" << "\tSpecify the server TCP port number" << std::endl;
    std::cout << "\t" << "-t" << "\tSpecify the table number to join" << std::endl << std::endl;

    std::cout << "Mandatory arguments are:" << std::endl;
    std::cout << "\t" << "-c" << "\tConnect to the specified server IP address" << std::endl;

    std::cout << std::endl << "The filename is a json configuration file for bots and must be passed as a last argument." << std::endl;
}
/*****************************************************************************/
/**
 * @brief Entry point of the dedicated bot client
 */
int main(int argc, char *argv[])
{
    CommandLine cmd(argc, argv);

    std::cout << "TarotClub bot(s) client " << cTarotClientVersion << std::endl;

    if(cmd.Exists("-h"))
    {
        PrintHelp(argv[0]);
        exit(-1);
    }

    if (argc == 1)
    {
        std::cout << "No argument specified, exiting ..." << std::endl;
        PrintHelp(argv[0]);
        exit(-1);
    }

    // Parse arguments
    std::string address(cmd.GetOption("-c"));
    std::string port(cmd.GetOption("-p"));
    std::string table(cmd.GetOption("-t"));

    std::uint32_t tableId = Protocol::TABLES_UID;
    std::uint16_t tcpPort = ServerConfig::DEFAULT_GAME_TCP_PORT;
    if (table.size() > 0)
    {
        tableId = std::stoi(table);
    }
    else
    {
         std::cout << "No table specified, using default one: " << tableId << std::endl;
    }
    std::cout << "Join table ID: " << tableId << std::endl;

    if (port.size() > 0)
    {
        tcpPort = std::stoi(port);
    }
    else
    {
        std::cout << "No port specified, using default one: " << tcpPort << std::endl;
    }

    if (address.size() == 0)
    {
        std::cout << "IP Address is not valid, exiting ..." << std::endl;
        PrintHelp(argv[0]);
        exit(-1);
    }

    System::Initialize();
    Log::SetLogPath(System::LogPath());
    Protocol::GetInstance().Initialize();

    std::uint32_t numberOfBots = 0U;
    std::vector<Bot *> bots;

    std::cout << cmd.GetLastOption() << std::endl;

    // Parse bots.json file to retrieve the bot identities
    JsonValue reader;
    if (JsonReader::ParseFile(reader, cmd.GetLastOption()))
    {
        JsonArray identities = reader.GetArray();
        if (identities.Size() > 0)
        {
            for (JsonArray::Iterator iter = identities.Begin(); iter != identities.End(); ++iter)
            {
                if (iter->IsObject())
                {
                    JsonObject obj = iter->GetObject();
                    Identity ident;
                    ident.nickname = obj.GetValue("name").GetString();
                    ident.avatar = obj.GetValue("avatar").GetString();
                    std::string stringVal = obj.GetValue("gender").GetString();
                    if (stringVal == "female")
                    {
                        ident.gender = Identity::cGenderFemale;
                    }
                    else
                    {
                        ident.gender = Identity::cGenderMale;
                    }
                    stringVal = obj.GetValue("bot_ai").GetString();

                    // Create the bot
                    numberOfBots++;
                    Bot *bot = new Bot();
                    bots.push_back(bot);
                    bot->SetIdentity(ident);
                    bot->SetAiScript(stringVal);
                }
            }
        }
    }


    if (numberOfBots == 0U)
    {
        std::cout << "Error: specify at least one bot." << std::endl;
    }
    else
    {
        // ----------- Connect all the created bots
        for (std::vector<Bot *>::iterator iter = bots.begin(); iter != bots.end(); ++iter)
        {
            (*iter)->SetTableToJoin(tableId);
            (*iter)->SetTimeBeforeSend(0U);
            (*iter)->Initialize();
            // Connect the bot to the server
            std::cout << "Bot is connecting to " << address << ":" << tcpPort << std::endl;
            (*iter)->ConnectToHost(address, tcpPort);
        }

        std::cout << "Hit the enter button to exit the client" << std::endl;
        std::cin.get();

        // ----------- Delete all the created bots
        for (std::vector<Bot *>::iterator iter = bots.begin(); iter != bots.end(); ++iter)
        {
            if (*iter != NULL)
            {
                std::cout << "Deleting bot ..." << std::endl;
                (*iter)->Close();
                delete (*iter);
            }
        }
    } // end if bots > 0

    Protocol::GetInstance().Stop();
    std::cout << "Bot client is exiting..." << std::endl;
    return 0;
}

//=============================================================================
// End of file main.cpp
//=============================================================================
