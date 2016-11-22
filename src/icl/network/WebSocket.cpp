/*=============================================================================
 * TarotClub - WebSocket.cpp
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

#include "WebSocket.h"

/*****************************************************************************/
void WebSocketRequest::Parse(const std::string &msg)
{
    resource = Match(msg, "GET (.*) HTTP");
    host = Match(msg, "Host: (.*)\r\n");
    origin = Match(msg, "Origin: (.*)\r\n");
    key = Match(msg, "Sec-WebSocket-Key: (.*)\r\n");
    protocol = Match(msg, "Sec-WebSocket-Protocol: (.*)\r\n");
}
/*****************************************************************************/
bool WebSocketRequest::IsProtocol(const std::string &proto)
{
    bool ret = false;
    if (protocol == proto)
    {
        ret = true;
    }
    return ret;
}
/*****************************************************************************/
std::string WebSocketRequest::Accept()
{
    /*
        The client sends a Sec-WebSocket-Key which is a random value that has been base64 encoded.
        To form a response, the magic string 258EAFA5-E914-47DA-95CA-C5AB0DC85B11 is appended to this (undecoded) key.
        The resulting string is then hashed with SHA-1, then base64 encoded. Finally, the resulting reply occurs in
        the header Sec-WebSocket-Accept.
    */
    std::string magic = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
    std::string encoded = key + magic;

    Sha1 sha;
    sha.Update(encoded);
    encoded = sha.Final(false);
    encoded = Base64::Encode(encoded);
    return encoded;
}
/*****************************************************************************/
std::string WebSocketRequest::Upgrade(const std::string &proto)
{
    std::string accept = Accept();

    std::string header = "HTTP/1.1 101 Switching Protocols\r\n";
    header += "Upgrade: websocket\r\n";
    header += "Connection: Upgrade\r\n";
    header += "Sec-WebSocket-Accept: " + accept + "\r\n";
    header += "Sec-WebSocket-Protocol: " + proto + "\r\n";
    header += "\r\n";

    return header;
}
/*****************************************************************************/
bool WebSocketRequest::IsValid()
{
    bool valid = false;
    if ((resource.size() != 0) &&
            (host.size() != 0) &&
            (origin.size() != 0) &&
            (key.size() != 0) &&
            (protocol.size() != 0))
    {
        valid = true;
    }
    return valid;
}
/*****************************************************************************/
std::string WebSocketRequest::Match(const std::string &msg, const std::string &patternString)
{
    std::regex pattern(patternString);
    std::smatch matcher;
    std::string subMatch;

    std::regex_search(msg, matcher, pattern);

    if (matcher.size() == 2)
    {
        // Extracted value is located at index 1
        subMatch = matcher[1].str();
    }
    return subMatch; // empty string if not found
}

//=============================================================================
// End of file WebSocket.cpp
//=============================================================================
