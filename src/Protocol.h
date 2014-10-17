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
#include <thread>
#include <map>
#include "Player.h"
#include "Identity.h"
#include "ByteArray.h"
#include "ThreadQueue.h"
#include "Score.h"

class Protocol
{

public:

    static const std::uint8_t   VERSION;        //!< Protocol version

    // Reserved UUIDs: [0..9]
    static const std::uint32_t  ALL_PLAYERS;   //!< Broadcast UUID (send the packet to all players)
    static const std::uint32_t  SERVER_UID;    //!< Server
    static const std::uint32_t  STAGING_UID;   //!< A staging client
    static const std::uint32_t  SYSTEM_UID;    //!< System user

    static const std::uint32_t  USERS_UID;    //!< Start of users UUID

    struct PacketInfo
    {
        std::uint16_t offset;
        std::uint16_t size;
    };

    enum Command
    {
        // client (player) -> server
        CLIENT_MESSAGE          = 0x10, //!< Chat message
        CLIENT_REPLY_LOGIN      = 0x11, //!< Client sends its identity to the server
        CLIENT_BID              = 0x12,
        CLIENT_DISCARD          = 0x13, //!< Client send the discard
        CLIENT_CARD             = 0x14,
        CLIENT_HANDLE           = 0x15, //!< Client handle declaration reply
        CLIENT_ERROR            = 0x16,
        CLIENT_SYNC_NEW_GAME    = 0x17, //!< Used to synchronize all clients for a new game
        CLIENT_SYNC_SHOW_DOG    = 0x18, //!< Used to synchronize all clients that are looking at the dog cards
        CLIENT_SYNC_HANDLE      = 0x19, //!< Used to synchronize all clients that are looking at the handle cards
        CLIENT_SYNC_TRICK       = 0x1A, //!< Used to synchronize all clients that are looking at the trick cards
        CLIENT_SYNC_SHOW_CARD   = 0x1B, //!< Used to synchronize all clients to ensure that they have received the card played
        CLIENT_SYNC_START       = 0x1C, //!< Used to synchronize all clients to start the deal
        CLIENT_SYNC_SHOW_BID    = 0x1D, //!< Used to synchronize all clients that are looking the bid declared
        CLIENT_SYNC_NEW_DEAL    = 0x1E, //!< Used to synchronize all clients that have received their cards in hand
        CLIENT_SYNC_ALL_PASSED  = 0x1F, //!< All the players have passed, new deal will be played
        CLIENT_SYNC_END_OF_DEAL = 0x20, //!< The score has been displayed, all players must ack to go to the next deal or terminate
        CLIENT_JOIN_TABLE       = 0x21, //!< A user wants to join the table

        // server -> client (player)
        SERVER_CHAT_MESSAGE     = 0x40, //!< chat message broadcasted to all clients
        SERVER_REQUEST_LOGIN    = 0x41, //!< Server assigns a place to a client and he must reply back the identity
        SERVER_LOGIN_RESULT     = 0x42,
        // FIXME: Not implemented in the server side
        SERVER_DISCONNECT       = 0x43, //!< Ask a client to quit the game

        // table -> client (player)
        TABLE_JOIN_REPLY        = 0x70, //!< Player join to the table
        TABLE_CHAT_MESSAGE      = 0x71,
        TABLE_PLAYERS_LIST      = 0x72, //!< Once the players list has been received, players must indicate if they are ready
        TABLE_NEW_GAME          = 0x73, //!< New game mode, and shuffle type
        TABLE_NEW_DEAL          = 0x74, //!< new deal, cleanup everything. Cards are flushed and to be sent to players
        TABLE_REQUEST_BID       = 0x75, //!< ask for a bid
        TABLE_ALL_PASSED        = 0x76,
        TABLE_ASK_FOR_DISCARD   = 0x77,
        TABLE_START_DEAL        = 0x78, //!< Bid sequence is finished and discard is done, deal game can start
        TABLE_PLAY_CARD         = 0x79,
        TABLE_SHOW_CARD         = 0x7A, //!< card played broadcasted to all clients
        TABLE_SHOW_PLAYER_BID   = 0x7B, //!< one player has announced something, broacast it to all clients
        TABLE_SHOW_DOG          = 0x7C,
        TABLE_ASK_FOR_HANDLE    = 0x7D,
        TABLE_SHOW_HANDLE       = 0x7E, //!< show a handle to all players
        TABLE_END_OF_TRICK      = 0x7F,
        TABLE_END_OF_DEAL       = 0x80,
        TABLE_END_OF_GAME       = 0x81, //!< end of the game mode (tournament ...)
        TABLE_ERROR_FULL        = 0x82, //!< Server is full, cannot join game

        // system -> server
        SYSTEM_CREATE_TABLE     = 0xA0, //!< Ask the server to start a new table with N players
        SYSTEM_REMOVE_PLAYER    = 0xA2, //!< Remove player
        SYSTEM_QUIT_GAME        = 0xA3,

        // admin -> server
        ADMIN_NEW_GAME          = 0xB0, //!< Start a new game with a specified mode

