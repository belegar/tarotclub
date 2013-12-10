/*=============================================================================
 * TarotClub - NetPlayer.h
 *=============================================================================
 * Networked remote player, used in the server side
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

#ifndef _NET_PLAYER_H
#define _NET_PLAYER_H

#include <QtNetwork>
#include "defines.h"
#include "Deck.h"
#include "Identity.h"
#include "Player.h"

/*****************************************************************************/
/*****************************************************************************/
class UserId
{
public:
    UserId(std::uint32_t min, std::uint32_t max)
        : mMin(min)
        , mMax(max)
    {

    }

    std::uint32_t TakeId()
    {
        std::uint32_t id;

        for (id = mMin; id <= mMax; id++)
        {
            if (std::find(mUsedIds.begin(), mUsedIds.end(), id) == mUsedIds.end())
            {
                // Id not used
                mUsedIds.push_back(id);
                break;
            }
        }
        return id;
    }

    void ReleaseId(std::uint32_t id)
    {
        // TODO
    }

private:
    std::uint32_t mMin;
    std::uint32_t mMax;
    std::list<std::uint32_t> mUsedIds;
};

#endif // _NET_PLAYER_H

//=============================================================================
// End of file NetPlayer.h
//=============================================================================
