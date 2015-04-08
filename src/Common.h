/*=============================================================================
 * TarotClub - Common.h
 *=============================================================================
 * Common Tarot related classes
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

#ifndef COMMON_H
#define COMMON_H

#include <string>
#include <cstdint>
#include <vector>
#include "ByteStreamReader.h"
#include "ByteStreamWriter.h"
#include "Util.h"


/*****************************************************************************/
enum Team           { ATTACK = 0, DEFENSE = 1, NO_TEAM = 0xFF };

/*****************************************************************************/
class Place
{
public:
    static const std::string STR_SOUTH;
    static const std::string STR_EAST;
    static const std::string STR_NORTH;
    static const std::string STR_WEST;
    static const std::string STR_FIFTH;
    static const std::string STR_NOWHERE;

    static const std::uint8_t SOUTH;
    static const std::uint8_t EAST;
    static const std::uint8_t NORTH;
    static const std::uint8_t WEST;
    static const std::uint8_t FIFTH;
    static const std::uint8_t NOWHERE;

    // Constructors
    Place();
    Place(std::uint32_t p);
    Place(std::uint8_t p);
    Place(std::string p);

    // Helpers
    std::string ToString() const;
    std::uint8_t Value();

    /**
     * @brief Next
     *
     * Calculate the next player on the right of the current one
     *
     * @param max
     * @return
     */
    Place Next(std::uint8_t numberOfPlayers);
    Place Previous(std::uint8_t numberOfPlayers);

    Place &operator = (Place const &rhs)
    {
        mPlace = rhs.mPlace;
        return *this;
    }

    inline bool operator == (const Place &rhs) const
    {
        return (this->mPlace == rhs.mPlace);
    }
    inline bool operator != (const Place &rhs) const
    {
        return (this->mPlace != rhs.mPlace);
    }
    inline bool operator < (const Place &rhs) const
    {
        return (this->mPlace < rhs.mPlace);
    }
    inline bool operator > (const Place &rhs) const
    {
        return (this->mPlace > rhs.mPlace);
    }

    friend ByteStreamWriter &operator<<(ByteStreamWriter &out, const Place &p)
    {
        out << p.mPlace;
        return out;
    }

    friend ByteStreamReader &operator>>(ByteStreamReader &s, Place &p)
    {
        s >> p.mPlace;
        return s;
    }

private:
    std::uint8_t mPlace;
    static std::vector<std::string> mStrings;

    static std::vector<std::string> Initialize();
};
/*****************************************************************************/
class Contract
{
public:
    static const std::string STR_PASS;
    static const std::string STR_TAKE;
    static const std::string STR_GUARD;
    static const std::string STR_GUARD_WITHOUT;
    static const std::string STR_GUARD_AGAINST;

    static const std::uint8_t PASS;
    static const std::uint8_t TAKE;
    static const std::uint8_t GUARD;
    static const std::uint8_t GUARD_WITHOUT;
    static const std::uint8_t GUARD_AGAINST;

    // Constructors
    Contract();
    Contract(int c);
    Contract(std::uint8_t c);
    Contract(std::uint32_t c);
    Contract(std::string c);

    std::string ToString() const;
    std::uint8_t Value();

    Contract &operator = (Contract const &rhs)
    {
        mContract = rhs.mContract;
        return *this;
    }

    inline bool operator == (const Contract &rhs) const
    {
        return (this->mContract == rhs.mContract);
    }
    inline bool operator != (const Contract &rhs) const
    {
        return (this->mContract != rhs.mContract);
    }
    inline bool operator < (const Contract &rhs) const
    {
        return (this->mContract < rhs.mContract);
    }
    inline bool operator <= (const Contract &rhs) const
    {
        return (this->mContract <= rhs.mContract);
    }
    inline bool operator > (const Contract &rhs) const
    {
        return (this->mContract > rhs.mContract);
    }
    inline bool operator >= (const Contract &rhs) const
    {
        return (this->mContract >= rhs.mContract);
    }

    friend ByteStreamWriter &operator<<(ByteStreamWriter &out, const Contract &c)
    {
        out << c.mContract;
        return out;
    }

    friend ByteStreamReader &operator>>(ByteStreamReader &s, Contract &c)
    {
        s >> c.mContract;
        return s;
    }

private:
    std::uint8_t mContract;
    static std::vector<std::string> mStrings;

    static std::vector<std::string> Initialize();
};
/*****************************************************************************/
/**
 * @brief The Tarot class
 * Various utilities and structures with regard of the Tarot rules
 */
class Tarot
{
public:
    struct Bid
    {
        Place       taker;     // who has annouced the contract
        Contract    contract;  // contract announced
        bool        slam;      // true if the taker has announced a slam (chelem)

        Bid()
        {
            Initialize();
        }

        Bid(const Bid &other)
            : taker(other.taker)
            , contract(other.contract)
            , slam(other.slam)
        {

        }

        Bid & operator= (const Bid & other)
        {
            if (this != &other) // protect against invalid self-assignment
            {
                this->contract = other.contract;
                this->taker = other.taker;
                this->slam = other.slam;
            }
            // by convention, always return *this
            return *this;
        }

        void Initialize()
        {
            contract = Contract::PASS;
            slam = false;
            taker = Place::NOWHERE;
        }
    };

    typedef std::uint8_t Handle;
    static const std::uint8_t NO_HANDLE     = 0U;
    static const std::uint8_t SIMPLE_HANDLE = 1U;
    static const std::uint8_t DOUBLE_HANDLE = 2U;
    static const std::uint8_t TRIPLE_HANDLE = 3U;

    struct Shuffle
    {
        static const std::uint8_t RANDOM_DEAL   = 0U;
        static const std::uint8_t CUSTOM_DEAL   = 1U;
        static const std::uint8_t NUMBERED_DEAL = 2U;

        std::uint8_t    type;
        std::string     file;
        std::uint32_t   seed;

        Shuffle()
        {
            Initialize();
        }

        void Initialize()
        {
            type = RANDOM_DEAL;
            file.clear();
            seed = 0U;
        }

        friend ByteStreamWriter &operator<<(ByteStreamWriter &out, const Shuffle &sh)
        {
            out << sh.type
                << sh.file
                << sh.seed;
            return out;
        }

        friend ByteStreamReader &operator>>(ByteStreamReader &in, Shuffle &sh)
        {
            in >> sh.type;
            in >> sh.file;
            in >> sh.seed;
            return in;
        }
    };

    enum GameMode
    {
        ONE_DEAL    = 0xAA, //!< The game will stop after one full deal played
        TOURNAMENT  = 0xBB, //!< The game will stop after a number of consecutive random deals (server configuration)
    };

    static std::uint8_t NumberOfDogCards(std::uint8_t numberOfPlayers);
    static std::uint8_t NumberOfCardsInHand(std::uint8_t numberOfPlayers);
    static bool IsDealFinished(std::uint8_t trickCounter, std::uint8_t numberOfPlayers);
    static int GetHandlePoints(Tarot::Handle handle);
    static Tarot::Handle GetHandleType(std::uint32_t size);
    static std::int32_t PointsToDo(std::uint8_t numberOfOudlers);
    static std::uint32_t GetMultiplier(Contract c);
};

#endif // COMMON_H

//=============================================================================
// End of file Common.h
//=============================================================================
