/*=============================================================================
 * TarotClub - Console.cpp
 *=============================================================================
 * TCP/IP console to manage the server using command line interface
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

#include <sstream>
#include "Terminal.h"
#include "Util.h"
#include "JsonValue.h"
#include "Version.h"
#include "Server.h"
#include "Observer.h"

static const std::string cConsoleString = "tcds> ";


/*****************************************************************************/
Terminal::Terminal(IScriptEngine &jsEngine, IEventLoop &ev)
    : mScriptEngine(jsEngine)
    , mEventLoop(ev)
    , mTcpServer(*this)
    , mExit(false)
{

}
/*****************************************************************************/
Terminal::~Terminal()
{

}
/*****************************************************************************/
void Terminal::Print(const std::string &msg)
{
    if (mPeer.IsValid())
    {
        tcp::TcpSocket::SendToSocket(msg, mPeer);
    }
}
/*****************************************************************************/
std::string Terminal::GetName()
{
    return "Terminal";
}
/*****************************************************************************/
void Terminal::Initialize()
{
    std::stringstream ss;

    mScriptEngine.RegisterPrinter(this);

    ss << "Starting management console on TCP port: " << mPort;
    TLogServer(ss.str());
    mTcpServer.Start(10U, true, mPort);
}
/*****************************************************************************/
void Terminal::Stop()
{

}
/*****************************************************************************/
void Terminal::SetPort(uint16_t port)
{
    mPort = port;
}
/*****************************************************************************/
void Terminal::NewConnection(const tcp::Conn &conn)
{
    mPeer = conn.peer;
    tcp::TcpSocket::SendToSocket(cConsoleString, mPeer);
}
/*****************************************************************************/
void Terminal::ReadData(const tcp::Conn &conn)
{
    (void) conn;

    std::string output;
    (void) mScriptEngine.EvaluateString(conn.payload, output);
    std::stringstream ss;
    ss << output << std::endl << cConsoleString;

    tcp::TcpSocket::SendToSocket(ss.str(), mPeer);
}
/*****************************************************************************/
void Terminal::ClientClosed(const tcp::Conn &conn)
{
    (void) conn;
    mPeer.socket = -1;
}
/*****************************************************************************/
void Terminal::ServerTerminated(tcp::TcpServer::IEvent::CloseType type)
{
    (void) type;
    mPeer.socket = -1;
}

//=============================================================================
// End of file Terminal.cpp
//=============================================================================

