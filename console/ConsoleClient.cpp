

// (S) U+2660	(H) U+2665	(D) U+2666	(C) U+2663

//  _setmode(_fileno(stdout), _O_U16TEXT);
//   std::cout << "Try 1: ♠	♥	♦	♣" << std::endl;
//   std::wcout << L"Try 2: \u2660 \u2665 \u2666 \u2663" << std::endl;

//    std::wstring s = L"Try 2: \u2660 \u2665 \u2666 \u2663";

//    WriteConsoleW(GetStdHandle(STD_OUTPUT_HANDLE), s.c_str(), s.size(), NULL, NULL);


#include "ConsoleClient.h"
#include "Embedded.h"
#include "Protocol.h"

#include <iostream>
#include <string>
#include <locale>
#include <fstream>
#include <codecvt>

static const char* TAROTCLUB_CONSOLE_VERSION = "1.0";

ConsoleClient::ConsoleClient()
    : Observer(Log::Error | Log::Info)
    , mSession(*this)
{
    mClient.mNickName = "Humain";
}

void ConsoleClient::Update(const std::string &info)
{
    std::cout << info << std::endl;
}

void ConsoleClient::Start(std::uint16_t tcp_port)
{
    std::cout << "Starting lobby on TCP port: " << tcp_port << std::endl;

    std::string localIp = "127.0.0.1";
    mSession.Initialize();
    mSession.ConnectToHost(localIp, tcp_port);
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
        mBots.ConnectBot(botId, localIp, tcp_port);
    }
}

void ConsoleClient::Stop()
{
    mSession.Close();
    mBots.Close();
    mBots.KillBots();
}

void ConsoleClient::Run(std::uint16_t tcp_port)
{
    char input;

    std::cout << "TarotClub Console " << TAROTCLUB_CONSOLE_VERSION << std::endl;
    std::cout << "'q' to exit, 'c' to connect, 's' to start game" << std::endl;

    do
    {
        std::cin >> input;
        std::cout << input;

        if (input == 'c')
        {
            Start(tcp_port);
        }

    } while (input != 'q');
}

void ConsoleClient::Signal(std::uint32_t sig) { (void) sig; }

bool ConsoleClient::Deliver(uint32_t src_uuid, uint32_t dest_uuid, const std::string &arg, std::vector<Reply> &out)
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
            TLogInfo("Connected to Lobby");
            break;
        }
        case BasicClient::JOIN_TABLE:
        {
            TLogInfo("Entered table in position: " + mClient.mPlace.ToString());
            break;
        }
        case BasicClient::NEW_DEAL:
        {
            TLogInfo("Received cards: " + mClient.mDeck.ToString());
            break;
        }
        case BasicClient::REQ_BID:
        {
            // Only reply a bid if it is our place to anwser
            if (mClient.mBid.taker == mClient.mPlace)
            {
                mClient.BuildBid(Contract("Pass"), false, out);
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
            TLogInfo("Player " + mClient.mCurrentPlayer.ToString() + " has played: " + mClient.mCurrentTrick.Last().ToString());
            break;
        }
        case BasicClient::PLAY_CARD:
        {
            // Only reply a bid if it is our place to anwser
            if (mClient.mCurrentPlayer == mClient.mPlace)
            {
                TLogInfo("Console client deck is: " + mClient.mDeck.ToString());

                Card c = mClient.Play();

                TLogInfo("Console client is playing: " + c.ToString());

                mClient.mDeck.Remove(c);
                mClient.SendCard(c, out);
            }
            break;
        }
        case BasicClient::ASK_FOR_HANDLE:
        {
            mClient.SendHandle(Deck(), out);
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

std::uint32_t ConsoleClient::AddUser(std::vector<Reply> &) { return 0U; /* stub */ }

void ConsoleClient::RemoveUser(std::uint32_t, std::vector<Reply> &) {}
