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


static const std::uint16_t HEADER_SIZE = 8U;

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
std::vector<Protocol::PacketInfo> Protocol::DecodePacket(const ByteArray &data)
{
    bool found = true;
    std::vector<PacketInfo> packets;
    std::uint16_t offset = 0U;

    if (data.Size() >= HEADER_SIZE)
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
ByteArray Protocol::BuildClientReady(std::uint32_t uuid)
{
    return BuildCommand(Protocol::CLIENT_READY, uuid);
}
/*****************************************************************************/
ByteArray Protocol::BuildClientSyncDog(std::uint32_t uuid)
{
    return BuildCommand(Protocol::CLIENT_SYNC_DOG, uuid);
}
/*****************************************************************************/
ByteArray Protocol::BuildClientSyncHandle(std::uint32_t uuid)
{
    return BuildCommand(Protocol::CLIENT_SYNC_HANDLE, uuid);
}
/*****************************************************************************/
ByteArray Protocol::BuildClientSyncTrick(std::uint32_t uuid)
{
    return BuildCommand(Protocol::CLIENT_SYNC_TRICK, uuid);
}
/*****************************************************************************/
ByteArray Protocol::BuildClientSyncCard(std::uint32_t uuid)
{
    return BuildCommand(Protocol::CLIENT_SYNC_CARD, uuid);
}
/*****************************************************************************/
ByteArray Protocol::BuildClientSyncStart(std::uint32_t uuid)
{
    return BuildCommand(Protocol::CLIENT_SYNC_START, uuid);
}
/*****************************************************************************/
ByteArray Protocol::BuildClientSyncBid(std::uint32_t uuid)
{
    return BuildCommand(Protocol::CLIENT_SYNC_BID, uuid);
}
/*****************************************************************************/
ByteArray Protocol::BuildClientError(std::uint32_t uuid)
{
    return BuildCommand(Protocol::CLIENT_ERROR, uuid);
}
/*****************************************************************************/
ByteArray Protocol::BuildErrorServerFull(std::uint32_t uuid)
{
    return BuildCommand(Protocol::SERVER_ERROR_FULL, uuid);
}
/*****************************************************************************/
ByteArray Protocol::BuildDiscardRequest(std::uint32_t uuid)
{
    return BuildCommand(Protocol::SERVER_BUILD_DISCARD, uuid);
}
/*****************************************************************************/
ByteArray Protocol::BuildDisconnect(std::uint32_t uuid)
{
    return BuildCommand(Protocol::ADMIN_DISCONNECT, uuid);
}
/*****************************************************************************/
ByteArray Protocol::BuildDealAgain()
{
    return BuildCommand(Protocol::SERVER_DEAL_AGAIN, Protocol::ALL_PLAYERS);
}
/*****************************************************************************/
ByteArray Protocol::BuildAddPlayer(std::uint32_t uuid)
{
    ByteArray packet;
    ByteStreamWriter out(packet);

    BuildHeader(packet, Protocol::ADMIN_ADD_PLAYER, Protocol::ADMIN_UID);
    out.Seek(HEADER_SIZE);
    out << uuid;    // new player's uuid
    UpdateHeader(packet);

    return packet;
}
/*****************************************************************************/
ByteArray Protocol::BuildRequestIdentity(Place p, std::uint8_t nbPlayers, Game::Mode mode, std::uint32_t uuid)
{
    ByteArray packet;
    ByteStreamWriter out(packet);

    BuildHeader(packet, Protocol::SERVER_REQUEST_IDENTITY, uuid);
    out.Seek(HEADER_SIZE);
    out << (std::uint8_t)p;         // assigned place
    out << uuid;                    // assigned unique id
    out << (std::uint8_t)nbPlayers; // number of players in the current game
    out << (std::uint8_t)mode;      // game mode
    UpdateHeader(packet);

    return packet;
}
/*****************************************************************************/
ByteArray Protocol::BuildReplyIdentity(const Identity &ident, std::uint32_t uuid)
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
ByteArray Protocol::BuildClientDiscard(const Deck &discard, std::uint32_t uuid)
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
ByteArray Protocol::BuildClientHandle(const Deck &handle, std::uint32_t uuid)
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
ByteArray Protocol::BuildClientCard(const std::string &card, std::uint32_t uuid)
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
ByteArray Protocol::BuildServerChatMessage(const std::string &message)
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
ByteArray Protocol::BuildClientChatMessage(const std::string &message, std::uint32_t uuid)
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
ByteArray Protocol::BuildShowBid(Contract c, bool slam, Place p)
{
    ByteArray packet;
    ByteStreamWriter out(packet);

    BuildHeader(packet, Protocol::SERVER_SHOW_PLAYER_BID, Protocol::ALL_PLAYERS);
    out.Seek(HEADER_SIZE);
    out << (std::uint8_t)p   // current player bid
        << (std::uint8_t)c;  // contract to show
    if (slam)
    {
        out << (std::uint8_t)1;
    }
    else
    {
        out << (std::uint8_t)0;
    }
    UpdateHeader(packet);

    return packet;
}
/*****************************************************************************/
ByteArray Protocol::BuildClientBid(Contract c, bool slam, std::uint32_t uuid)
{
    ByteArray packet;
    ByteStreamWriter out(packet);

    BuildHeader(packet, Protocol::CLIENT_BID, uuid);
    out.Seek(HEADER_SIZE);
    out << (std::uint8_t)c  // contract to show
        << slam;
    UpdateHeader(packet);

    return packet;
}
/*****************************************************************************/
ByteArray Protocol::BuildPlayersList(std::map<Place, Identity> players)
{
    ByteArray packet;
    ByteStreamWriter out(packet);

    BuildHeader(packet, Protocol::SERVER_PLAYERS_LIST, Protocol::ALL_PLAYERS);
    out.Seek(HEADER_SIZE);
    out << (std::uint8_t)players.size();
    std::map<Place, Identity>::iterator iter;

    for(iter = players.begin(); iter != players.end(); iter++)
    {
        out << (std::uint8_t)iter->first;
        out << iter->second;
    }

    UpdateHeader(packet);

    return packet;
}
/*****************************************************************************/
ByteArray Protocol::BuildShowCard(Card *c, Place p)
{
    ByteArray packet;
    ByteStreamWriter out(packet);

    BuildHeader(packet, Protocol::SERVER_SHOW_CARD, Protocol::ALL_PLAYERS);
    out.Seek(HEADER_SIZE);
    out << (std::uint8_t)p
        << c->GetName();
    UpdateHeader(packet);

    return packet;
}

/*****************************************************************************/
ByteArray Protocol::BuildShowHandle(Deck &handle, Place p)
{
    ByteArray packet;
    ByteStreamWriter out(packet);

    BuildHeader(packet, Protocol::SERVER_SHOW_HANDLE, Protocol::ALL_PLAYERS);
    out.Seek(HEADER_SIZE);
    out << (std::uint8_t)p;
    out << handle;
    UpdateHeader(packet);

    return packet;
}
/*****************************************************************************/
ByteArray Protocol::BuildSendCards(std::uint32_t uuid, Deck &cards)
{
    ByteArray packet;
    ByteStreamWriter out(packet);

    BuildHeader(packet, Protocol::SERVER_SEND_CARDS, uuid);
    out.Seek(HEADER_SIZE);
    out << cards;
    UpdateHeader(packet);

    return packet;
}
/*****************************************************************************/
ByteArray Protocol::BuildEndOfDeal(Score &score)
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
ByteArray Protocol::BuildEndOfTrick(Place winner)
{
    ByteArray packet;
    ByteStreamWriter out(packet);

    BuildHeader(packet, Protocol::SERVER_END_OF_TRICK, Protocol::ALL_PLAYERS);
    out.Seek(HEADER_SIZE);
    out << (std::uint8_t)winner;
    UpdateHeader(packet);

    return packet;
}
/*****************************************************************************/
ByteArray Protocol::BuildStartDeal(Place taker, Contract contract, const Game::Shuffle &sh)
{
    ByteArray packet;
    ByteStreamWriter out(packet);

    BuildHeader(packet, Protocol::SERVER_START_DEAL, Protocol::ALL_PLAYERS);
    out.Seek(HEADER_SIZE);
    out << (std::uint8_t)taker;
    out << (std::uint8_t)contract;
    out << sh;
    UpdateHeader(packet);

    return packet;
}
/*****************************************************************************/
ByteArray Protocol::BuildPlayCard(Place p)
{
    ByteArray packet;
    ByteStreamWriter out(packet);

    BuildHeader(packet, Protocol::SERVER_PLAY_CARD, Protocol::ALL_PLAYERS);
    out.Seek(HEADER_SIZE);
    out << (std::uint8_t)p; // this player has to play a card
    UpdateHeader(packet);

    return packet;
}
/*****************************************************************************/
ByteArray Protocol::BuildBidRequest(Contract c, Place p)
{
    ByteArray packet;
    ByteStreamWriter out(packet);

    BuildHeader(packet, Protocol::SERVER_REQUEST_BID, Protocol::ALL_PLAYERS);
    out.Seek(HEADER_SIZE);
    out << (std::uint8_t)c; // previous bid
    out << (std::uint8_t)p; // player to declare something
    UpdateHeader(packet);

    return packet;
}
/*****************************************************************************/
ByteArray Protocol::BuildShowDog(Deck &dog)
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
ByteArray Protocol::BuildNewGame(Game::Mode gameMode, std::uint8_t nbPlayers, const Game::Shuffle &shuffle)
{
    ByteArray packet;
    ByteStreamWriter out(packet);

    BuildHeader(packet, Protocol::ADMIN_NEW_SERVER_GAME, Protocol::ADMIN_UID);
    out.Seek(HEADER_SIZE);
    out << (std::uint8_t)gameMode;
    out << (std::uint8_t)nbPlayers; // number of players in the current game
    out << shuffle;
    UpdateHeader(packet);

    return packet;
}




//=============================================================================
// End of file Protocol.cpp
//=============================================================================
