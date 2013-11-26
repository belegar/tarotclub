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

#include <QObject>
#include <QByteArray>
#include <QDataStream>
#include "Player.h"

class Protocol : public QObject
{
    Q_OBJECT

public:
    enum Command
    {
        // client -> server
        CLIENT_MESSAGE      = 0x10, //!< Chat message
        CLIENT_INFOS        = 0x11, //!< Client sends its identity to the server
        CLIENT_BID          = 0x12,
        CLIENT_DISCARD      = 0x13, //!< Client send the discard
        CLIENT_CARD         = 0x14,
        CLIENT_HANDLE       = 0x15, //!< Client has declared a handle
        CLIENT_READY        = 0x16,
        CLIENT_ERROR        = 0x17,
        CLIENT_SYNC_DOG     = 0x18, //!< Used to synchronize all clients that are looking at the dog cards
        CLIENT_SYNC_HANDLE  = 0x19, //!< Used to synchronize all clients that are looking at the handle cards
        CLIENT_SYNC_TRICK   = 0x20, //!< Used to synchronize all clients that are looking at the trick cards
        CLIENT_SYNC_CARD    = 0x21, //!< Used to synchronize all clients to ensure that they have received the card played
        CLIENT_SYNC_START   = 0x22, //!< Used to synchronize all clients to start the deal
        CLIENT_SYNC_BID     = 0x23, //!< Used to synchronize all clients that are looking the bid declared

        // server -> client
        SERVER_MESSAGE          = 0x70, //!< chat message broadcasted to all clients
        SERVER_REQUEST_IDENTITY = 0x71, //!< Server assigns a place to a client and he must reply back the identity
        SERVER_PLAYERS_LIST     = 0x72,
        SERVER_SEND_CARDS       = 0x73,
        SERVER_REQUEST_BID      = 0x75, //!< ask for a bid
        SERVER_BUILD_DISCARD    = 0x76,
        SERVER_START_DEAL       = 0x77,
        SERVER_DEAL_AGAIN       = 0x78, //!< new deal, because of all players has passed for instance
        SERVER_PLAY_CARD        = 0x79,
        SERVER_SHOW_CARD        = 0x80, //!< card played broadcasted to all clients
        SERVER_SHOW_PLAYER_BID  = 0x81, //!< one player has announced something, broacast it to all clients
        SERVER_SHOW_DOG         = 0x82,
        SERVER_SHOW_HANDLE      = 0x83, //!< show a handle to all players
        SERVER_END_OF_TRICK     = 0x84,
        SERVER_END_OF_DEAL      = 0x85,
        SERVER_END_OF_GAME      = 0x86, //!< end of the game mode (tournament ...)
        SERVER_ERROR_FULL       = 0x87, //!< Server is full, cannot join game
        SERVER_DISCONNECT       = 0x88  //!< Ask clients to quit properly
    };


    bool DecodePacket(QDataStream &in);

    QByteArray BuildCommand(QByteArray &packet, Command cmd);
};

#endif // PROTOCOL_H

//=============================================================================
// End of file Protocol.h
//=============================================================================
