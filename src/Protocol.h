/*=============================================================================
 * TarotClub - Protocol.h
 *=============================================================================
 * Network tarot game protocol
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
#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <cstdint>
#include <string>

class Protocol
{

public:
    // Reserved UUIDs: [0..9]
    static const std::uint32_t  INVALID_UID;
    static const std::uint32_t  LOBBY_UID;      //!< The lobby itself

    static const std::uint32_t  USERS_UID;      //!< Start of users UUID
    static const std::uint32_t  MAXIMUM_USERS;  //!< Maximum number of users
    static const std::uint32_t  TABLES_UID;     //!< Start of tables UUID
    static const std::uint32_t  MAXIMUM_TABLES; //!< Maximum number of tables
    static const std::uint32_t  NO_TABLE;       //!< Identifier for "no table"

    // Packets types
    static const std::string cTypeData; ///< Means that there is a data argument

    // Protocol constants
    static const std::uint32_t cHeaderSize;

    Protocol();
    ~Protocol();

    std::uint32_t GetSourceUuid();
    std::uint32_t GetDestUuid();
    std::uint32_t GetOption() { return mOption; }
    std::string GetType();
    std::uint32_t GetSize() { return mSize; }

    void Add(const std::string &packet) { mPacket += packet; }
    bool Parse(std::string &payload);
    static std::string Build(std::uint32_t option, std::uint32_t src, std::uint32_t dst, const std::string &type, const std::string &arg);

private:    
    std::uint32_t mSrcUuid;
    std::uint32_t mDstUuid;
    std::uint32_t mOption;
    std::uint32_t mSize;
    std::string mPacket;
    std::string mType;

    bool ParseUint32(const char *data, uint32_t size, std::uint32_t &value);
    bool Extract(const std::string &header);
};

#endif // PROTOCOL_H

//=============================================================================
// End of file Protocol.h
//=============================================================================
