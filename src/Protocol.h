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
#include <list>
#include <map>
#include "Player.h"
#include "Identity.h"
#include "ByteArray.h"
#include "Score.h"

class Protocol
{

public:

    static const std::uint8_t   VERSION;        //!< Protocol version
    static const std::uint32_t  SERVER_UID;     //!< Server unique identifier, 1, reserved
    static const std::uint32_t  ALL_PLAYERS;    //!< 0, send to all players
    static const std::uint32_t  SYSTEM_UID;     //!< System user

    struct PacketInfo
    {
        std::uint16_t offset;
        std::uint16_t size;
    };

    enum Command
    {
        // client (player) -> server
        CLIENT_MESSAGE          = 0x10, //!< Chat message
        CLIENT_INFOS            = 0x11, //!< Client sends its identity to the server
        CLIENT_BID              = 0x12,
        CLIENT_DISCARD          = 0x13, //!< Client send the discard
        CLIENT_CARD             = 0x14,
        CLIENT_HANDLE           = 0x15, //!< Client handle declaration reply
        CLIENT_ERROR            = 0x16,
        CLIENT_SYNC_READY       = 0x17, //!< Used to synchronize all clients for a new game
        CLIENT_SYNC_SHOW_DOG    = 0x18, //!< Used to synchronize all clients that are looking at the dog cards
        CLIENT_SYNC_HANDLE      = 0x19, //!< Used to synchronize all clients that are looking at the handle cards
        CLIENT_SYNC_TRICK       = 0x1A, //!< Used to synchronize all clients that are looking at the trick cards
        CLIENT_SYNC_SHOW_CARD   = 0x1B, //!< Used to synchronize all clients to ensure that they have received the card played
        CLIENT_SYNC_START       = 0x1C, //!< Used to synchronize all clients to start the deal
        CLIENT_SYNC_SHOW_BID    = 0x1D, //!< Used to synchronize all clients that are looking the bid declared
        CLIENT_SYNC_CARDS       = 0x1E, //!< Used to synchronize all clients that have received their cards in hand
        CLIENT_SYNC_ALL_PASSED  = 0x1F, //!< All the players have passed, new deal will be played
        CLIENT_SYNC_END_OF_DEAL = 0x20, //!< The score has been displayed, all players must ack to go to the next deal or terminate

        // server -> client (player)
        SERVER_MESSAGE          = 0x70, //!< chat message broadcasted to all clients
        SERVER_REQUEST_IDENTITY = 0x71, //!< Server assigns a place to a client and he must reply back the identity
        SERVER_PLAYERS_LIST     = 0x72, //!< Once the players list has been received, players must indicate if they are ready
        SERVER_NEW_GAME         = 0x73, //!< New game mode, and shuffle type
        SERVER_NEW_DEAL         = 0x74, //!< new deal, cleanup everything. Cards are flushed and to be sent to players
        SERVER_REQUEST_BID      = 0x75, //!< ask for a bid
        SERVER_ASK_FOR_DISCARD  = 0x76,
        SERVER_START_DEAL       = 0x77, //!< Bid sequence is finished and discard is done, deal game can start
        SERVER_PLAY_CARD        = 0x78,
        SERVER_SHOW_CARD        = 0x79, //!< card played broadcasted to all clients
        SERVER_SHOW_PLAYER_BID  = 0x7A, //!< one player has announced something, broacast it to all clients
        SERVER_SHOW_DOG         = 0x7B,
        SERVER_ASK_FOR_HANDLE   = 0x7C,
        SERVER_SHOW_HANDLE      = 0x7D, //!< show a handle to all players
        SERVER_END_OF_TRICK     = 0x7E,
        SERVER_END_OF_DEAL      = 0x7F,
        SERVER_END_OF_GAME      = 0x80, //!< end of the game mode (tournament ...)
        SERVER_ERROR_FULL       = 0x81, //!< Server is full, cannot join game

        // FIXME: Not implemented in the server side
        SERVER_DISCONNECT       = 0x82, //!< Ask a client to quit the game

        // system -> server
        SYSTEM_CREATE_TABLE     = 0xA0, //!< Ask the server to start a new table with N players
        SYSTEM_ADD_PLAYER       = 0xA1, //!< A new player is entering the game
        SYSTEM_QUIT_GAME        = 0xA2,

