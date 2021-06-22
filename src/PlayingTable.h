/*=============================================================================
 * TarotClub - PlayingTable.h
 *=============================================================================
 * Manage TarotClub protocol requests within a Tarot context
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
#ifndef PLAYING_TABLE_H
#define PLAYING_TABLE_H

#include <vector>
#include "Protocol.h"
#include "Engine.h"
#include "JsonValue.h"
#include "Observer.h"
#include "Network.h"

/*****************************************************************************/
class PlayingTable
{

public:
    PlayingTable();
    virtual ~PlayingTable () { /* nothing to do */ }

    void Initialize();
    bool ExecuteRequest(std::uint32_t src_uuid, std::uint32_t dest_uuid, const JsonValue &json, std::vector<Reply> &out);

    std::string GetName();
    void SetName(const std::string &name) { mName = name; }

    std::uint32_t GetId() { return mId; }
    void SetId(std::uint32_t id) { mId = id; }

    void CreateTable(std::uint8_t nbPlayers);
    void SetupGame(const Tarot::Game &game);
    void SetAdminMode(bool enable); // Automatic or table managed by the admin
    Place AddPlayer(std::uint32_t uuid, std::uint8_t &nbPlayers);
    bool RemovePlayer(std::uint32_t kicked_player);
    Score GetScore() { return mScore; }

private:
    struct Challenger
    {
        std::uint32_t uuid;
        bool ack;

        Challenger()
        {
            Clear();
        }

        void Clear()
        {
            uuid = Protocol::INVALID_UID;
            ack = false;
        }

        bool IsFree()
        {
            if (uuid == Protocol::INVALID_UID)
            {
                return true;
            }
            else
            {
                return false;
            }
        }

    };

    Engine mEngine;
    Challenger mPlayers[5]; ///< Players around the table, sorted by Place value
    bool mFull;
    std::uint32_t mAdmin;   ///< Admin player (first player connected)
    std::string mName;      ///< Name of this table
    std::uint32_t mId;      ///< Table ID
    Score   mScore;         ///< Score of this table
    Tarot::Game mGame;      ///< Game mode
    bool mAdminMode;

    void NewGame(std::vector<Reply> &out);
    void NewDeal(std::vector<Reply> &out);
    void StartDeal(std::vector<Reply> &out);
    void SendNextBidSequence(std::vector<Reply> &out);
    void GameSequence(std::vector<Reply> &out);
    void EndOfDeal(std::vector<Reply> &out);
    bool Sync(Engine::Sequence sequence, std::uint32_t uuid);
    Place GetPlayerPlace(std::uint32_t uuid);
    void ResetAck();
    bool AckFromAllPlayers();
    std::uint32_t GetPlayerUuid(Place p);
    void SendToAllPlayers(std::vector<Reply> &out, JsonObject &obj);
    void ShowKingCall(const Card &c, std::vector<Reply> &out);
};

#endif // PLAYING_TABLE_H

//=============================================================================
// End of file PlayingTable.h
//=============================================================================
