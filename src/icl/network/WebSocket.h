/*=============================================================================
 * TarotClub - WebSocket.h
 *=============================================================================
 * WebSocket handshake manager
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

#ifndef WEB_SOCKET_H
#define WEB_SOCKET_H

#include <regex>

#include "Sha1.h"
#include "Base64.h"

/*****************************************************************************/
/**
 * @brief WebSocketRequest
 */
class WebSocketRequest
{
public:
    void Parse(const std::string &msg);
    bool IsProtocol(const std::string &proto);
    std::string Accept();
    std::string Upgrade(const std::string &proto);
    bool IsValid();

private:
    std::string Match(const std::string &msg, const std::string &patternString);

    std::string resource;
    std::string host;
    std::string origin;
    std::string key;
    std::string protocol;
};

#endif // WEB_SOCKET_H

//=============================================================================
// End of file WebSocket.h
//=============================================================================
