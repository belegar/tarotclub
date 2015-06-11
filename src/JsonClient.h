/*=============================================================================
 * TarotClub - JsonClient.h
 *=============================================================================
 * Network client implementation that translates commands into JSON
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

#ifndef JSON_CLIENT_H
#define JSON_CLIENT_H


#include <string>
#include <map>

#include "Card.h"
#include "Deck.h"
#include "Player.h"
#include "Protocol.h"
#include "Common.h"
#include "Score.h"

/*****************************************************************************/
struct ClientHelper : public Player
{
public:
    ClientHelper &operator = (const Deck &d)
    {
        Player::Deck::operator =(d);
        return *this;
    }

    bool TestDiscard(const Deck &discard);
    Contract CalculateBid();
    void UpdateStatistics();
    Card Play();
    bool IsValid(const Card &c);
    Deck AutoDiscard();

    Deck::Statistics   mStats;   // statistics on player's cards
    Tarot::Game mGame;
    Tarot::Bid  mBid;
    Tarot::Distribution mShuffle;
    Deck mCurrentTrick;
    Deck mDog;
    std::uint8_t mNbPlayers;
    std::uint32_t mTableId;
    Place mPlace;
};


/*****************************************************************************/
struct JsonClient : public Protocol::IWorkItem
{

public:
    class IEvent
    {
    public:
        static const std::uint32_t ErrLobbyAccessRefused        = 4000U;
        static const std::uint32_t ErrTableAccessRefused        = 5000U;
        static const std::uint32_t ErrTableFull                 = 5001U;

        virtual void EmitEvent(const std::string &event) = 0;
        virtual void EmitError(std::uint32_t errorId) = 0;
    };

    JsonClient(IEvent &handler);


private:
    IEvent     &mEventHandler;

    // From Protocol::WorkItem
    bool DoAction(std::uint8_t cmd, std::uint32_t src_uuid, std::uint32_t dest_uuid, const ByteArray &data);
};



#endif // JSON_CLIENT_H

//=============================================================================
// End of file JsonClient.h
//=============================================================================
