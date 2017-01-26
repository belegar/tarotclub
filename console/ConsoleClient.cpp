

// (S) U+2660	(H) U+2665	(D) U+2666	(C) U+2663

//  _setmode(_fileno(stdout), _O_U16TEXT);
//   std::cout << "Try 1: ♠	♥	♦	♣" << std::endl;
//   std::wcout << L"Try 2: \u2660 \u2665 \u2666 \u2663" << std::endl;

//    std::wstring s = L"Try 2: \u2660 \u2665 \u2666 \u2663";

//    WriteConsoleW(GetStdHandle(STD_OUTPUT_HANDLE), s.c_str(), s.size(), NULL, NULL);


#include "ConsoleClient.h"
#include "Embedded.h"
#include "Protocol.h"

#include <sstream>
#include <iomanip>

static const std::wstring TAROTCLUB_CONSOLE_VERSION = L"1.0";
static const std::uint32_t gLineLen = 80U;
static const std::uint32_t gLogSize = 10U;
static const std::uint32_t gLogWidth = 38U;

std::wstring gBlank;

ConsoleClient::ConsoleClient()
    : Observer(Log::Error)
    , mSession(*this)
{
    mClient.mNickName = "Humain";

    mConsole.HideCursor();

    // Clear log
    gBlank.assign(gLogWidth, L' ');
    for (std::uint32_t i = 0U; i < gLogSize; i++)
    {
        mLog.push_back(gBlank);
    }
}


void ConsoleClient::AppendToLog(const std::wstring &str)
{
    if (mLog.size() >= gLogSize)
    {
        mLog.pop_front();
    }

    std::wstring line = str + gBlank;
    line.resize(gLogWidth);
    mLog.push_back(line);

    // Print log
    for (std::uint32_t i = 0U; i < mLog.size(); i++)
    {
        mConsole.GotoXY(37, 5 + i);
        mConsole.Write(mLog[i]);
    }
}

void ConsoleClient::DrawLine(const std::wstring &left, const std::wstring &middle, const std::wstring &right)
{
    std::wstringstream ss;
    ss << left;

    for (int i = 0; i < 80; i++)
    {
        ss << middle;
    }
    ss << right << std::endl;

    mConsole.Write(ss.str());
}

void ConsoleClient::DrawHLine(const std::wstring &left, const std::wstring &fill, const std::wstring &right, const std::wstring &middle, std::uint32_t middle_pos)
{
    std::wstringstream ss;
    ss << left;

    std::uint32_t counter = 1U;

    for (std::uint32_t i = 0; i < middle_pos; i++)
    {
        ss << fill;
        counter++;
    }

    if (middle_pos > 0)
    {
        ss << middle;
        counter++;
    }

    for (; counter < (gLineLen - 1); counter++)
    {
        ss << fill;
    }

    ss << right << std::endl;

    mConsole.Write(ss.str());
}

void ConsoleClient::FillBox(std::uint32_t size, std::uint32_t middle_pos)
{
    for (std::uint32_t i = 0; i < size; i++)
    {
        DrawHLine(L"\u2551", L" ", L"\u2551", L"\u2551", middle_pos);
    }
}

void ConsoleClient::DrawBox(std::uint32_t size, std::uint32_t middle_pos)
{
    if (size > 2)
    {
        if (middle_pos > 0U)
        {
            size -= 2U;
        }

        DrawHLine(L"\u2560", L"\u2550", L"\u2563", L"\u2566", middle_pos);

        FillBox(size, middle_pos);

        DrawHLine(L"\u2560", L"\u2550", L"\u2563", L"\u2569", middle_pos);
    }
}