        // admin -> server
        ADMIN_NEW_GAME          = 0xB0, //!< Start a new game with a specified mode

        // server -> admin
        ADMIN_GAME_FULL         = 0xC0  //!< Game is full, the admin can start a game

    };

    /**
     * @brief DecodePacket
     *
     * The number of valid packets is the vector size.
     *
     * @param packet
     * @return A vector of offset of valid packets found inside the byte array
     */
    static std::vector<PacketInfo> DecodePacket(const ByteArray &data);

    // Client to server packets
    static ByteArray ClientReplyIdentity(const Identity &ident, std::uint32_t uuid);
    static ByteArray ClientChatMessage(const std::string &message, std::uint32_t uuid);
    static ByteArray ClientReady(std::uint32_t uuid);
    static ByteArray ClientError(std::uint32_t uuid);
    static ByteArray ClientBid(Contract c, bool slam, std::uint32_t uuid);
    static ByteArray ClientSyncDog(std::uint32_t uuid);
    static ByteArray ClientSyncHandle(std::uint32_t uuid);
    static ByteArray ClientSyncTrick(std::uint32_t uuid);
    static ByteArray ClientSyncShowCard(std::uint32_t uuid);
    static ByteArray ClientSyncCards(std::uint32_t uuid);
    static ByteArray ClientSyncStart(std::uint32_t uuid);
    static ByteArray ClientSyncBid(std::uint32_t uuid);
    static ByteArray ClientSyncEndOfDeal(std::uint32_t uuid);
    static ByteArray ClientDiscard(const Deck &discard, std::uint32_t uuid);
    static ByteArray ClientHandle(const Deck &handle, std::uint32_t uuid);
    static ByteArray ClientCard(const std::string &card, std::uint32_t uuid);

    // Server to client packets
    static ByteArray ServerFullMessage(std::uint32_t uuid);
    static ByteArray ServerAskForDiscard(std::uint32_t uuid);
    static ByteArray ServerDisconnect(std::uint32_t uuid);
    static ByteArray ServerAskForHandle(std::uint32_t uuid);
    static ByteArray ServerNewGame(Tarot::GameMode gameMode, const Tarot::Shuffle &shuffle);
    static ByteArray ServerRequestIdentity(Place p, std::uint8_t nbPlayers, std::uint32_t uuid);
    static ByteArray ServerChatMessage(const std::string &message);
    static ByteArray ServerShowBid(Contract c, bool slam, Place p);
    static ByteArray ServerPlayersList(std::map<Place, Identity> players);
    static ByteArray ServerShowCard(Card *c, Place p);
    static ByteArray ServerShowHandle(Deck &handle, Place p);
    static ByteArray ServerSendCards(Player *player);
    static ByteArray ServerStartDeal(const Tarot::Bid &bid, const Tarot::Shuffle &sh);
    static ByteArray ServerPlayCard(Place p);
    static ByteArray ServerBidRequest(Contract c, Place p);
    static ByteArray ServerShowDog(const Deck &dog);
    static ByteArray ServerEndOfTrick(Place winner);
    static ByteArray ServerEndOfDeal(Score &score);
    static ByteArray ServerEndOfGame(Place winner);

    // System to controller packets
    static ByteArray SystemAddPlayer(std::uint32_t new_player_uuid);
    static ByteArray SystemCreateTable(std::uint8_t nbPlayers);
    static ByteArray SystemQuitGame();

    // Admin to controller packets
    static ByteArray AdminNewGame(Tarot::GameMode gameMode, const Tarot::Shuffle &shuffle);
    static ByteArray AdminGameFull(bool full, std::uint32_t uuid);

private:

    /**
     * @brief BuildCommand
     * @param packet
     * @param cmd
     * @param uuid User Uniquer Identifier
     * @return
     */
    static void BuildHeader(ByteArray &packet, Command cmd, std::uint32_t uuid);
    static void UpdateHeader(ByteArray &packet);
    static ByteArray BuildCommand(Command cmd, std::uint32_t uuid);
};

#endif // PROTOCOL_H

//=============================================================================
// End of file Protocol.h
//=============================================================================
