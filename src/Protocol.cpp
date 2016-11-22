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
/*****************************************************************************/
std::string Protocol::Build(std::uint32_t option, std::uint32_t src, std::uint32_t dst, const std::string &type, const std::string &arg)
{
    std::stringstream stream;

    stream << ":" << std::setfill ('0') << std::setw(2) << std::hex << option;
    stream << ":" << std::setfill ('0') << std::setw(4) << std::hex << src;
    stream << ":" << std::setfill ('0') << std::setw(4) << std::hex << dst;
    stream << ":" << std::setfill ('0') << type;
    stream << ":" << std::setfill ('0') << std::setw(4) << std::hex << arg.size() << arg;

    return stream.str();
}


#if 0

/*****************************************************************************/
ByteArray Protocol::ClientSyncNewGame(std::uint32_t client_uuid, std::uint32_t tableId)
{
    return BuildCommand(Protocol::CLIENT_SYNC_NEW_GAME, client_uuid, tableId);
}
/*****************************************************************************/
ByteArray Protocol::ClientSyncDog(std::uint32_t client_uuid, std::uint32_t tableId)
{
    return BuildCommand(Protocol::CLIENT_SYNC_SHOW_DOG, client_uuid, tableId);
}
/*****************************************************************************/
ByteArray Protocol::ClientSyncHandle(std::uint32_t client_uuid, std::uint32_t tableId)
{
    return BuildCommand(Protocol::CLIENT_SYNC_HANDLE, client_uuid, tableId);
}
/*****************************************************************************/
ByteArray Protocol::ClientSyncTrick(std::uint32_t client_uuid, std::uint32_t tableId)
{
    return BuildCommand(Protocol::CLIENT_SYNC_TRICK, client_uuid, tableId);
}
/*****************************************************************************/
ByteArray Protocol::ClientSyncEndOfDeal(std::uint32_t client_uuid, std::uint32_t tableId)
{
    return BuildCommand(Protocol::CLIENT_SYNC_END_OF_DEAL, client_uuid, tableId);
}
/*****************************************************************************/
ByteArray Protocol::ClientSyncShowCard(std::uint32_t client_uuid, std::uint32_t tableId)
{
    return BuildCommand(Protocol::CLIENT_SYNC_SHOW_CARD, client_uuid, tableId);
}
/*****************************************************************************/
ByteArray Protocol::ClientSyncStart(std::uint32_t client_uuid, std::uint32_t tableId)
{
    return BuildCommand(Protocol::CLIENT_SYNC_START, client_uuid, tableId);
}
/*****************************************************************************/
ByteArray Protocol::ClientSyncBid(std::uint32_t client_uuid, std::uint32_t tableId)
{
    return BuildCommand(Protocol::CLIENT_SYNC_SHOW_BID, client_uuid, tableId);
}
/*****************************************************************************/
ByteArray Protocol::ClientSyncAllPassed(std::uint32_t client_uuid, std::uint32_t tableId)
{
    return BuildCommand(Protocol::CLIENT_SYNC_ALL_PASSED, client_uuid, tableId);
}
/*****************************************************************************/
ByteArray Protocol::ClientSyncNewDeal(std::uint32_t client_uuid, std::uint32_t tableId)
{
    return BuildCommand(Protocol::CLIENT_SYNC_NEW_DEAL, client_uuid, tableId);
}
/*****************************************************************************/
ByteArray Protocol::ClientError(std::uint32_t client_uuid)
{
    return BuildCommand(Protocol::CLIENT_ERROR, client_uuid, LOBBY_UID);
}
/*****************************************************************************/
ByteArray Protocol::ClientJoinTable(std::uint32_t client_uuid, std::uint32_t tableId)
{
    ByteArray packet;
    ByteStreamWriter out(packet);

    BuildHeader(packet, Protocol::CLIENT_JOIN_TABLE, client_uuid, LOBBY_UID);
    out.Seek(HEADER_SIZE);
    out << tableId;
    UpdateHeader(packet);

    return packet;
}
/*****************************************************************************/
ByteArray Protocol::ClientQuitTable(std::uint32_t client_uuid, std::uint32_t tableId)
{
    ByteArray packet;
    ByteStreamWriter out(packet);

    BuildHeader(packet, Protocol::CLIENT_QUIT_TABLE, client_uuid, LOBBY_UID);
    out.Seek(HEADER_SIZE);
    out << tableId;
    UpdateHeader(packet);

    return packet;
}
/*****************************************************************************/
ByteArray Protocol::ClientReplyLogin(std::uint32_t client_uuid, const Identity &ident)
{
    ByteArray packet;
    ByteStreamWriter out(packet);

    BuildHeader(packet, Protocol::CLIENT_REPLY_LOGIN, client_uuid, LOBBY_UID);
    out.Seek(HEADER_SIZE);
    out << ident;
    UpdateHeader(packet);

    return packet;
}
/*****************************************************************************/
ByteArray Protocol::ClientChangeIdentity(std::uint32_t client_uuid, const Identity &ident)
{
    ByteArray packet;
    ByteStreamWriter out(packet);

    BuildHeader(packet, Protocol::CLIENT_CHANGE_IDENTITY, client_uuid, LOBBY_UID);
    out.Seek(HEADER_SIZE);
    out << ident;
    UpdateHeader(packet);

    return packet;
}
/*****************************************************************************/
ByteArray Protocol::ClientLobbyMessage(const std::string &message, std::uint32_t client_uuid, std::uint32_t target)
{
    ByteArray packet;
    ByteStreamWriter out(packet);

    BuildHeader(packet, Protocol::CLIENT_CHAT_MESSAGE, client_uuid, Protocol::LOBBY_UID);
    out.Seek(HEADER_SIZE);
    out << message;
    out << target;
    UpdateHeader(packet);

    return packet;
}
/*****************************************************************************/
ByteArray Protocol::ClientDiscard(const Deck &discard, std::uint32_t client_uuid, std::uint32_t tableId)
{
    ByteArray packet;
    ByteStreamWriter out(packet);

    BuildHeader(packet, Protocol::CLIENT_DISCARD, client_uuid, tableId);
    out.Seek(HEADER_SIZE);
    out << discard;
    UpdateHeader(packet);

    return packet;
}
/*****************************************************************************/
ByteArray Protocol::ClientHandle(const Deck &handle, std::uint32_t client_uuid, std::uint32_t tableId)
{
    ByteArray packet;
    ByteStreamWriter out(packet);

    BuildHeader(packet, Protocol::CLIENT_HANDLE, client_uuid, tableId);
    out.Seek(HEADER_SIZE);
    out << handle;
    UpdateHeader(packet);

    return packet;
}
/*****************************************************************************/
ByteArray Protocol::ClientCard(const std::string &card, std::uint32_t client_uuid, std::uint32_t tableId)
{
    ByteArray packet;
    ByteStreamWriter out(packet);

    BuildHeader(packet, Protocol::CLIENT_CARD, client_uuid, tableId);
    out.Seek(HEADER_SIZE);
    out << card;
    UpdateHeader(packet);

    return packet;
}
/*****************************************************************************/
ByteArray Protocol::ClientBid(Contract c, bool slam, std::uint32_t client_uuid, std::uint32_t tableId)
{
    ByteArray packet;
    ByteStreamWriter out(packet);

    BuildHeader(packet, Protocol::CLIENT_BID, client_uuid, tableId);
    out.Seek(HEADER_SIZE);
    out << c;  // contract to show
    out << slam;
    UpdateHeader(packet);

    return packet;
}
/*****************************************************************************/
ByteArray Protocol::ClientSyncJoinTable(std::uint32_t client_uuid, std::uint32_t tableId)
{
    return BuildCommand(Protocol::CLIENT_SYNC_JOIN_TABLE, client_uuid, tableId);
}
/*****************************************************************************/
ByteArray Protocol::TableFullMessage(std::uint32_t uuid, std::uint32_t tableId)
{
    return BuildCommand(Protocol::TABLE_ERROR_FULL, tableId, uuid);
}
/*****************************************************************************/
ByteArray Protocol::TableAllPassed(std::uint32_t tableId)
{
    return BuildCommand(Protocol::TABLE_ALL_PASSED, tableId, tableId);
}
/*****************************************************************************/
ByteArray Protocol::TableAskForHandle(std::uint32_t uuid, std::uint32_t tableId)
{
    return BuildCommand(Protocol::TABLE_ASK_FOR_HANDLE, tableId, uuid);
}
/*****************************************************************************/
ByteArray Protocol::TableAskForDiscard(std::uint32_t uuid, std::uint32_t tableId)
{
    return BuildCommand(Protocol::TABLE_ASK_FOR_DISCARD, tableId, uuid);
}
/*****************************************************************************/
ByteArray Protocol::TableQuitEvent(std::uint32_t uuid, std::uint32_t tableId)
{
    return BuildCommand(Protocol::TABLE_QUIT_EVENT, tableId, uuid);
}
/*****************************************************************************/
ByteArray Protocol::LobbyJoinTableReply(bool status, Place p, std::uint8_t nbPlayers, std::uint32_t uuid, std::uint32_t tableId)
{
    ByteArray packet;
    ByteStreamWriter out(packet);

    BuildHeader(packet, Protocol::LOBBY_JOIN_TABLE_REPLY, tableId, uuid);
    out.Seek(HEADER_SIZE);
    out << tableId;
    out << status;
    out << p;                       // assigned place
    out << nbPlayers;               // number of players in the current game
    UpdateHeader(packet);

    return packet;
}
/*****************************************************************************/
ByteArray Protocol::TableShowBid(Contract c, bool slam, Place p, std::uint32_t tableId)
{
    ByteArray packet;
    ByteStreamWriter out(packet);

    BuildHeader(packet, Protocol::TABLE_SHOW_PLAYER_BID, tableId, tableId);
    out.Seek(HEADER_SIZE);
    out << p; // current player bid
    out << c; // contract to show
    out << slam; // if slam declared
    UpdateHeader(packet);

    return packet;
}
/*****************************************************************************/
ByteArray Protocol::TablePlayersList(const std::map<Place, uint32_t> &players, std::uint32_t tableId)
{
    ByteArray packet;
    ByteStreamWriter out(packet);

    BuildHeader(packet, Protocol::TABLE_PLAYERS_LIST, tableId, tableId);
    out.Seek(HEADER_SIZE);
    out << (std::uint8_t)players.size();
    std::map<Place, std::uint32_t>::const_iterator iter;

    for (iter = players.begin(); iter != players.end(); ++iter)
    {
        out << iter->first;
        out << iter->second;
    }

    UpdateHeader(packet);

    return packet;
}
/*****************************************************************************/
ByteArray Protocol::TableShowCard(const Card &c, Place p, std::uint32_t tableId)
{
    ByteArray packet;
    ByteStreamWriter out(packet);

    BuildHeader(packet, Protocol::TABLE_SHOW_CARD, tableId, tableId);
    out.Seek(HEADER_SIZE);
    out << p
        << c.GetName();
    UpdateHeader(packet);

    return packet;
}
/*****************************************************************************/
ByteArray Protocol::TableShowHandle(Deck &handle, Place p, std::uint32_t tableId)
{
    ByteArray packet;
    ByteStreamWriter out(packet);

    BuildHeader(packet, Protocol::TABLE_SHOW_HANDLE, tableId, tableId);
    out.Seek(HEADER_SIZE);
    out << p;
    out << handle;
    UpdateHeader(packet);

    return packet;
}
/*****************************************************************************/
ByteArray Protocol::TableNewDeal(Player *player, std::uint32_t tableId)
{
    ByteArray packet;
    ByteStreamWriter out(packet);

    if (player != NULL)
    {
        BuildHeader(packet, Protocol::TABLE_NEW_DEAL, tableId, player->GetUuid());
        out.Seek(HEADER_SIZE);
        out << *player; // deck of cards
        UpdateHeader(packet);
    }
    return packet;
}
/*****************************************************************************/
ByteArray Protocol::TableEndOfDeal(const Points &points, const std::string &result, std::uint32_t tableId)
{
    ByteArray packet;
    ByteStreamWriter out(packet);

    BuildHeader(packet, Protocol::TABLE_END_OF_DEAL, tableId, tableId);
    out.Seek(HEADER_SIZE);
    out << points;
    out << result;
    UpdateHeader(packet);

    return packet;
}
/*****************************************************************************/
ByteArray Protocol::TableEndOfGame(Place winner, std::uint32_t tableId)
{
    ByteArray packet;
    ByteStreamWriter out(packet);

    BuildHeader(packet, Protocol::TABLE_END_OF_GAME, tableId, tableId);
    out.Seek(HEADER_SIZE);
    out << winner;
    UpdateHeader(packet);

    return packet;
}
/*****************************************************************************/
ByteArray Protocol::TableEndOfTrick(Place winner, std::uint32_t tableId)
{
    ByteArray packet;
    ByteStreamWriter out(packet);

    BuildHeader(packet, Protocol::TABLE_END_OF_TRICK, tableId, tableId);
    out.Seek(HEADER_SIZE);
    out << winner;
    UpdateHeader(packet);

    return packet;
}
/*****************************************************************************/
ByteArray Protocol::TableStartDeal(Place firstPlayer, const Tarot::Bid &bid, const Tarot::Distribution &sh, std::uint32_t tableId)
{
    ByteArray packet;
    ByteStreamWriter out(packet);

    BuildHeader(packet, Protocol::TABLE_START_DEAL, tableId, tableId);
    out.Seek(HEADER_SIZE);
    out << firstPlayer;     // first player to play
    out << bid.taker;       // taker
    out << bid.contract;    // bid declared
    out << bid.slam;        // slame declared
    out << sh;
    UpdateHeader(packet);

    return packet;
}
/*****************************************************************************/
ByteArray Protocol::TablePlayCard(Place p, std::uint32_t tableId)
{
    ByteArray packet;
    ByteStreamWriter out(packet);

    BuildHeader(packet, Protocol::TABLE_PLAY_CARD, tableId, tableId);
    out.Seek(HEADER_SIZE);
    out << p; // this player has to play a card
    UpdateHeader(packet);

    return packet;
}
/*****************************************************************************/
ByteArray Protocol::TableBidRequest(Contract c, Place p, std::uint32_t tableId)
{
    ByteArray packet;
    ByteStreamWriter out(packet);

    BuildHeader(packet, Protocol::TABLE_REQUEST_BID, tableId, tableId);
    out.Seek(HEADER_SIZE);
    out << c; // previous bid
    out << p; // player to declare something
    UpdateHeader(packet);

    return packet;
}
/*****************************************************************************/
ByteArray Protocol::TableShowDog(const Deck &dog, std::uint32_t tableId)
{
    ByteArray packet;
    ByteStreamWriter out(packet);

    BuildHeader(packet, Protocol::TABLE_SHOW_DOG, tableId, tableId);
    out.Seek(HEADER_SIZE);
    out << dog;
    UpdateHeader(packet);

    return packet;
}
/*****************************************************************************/
ByteArray Protocol::TableNewGame(const Tarot::Game &game, std::uint32_t tableId)
{
    ByteArray packet;
    ByteStreamWriter out(packet);

    BuildHeader(packet, Protocol::TABLE_NEW_GAME, tableId, tableId);
    out.Seek(HEADER_SIZE);
    out << game;
    UpdateHeader(packet);

    return packet;
}
/*****************************************************************************/
ByteArray Protocol::LobbyChatMessage(const std::string &message, std::uint32_t target)
{
    ByteArray packet;
    ByteStreamWriter out(packet);

    BuildHeader(packet, Protocol::LOBBY_CHAT_MESSAGE, LOBBY_UID, target);
    out.Seek(HEADER_SIZE);
    out << message;
    UpdateHeader(packet);

    return packet;
}
/*****************************************************************************/
ByteArray Protocol::LobbyRequestLogin(std::uint32_t uuid)
{
    ByteArray packet;
    ByteStreamWriter out(packet);

    BuildHeader(packet, Protocol::LOBBY_REQUEST_LOGIN, LOBBY_UID, uuid);
    out.Seek(HEADER_SIZE);
    out << uuid;                    // assigned unique id
    UpdateHeader(packet);

    return packet;
}
/*****************************************************************************/
ByteArray Protocol::LobbyLoginResult(bool accepted, const std::map<std::string, std::uint32_t> &tableList, std::uint32_t uuid)
{
    ByteArray packet;
    ByteStreamWriter out(packet);

    BuildHeader(packet, Protocol::LOBBY_LOGIN_RESULT, LOBBY_UID, uuid);
    out.Seek(HEADER_SIZE);
    out << accepted;                // true if we are accepted to the server
    out << static_cast<std::uint32_t>(tableList.size());

    for (std::map<std::string, std::uint32_t>::const_iterator iter = tableList.begin(); iter != tableList.end(); ++iter)
    {
        out << iter->first;
        out << iter->second;
    }

    UpdateHeader(packet);

    return packet;
}
/*****************************************************************************/
ByteArray Protocol::LobbyPlayersList(const std::map<std::uint32_t, Identity> &players)
{
    ByteArray packet;
    ByteStreamWriter out(packet);

    BuildHeader(packet, Protocol::LOBBY_PLAYERS_LIST, LOBBY_UID, LOBBY_UID);
    out.Seek(HEADER_SIZE);
    out << (std::uint8_t)players.size();
    std::map<std::uint32_t, Identity>::const_iterator iter;

    for (iter = players.begin(); iter != players.end(); ++iter)
    {
        out << iter->first;
        out << iter->second;
    }

    UpdateHeader(packet);

    return packet;
}
/*****************************************************************************/
ByteArray Protocol::AdminNewGame(const Tarot::Game &game, std::uint32_t uuid, std::uint32_t tableId)
{
    ByteArray packet;
    ByteStreamWriter out(packet);

    BuildHeader(packet, Protocol::ADMIN_NEW_GAME, uuid, tableId);
    out.Seek(HEADER_SIZE);
    out << game;
    UpdateHeader(packet);

    return packet;
}
/*****************************************************************************/
ByteArray Protocol::AdminGameFull(bool full, std::uint32_t uuid, std::uint32_t tableId)
{
    ByteArray packet;
    ByteStreamWriter out(packet);

    BuildHeader(packet, Protocol::ADMIN_GAME_FULL, tableId, uuid);
    out.Seek(HEADER_SIZE);
    out << full;
    UpdateHeader(packet);

    return packet;
}

#endif


//=============================================================================
// End of file Protocol.cpp
//=============================================================================