/*
Box Drawing[1]
Official Unicode Consortium code chart (PDF)
        0 	1 	2 	3 	4 	5 	6 	7 	8 	9 	A 	B 	C 	D 	E 	F
U+250x 	─ 	━ 	│ 	┃ 	┄ 	┅ 	┆ 	┇ 	┈ 	┉ 	┊ 	┋ 	┌ 	┍ 	┎ 	┏
U+251x 	┐ 	┑ 	┒ 	┓ 	└ 	┕ 	┖ 	┗ 	┘ 	┙ 	┚ 	┛ 	├ 	┝ 	┞ 	┟
U+252x 	┠ 	┡ 	┢ 	┣ 	┤ 	┥ 	┦ 	┧ 	┨ 	┩ 	┪ 	┫ 	┬ 	┭ 	┮ 	┯
U+253x 	┰ 	┱ 	┲ 	┳ 	┴ 	┵ 	┶ 	┷ 	┸ 	┹ 	┺ 	┻ 	┼ 	┽ 	┾ 	┿
U+254x 	╀ 	╁ 	╂ 	╃ 	╄ 	╅ 	╆ 	╇ 	╈ 	╉ 	╊ 	╋ 	╌ 	╍ 	╎ 	╏
U+255x 	═ 	║ 	╒ 	╓ 	╔ 	╕ 	╖ 	╗ 	╘ 	╙ 	╚ 	╛ 	╜ 	╝ 	╞ 	╟
U+256x 	╠ 	╡ 	╢ 	╣ 	╤ 	╥ 	╦ 	╧ 	╨ 	╩ 	╪ 	╫ 	╬ 	╭ 	╮ 	╯
U+257x 	╰ 	╱ 	╲ 	╳ 	╴ 	╵ 	╶ 	╷ 	╸ 	╹ 	╺ 	╻ 	╼ 	╽ 	╾ 	╿
*/

void ConsoleClient::BuildBoard()
{
    DrawHLine(L"\u2554", L"\u2550", L"\u2557");
    FillBox(3);
    DrawBox(12, 35);
    FillBox(4);
    DrawHLine(L"\u255A", L"\u2550", L"\u255D");
}

std::wstring ConsoleClient::ToString(const Card &c)
{
    std::wstringstream ss;

    // Cast internal value to ingeter to interpret the char as an "integer"
    ss << (std::uint32_t)c.GetValue();

    // (S) U+2660	(H) U+2665	(D) U+2666	(C) U+2663
    if (c.GetSuit() == Card::CLUBS)
    {
        ss << L"\u2663";
    }
    else if (c.GetSuit() == Card::DIAMONDS)
    {
        ss << L"\u2666";
    }
    else if (c.GetSuit() == Card::SPADES)
    {
        ss << L"\u2660";
    }
    else if (c.GetSuit() == Card::HEARTS)
    {
        ss << L"\u2665";
    }
    else
    {
        ss << L"T"; // try L"\u2591" or L"\u2591"
    }

    return ss.str();
}

void ConsoleClient::Update(const std::string &info)
{
    std::cout << info << std::endl;
}

void ConsoleClient::Start(std::uint16_t tcp_port)
{
    std::wstringstream ss;

    ss << L"Starting lobby on TCP port: " << (std::uint32_t)tcp_port;
    AppendToLog(ss.str());

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
    std::wstringstream ss;

    BuildBoard();

    mConsole.GotoXY(2, 1);
    ss << L"TarotClub Console " << TAROTCLUB_CONSOLE_VERSION;
    mConsole.Write(ss.str());

    mConsole.GotoXY(2, 2);
    mConsole.Write(L"Press 'F1' to connect and start the game, 'F2' to exit.");

    mConsole.GotoXY(14, 10);
    mConsole.Write(L"South");
    mConsole.GotoXY(14, 6);
    mConsole.Write(L"North");
    mConsole.GotoXY(4, 8);
    mConsole.Write(L"West");
    mConsole.GotoXY(22, 8);
    mConsole.Write(L"East");

    bool quitGame = false;
    while (!quitGame)
    {
        Console::KeyEvent event = mConsole.ReadKeyboard();

        switch (event)
        {
        case Console::KEY_F1:
            Start(tcp_port);
            break;
        case Console::KEY_F2:
            quitGame = true;
            break;
        default:
            break;
        }
    }
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
            AppendToLog(L"Connected to Lobby");
            break;
        }
        case BasicClient::JOIN_TABLE:
        {
            //std::wstring s = std::wstring(mClient.mPlace.ToString());
            std::wstringstream ss;
            ss << L"Entered table in position: ";
            AppendToLog(ss.str());
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
            std::int32_t x = 0;
            mConsole.GotoXY(2, 19);
            //if (mClient.mCurrentPlayer.)
           // mConsole.Write("Player " + mClient.mCurrentPlayer.ToString() + " has played: " + mClient.mCurrentTrick.Last().ToString());
            break;
        }
        case BasicClient::PLAY_CARD:
        {
            // Only reply a bid if it is our place to anwser
            if (mClient.mCurrentPlayer == mClient.mPlace)
            {
                // ⇧


                TLogInfo("Console client deck is: " + mClient.mDeck.ToString());

                Card c = mClient.Play();

                TLogInfo("Console client is playing: " + c.ToString());

                std::wstringstream ss;
                ss << L"You are playing: " << ToString(c);
                AppendToLog(ss.str());

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
