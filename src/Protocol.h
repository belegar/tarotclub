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
public:
    enum Command
    {
        // client -> server
        CLIENT_MESSAGE      = 0x10, //!< Chat message
        CLIENT_INFOS        = 0x11, //!< Client sends its identity to the server
        CLIENT_BID          = 0x12,
        CLIENT_DOG          = 0x13, //!< Client send the discard
        CLIENT_CARD         = 0x14,
        CLIENT_HANDLE       = 0x15, //!< Client has declared a handle
        CLIENT_READY        = 0x16,
        CLIENT_ERROR        = 0x17,
        CLIENT_SYNC_DOG     = 0x18, //!< Used to synchronize all clients that are looking at the dog cards
        CLIENT_SYNC_HANDLE  = 0x19, //!< Used to synchronize all clients that are looking at the trick cards
        CLIENT_SYNC_TRICK   = 0x20, //!< Used to synchronize all clients that are looking at the trick cards

        // server -> client
        SERVER_MESSAGE          = 0x70, //!< chat message broadcasted to all clients
        SERVER_IDENTITY         = 0x71, //!< Server assigns a place to a client and he must reply back the identity
        SERVER_PLAYERS_LIST     = 0x72,
        SERVER_SEND_CARDS       = 0x73,
        SERVER_SELECT_PLAYER    = 0x74,
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
        SERVER_END_OF_GAME      = 0x86  //!< end of the game mode (tournament ...)
    };

    void DecodePacket(QDataStream &in, qint64 bytes);

    QByteArray BuildCommand(QDataStream &in, Command cmd);

    /**
     * @brief Child classes must implement the data management
     *
     * Child classes are Server or Client
     *
     * @param in
     */
    virtual void DoAction(QDataStream &in) = 0;

};

#endif // PROTOCOL_H

//=============================================================================
// End of file Protocol.h
//=============================================================================
