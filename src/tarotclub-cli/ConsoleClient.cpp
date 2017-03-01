
#include "ConsoleClient.h"
#include "Embedded.h"
#include "Protocol.h"
#include "Util.h"

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
    , mCanPlay(false)
    , mArrowPosition(0U)
{
    mClient.mMyself.identity.nickname = "Humain";

    mConsole.SetCursor(false);

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
    ss <<  std::setfill(L' ') << std::setw(2) << (std::uint32_t)c.GetValue();

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
    std::string localIp = "127.0.0.1";
    mSession.Initialize();
    mSession.ConnectToHost(localIp, tcp_port);
    static const Identity ident[3] = {
        Identity("Bender", "", Identity::cGenderRobot),
        Identity("T800", "", Identity::cGenderRobot),
        Identity("C3PO", "", Identity::cGenderRobot)
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
    mConsole.Write(L"Press 'c' to connect and start the game, 'q' to exit.");

    mConsole.GotoXY(2, 3);
    mConsole.Write(L"Use 'left' and 'right' keys to select a card, 'space' to validate.");

    mConsole.GotoXY(14, 10);
    mConsole.Write(L"South");
    mConsole.GotoXY(14, 6);
    mConsole.Write(L"North");
    mConsole.GotoXY(4, 8);
    mConsole.Write(L"West");
    mConsole.GotoXY(22, 8);
    mConsole.Write(L"East");

    bool quitGame = false;
    bool gameStarted = false;
    while (!quitGame)
    {
        Console::KeyEvent event = mConsole.ReadKeyboard();

        switch (event)
        {
        case Console::KB_C:

            if (!gameStarted)
            {
                gameStarted = true;
                Start(tcp_port);
            }
            break;
        case Console::KB_LEFT:
        case Console::KB_RIGHT:
        case Console::KB_SPACE:
            mMutex.lock();
            if (mCanPlay)
            {
                HandleEvent(event);
            }
            mMutex.unlock();
            break;
        case Console::KB_Q:
            quitGame = true;
            break;
        default:
            break;
        }
    }

    mConsole.Cls();
    mConsole.GotoXY(1, 1);
    mConsole.Write(L"Bye!\r\n");
}


void ConsoleClient::DisplayDeck()
{
    std::uint32_t offset = 0U;

    mClient.mDeck.Sort("THSDC");
    mConsole.GotoXY(2, 16);
    mConsole.Write(L"                                                                        ");
    for (Deck::ConstIterator it = mClient.mDeck.Begin(); it != mClient.mDeck.End(); ++it, offset++)
    {
        mConsole.GotoXY(2 + (4*offset), 16);
        mConsole.Write(ToString(*it));
    }
}

void ConsoleClient::ClearBoard()
{
    for (std::uint32_t i = 0U; i < 4; i++)
    {
        DisplayText(L"   ", Place(i));
    }
}

void ConsoleClient::ClearUserInfos()
{
    mConsole.GotoXY(2, 18);
    mConsole.Write(L"                                                                        ");

}

void ConsoleClient::DisplayUserInfos(const std::wstring &txt)
{
    ClearUserInfos();
    mConsole.GotoXY(2, 18);
    mConsole.Write(txt);
}

void ConsoleClient::DisplayText(const std::wstring &txt, Place p)
{
    if (p == Place(Place::SOUTH))
    {
        mConsole.GotoXY(14, 11);
    }
    else if (p == Place(Place::EAST))
    {
        mConsole.GotoXY(22, 9);
    }
    else if (p == Place(Place::WEST))
    {
        mConsole.GotoXY(4, 9);
    }
    else
    {
        // North
        mConsole.GotoXY(14, 7);
    }

    mConsole.Write(txt);
}

void ConsoleClient::DisplayCard(Card c, Place p)
{
    DisplayText(ToString(c), p);
}


void ConsoleClient::HandleEvent(Console::KeyEvent event)
{
    switch (event)
    {
    case Console::KB_LEFT:
        if (mArrowPosition > 1U)
        {
            mArrowPosition--;
            ClearUserInfos();
        }
        break;
    case Console::KB_RIGHT:
        if (mArrowPosition < mClient.mDeck.Size())
        {
            mArrowPosition++;
            ClearUserInfos();
        }
        break;
    case Console::KB_SPACE:
    {
        // Validate only if the card is correct
        Card c = mClient.mDeck.At(mArrowPosition - 1U);
        // Must be a valid Tarot card
        if (c.IsValid())
        {
            // Must follow the Tarot rules
            if (mClient.IsValid(c))
            {
                ClearUserInfos();
                mCanPlay = false;
                std::vector<Reply> out;
                mClient.BuildSendCard(c, out);
                mClient.mDeck.Remove(c);
                DisplayDeck();
                mSession.Send(out);
            }
            else
            {
                DisplayUserInfos(L"Invalid card.");
            }
        }
        break;
    }
    default:
        break;
    }
    // Some debug code
    //std::wstringstream ss;
    //ss << L"Pos: " << mArrowPosition;
    //AppendToLog(ss.str());
    DisplayArrow();
}

void ConsoleClient::ClearArrow()
{
    mConsole.GotoXY(2, 17);
    mConsole.Write(L"                                                                        ");
}

/**
 * @brief ConsoleClient::DisplayArrow
 *
 * Display arrow in the middle of the deck
 */
void ConsoleClient::DisplayArrow()
{
    ClearArrow();
    mConsole.GotoXY(mArrowPosition * 4U, 17); // transform position into a X coordinate
    mConsole.Write(L"\u2191"); // ⬆
}

void ConsoleClient::Signal(std::uint32_t sig) { (void) sig; }

bool ConsoleClient::Deliver(uint32_t src_uuid, uint32_t dest_uuid, const std::string &arg, std::vector<Reply> &out)
{
    bool ret = true;

    if (mClient.mMyself.uuid != Protocol::INVALID_UID)
    {
        if (dest_uuid != mClient.mMyself.uuid)
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
            mClient.BuildJoinTable(Protocol::TABLES_UID, out);
            AppendToLog(L"Connected to Lobby");
            break;
        }
        case BasicClient::JOIN_TABLE:
        {
            std::wstringstream ss;
            ss << L"Entered table in position: " << Util::ToWString(mClient.mMyself.place.ToString());
            AppendToLog(ss.str());
            break;
        }
        case BasicClient::NEW_DEAL:
        {
            TLogInfo("Received cards: " + mClient.mDeck.ToString());
            DisplayDeck();
            std::this_thread::sleep_for(std::chrono::seconds(1));
            break;
        }
        case BasicClient::REQ_BID:
        {
            // Only reply a bid if it is our place to anwser
            if (mClient.mBid.taker == mClient.mMyself.place)
            {
                mClient.BuildBid(Contract("Pass"), false, out);
            }
            break;
        }
        case BasicClient::START_DEAL:
        {
            std::wstringstream ss;
            ss << L"Start deal, taker is: " << Util::ToWString(mClient.mBid.taker.ToString());
            AppendToLog(ss.str());
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
            DisplayCard(mClient.mCurrentTrick.Last(), mClient.mCurrentPlayer);
            break;
        }
        case BasicClient::PLAY_CARD:
        {
            // Only reply a bid if it is our place to anwser
            if (mClient.IsMyTurn())
            {
                // Display the arrow at initial position
                mArrowPosition = (mClient.mDeck.Size()/2);
                DisplayArrow();

                mMutex.lock();
                mCanPlay = true;
                mMutex.unlock();
            }
            break;
        }
        case BasicClient::ASK_FOR_HANDLE:
        {
            mClient.BuildHandle(Deck(), out);
            break;
        }
        case BasicClient::END_OF_TRICK:
        {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            ClearBoard();
            mClient.Sync("EndOfTrick", out);
            break;
        }
        case BasicClient::END_OF_GAME:
        {
            std::wstringstream ss;
            ss << L"End of game, winner is: " << Util::ToWString(mClient.mCurrentPlayer.ToString());
            AppendToLog(ss.str());

            ClearBoard();
            mClient.Sync("Ready", out);
            break;
        }
        case BasicClient::ALL_PASSED:
        {
            AppendToLog(L"All players have passed! New turn...");
            std::this_thread::sleep_for(std::chrono::seconds(1));
            break;
        }
        case BasicClient::END_OF_DEAL:
            mClient.Sync("EndOfDeal", out);
            break;

        case BasicClient::JSON_ERROR:
        case BasicClient::BAD_EVENT:
        case BasicClient::REQ_LOGIN:
        case BasicClient::MESSAGE:
        case BasicClient::PLAYER_LIST:
        case BasicClient::QUIT_TABLE:
        case BasicClient::SHOW_BID:
            // FIXME: send all the declared bids to the bot so he can use them (AI improvements)
        case BasicClient::SHOW_DOG:
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
