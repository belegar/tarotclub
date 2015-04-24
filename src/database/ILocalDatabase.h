/*=============================================================================
 * TarotClub - IDataBase.h
 *=============================================================================
 * An application interface to write and read data from a database
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

#ifndef IDATABASE_H
#define IDATABASE_H

#include <cstdint>

/*****************************************************************************/
class ILocalDataBase
{
public:
    struct Stats
    {
        Stats() { Reset(); }

        void Reset()
        {
            min = 0U;
            max = 0U;
            current = 0U;
            total = 0U;
        }

        std::uint32_t min;
        std::uint32_t max;
        std::uint32_t current;
        std::uint32_t total;
    };

    virtual ~ILocalDataBase() { /* nothing to do */ }

    virtual void Initialize() = 0;
    virtual void AddPlayer() = 0;
    virtual void DecPlayer() = 0;

};


#endif // IDATABASE_H

//=============================================================================
// End of file IDataBase.h
//=============================================================================
