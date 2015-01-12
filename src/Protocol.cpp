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

const std::uint8_t  Protocol::VERSION       = 2U;

// Specific static UUID
const std::uint32_t Protocol::INVALID_UID   = 0U;
const std::uint32_t Protocol::ALL_LOBBY     = 1U;
const std::uint32_t Protocol::ALL_TABLE     = 2U;
const std::uint32_t Protocol::LOBBY_UID     = 3U;
const std::uint32_t Protocol::TABLE_UID     = 4U;

const std::uint32_t Protocol::USERS_UID     = 10U;
const std::uint32_t Protocol::NO_TABLE      = 0U;

// Variables to parse the packet
static const std::uint16_t HEADER_SIZE      = 12U;

// Offsets (in bytes) from the start of one packet
static const std::uint32_t Protocol::VERSION_OFFSET   = 2U;
static const std::uint32_t Protocol::SRC_UUID_OFFSET  = 3U;
static const std::uint32_t Protocol::DEST_UUID_OFFSET = 7U;
static const std::uint32_t Protocol::COMMAND_OFFSET   = 11U;

/*****************************************************************************/
Protocol::Protocol()
    : mInitialized(false)
{


}
/*****************************************************************************/
Protocol::~Protocol()
{

}
/*****************************************************************************/
void Protocol::BuildHeader(ByteArray &packet, Command cmd, std::uint32_t src_uuid, std::uint32_t dest_uuid)
{
    ByteStreamWriter out(packet);

    // Build the header, size will be completed later
    out << (std::uint16_t)(0U)  // actually, the size of the packet, including this field
        << (std::uint8_t)Protocol::VERSION  // Version of the protocol
        << src_uuid     // Packet source uuid
        << dest_uuid    // packet destination uuid
        << (std::uint8_t)cmd; // command
}
/*****************************************************************************/
void Protocol::UpdateHeader(ByteArray &packet)
{
    ByteStreamWriter out(packet);

    out.Seek(0U); // Get back to the header area

    // Update the header size field
    out << (std::uint16_t)packet.Size();
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
ByteArray Protocol::BuildCommand(Command cmd, std::uint32_t src_uuid, std::uint32_t dest_uuid)
{
    ByteArray packet;
    BuildHeader(packet, cmd, src_uuid, dest_uuid);
    UpdateHeader(packet);
    return packet;
}
/*****************************************************************************/
static Protocol mProtocol;
Protocol &Protocol::GetInstance()
{
    return mProtocol;
}
/*****************************************************************************/
std::uint32_t Protocol::GetSourceUuid(const ByteArray &packet)
{
    return packet.GetUint32(SRC_UUID_OFFSET);
}
/*****************************************************************************/
std::uint32_t Protocol::GetDestUuid(const ByteArray &packet)
{
    return packet.GetUint32(DEST_UUID_OFFSET);
}
/*****************************************************************************/
void Protocol::Initialize()
{
    if (!mInitialized)
    {
        mInitialized = true;
        mThread = std::thread(Protocol::EntryPoint, this);
    }
}
/*****************************************************************************/
void Protocol::Stop()
{
    if (mInitialized)
    {
        mQueue.Push(std::pair<bool, Protocol::IWorkItem*>(false, NULL));
        mThread.join();
        mInitialized = false;
    }
}
/*****************************************************************************/
void Protocol::Execute(IWorkItem *item)
{
    mQueue.Push(std::pair<bool, Protocol::IWorkItem*>(true, item));
}
/*****************************************************************************/
void Protocol::EntryPoint(void *pthis)
{
    Protocol *pt = static_cast<Protocol *>(pthis);
    pt->Run();
}
/*****************************************************************************/
/**
 * @brief Controller::Run
 *
 * This is the main shared work item thread; It manages the network (or not) packets
 * sent by client, the lobby or an admin. All the packets are serialized to ensure
 * that they are treated in a queue, one at a time.
 *
 */
void Protocol::Run()
{
   // (void) pthis;
    std::pair<bool, IWorkItem *> request;
    bool continueThread = true;

    while (continueThread)
    {
        mQueue.WaitAndPop(request);

        if (request.first)
        {
            IWorkItem *item = request.second;
            if (item != NULL)
            {
                ByteArray data = item->GetPacket();
                continueThread = DataManagement(item, data);
            }
        }
        else
        {
            continueThread = false;
        }
    }
}
/*****************************************************************************/
bool Protocol::DataManagement(IWorkItem *item, const ByteArray &data)
{
    bool ret = true;
    std::vector<Protocol::PacketInfo> packets = Protocol::DecodePacket(data);

    // Execute all packets
    for (std::uint16_t i = 0U; i < packets.size(); i++)
    {
        Protocol::PacketInfo inf = packets[i];

        ByteArray subArray = data.SubArray(inf.offset, inf.size);
        std::uint32_t src_uuid = subArray.GetUint32(SRC_UUID_OFFSET);
        std::uint32_t dest_uuid = subArray.GetUint32(DEST_UUID_OFFSET);
        std::uint8_t cmd = subArray.Get(COMMAND_OFFSET);
        subArray.Erase(0U, HEADER_SIZE);
        if (!item->DoAction(cmd, src_uuid, dest_uuid, subArray))
        {
            ret = false;
        }
    }
    return ret;
}
/*****************************************************************************/
std::vector<Protocol::PacketInfo> Protocol::DecodePacket(const ByteArray &data)
{
    std::vector<PacketInfo> packets;
    std::uint16_t offset = 0U;
    std::uint16_t totalSize = 0U;

    if (data.Size() >= HEADER_SIZE)
    {
        bool found = true;
        // Search for valid packets in the data stream
        while (found)
        {
            // First half word is the data size
            std::uint16_t blockSize = data.GetUint16(offset);

            if ((blockSize <= data.Size()) &&
                    blockSize >= HEADER_SIZE)
            {
                // Get the protocol version
                std::uint8_t version = data.Get(offset + VERSION_OFFSET);
                if (version == Protocol::VERSION)
                {
                    // Valid packet found
                    PacketInfo inf;
                    inf.offset = offset;
                    inf.size = blockSize;
                    packets.push_back(inf);

                    totalSize += blockSize;
                    // Is there another packet available ?
                    if (totalSize >= data.Size())
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
                    // Drop the rest of the packet
                    found = false;
                    TLogNetwork("Bad protocol version.");
                }
            }
            else
            {
                found = false;
                TLogNetwork("Sub-packet too small.");
            }
        }
    }
    else
    {
        TLogNetwork("Received packet too small.");
    }
    return packets;
}
/*****************************************************************************/
ByteArray Protocol::ClientSyncNewGame(std::uint32_t client_uuid)
{
    return BuildCommand(Protocol::CLIENT_SYNC_NEW_GAME, client_uuid, TABLE_UID);
}
/*****************************************************************************/
ByteArray Protocol::ClientSyncDog(std::uint32_t client_uuid)
{
    return BuildCommand(Protocol::CLIENT_SYNC_SHOW_DOG, client_uuid, TABLE_UID);
}
/*****************************************************************************/
ByteArray Protocol::ClientSyncHandle(std::uint32_t client_uuid)
{
    return BuildCommand(Protocol::CLIENT_SYNC_HANDLE, client_uuid, TABLE_UID);
}
/*****************************************************************************/
ByteArray Protocol::ClientSyncTrick(std::uint32_t client_uuid)
{
    return BuildCommand(Protocol::CLIENT_SYNC_TRICK, client_uuid, TABLE_UID);
}
/*****************************************************************************/
ByteArray Protocol::ClientSyncEndOfDeal(std::uint32_t client_uuid)
{
    return BuildCommand(Protocol::CLIENT_SYNC_END_OF_DEAL, client_uuid, TABLE_UID);
}
/*****************************************************************************/
ByteArray Protocol::ClientSyncShowCard(std::uint32_t client_uuid)
{
    return BuildCommand(Protocol::CLIENT_SYNC_SHOW_CARD, client_uuid, TABLE_UID);
}
/*****************************************************************************/
ByteArray Protocol::ClientSyncStart(std::uint32_t client_uuid)
{
    return BuildCommand(Protocol::CLIENT_SYNC_START, client_uuid, TABLE_UID);
}
/*****************************************************************************/
ByteArray Protocol::ClientSyncBid(std::uint32_t client_uuid)
{
    return BuildCommand(Protocol::CLIENT_SYNC_SHOW_BID, client_uuid, TABLE_UID);
}
/*****************************************************************************/
ByteArray Protocol::ClientSyncAllPassed(std::uint32_t client_uuid)
{
    return BuildCommand(Protocol::CLIENT_SYNC_ALL_PASSED, client_uuid, TABLE_UID);
}
/*****************************************************************************/
ByteArray Protocol::ClientSyncCards(std::uint32_t client_uuid)
{
    return BuildCommand(Protocol::CLIENT_SYNC_NEW_DEAL, client_uuid, TABLE_UID);
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
// Client does not have yet any UUID, so we use a generic one
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
ByteArray Protocol::ClientLobbyMessage(const std::string &message, std::uint32_t client_uuid)
{
    ByteArray packet;
    ByteStreamWriter out(packet);

    BuildHeader(packet, Protocol::CLIENT_LOBBY_MESSAGE, client_uuid, LOBBY_UID);
    out.Seek(HEADER_SIZE);
    out << message;
    UpdateHeader(packet);

    return packet;
}
/*****************************************************************************/
ByteArray Protocol::ClientTableMessage(const std::string &message, std::uint32_t client_uuid)
{
    ByteArray packet;
    ByteStreamWriter out(packet);

    BuildHeader(packet, Protocol::CLIENT_TABLE_MESSAGE, client_uuid, TABLE_UID);
    out.Seek(HEADER_SIZE);
    out << message;
    UpdateHeader(packet);

    return packet;
}
/*****************************************************************************/
ByteArray Protocol::ClientDiscard(const Deck &discard, std::uint32_t client_uuid)
{
    ByteArray packet;
    ByteStreamWriter out(packet);

    BuildHeader(packet, Protocol::CLIENT_DISCARD, client_uuid, TABLE_UID);
    out.Seek(HEADER_SIZE);
    out << discard;
    UpdateHeader(packet);

    return packet;
}
/*****************************************************************************/
ByteArray Protocol::ClientHandle(const Deck &handle, std::uint32_t client_uuid)
{
    ByteArray packet;
    ByteStreamWriter out(packet);

    BuildHeader(packet, Protocol::CLIENT_HANDLE, client_uuid, TABLE_UID);
    out.Seek(HEADER_SIZE);
    out << handle;
    UpdateHeader(packet);

    return packet;
}
/*****************************************************************************/
ByteArray Protocol::ClientCard(const std::string &card, std::uint32_t client_uuid)
{
    ByteArray packet;
    ByteStreamWriter out(packet);

    BuildHeader(packet, Protocol::CLIENT_CARD, client_uuid, TABLE_UID);
    out.Seek(HEADER_SIZE);
    out << card;
    UpdateHeader(packet);

    return packet;
}
/*****************************************************************************/
ByteArray Protocol::ClientBid(Contract c, bool slam, std::uint32_t client_uuid)
{
    ByteArray packet;
    ByteStreamWriter out(packet);

    BuildHeader(packet, Protocol::CLIENT_BID, client_uuid, TABLE_UID);
    out.Seek(HEADER_SIZE);
    out << c;  // contract to show
    out << slam;
    UpdateHeader(packet);

    return packet;
}
/*****************************************************************************/
ByteArray Protocol::TableFullMessage(std::uint32_t uuid)
{
    return BuildCommand(Protocol::TABLE_ERROR_FULL, TABLE_UID, uuid);
}
/*****************************************************************************/
ByteArray Protocol::TableAllPassed()
{
    return BuildCommand(Protocol::TABLE_ALL_PASSED, TABLE_UID, ALL_TABLE);
}
/*****************************************************************************/
ByteArray Protocol::TableAskForHandle(std::uint32_t uuid)
{
    return BuildCommand(Protocol::TABLE_ASK_FOR_HANDLE, TABLE_UID, uuid);
}
/*****************************************************************************/
ByteArray Protocol::TableAskForDiscard(std::uint32_t uuid)
{
    return BuildCommand(Protocol::TABLE_ASK_FOR_DISCARD, TABLE_UID, uuid);
}
/*****************************************************************************/
ByteArray Protocol::TableQuitEvent(std::uint32_t uuid, std::uint32_t tableId)
{
    ByteArray packet;
    ByteStreamWriter out(packet);

    BuildHeader(packet, Protocol::TABLE_QUIT_EVENT, TABLE_UID, uuid);
    out.Seek(HEADER_SIZE);
    out << tableId;
    UpdateHeader(packet);

    return packet;
}
/*****************************************************************************/
ByteArray Protocol::TableJoinReply(bool status, Place p, std::uint8_t nbPlayers, std::uint32_t uuid)
{
    ByteArray packet;
    ByteStreamWriter out(packet);

    BuildHeader(packet, Protocol::TABLE_JOIN_REPLY, TABLE_UID, uuid);
    out.Seek(HEADER_SIZE);
    out << status;
    out << p;                       // assigned place
    out << nbPlayers;               // number of players in the current game
    UpdateHeader(packet);

    return packet;
}
/*****************************************************************************/
ByteArray Protocol::TableChatMessage(const std::string &message)
{
    ByteArray packet;
    ByteStreamWriter out(packet);

    BuildHeader(packet, Protocol::TABLE_CHAT_MESSAGE, TABLE_UID, Protocol::ALL_TABLE);
    out.Seek(HEADER_SIZE);
    out << message;
    UpdateHeader(packet);

    return packet;
}
/*****************************************************************************/
ByteArray Protocol::TableShowBid(Contract c, bool slam, Place p)
{
    ByteArray packet;
    ByteStreamWriter out(packet);

    BuildHeader(packet, Protocol::TABLE_SHOW_PLAYER_BID, TABLE_UID, Protocol::ALL_TABLE);
    out.Seek(HEADER_SIZE);
    out << p; // current player bid
    out << c; // contract to show
    out << slam; // if slam declared
    UpdateHeader(packet);

    return packet;
}
/*****************************************************************************/
ByteArray Protocol::TablePlayersList(const std::map<Place, Identity> &players)
{
    ByteArray packet;
    ByteStreamWriter out(packet);

    BuildHeader(packet, Protocol::TABLE_PLAYERS_LIST, TABLE_UID, Protocol::ALL_TABLE);
    out.Seek(HEADER_SIZE);
    out << (std::uint8_t)players.size();
    std::map<Place, Identity>::const_iterator iter;

    for (iter = players.begin(); iter != players.end(); ++iter)
    {
        out << iter->first;
        out << iter->second;
    }

    UpdateHeader(packet);

    return packet;
}
/*****************************************************************************/
ByteArray Protocol::TableShowCard(const Card &c, Place p)
{
    ByteArray packet;
    ByteStreamWriter out(packet);

    BuildHeader(packet, Protocol::TABLE_SHOW_CARD, TABLE_UID, Protocol::ALL_TABLE);
    out.Seek(HEADER_SIZE);
    out << p
        << c.GetName();
    UpdateHeader(packet);

    return packet;
}
/*****************************************************************************/
ByteArray Protocol::TableShowHandle(Deck &handle, Place p)
{
    ByteArray packet;
    ByteStreamWriter out(packet);

    BuildHeader(packet, Protocol::TABLE_SHOW_HANDLE, TABLE_UID, Protocol::ALL_TABLE);
    out.Seek(HEADER_SIZE);
    out << p;
    out << handle;
    UpdateHeader(packet);

    return packet;
}
/*****************************************************************************/
ByteArray Protocol::TableNewDeal(Player *player)
{
    ByteArray packet;
    ByteStreamWriter out(packet);

    if (player != NULL)
    {
        BuildHeader(packet, Protocol::TABLE_NEW_DEAL, TABLE_UID, player->GetUuid());
        out.Seek(HEADER_SIZE);
        out << *player;
        UpdateHeader(packet);
    }
    return packet;
}
/*****************************************************************************/
ByteArray Protocol::TableEndOfDeal(Score &score)
{
    ByteArray packet;
    ByteStreamWriter out(packet);

    BuildHeader(packet, Protocol::TABLE_END_OF_DEAL, TABLE_UID, Protocol::ALL_TABLE);
    out.Seek(HEADER_SIZE);
    out << score;
    UpdateHeader(packet);

    return packet;
}
/*****************************************************************************/
ByteArray Protocol::TableEndOfGame(Place winner)
{
    ByteArray packet;
    ByteStreamWriter out(packet);

    BuildHeader(packet, Protocol::TABLE_END_OF_GAME, TABLE_UID, Protocol::ALL_TABLE);
    out.Seek(HEADER_SIZE);
    out << winner;
    UpdateHeader(packet);

    return packet;
}
/*****************************************************************************/
ByteArray Protocol::TableEndOfTrick(Place winner)
{
    ByteArray packet;
    ByteStreamWriter out(packet);

    BuildHeader(packet, Protocol::TABLE_END_OF_TRICK, TABLE_UID, Protocol::ALL_TABLE);
    out.Seek(HEADER_SIZE);
    out << winner;
    UpdateHeader(packet);

    return packet;
}
/*****************************************************************************/
ByteArray Protocol::TableStartDeal(Place firstPlayer, const Tarot::Bid &bid, const Tarot::Shuffle &sh)
{
    ByteArray packet;
    ByteStreamWriter out(packet);

    BuildHeader(packet, Protocol::TABLE_START_DEAL, TABLE_UID, Protocol::ALL_TABLE);
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
ByteArray Protocol::TablePlayCard(Place p)
{
    ByteArray packet;
    ByteStreamWriter out(packet);

    BuildHeader(packet, Protocol::TABLE_PLAY_CARD, TABLE_UID, Protocol::ALL_TABLE);
    out.Seek(HEADER_SIZE);
    out << p; // this player has to play a card
    UpdateHeader(packet);

    return packet;
}
/*****************************************************************************/
ByteArray Protocol::TableBidRequest(Contract c, Place p)
{
    ByteArray packet;
    ByteStreamWriter out(packet);

    BuildHeader(packet, Protocol::TABLE_REQUEST_BID, TABLE_UID, Protocol::ALL_TABLE);
    out.Seek(HEADER_SIZE);
    out << c; // previous bid
    out << p; // player to declare something
    UpdateHeader(packet);

    return packet;
}
/*****************************************************************************/
ByteArray Protocol::TableShowDog(const Deck &dog)
{
    ByteArray packet;
    ByteStreamWriter out(packet);

    BuildHeader(packet, Protocol::TABLE_SHOW_DOG, TABLE_UID, Protocol::ALL_TABLE);
    out.Seek(HEADER_SIZE);
    out << dog;
    UpdateHeader(packet);

    return packet;
}
/*****************************************************************************/
ByteArray Protocol::TableNewGame(Tarot::GameMode gameMode, const Tarot::Shuffle &shuffle, std::uint8_t numberOfTurns)
{
    ByteArray packet;
    ByteStreamWriter out(packet);

    BuildHeader(packet, Protocol::TABLE_NEW_GAME, TABLE_UID, Protocol::ALL_TABLE);
    out.Seek(HEADER_SIZE);
    out << (std::uint8_t)gameMode;
    out << shuffle;
    out << numberOfTurns;
    UpdateHeader(packet);

    return packet;
}
/*****************************************************************************/
ByteArray Protocol::LobbyChatMessage(const std::string &message)
{
    ByteArray packet;
    ByteStreamWriter out(packet);

    BuildHeader(packet, Protocol::LOBBY_CHAT_MESSAGE, LOBBY_UID, Protocol::ALL_LOBBY);
    out.Seek(HEADER_SIZE);
    out << message;
    UpdateHeader(packet);

    return packet;
}
/*****************************************************************************/
ByteArray Protocol::LobbyDisconnect(std::uint32_t uuid)
{
    return BuildCommand(Protocol::LOBBY_DISCONNECT, LOBBY_UID, uuid);
}
/*****************************************************************************/
ByteArray Protocol::LobbyRemovePlayer(std::uint32_t player_uuid)
{
    ByteArray packet;
    ByteStreamWriter out(packet);

    BuildHeader(packet, Protocol::LOBBY_REMOVE_PLAYER, Protocol::LOBBY_UID, TABLE_UID);
    out.Seek(HEADER_SIZE);
    out << player_uuid;
    UpdateHeader(packet);

    return packet;
}
/*****************************************************************************/
ByteArray Protocol::LobbyAddPlayer(std::uint32_t player_uuid, const Identity &ident)
{
    ByteArray packet;
    ByteStreamWriter out(packet);

    BuildHeader(packet, Protocol::LOBBY_ADD_PLAYER, Protocol::LOBBY_UID, TABLE_UID);
    out.Seek(HEADER_SIZE);
    out << player_uuid;
    out << ident;
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
ByteArray Protocol::LobbyCreateTable(std::uint8_t nbPlayers)
{
    ByteArray packet;
    ByteStreamWriter out(packet);

    BuildHeader(packet, Protocol::LOBBY_CREATE_TABLE, Protocol::LOBBY_UID, TABLE_UID);
    out.Seek(HEADER_SIZE);
    out << nbPlayers; // number of players in the current game
    UpdateHeader(packet);

    return packet;
}

/*****************************************************************************/
ByteArray Protocol::LobbyPlayersList(const std::map<std::uint32_t, std::string> &players)
{
    ByteArray packet;
    ByteStreamWriter out(packet);

    BuildHeader(packet, Protocol::LOBBY_PLAYERS_LIST, LOBBY_UID, Protocol::ALL_LOBBY);
    out.Seek(HEADER_SIZE);
    out << (std::uint8_t)players.size();
    std::map<std::uint32_t, std::string>::const_iterator iter;

    for (iter = players.begin(); iter != players.end(); ++iter)
    {
        out << iter->first;
        out << iter->second;
    }

    UpdateHeader(packet);

    return packet;
}
/*****************************************************************************/
ByteArray Protocol::AdminNewGame(Tarot::GameMode gameMode, const Tarot::Shuffle &shuffle, std::uint8_t numberOfTurns, std::uint32_t uuid)
{
    ByteArray packet;
    ByteStreamWriter out(packet);

    BuildHeader(packet, Protocol::ADMIN_NEW_GAME, uuid, TABLE_UID);
    out.Seek(HEADER_SIZE);
    out << (std::uint8_t)gameMode;
    out << shuffle;
    out << numberOfTurns;
    UpdateHeader(packet);

    return packet;
}
/*****************************************************************************/
ByteArray Protocol::AdminGameFull(bool full, std::uint32_t uuid)
{
    ByteArray packet;
    ByteStreamWriter out(packet);

    BuildHeader(packet, Protocol::ADMIN_GAME_FULL, TABLE_UID, uuid);
    out.Seek(HEADER_SIZE);
    out << full;
    UpdateHeader(packet);

    return packet;
}


//=============================================================================
// End of file Protocol.cpp
//=============================================================================
