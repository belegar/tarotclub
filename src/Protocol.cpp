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

#include <iomanip>
#include <sstream>
#include "Protocol.h"
#include "Log.h"
#include "Util.h"


// Specific static UUID
const std::uint32_t Protocol::INVALID_UID       = 0U;
const std::uint32_t Protocol::LOBBY_UID         = 1U;

const std::uint32_t Protocol::USERS_UID         = 10U;
const std::uint32_t Protocol::MAXIMUM_USERS     = 200U;
const std::uint32_t Protocol::TABLES_UID        = 1000U;
const std::uint32_t Protocol::MAXIMUM_TABLES    = 50U;


const std::string Protocol::cTypeData = "DATA";
const std::uint32_t Protocol::cHeaderSize = 23U;

/**
 * \page protocol Protocol format
 * The aim of the protocol is to be simple and printable (all ASCII).
 * Room is reserved for future improvements such as encryption facilities
 *
 *     OO:SSSS:DDDD:LLLL:TTTT:<argument>
 *
 * OO protocol option byte, in HEX (ex: B4)
 * SSSS is always a 4 digits unsigned integer in HEX that indicates the source UUID (max: FFFF)
 * DDDD same format, indicates the destination UUID (max: FFFF)
 * LLLL: the length of the argument (can be zero), followed by the payload bytes <argument>, typically in JSON format that allow complex structures
 * TTTT: Packet type in ASCII
 */

/*****************************************************************************/
Protocol::Protocol()
    : mSrcUuid(0U)
    , mDstUuid(0U)
    , mOption(0U)
    , mSize(0U)
{


}
/*****************************************************************************/
Protocol::~Protocol()
{

}
/*****************************************************************************/
std::string Protocol::Build(std::uint32_t src, std::uint32_t dst, const std::string &arg)
{
    std::stringstream stream;
    static const std::uint32_t option = 0U;

    stream << std::setfill ('0') << std::setw(2) << std::hex << option;
    stream << ":" << std::setfill ('0') << std::setw(4) << std::hex << src;
    stream << ":" << std::setfill ('0') << std::setw(4) << std::hex << dst;
    stream << ":" << std::setfill ('0') << std::setw(4) << std::hex << arg.size();
    stream << ":" << Protocol::cTypeData;
    stream << ":" << arg;

    return stream.str();
}
/*****************************************************************************/
bool Protocol::ParseUint32(const char* data, std::uint32_t size, std::uint32_t &value)
{
    bool ret = false;
    char *ptr;
    value = std::strtoul(data, &ptr, 16);
    std::uint32_t comp_size = ptr - data;
    if (size == comp_size)
    {
        ret = true;
    }
    return ret;
}
/*****************************************************************************/
bool Protocol::Parse(std::string &payload)
{
    bool valid = false;

    if (mPacket.size() >= cHeaderSize)
    {
        if (Extract(mPacket))
        {
            // Compute if we have a full payload
            std::uint32_t computedSize = cHeaderSize + mSize;
            if (mPacket.size() >= computedSize)
            {
                payload = mPacket.substr(cHeaderSize, mSize);
                mPacket.erase(0, computedSize);
                valid = true;
            }
            else
            {
                TLogNetwork("Proto: partial packet received, waiting for full one");
            }
        }
        else
        {
            TLogNetwork("Bad packet, cleaning...");
            mPacket.erase(0, cHeaderSize); // Skip this header, try to find another one
        }
    }

    return valid;
}
/*****************************************************************************/
bool Protocol::Extract(const std::string &header)
{
    bool ret = false;

    std::uint32_t cpt = 0U;

    // Analyze the packet without buffer copy
    std::uint32_t start = 0U, end = 0U;

    while (end < header.size())
    {
        if (header[end] == ':')
        {
            std::uint32_t size = end-start;
            if ((cpt == 0U) && (size == 2U))
            {
                ret = ParseUint32(&header[start], size, mOption);
            }
            else if ((cpt == 1U) && (size == 4U))
            {
                ret = ret && ParseUint32(&header[start], size, mSrcUuid);
            }
            else if ((cpt == 2U) && (size == 4U))
            {
                ret = ret && ParseUint32(&header[start], size, mDstUuid);
            }
            else if ((cpt == 3U) && (size == 4U))
            {
                ret = ret && ParseUint32(&header[start], size, mSize);
            }
            else if ((cpt == 4U) && (size == 4U))
            {
                mType.assign(&header[start], size);
            }
            else
            {
                // bad header or partial one
                break;
            }
            cpt++;
            start = end + 1U;
        }
        end++;
    }

    if (cpt != 5U)
    {
        ret = false;
    }

    return ret;
}

/*****************************************************************************/
std::uint32_t Protocol::GetSourceUuid()
{
    return mSrcUuid;
}
/*****************************************************************************/
std::uint32_t Protocol::GetDestUuid()
{
    return mDstUuid;
}
/*****************************************************************************/
std::string Protocol::GetType()
{
    return mType;
}


//=============================================================================
// End of file Protocol.cpp
//=============================================================================
