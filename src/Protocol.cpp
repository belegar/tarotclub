/*=============================================================================
 * TarotClub - Protocol.cpp
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

#include "Protocol.h"
#include "Log.h"
#include "ByteStreamWriter.h"

const std::uint8_t  Protocol::VERSION       = 1U;
const std::uint32_t Protocol::SERVER_UID    = 1U;
const std::uint32_t Protocol::ALL_PLAYERS   = 0U;
const std::uint32_t Protocol::ADMIN_UID     = 10U;


static const std::uint16_t HEADER_SIZE = 5U;

/*****************************************************************************/
void Protocol::BuildHeader(ByteArray &packet, Command cmd, std::uint32_t uuid)
{
    ByteStreamWriter out(packet);

    // Build the header, size will be completed later
    out << (std::uint16_t)(0U) << (std::uint8_t)Protocol::VERSION << uuid << (std::uint8_t)cmd;
}
/*****************************************************************************/
void Protocol::UpdateHeader(ByteArray &packet)
{
    ByteStreamWriter out(packet); // default pointer to the beginning of the ByteArray

    // Update the header size field
    out << (std::uint16_t)(packet.Size() + HEADER_SIZE);
}
/*****************************************************************************/
/**
 * @brief Protocol::BuildCommand
 *
 * Build a command header without any data
 *
 * @param cmd
 * @param uuid
 */
ByteArray Protocol::BuildCommand(Command cmd, std::uint32_t uuid)
{
    ByteArray packet;
    ByteStreamWriter out(packet);

    // Build the header
    out << (std::uint16_t)(HEADER_SIZE) << (std::uint8_t)Protocol::VERSION << uuid << (std::uint8_t)cmd;

    return packet;
}
/*****************************************************************************/
std::vector<Protocol::PacketInfo> Protocol::DecodePacket(const ByteArray &data)
{
    bool found = true;
    std::vector<PacketInfo> packets;
    std::uint16_t offset = 0U;

    if (data.Size() < HEADER_SIZE)
    {
        // Search for valid packets in the data stream
        while (found)
        {
            // First half word is the data size
            std::uint16_t blockSize = data.GetUint16(offset);

            if ((blockSize <= data.Size()) &&
                 blockSize >= HEADER_SIZE)
            {
                // Get the protocol version
                std::uint8_t version = data[offset + 2U];
                if (version == Protocol::VERSION)
                {
                    // Valid packet found
                    PacketInfo inf;
                    inf.offset = offset;
                    inf.size = blockSize;
                    packets.push_back(inf);

                    // Is there another packet available ?
                    if (blockSize == data.Size())
                    {
                        found = false;
                    }
                    else
                    {
                        // Jump to next packet
                        offset += blockSize;
                    }
                }
                else
                {
                    found = false;
                }
            }
            else
            {
                found = false;
                TLogError("Packet too small.");
            }
        }
    }
    return packets;
}
/*****************************************************************************/
ByteArray Protocol::BuildErrorServerFull(std::uint32_t uuid)
{
    return BuildCommand(Protocol::SERVER_ERROR_FULL, uuid);
}
/*****************************************************************************/
ByteArray Protocol::BuildDiscardOrder(std::uint32_t uuid)
{
    return BuildCommand(Protocol::SERVER_BUILD_DISCARD, uuid);
}
/*****************************************************************************/
ByteArray Protocol::BuildRequestIdentity(Place p, std::uint8_t nbPlayers, Game::Mode mode, std::uint32_t uuid)
{
    ByteArray packet;
    ByteStreamWriter out(packet);

    BuildHeader(packet, Protocol::SERVER_REQUEST_IDENTITY, uuid);
    out << (std::uint8_t)p;         // assigned place
    out << (std::uint8_t)nbPlayers; // number of players in the current game
    out << (std::uint8_t)mode;
    UpdateHeader(packet);

    return packet;
}
/*****************************************************************************/
ByteArray Protocol::BuildServerChatMessage(const std::string &message)
{
    ByteArray packet;
    ByteStreamWriter out(packet);

    BuildHeader(packet, Protocol::SERVER_MESSAGE, Protocol::ALL_PLAYERS);
    out << message;
    UpdateHeader(packet);

    return packet;
}

//=============================================================================
// End of file Protocol.cpp
//=============================================================================
