/*=============================================================================
 * TarotClub - UniqueId.cpp
 *=============================================================================
 * Unique User identifier utility class
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

#include <algorithm>
#include "UniqueId.h"


/*****************************************************************************/
UniqueId::UniqueId(std::uint32_t min, std::uint32_t max)
    : mMin(min)
    , mMax(max)
{

}
/*****************************************************************************/
std::uint32_t UniqueId::TakeId()
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
/*****************************************************************************/
void UniqueId::ReleaseId(std::uint32_t id)
{
    (void)id;
}

//=============================================================================
// End of file UniqueId.cpp
//=============================================================================
