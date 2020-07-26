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
    static const std::uint32_t cTagSize;
    static const std::uint32_t cIVSize;

    struct Header {

        std::uint32_t option;
        std::uint32_t src_uid;
        std::uint32_t dst_uid;
        std::uint32_t payload_size;
        std::uint32_t frame_counter;

        Header() {
            option = 0;
            src_uid = 0;
            dst_uid = 0;
            payload_size = 0;
            frame_counter = 0;
        }
    };

    Protocol();
    ~Protocol();

    void Clear() { mPacket.clear(); }
    void Add(const std::string &packet) { mPacket += packet; }
    bool Parse(std::string &payload, Header &h);
    std::string Build(std::uint32_t src, std::uint32_t dst, const std::string &arg);

    // Session Encryption Key
    // Session Signature Key
    void SetSecurty(const std::string &usk);

private:
    std::string mPacket;
    std::string mUSK;
    uint32_t mTxFrameCounter = 0;
    uint32_t mRxFrameCounter = 0;

    bool ParseUint32(const char *data, uint32_t size, std::uint32_t &value);
    bool Extract(const std::string &headerString, Header &h);
    void Encrypt(const std::string &header, const std::string &payload, const std::string &iv, std::string &output);
    bool Decrypt(const std::string &header, uint8_t *ciphered, uint32_t size, std::string &output);
};

#endif // PROTOCOL_H

//=============================================================================
// End of file Protocol.h
//=============================================================================
