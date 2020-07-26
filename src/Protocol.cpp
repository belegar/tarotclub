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
#include <cstring>
#include <sstream>
#include "Protocol.h"
#include "Log.h"
#include "Util.h"
#include "mbedtls/md.h"
#include "mbedtls/sha256.h"
#include "mbedtls/gcm.h"
#include "mbedtls/aes.h"

// Specific static UUID
const std::uint32_t Protocol::INVALID_UID       = 0U;
const std::uint32_t Protocol::LOBBY_UID         = 1U;
const std::uint32_t Protocol::WEBSITE_UID       = 2U;

const std::uint32_t Protocol::USERS_UID         = 10U;
const std::uint32_t Protocol::MAXIMUM_USERS     = 200U;
const std::uint32_t Protocol::TABLES_UID        = 1000U;
const std::uint32_t Protocol::MAXIMUM_TABLES    = 50U;


const std::uint32_t Protocol::cHeaderSize       = 23U;
const std::uint32_t Protocol::cTagSize          = 16U;
const std::uint32_t Protocol::cIVSize           = 12U;

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
{


}
/*****************************************************************************/
Protocol::~Protocol()
{

}
/*****************************************************************************/
void Protocol::Encrypt(const std::string &header, const std::string &payload, const std::string &iv, std::string &output)
{
    uint8_t tag[cTagSize];
    uint8_t scratch[payload.size()];

/*
 * uint8_t plainText[payload.size()];
    mbedtls_gcm_context ctx;
    mbedtls_gcm_init (&ctx);
    mbedtls_gcm_setkey (&ctx, MBEDTLS_CIPHER_ID_AES, reinterpret_cast<const unsigned char *>(mUSK.data()), 128);
    mbedtls_gcm_crypt_and_tag(&ctx, MBEDTLS_GCM_ENCRYPT, payload.size(),
                              reinterpret_cast<const unsigned char *>(iv.data()), 12,
                              reinterpret_cast<const unsigned char *>(header.data()), header.size(),
                              reinterpret_cast<const unsigned char *>(payload.data()),
                              scratch, 16, tag
                              );

    mbedtls_gcm_init (&ctx);
    mbedtls_gcm_setkey (&ctx, MBEDTLS_CIPHER_ID_AES, reinterpret_cast<const unsigned char *>(mUSK.data()), 128);
    int ret = mbedtls_gcm_auth_decrypt(&ctx, payload.size(),
                              reinterpret_cast<const unsigned char *>(iv.data()), 12,
                              reinterpret_cast<const unsigned char *>(header.data()), header.size(),
                               tag, cTagSize,
                               scratch, plainText
                              );

    if (ret == 0)
    {
        printf("success");
    }
    */

    mbedtls_gcm_context ctx;
    mbedtls_gcm_init (&ctx);
    mbedtls_gcm_setkey (&ctx, MBEDTLS_CIPHER_ID_AES, reinterpret_cast<const unsigned char *>(mUSK.data()), 128);
    mbedtls_gcm_starts (&ctx, MBEDTLS_GCM_ENCRYPT,
                        reinterpret_cast<const unsigned char *>(iv.data()), 12,
                        reinterpret_cast<const unsigned char *>(header.data()), header.size());

    int bound_size = (payload.size() >> 4) * 16;
    int last_packet_size = payload.size() - bound_size;

    if (bound_size > 0)
    {
        mbedtls_gcm_update (&ctx, bound_size, reinterpret_cast<const unsigned char *>(payload.data()), scratch);
    }
    if (last_packet_size > 0)
    {
        mbedtls_gcm_update (&ctx, last_packet_size, reinterpret_cast<const unsigned char *>(payload.data() + bound_size) , scratch + bound_size);
    }

    // After the loop
    mbedtls_gcm_finish (&ctx, tag, cTagSize);
    mbedtls_gcm_free (&ctx);

    output.append(iv);
    output.append(reinterpret_cast<char *>(scratch), sizeof(scratch));
    output.append(reinterpret_cast<char *>(tag), cTagSize);

}
/*****************************************************************************/
bool Protocol::Decrypt(const std::string &header, uint8_t *ciphered, uint32_t size, std::string &output)
{
    uint8_t tag_computed[cTagSize];
    uint8_t tag[cTagSize];
    uint8_t iv[cIVSize];

    uint8_t *payload = ciphered + cIVSize;

    memcpy(tag, ciphered + (size - cTagSize), cTagSize);
    memcpy(iv, ciphered , cIVSize);

    uint32_t plainTextSize = size - (cIVSize + cTagSize);
    uint8_t plainText[plainTextSize];

    mbedtls_gcm_context ctx;
    mbedtls_gcm_init (&ctx);
    mbedtls_gcm_setkey (&ctx, MBEDTLS_CIPHER_ID_AES, reinterpret_cast<const unsigned char *>(mUSK.data()), 128);
/*
    int ret = mbedtls_gcm_auth_decrypt(&ctx, plainTextSize,
                                       iv, cIVSize,
                                       reinterpret_cast<const unsigned char *>(header.data()), header.size(),
                                       tag, cTagSize,
                                       payload, plainText
                                       );*/

    mbedtls_gcm_starts (&ctx, MBEDTLS_GCM_DECRYPT,
                        iv, cIVSize,
                        reinterpret_cast<const unsigned char *>(header.data()), header.size());

    int bound_size = (plainTextSize >> 4) * 16;
    int last_packet_size = plainTextSize - bound_size;

    if (bound_size > 0)
    {
        mbedtls_gcm_update (&ctx, bound_size, payload, plainText);
    }
    if (last_packet_size > 0)
    {
        mbedtls_gcm_update (&ctx, last_packet_size, payload + bound_size , plainText + bound_size);
    }

    // After the loop
    mbedtls_gcm_finish (&ctx, tag_computed, cTagSize);
    mbedtls_gcm_free (&ctx);
    output.append(reinterpret_cast<char *>(plainText), sizeof(plainText));
    return std::memcmp(&tag_computed[0], tag, cTagSize) == 0;
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
    stream << ":" << std::setfill ('0') << std::setw(4) << std::hex << mTxFrameCounter;
    stream << ":";

    std::string payload; // IV + data + tag
    std::string iv = "000000000000"; // Util::GenerateRandomString(cIVSize)
    Encrypt(stream.str(), arg, iv, payload);
    stream << Util::ToHex(reinterpret_cast<const char *>(payload.data()), payload.size());

    mTxFrameCounter++;
    return stream.str();
}
/*****************************************************************************/
void Protocol::SetSecurty(const std::string &usk)
{
    mUSK = usk;
    mRxFrameCounter = 0;
    mTxFrameCounter = 0;
}
/*****************************************************************************/
bool Protocol::ParseUint32(const char* data, std::uint32_t size, std::uint32_t &value)
{
    bool ret = false;
    char *ptr;
    value = std::strtoul(data, &ptr, 16);
    std::uint32_t comp_size = static_cast<std::uint32_t>(ptr - data);
    if (size == comp_size)
    {
        ret = true;
    }
    return ret;
}
/*****************************************************************************/
bool Protocol::Parse(std::string &payload, Header &h)
{
    bool valid = false;

    if (mPacket.size() >= cHeaderSize)
    {
        if (Extract(mPacket, h))
        {
            if (h.payload_size > 0)
            {
                // Compute if we have a full payload
                uint32_t hexPayloadSize = (cIVSize + h.payload_size + cTagSize) * 2;
                std::uint32_t computedSize = cHeaderSize + hexPayloadSize;
                if (mPacket.size() >= computedSize)
                {
                    std::string header = mPacket.substr(0, cHeaderSize);
                    std::string hexString = mPacket.substr(cHeaderSize, hexPayloadSize);
                    uint32_t cipheredPayloadSize = hexPayloadSize / 2;
                    uint8_t ciphered[cipheredPayloadSize];
                    Util::HexStringToUint8(hexString, ciphered);
                    valid = Decrypt(header, ciphered, cipheredPayloadSize, payload);
                    mPacket.erase(0, computedSize);
                }
                else
                {
                    TLogNetwork("Proto: partial packet received, waiting for full one");
                }
            }
            else
            {
                valid = true;
                mPacket.erase(0, cHeaderSize);
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
bool Protocol::Extract(const std::string &headerString, Header &h)
{
    bool ret = false;

    std::uint32_t cpt = 0U;

    // Analyze the packet without buffer copy
    std::uint32_t start = 0U, end = 0U;

    while (end < headerString.size())
    {
        if (headerString[end] == ':')
        {
            std::uint32_t size = end-start;
            if ((cpt == 0U) && (size == 2U))
            {
                ret = ParseUint32(&headerString[start], size, h.option);
            }
            else if ((cpt == 1U) && (size == 4U))
            {
                ret = ret && ParseUint32(&headerString[start], size, h.src_uid);
            }
            else if ((cpt == 2U) && (size == 4U))
            {
                ret = ret && ParseUint32(&headerString[start], size, h.dst_uid);
            }
            else if ((cpt == 3U) && (size == 4U))
            {
                ret = ret && ParseUint32(&headerString[start], size, h.payload_size);
            }
            else if ((cpt == 4U) && (size == 4U))
            {
                ret = ret && ParseUint32(&headerString[start], size, h.frame_counter);
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


//=============================================================================
// End of file Protocol.cpp
//=============================================================================
