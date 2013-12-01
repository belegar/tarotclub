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

const std::uint8_t Protocol::VERSION = 1U;

static const std::uint16_t HEADER_SIZE = 5U;

/*****************************************************************************/
ByteArray Protocol::BuildCommand(const ByteArray &packet, Command cmd, std::uint32_t uuid)
{
    ByteArray block;
    ByteStream out(block);

    // Build the header
    out << (quint16)(packet.Size() + HEADER_SIZE) << (quint8)Protocol::VERSION << uuid << (quint8)cmd;
    // Append the data
    block += packet;
    return block;
}
/*****************************************************************************/
std::uint8_t Protocol::DecodePacket(const ByteArray &data)
{
    bool found = true;
    std::uint8_t count = 0U;
    std::uint16_t offset = 0U;

    if (data.Size() < HEADER_SIZE)
    {
        return 0U;
    }

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
                count++;
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
    return count;
}


//=============================================================================
// End of file Protocol.cpp
//=============================================================================
