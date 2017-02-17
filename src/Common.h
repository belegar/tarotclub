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
#include "Util.h"

#include "Log.h"

/*****************************************************************************/
class Team
{
public:
    static const std::uint8_t ATTACK;
    static const std::uint8_t DEFENSE;
    static const std::uint8_t NO_TEAM;

    Team() : mTeam(NO_TEAM) {  }
    explicit Team(const std::uint8_t team) : mTeam(team) {  }

    std::uint8_t Value() { return mTeam; }

    Team &operator = (Team const &rhs)
    {
        mTeam = rhs.mTeam;
        return *this;
    }

    Team &operator = (const std::uint8_t &rhs)
    {
        mTeam = rhs;
        return *this;
    }

    inline bool operator == (const Team &rhs) const
    {
        return (this->mTeam == rhs.mTeam);
    }
    inline bool operator == (const std::uint8_t &rhs) const
    {
        return (this->mTeam == rhs);
    }

    inline bool operator != (const Team &rhs) const
    {
        return (this->mTeam != rhs.mTeam);
    }

private:
    std::uint8_t mTeam;
};

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
    explicit Place(std::uint32_t p);
    explicit Place(std::uint8_t p);
    explicit Place(std::string p);
    explicit Place(int p);

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
    bool IsValid();

    Place &operator = (Place const &rhs)
    {
        mPlace = rhs.mPlace;
        return *this;
    }

    Place &operator = (std::uint8_t const &rhs)
    {
        if (rhs > NOWHERE)
        {
            TLogError("Invalid Place value");
            mPlace = SOUTH;
        }
        else
        {
            mPlace = rhs;
        }
        return *this;
    }

    Place &operator = (std::uint32_t const &rhs)
    {
        *this = static_cast<std::uint8_t>(rhs);
        return *this;
    }

    Place &operator = (std::string const &rhs)
    {
        mPlace = NOWHERE;
        for (std::uint32_t i = 0U; i < mStrings.size(); i++)
        {
            if (rhs == mStrings[i])
            {
                mPlace = static_cast<std::uint8_t>(i);
            }
        }
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
    inline bool operator != (const std::uint8_t &rhs) const
    {
        return (this->mPlace != rhs);
    }

    inline bool operator < (const Place &rhs) const
    {
        return (this->mPlace < rhs.mPlace);
    }
    inline bool operator > (const Place &rhs) const
    {
        return (this->mPlace > rhs.mPlace);
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
    explicit Contract(std::uint8_t c);
    explicit Contract(std::uint32_t c);
    explicit Contract(std::string c);

    std::string ToString() const;
    std::uint8_t Value();

    Contract &operator = (Contract const &rhs)
    {
        mContract = rhs.mContract;
        return *this;
    }

    Contract &operator = (const std::uint8_t &rhs)
    {
        if (rhs > GUARD_AGAINST)
        {
            TLogError("Invalid contract value");
            mContract = PASS;
        }
        else
        {
            mContract = rhs;
        }
        return *this;
    }


    Contract &operator = (const std::uint32_t &rhs)
    {
        *this = static_cast<std::uint8_t>(rhs);
        return *this;
    }

    Contract &operator = (const std::string &rhs)
    {
        mContract = PASS;
        for (std::uint32_t i = 0U; i < mStrings.size(); i++)
        {
            if (rhs == mStrings[i])
            {
                mContract = static_cast<std::uint8_t>(i);
            }
        }
        return *this;
    }

    inline bool operator == (const Contract &rhs) const
    {
        return (this->mContract == rhs.mContract);
    }

    inline bool operator == (const std::uint32_t &rhs) const
    {
        return (this->mContract == static_cast<std::uint8_t>(rhs));
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

    struct Distribution
    {
        static const std::uint8_t RANDOM_DEAL   = 0U;
        static const std::uint8_t NUMBERED_DEAL = 1U;
        static const std::uint8_t CUSTOM_DEAL   = 2U;

        static const std::string cRandomTxt;
        static const std::string cNumberedTxt;
        static const std::string cCustomTxt;

        std::uint8_t    mType;
        std::string     mFile;
        std::uint32_t   mSeed;

        void TypeFromString(const std::string &type)
        {
            if (type == cRandomTxt)
            {
                mType = RANDOM_DEAL;
            }
            else if (type == cNumberedTxt)
            {
                mType = NUMBERED_DEAL;
            }
            else
            {
                mType = CUSTOM_DEAL;
            }
        }

        std::string TypeToString() const
        {
            if (mType == RANDOM_DEAL)
            {
                return cRandomTxt;
            }
            else if (mType == NUMBERED_DEAL)
            {
                return cNumberedTxt;
            }
            else
            {
                return cCustomTxt;
            }
        }

        Distribution()
        {
            Initialize();
        }

        Distribution(std::uint8_t type, const std::string &file = std::string(), std::uint32_t seed = 0U)
            : mType(type)
            , mFile(file)
            , mSeed(seed)
        {

        }

        void Initialize()
        {
            mType = RANDOM_DEAL;
            mFile.clear();
            mSeed = 0U;
        }

    };

    struct Game
    {
        static const std::uint8_t cQuickDeal        = 1U; ///< Play one deal
        static const std::uint8_t cSimpleTournament = 2U; ///< Play N consecutive random deals with final score
        static const std::uint8_t cCustom           = 3U;

        static const std::string cQuickDealTxt;
        static const std::string cSimpleTournamentTxt;
        static const std::string cCustomTxt;

        std::uint8_t mode;  ///< Current game mode
        std::vector<Tarot::Distribution> deals;

        void Set(const std::string &modeTxt)
        {
            if (modeTxt == cCustomTxt)
            {
                mode = cCustom;
            }
            else if (modeTxt == cSimpleTournamentTxt)
            {
                mode = cSimpleTournament;
            }
            else
            {
                mode = cQuickDeal;
            }
        }

        std::string Get() const
        {
            if (mode == cCustom)
            {
                return cCustomTxt;
            }
            else if (mode == cSimpleTournament)
            {
                return cSimpleTournamentTxt;
            }
            else
            {
                return cQuickDealTxt;
            }
        }

        Game()
        {
            mode = cQuickDeal;
            deals.push_back(Tarot::Distribution());
        }
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