        // server -> admin
        ADMIN_GAME_FULL         = 0xC0  //!< Game is full, the admin can start a game

    };

    class IWorkItem
    {
    public:
          virtual bool DoAction(std::uint8_t cmd, std::uint32_t src_uuid, std::uint32_t dest_uuid, const ByteArray &data) = 0;
          virtual ByteArray GetPacket() = 0;
    };

    Protocol();
    ~Protocol();

    static Protocol &GetInstance();
    static std::uint32_t GetSourceUuid(ByteArray &packet);
    static bool DataManagement(IWorkItem *item, const ByteArray &data);

    void Initialize();
    void Stop();
    void Execute(IWorkItem *item);

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
    static ByteArray ClientReplyLogin(const Identity &ident);
    static ByteArray ClientChatMessage(const std::string &message, std::uint32_t client_uuid);
    static ByteArray ClientSyncNewGame(std::uint32_t client_uuid);
    static ByteArray ClientError(std::uint32_t client_uuid);
    static ByteArray ClientBid(Contract c, bool slam, std::uint32_t client_uuid);
    static ByteArray ClientSyncAllPassed(std::uint32_t client_uuid);
    static ByteArray ClientSyncDog(std::uint32_t client_uuid);
    static ByteArray ClientSyncHandle(std::uint32_t client_uuid);
    static ByteArray ClientSyncTrick(std::uint32_t client_uuid);
    static ByteArray ClientSyncShowCard(std::uint32_t client_uuid);
    static ByteArray ClientSyncCards(std::uint32_t client_uuid);
    static ByteArray ClientSyncStart(std::uint32_t client_uuid);
    static ByteArray ClientSyncBid(std::uint32_t client_uuid);
    static ByteArray ClientSyncEndOfDeal(std::uint32_t client_uuid);
    static ByteArray ClientDiscard(const Deck &discard, std::uint32_t client_uuid);
    static ByteArray ClientHandle(const Deck &handle, std::uint32_t client_uuid);
    static ByteArray ClientCard(const std::string &card, std::uint32_t client_uuid);
    static ByteArray ClientJoinTable(std::uint32_t client_uuid);

    // table -> client (player): manage the Tarot game protocol and all the table related stuff
    static ByteArray TableJoinReply(Place p, std::uint8_t nbPlayers, std::uint32_t uuid);
    static ByteArray TableAllPassed();
    static ByteArray TableFullMessage(std::uint32_t uuid);
    static ByteArray TableAskForDiscard(std::uint32_t uuid);
    static ByteArray TableAskForHandle(std::uint32_t uuid);
    static ByteArray TableNewGame(Tarot::GameMode gameMode, const Tarot::Shuffle &shuffle);
    static ByteArray TableChatMessage(const std::string &message);
    static ByteArray TableShowBid(Contract c, bool slam, Place p);
    static ByteArray TablePlayersList(std::map<Place, Identity> players);
    static ByteArray TableShowCard(const Card &c, Place p);
    static ByteArray TableShowHandle(Deck &handle, Place p);
    static ByteArray TableNewDeal(Player *player);
    static ByteArray TableStartDeal(Place firstPlayer, const Tarot::Bid &bid, const Tarot::Shuffle &sh);
    static ByteArray TablePlayCard(Place p);
    static ByteArray TableBidRequest(Contract c, Place p);
    static ByteArray TableShowDog(const Deck &dog);
    static ByteArray TableEndOfTrick(Place winner);
    static ByteArray TableEndOfDeal(Score &score);
    static ByteArray TableEndOfGame(Place winner);

    // System to controller packets
    static ByteArray SystemRemovePlayer(std::uint32_t player_uuid);
    static ByteArray SystemCreateTable(std::uint8_t nbPlayers);
    static ByteArray SystemQuitGame();

    // Admin -> controller
    static ByteArray AdminNewGame(Tarot::GameMode gameMode, const Tarot::Shuffle &shuffle, std::uint32_t uuid);

    // Controller -> Admin
    static ByteArray AdminGameFull(bool full, std::uint32_t uuid);

    // Server -> client
    static ByteArray ServerLoginResult(bool accepted, std::uint32_t uuid);
    static ByteArray ServerRequestLogin();
    static ByteArray ServerDisconnect(std::uint32_t uuid);

private:

    /**
     * @brief Protocol executor shared thread
     */
    static void EntryPoint(void *pthis);
    void Run();

    /**
     * @brief BuildCommand
     * @param packet
     * @param cmd
     * @param uuid User Uniquer Identifier
     * @return
     */
    static void BuildHeader(ByteArray &packet, Command cmd, std::uint32_t src_uuid, std::uint32_t dest_uuid);
    static void UpdateHeader(ByteArray &packet);
    static ByteArray BuildCommand(Command cmd, std::uint32_t src_uuid, std::uint32_t dest_uuid);

    // Work thread that can execute work items
    std::thread mThread;
    ThreadQueue<std::pair<bool, IWorkItem*> > mQueue; //!< Queue of network packets received
    bool mInitialized;
};

#endif // PROTOCOL_H

//=============================================================================
// End of file Protocol.h
//=============================================================================
