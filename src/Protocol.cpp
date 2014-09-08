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
const std::uint32_t Protocol::SYSTEM_UID    = 10U;


static const std::uint16_t HEADER_SIZE = 8U;

//std::thread Protocol::mThread;
//ThreadQueue<std::pair<bool, Protocol::WorkItem*> > Protocol::mQueue;
//bool Protocol::mInitialized = false;
/*****************************************************************************/
Protocol::Protocol()
    : mInitialized(false)
{


}
/*****************************************************************************/
Protocol::~Protocol()
{
    Stop();
}
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
ByteArray Protocol::BuildCommand(Command cmd, std::uint32_t uuid)
{
    ByteArray packet;
    ByteStreamWriter out(packet);

    // Build the header
    out << (std::uint16_t)(HEADER_SIZE) << (std::uint8_t)Protocol::VERSION << uuid << (std::uint8_t)cmd;

    return packet;
}
/*****************************************************************************/
Protocol &Protocol::GetInstance()
{
    static Protocol mProtocol;
    return mProtocol;
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
    mQueue.Push(std::pair<bool, Protocol::WorkItem*>(false, NULL));
    mThread.join();
}
/*****************************************************************************/
void Protocol::Execute(WorkItem *item)
{
    mQueue.Push(std::pair<bool, Protocol::WorkItem*>(true, item));
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
 * This is the main controller thread; It manages the network (or not) packets
 * sent by client, system or an admin. All the packets are serialized to ensure
 * that they are treated in a queue, one at a time.
 *
 */
void Protocol::Run()
{
   // (void) pthis;
    std::pair<bool, WorkItem *> request;
    bool continueThread = true;

    while (continueThread)
    {
        mQueue.WaitAndPop(request);

        if (request.first)
        {
            WorkItem *item = request.second;
            if (item != NULL)
            {
                ByteArray data = item->GetPacket();
                std::vector<Protocol::PacketInfo> packets = Protocol::DecodePacket(data);

                // Execute all packets
                for (std::uint16_t i = 0U; i < packets.size(); i++)
                {
                    Protocol::PacketInfo inf = packets[i];

                    ByteArray subArray = data.SubArray(inf.offset, inf.size);
                    if (!item->DoAction(subArray))
                    {
                        // Quit thread
                        return;
                    }
                }
            }
        }
        else
        {
            continueThread = false;
        }
    }
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
                std::uint8_t version = data.Get(offset + 2U);
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
ByteArray Protocol::ClientSyncNewGame(std::uint32_t uuid)
{
    return BuildCommand(Protocol::CLIENT_SYNC_NEW_GAME, uuid);
}
/*****************************************************************************/
ByteArray Protocol::ClientSyncDog(std::uint32_t uuid)
{
    return BuildCommand(Protocol::CLIENT_SYNC_SHOW_DOG, uuid);
}
/*****************************************************************************/
ByteArray Protocol::ClientSyncHandle(std::uint32_t uuid)
{
    return BuildCommand(Protocol::CLIENT_SYNC_HANDLE, uuid);
}
/*****************************************************************************/
ByteArray Protocol::ClientSyncTrick(std::uint32_t uuid)
{
    return BuildCommand(Protocol::CLIENT_SYNC_TRICK, uuid);
}
/*****************************************************************************/
ByteArray Protocol::ClientSyncEndOfDeal(std::uint32_t uuid)
{
    return BuildCommand(Protocol::CLIENT_SYNC_END_OF_DEAL, uuid);
}
/*****************************************************************************/
ByteArray Protocol::ClientSyncShowCard(std::uint32_t uuid)
{
    return BuildCommand(Protocol::CLIENT_SYNC_SHOW_CARD, uuid);
}
/*****************************************************************************/
ByteArray Protocol::ClientSyncStart(std::uint32_t uuid)
{
    return BuildCommand(Protocol::CLIENT_SYNC_START, uuid);
}
/*****************************************************************************/
ByteArray Protocol::ClientSyncBid(std::uint32_t uuid)
{
    return BuildCommand(Protocol::CLIENT_SYNC_SHOW_BID, uuid);
}
/*****************************************************************************/
ByteArray Protocol::ClientSyncAllPassed(std::uint32_t uuid)
{
    return BuildCommand(Protocol::CLIENT_SYNC_ALL_PASSED, uuid);
}
/*****************************************************************************/
ByteArray Protocol::ClientSyncCards(std::uint32_t uuid)
{
    return BuildCommand(Protocol::CLIENT_SYNC_NEW_DEAL, uuid);
}
/*****************************************************************************/
ByteArray Protocol::ClientError(std::uint32_t uuid)
{
    return BuildCommand(Protocol::CLIENT_ERROR, uuid);
}
/*****************************************************************************/
ByteArray Protocol::ServerFullMessage(std::uint32_t uuid)
{
    return BuildCommand(Protocol::SERVER_ERROR_FULL, uuid);
}
/*****************************************************************************/
ByteArray Protocol::ServerAllPassed()
{
    return BuildCommand(Protocol::SERVER_ALL_PASSED, ALL_PLAYERS);
}
/*****************************************************************************/
ByteArray Protocol::ServerDisconnect(std::uint32_t uuid)
{
    return BuildCommand(Protocol::SERVER_DISCONNECT, uuid);
}
/*****************************************************************************/
ByteArray Protocol::SystemQuitGame()
{
    return BuildCommand(Protocol::SYSTEM_QUIT_GAME, Protocol::SYSTEM_UID);
}
/*****************************************************************************/
ByteArray Protocol::ServerAskForHandle(std::uint32_t uuid)
{
    return BuildCommand(Protocol::SERVER_ASK_FOR_HANDLE, uuid);
}
/*****************************************************************************/
ByteArray Protocol::ServerAskForDiscard(std::uint32_t uuid)
{
    return BuildCommand(Protocol::SERVER_ASK_FOR_DISCARD, uuid);
}
/*****************************************************************************/
ByteArray Protocol::SystemAddPlayer(std::uint32_t new_player_uuid)
{
    ByteArray packet;
    ByteStreamWriter out(packet);

    BuildHeader(packet, Protocol::SYSTEM_ADD_PLAYER, Protocol::SYSTEM_UID);
    out.Seek(HEADER_SIZE);
    out << new_player_uuid;
    UpdateHeader(packet);

    return packet;
}
/*****************************************************************************/
ByteArray Protocol::ServerRequestIdentity(Place p, std::uint8_t nbPlayers, std::uint32_t uuid)
{
    ByteArray packet;
    ByteStreamWriter out(packet);

    BuildHeader(packet, Protocol::SERVER_REQUEST_IDENTITY, uuid);
    out.Seek(HEADER_SIZE);
    out << p;                       // assigned place
    out << uuid;                    // assigned unique id
    out << nbPlayers;               // number of players in the current game
    UpdateHeader(packet);

    return packet;
}
/*****************************************************************************/
ByteArray Protocol::ClientReplyIdentity(const Identity &ident, std::uint32_t uuid)
{
    ByteArray packet;
    ByteStreamWriter out(packet);

    BuildHeader(packet, Protocol::CLIENT_INFOS, uuid);
    out.Seek(HEADER_SIZE);
    out << ident;
    UpdateHeader(packet);

    return packet;
}
/*****************************************************************************/
ByteArray Protocol::ClientDiscard(const Deck &discard, std::uint32_t uuid)
{
    ByteArray packet;
    ByteStreamWriter out(packet);

    BuildHeader(packet, Protocol::CLIENT_DISCARD, uuid);
    out.Seek(HEADER_SIZE);
    out << discard;
    UpdateHeader(packet);

    return packet;
}
/*****************************************************************************/
ByteArray Protocol::ClientHandle(const Deck &handle, std::uint32_t uuid)
{
    ByteArray packet;
    ByteStreamWriter out(packet);

    BuildHeader(packet, Protocol::CLIENT_HANDLE, uuid);
    out.Seek(HEADER_SIZE);
    out << handle;
    UpdateHeader(packet);

    return packet;
}
/*****************************************************************************/
ByteArray Protocol::ClientCard(const std::string &card, std::uint32_t uuid)
{
    ByteArray packet;
    ByteStreamWriter out(packet);

    BuildHeader(packet, Protocol::CLIENT_CARD, uuid);
    out.Seek(HEADER_SIZE);
    out << card;
    UpdateHeader(packet);

    return packet;
}
/*****************************************************************************/
ByteArray Protocol::ServerChatMessage(const std::string &message)
{
    ByteArray packet;
    ByteStreamWriter out(packet);

    BuildHeader(packet, Protocol::SERVER_MESSAGE, Protocol::ALL_PLAYERS);
    out.Seek(HEADER_SIZE);
    out << message;
    UpdateHeader(packet);

    return packet;
}
/*****************************************************************************/
ByteArray Protocol::ClientChatMessage(const std::string &message, std::uint32_t uuid)
{
    ByteArray packet;
    ByteStreamWriter out(packet);

    BuildHeader(packet, Protocol::CLIENT_MESSAGE, uuid);
    out.Seek(HEADER_SIZE);
    out << message;
    UpdateHeader(packet);

    return packet;
}
/*****************************************************************************/
ByteArray Protocol::ServerShowBid(Contract c, bool slam, Place p)
{
    ByteArray packet;
    ByteStreamWriter out(packet);

    BuildHeader(packet, Protocol::SERVER_SHOW_PLAYER_BID, Protocol::ALL_PLAYERS);
    out.Seek(HEADER_SIZE);
    out << p; // current player bid
    out << c; // contract to show
    out << slam; // if slam declared
    UpdateHeader(packet);

    return packet;
}
/*****************************************************************************/
ByteArray Protocol::ClientBid(Contract c, bool slam, std::uint32_t uuid)
{
    ByteArray packet;
    ByteStreamWriter out(packet);

    BuildHeader(packet, Protocol::CLIENT_BID, uuid);
    out.Seek(HEADER_SIZE);
    out << c;  // contract to show
    out << slam;
    UpdateHeader(packet);

    return packet;
}
/*****************************************************************************/
ByteArray Protocol::ServerPlayersList(std::map<Place, Identity> players)
{
    ByteArray packet;
    ByteStreamWriter out(packet);

    BuildHeader(packet, Protocol::SERVER_PLAYERS_LIST, Protocol::ALL_PLAYERS);
    out.Seek(HEADER_SIZE);
    out << (std::uint8_t)players.size();
    std::map<Place, Identity>::iterator iter;

    for (iter = players.begin(); iter != players.end(); ++iter)
    {
        out << iter->first;
        out << iter->second;
    }

    UpdateHeader(packet);

    return packet;
}
/*****************************************************************************/
ByteArray Protocol::ServerShowCard(const Card &c, Place p)
{
    ByteArray packet;
    ByteStreamWriter out(packet);

    BuildHeader(packet, Protocol::SERVER_SHOW_CARD, Protocol::ALL_PLAYERS);
    out.Seek(HEADER_SIZE);
    out << p
        << c.GetName();
    UpdateHeader(packet);

    return packet;
}
/*****************************************************************************/
ByteArray Protocol::ServerShowHandle(Deck &handle, Place p)
{
    ByteArray packet;
    ByteStreamWriter out(packet);

    BuildHeader(packet, Protocol::SERVER_SHOW_HANDLE, Protocol::ALL_PLAYERS);
    out.Seek(HEADER_SIZE);
    out << p;
    out << handle;
    UpdateHeader(packet);

    return packet;
}
/*****************************************************************************/
ByteArray Protocol::ServerNewDeal(Player *player)
{
    ByteArray packet;
    ByteStreamWriter out(packet);

    if (player != NULL)
    {
        BuildHeader(packet, Protocol::SERVER_NEW_DEAL, player->GetUuid());
        out.Seek(HEADER_SIZE);
        out << *player;
        UpdateHeader(packet);
    }
    return packet;
}
/*****************************************************************************/
ByteArray Protocol::ServerEndOfDeal(Score &score)
{
    ByteArray packet;
    ByteStreamWriter out(packet);

    BuildHeader(packet, Protocol::SERVER_END_OF_DEAL, Protocol::ALL_PLAYERS);
    out.Seek(HEADER_SIZE);
    out << score;
    UpdateHeader(packet);

    return packet;
}
/*****************************************************************************/
ByteArray Protocol::ServerEndOfGame(Place winner)
{
    ByteArray packet;
    ByteStreamWriter out(packet);

    BuildHeader(packet, Protocol::SERVER_END_OF_GAME, Protocol::ALL_PLAYERS);
    out.Seek(HEADER_SIZE);
    out << winner;
    UpdateHeader(packet);

    return packet;
}
/*****************************************************************************/
ByteArray Protocol::ServerEndOfTrick(Place winner)
{
    ByteArray packet;
    ByteStreamWriter out(packet);

    BuildHeader(packet, Protocol::SERVER_END_OF_TRICK, Protocol::ALL_PLAYERS);
    out.Seek(HEADER_SIZE);
    out << winner;
    UpdateHeader(packet);

    return packet;
}
/*****************************************************************************/
ByteArray Protocol::ServerStartDeal(Place firstPlayer, const Tarot::Bid &bid, const Tarot::Shuffle &sh)
{
    ByteArray packet;
    ByteStreamWriter out(packet);

    BuildHeader(packet, Protocol::SERVER_START_DEAL, Protocol::ALL_PLAYERS);
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
ByteArray Protocol::ServerPlayCard(Place p)
{
    ByteArray packet;
    ByteStreamWriter out(packet);

    BuildHeader(packet, Protocol::SERVER_PLAY_CARD, Protocol::ALL_PLAYERS);
    out.Seek(HEADER_SIZE);
    out << p; // this player has to play a card
    UpdateHeader(packet);

    return packet;
}
/*****************************************************************************/
ByteArray Protocol::ServerBidRequest(Contract c, Place p)
{
    ByteArray packet;
    ByteStreamWriter out(packet);

    BuildHeader(packet, Protocol::SERVER_REQUEST_BID, Protocol::ALL_PLAYERS);
    out.Seek(HEADER_SIZE);
    out << c; // previous bid
    out << p; // player to declare something
    UpdateHeader(packet);

    return packet;
}
/*****************************************************************************/
ByteArray Protocol::ServerShowDog(const Deck &dog)
{
    ByteArray packet;
    ByteStreamWriter out(packet);

    BuildHeader(packet, Protocol::SERVER_SHOW_DOG, Protocol::ALL_PLAYERS);
    out.Seek(HEADER_SIZE);
    out << dog;
    UpdateHeader(packet);

    return packet;
}
/*****************************************************************************/
ByteArray Protocol::SystemCreateTable(std::uint8_t nbPlayers)
{
    ByteArray packet;
    ByteStreamWriter out(packet);

    BuildHeader(packet, Protocol::SYSTEM_CREATE_TABLE, Protocol::SYSTEM_UID);
    out.Seek(HEADER_SIZE);
    out << nbPlayers; // number of players in the current game
    UpdateHeader(packet);

    return packet;
}
/*****************************************************************************/
ByteArray Protocol::ServerNewGame(Tarot::GameMode gameMode, const Tarot::Shuffle &shuffle)
{
    ByteArray packet;
    ByteStreamWriter out(packet);

    BuildHeader(packet, Protocol::SERVER_NEW_GAME, Protocol::ALL_PLAYERS);
    out.Seek(HEADER_SIZE);
    out << (std::uint8_t)gameMode;
    out << shuffle;
    UpdateHeader(packet);

    return packet;
}
/*****************************************************************************/
ByteArray Protocol::AdminNewGame(Tarot::GameMode gameMode, const Tarot::Shuffle &shuffle, std::uint32_t uuid)
{
    ByteArray packet;
    ByteStreamWriter out(packet);

    BuildHeader(packet, Protocol::ADMIN_NEW_GAME, uuid);
    out.Seek(HEADER_SIZE);
    out << (std::uint8_t)gameMode;
    out << shuffle;
    UpdateHeader(packet);

    return packet;
}
/*****************************************************************************/
ByteArray Protocol::AdminGameFull(bool full, std::uint32_t uuid)
{
    ByteArray packet;
    ByteStreamWriter out(packet);

    BuildHeader(packet, Protocol::ADMIN_GAME_FULL, uuid);
    out.Seek(HEADER_SIZE);
    out << full;
    UpdateHeader(packet);

    return packet;
}


//=============================================================================
// End of file Protocol.cpp
//=============================================================================
