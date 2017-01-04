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
#include "BotManager.h"
#include "Embedded.h"

#include <fcntl.h>
#include <io.h>
#include <stdio.h>

/*****************************************************************************/
class Logger : public Observer<std::string>
{
public:
    Logger()
        : Observer(Log::All)
    {

    }

    void Update(const std::string &info)
    {
        std::cout << info << std::endl;
    }
};

static const char* TAROTCLUB_CONSOLE_VERSION = "1.0";

/*****************************************************************************/
struct ConsoleClient : public net::IEvent
{
    Session mSession;
    BasicClient mClient;
    Context mCtx;
    BotManager mBots;
    std::list<std::uint32_t> mIds;

    ConsoleClient()
     : mSession(*this)
    {
        mClient.mNickName = "Humain";
    }

    void Start()
    {
        std::string localIp = "127.0.0.1";
        mSession.Initialize();
        mSession.ConnectToHost(localIp, ServerConfig::DEFAULT_GAME_TCP_PORT);
        static const Identity ident[3] = {
         Identity("Bender", "", Identity::cGenderRobot, ""),
         Identity("T800", "", Identity::cGenderRobot, ""),
         Identity("C3PO", "", Identity::cGenderRobot, "")
        };

        ArrayPtr<const std::uint8_t> array = gen::GetFile();
        std::string buffer((const char *)array.Data(), array.Size());

        for (std::uint32_t i = 0U; i < 3; i++)
        {
            std::uint32_t botId = mBots.AddBot(Protocol::TABLES_UID, ident[i], 0U, buffer);
            mIds.push_back(botId);
            mBots.ConnectBot(botId, localIp, ServerConfig::DEFAULT_GAME_TCP_PORT);
        }
    }

    void Stop()
    {
        mSession.Close();
        mBots.Close();
        mBots.KillBots();
    }

    virtual void Signal(std::uint32_t sig) { (void) sig; }
    virtual bool Deliver(uint32_t src_uuid, uint32_t dest_uuid, const std::string &arg, std::vector<Reply> &out)
    {
        bool ret = true;

        if (mClient.mUuid != Protocol::INVALID_UID)
        {
            if (dest_uuid != mClient.mUuid)
            {
                ret = false;
            }
        }

        if (ret)
        {
            // Generic client decoder, fill the context and the client structure
            BasicClient::Event event = mClient.Decode(src_uuid, dest_uuid, arg, mCtx, out);

            switch (event)
            {
            case BasicClient::ACCESS_GRANTED:
            {
                // As soon as we have entered into the lobby, join the assigned table
                mClient.JoinTable(Protocol::TABLES_UID, out);
                std::cout << "Connected to Lobby" << std::endl;
                break;
            }
            case BasicClient::NEW_DEAL:
            {
                std::cout << "Received cards: " << mClient.mDeck.ToString();
                break;
            }
            case BasicClient::REQ_BID:
            {
                // Only reply a bid if it is our place to anwser
                if (mClient.mBid.taker == mClient.mPlace)
                {
                  //  RequestBid(out);
                }
                break;
            }
            case BasicClient::START_DEAL:
            {

                break;
            }
            case BasicClient::SHOW_HANDLE:
            {

                break;
            }
            case BasicClient::BUILD_DISCARD:
            {

                break;
            }
            case BasicClient::NEW_GAME:
            {

                break;
            }
            case BasicClient::SHOW_CARD:
            {

                break;
            }
            case BasicClient::PLAY_CARD:
            {
                // Only reply a bid if it is our place to anwser
                if (mClient.mCurrentPlayer == mClient.mPlace)
                {
                    //PlayCard(out);
                }
                break;
            }
            case BasicClient::ASK_FOR_HANDLE:
            {
                //AskForHandle(out);
                break;
            }
            case BasicClient::END_OF_TRICK:
            {
                mClient.Sync("EndOfTrick", out);
                break;
            }
            case BasicClient::END_OF_GAME:
            {
                mClient.Sync("Ready", out);
                break;
            }
            case BasicClient::JSON_ERROR:
            case BasicClient::BAD_EVENT:
            case BasicClient::REQ_LOGIN:
            case BasicClient::MESSAGE:
            case BasicClient::PLAYER_LIST:
            case BasicClient::QUIT_TABLE:
            case BasicClient::SHOW_BID:
                // FIXME: send all the declared bids to the bot so he can use them (AI improvements)
            case BasicClient::ALL_PASSED:
            case BasicClient::SHOW_DOG:
            case BasicClient::END_OF_DEAL:
            case BasicClient::SYNC:
            {
                // Nothing to do for that event
                break;
            }

            default:
                ret = false;
                break;
            }
        }

        return ret;
    }
    virtual std::uint32_t AddUser(std::vector<Reply> &/*out*/) { return 0U; /* stub */ }
    virtual void RemoveUser(std::uint32_t /* uuid */, std::vector<Reply> &/*out*/) {}
};

#include <iostream>
#include <string>
#include <locale>
#include <fstream>
#include <codecvt>

/*****************************************************************************/
/**
 * @brief Entry point of the dedicated game server
 */
int main(int /*argc*/, char ** /*argv[]*/)
{
    std::cout << "TarotClub Console " << TAROTCLUB_CONSOLE_VERSION << std::endl;
    std::string homePath = System::HomePath();
    System::Initialize(); // default home path

    std::cout << "Using home path: " << homePath << std::endl;

    // Init lobby
    Lobby lobby;
    lobby.CreateTable("Console");
    Server server(lobby);

    Logger logger;

    Log::SetLogPath(System::LogPath());
    Log::RegisterListener(logger);

    ServerOptions options = ServerConfig::GetDefault();
    options.localHostOnly = true;

    std::cout << "Starting lobby on TCP port: " << options.game_tcp_port << std::endl;
    server.Start(options); // Blocking call. On exit, quit the executable

    std::cout << "'q' to exit, 'c' to connect, 's' to start game" << std::endl;


    // (S) U+2660	(H) U+2665	(D) U+2666	(C) U+2663

  //  _setmode(_fileno(stdout), _O_U16TEXT);
 //   std::cout << "Try 1: ♠	♥	♦	♣" << std::endl;
 //   std::wcout << L"Try 2: \u2660 \u2665 \u2666 \u2663" << std::endl;

//    std::wstring s = L"Try 2: \u2660 \u2665 \u2666 \u2663";

//    WriteConsoleW(GetStdHandle(STD_OUTPUT_HANDLE), s.c_str(), s.size(), NULL, NULL);

    ConsoleClient client;

    char input;
    do
    {
        std::cin >> input;
        std::cout << input;

        if (input == 'c')
        {
            client.Start();
        }

    } while (input != 'q');

    server.Stop();
    client.Stop();

    return 0;
}

//=============================================================================
// End of file main.cpp
//=============================================================================
