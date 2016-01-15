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
#include <memory>
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
    static const std::uint32_t  INVALID_UID;
    static const std::uint32_t  LOBBY_UID;      //!< The lobby itself

    static const std::uint32_t  USERS_UID;      //!< Start of users UUID
    static const std::uint32_t  MAXIMUM_USERS;  //!< Maximum number of users
    static const std::uint32_t  TABLES_UID;     //!< Start of tables UUID
    static const std::uint32_t  MAXIMUM_TABLES; //!< Maximum number of tables
    static const std::uint32_t  NO_TABLE;       //!< Identifier for "no table"

    struct PacketInfo
    {
        std::uint16_t offset;
        std::uint16_t size;
    };

    enum Command
    {
        // client -> table
        CLIENT_BID              = 0x12,
        CLIENT_DISCARD          = 0x13, //!< Client send the discard
        CLIENT_CARD             = 0x14,
        CLIENT_HANDLE           = 0x15, //!< Client handle declaration reply
        CLIENT_ERROR            = 0x16,
        CLIENT_SYNC_JOIN_TABLE  = 0x17,
        CLIENT_SYNC_NEW_GAME    = 0x18, //!< Used to synchronize all clients for a new game
        CLIENT_SYNC_SHOW_DOG    = 0x19, //!< Used to synchronize all clients that are looking at the dog cards
        CLIENT_SYNC_HANDLE      = 0x1A, //!< Used to synchronize all clients that are looking at the handle cards
        CLIENT_SYNC_TRICK       = 0x1B, //!< Used to synchronize all clients that are looking at the trick cards
        CLIENT_SYNC_SHOW_CARD   = 0x1C, //!< Used to synchronize all clients to ensure that they have received the card played
        CLIENT_SYNC_START       = 0x1D, //!< Used to synchronize all clients to start the deal
        CLIENT_SYNC_SHOW_BID    = 0x1E, //!< Used to synchronize all clients that are looking the bid declared
        CLIENT_SYNC_NEW_DEAL    = 0x1F, //!< Used to synchronize all clients that have received their cards in hand
        CLIENT_SYNC_ALL_PASSED  = 0x21, //!< All the players have passed, new deal will be played
        CLIENT_SYNC_END_OF_DEAL = 0x22, //!< The score has been displayed, all players must ack to go to the next deal or terminate

        // client -> lobby
        CLIENT_CHAT_MESSAGE     = 0x30, //!< Chat message sent to the lobby or the table
        CLIENT_JOIN_TABLE       = 0x31, //!< A user wants to join the table
        CLIENT_QUIT_TABLE       = 0x32, //!< A user wants to quit the table
        CLIENT_REPLY_LOGIN      = 0x33, //!< Client sends its identity to the server
        CLIENT_CHANGE_IDENTITY  = 0x34, //!< New client identity

        // lobby -> client
        LOBBY_CHAT_MESSAGE      = 0x40, //!< chat message broadcasted to all clients (table or lobby main chat)
        LOBBY_REQUEST_LOGIN     = 0x41, //!< Server assigns a place to a client and he must reply back the identity
        LOBBY_LOGIN_RESULT      = 0x42,
        LOBBY_PLAYERS_LIST      = 0x43,
        // FIXME: Not implemented in the server side
        LOBBY_DISCONNECT        = 0x44, //!< Ask a client to quit the game
        LOBBY_JOIN_TABLE_REPLY  = 0x45, //!< Player joins the table (accepted or rejected)

        // table -> client (player)

        TABLE_QUIT_EVENT        = 0x70, //!< Player quit the table
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

        // admin -> server
        ADMIN_NEW_GAME          = 0xB0, //!< Start a new game with a specified mode

        // server -> admin
        ADMIN_GAME_FULL         = 0xC0  //!< Game is full, the admin can start a game
    };

    class IWorkItem
    {
    public:
        struct Data
        {
            std::shared_ptr<IWorkItem> item;
            ByteArray data;
            bool exit;

            Data()
                : item(nullptr)
                , exit(false)
            {

            }

            Data(bool e)
                : item(nullptr)
                , exit(e)
            {

            }

            Data(const std::shared_ptr<IWorkItem> &w)
                : item(w)
                , exit(false)
            {

            }

            Data(std::shared_ptr<IWorkItem> &w, const ByteArray &d)
                : item(w)
                , data(d)
                , exit(false)
            {

            }
        };

        virtual ~IWorkItem() {}
        virtual bool DoAction(std::uint8_t cmd, std::uint32_t src_uuid, std::uint32_t dest_uuid, const ByteArray &data) = 0;
    };

    Protocol();
    ~Protocol();

    static Protocol &GetInstance();
    static std::uint32_t GetSourceUuid(const ByteArray &packet);
    static std::uint32_t GetDestUuid(const ByteArray &packet);
    static std::uint8_t GetCommand(const ByteArray &packet);
    static bool DataManagement(IWorkItem *item, const ByteArray &data);

    void Initialize();
    void Stop();
    void Execute(const IWorkItem::Data &item);
    std::uint32_t QueueSize();

    /**
     * @brief DecodePacket
     *
     * The number of valid packets is the vector size.
     *
     * @param packet
     * @return A vector of offset of valid packets found inside the byte array
     */
    static std::vector<PacketInfo> DecodePacket(const ByteArray &data);

    // Client -> Lobby
    static ByteArray ClientReplyLogin(std::uint32_t client_uuid, const Identity &ident);
    static ByteArray ClientLobbyMessage(const std::string &message, std::uint32_t client_uuid, std::uint32_t target);
    static ByteArray ClientChangeIdentity(std::uint32_t client_uuid, const Identity &ident);

    // Client -> Controller
    static ByteArray ClientSyncNewGame(std::uint32_t client_uuid, std::uint32_t tableId);
    static ByteArray ClientError(std::uint32_t client_uuid);
    static ByteArray ClientBid(Contract c, bool slam, std::uint32_t client_uuid, std::uint32_t tableId);
    static ByteArray ClientSyncJoinTable(std::uint32_t client_uuid, std::uint32_t tableId);
    static ByteArray ClientSyncAllPassed(std::uint32_t client_uuid, std::uint32_t tableId);
    static ByteArray ClientSyncDog(std::uint32_t client_uuid, std::uint32_t tableId);
    static ByteArray ClientSyncHandle(std::uint32_t client_uuid, std::uint32_t tableId);
    static ByteArray ClientSyncTrick(std::uint32_t client_uuid, std::uint32_t tableId);
    static ByteArray ClientSyncShowCard(std::uint32_t client_uuid, std::uint32_t tableId);
    static ByteArray ClientSyncNewDeal(std::uint32_t client_uuid, std::uint32_t tableId);
    static ByteArray ClientSyncStart(std::uint32_t client_uuid, std::uint32_t tableId);
    static ByteArray ClientSyncBid(std::uint32_t client_uuid, std::uint32_t tableId);
    static ByteArray ClientSyncEndOfDeal(std::uint32_t client_uuid, std::uint32_t tableId);
    static ByteArray ClientDiscard(const Deck &discard, std::uint32_t client_uuid, std::uint32_t tableId);
    static ByteArray ClientHandle(const Deck &handle, std::uint32_t client_uuid, std::uint32_t tableId);
    static ByteArray ClientCard(const std::string &card, std::uint32_t client_uuid, std::uint32_t tableId);
    static ByteArray ClientJoinTable(std::uint32_t client_uuid, std::uint32_t tableId);
    static ByteArray ClientQuitTable(std::uint32_t client_uuid, std::uint32_t tableId);

    // table -> client (player): manage the Tarot game protocol and all the table related stuff
    static ByteArray TableAllPassed(std::uint32_t tableId);
    static ByteArray TableFullMessage(std::uint32_t uuid, std::uint32_t tableId);
    static ByteArray TableAskForDiscard(std::uint32_t uuid, std::uint32_t tableId);
    static ByteArray TableAskForHandle(std::uint32_t uuid, std::uint32_t tableId);
    static ByteArray TableNewGame(const Tarot::Game &game, std::uint32_t tableId);
    static ByteArray TableShowBid(Contract c, bool slam, Place p, std::uint32_t tableId);
    static ByteArray TablePlayersList(const std::map<Place, std::uint32_t> &players, std::uint32_t tableId);
    static ByteArray TableShowCard(const Card &c, Place p, std::uint32_t tableId);
    static ByteArray TableShowHandle(Deck &handle, Place p, std::uint32_t tableId);
    static ByteArray TableNewDeal(Player *player, std::uint32_t tableId);
    static ByteArray TableStartDeal(Place firstPlayer, const Tarot::Bid &bid, const Tarot::Distribution &sh, std::uint32_t tableId);
    static ByteArray TablePlayCard(Place p, std::uint32_t tableId);
    static ByteArray TableBidRequest(Contract c, Place p, std::uint32_t tableId);
    static ByteArray TableShowDog(const Deck &dog, std::uint32_t tableId);
    static ByteArray TableEndOfTrick(Place winner, std::uint32_t tableId);
    static ByteArray TableEndOfDeal(const Points &score, const std::string &result, std::uint32_t tableId);
    static ByteArray TableEndOfGame(Place winner, std::uint32_t tableId);
    static ByteArray TableQuitEvent(std::uint32_t uuid, std::uint32_t tableId);


    // Admin -> controller
    static ByteArray AdminNewGame(const Tarot::Game &game, std::uint32_t uuid, std::uint32_t tableId);

    // Controller -> Admin
    static ByteArray AdminGameFull(bool full, std::uint32_t uuid, std::uint32_t tableId);

    // Lobby -> client(s)
    static ByteArray LobbyJoinTableReply(bool status, Place p, std::uint8_t nbPlayers, std::uint32_t uuid, std::uint32_t tableId);
    static ByteArray LobbyRequestLogin(std::uint32_t uuid);
    static ByteArray LobbyLoginResult(bool accepted, const std::map<std::string, std::uint32_t> &tableList, std::uint32_t uuid);
    static ByteArray LobbyDisconnect(std::uint32_t uuid);
    static ByteArray LobbyChatMessage(const std::string &message, std::uint32_t target);
    static ByteArray LobbyPlayersList(const std::map<uint32_t, Identity> &players);

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
    ThreadQueue<IWorkItem::Data> mQueue; //!< Queue of network packets received
    bool mInitialized;
    std::mutex mMutex;
};

#endif // PROTOCOL_H

//=============================================================================
// End of file Protocol.h
//=============================================================================
