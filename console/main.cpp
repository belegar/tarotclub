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

    ConsoleClient()
     : mSession(*this)
    {

    }

    void Start()
    {
        mSession.Initialize();
        mSession.ConnectToHost("127.0.0.1", ServerConfig::DEFAULT_GAME_TCP_PORT);
    }

    void Stop()
    {
        mSession.Close();
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
              //  mClient.JoinTable(mTableToJoin, out);
                std::cout << "Connected to Lobby" << std::endl;
                break;
            }
            case BasicClient::NEW_DEAL:
            {
                /*
                JSEngine::StringList args;
                args.push_back(mClient.mDeck.ToString());
                mBotEngine.Call("ReceiveCards", args);
                */
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



/*****************************************************************************/
/**
 * @brief Entry point of the dedicated game server
 */
int main(int /*argc*/, char */*argv[]*/)
{
    std::cout << "TarotClub  " << TAROTCLUB_CONSOLE_VERSION << std::endl;
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
