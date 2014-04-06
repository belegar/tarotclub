/*=============================================================================
 * TarotClub - IButtonEvent.h
 *=============================================================================
 * Virtual class used to signal a button click between objects
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

#ifndef I_BUTTON_EVENT_H
#define I_BUTTON_EVENT_H

#include <cstdint>

class IButtonEvent
{

public:
    virtual ~IButtonEvent() {}

    /**
     * @brief Clicked
     * @param id
     * @param menu
     */
    virtual void Clicked(std::uint8_t id, std::uint8_t menu) = 0;
};

#endif // I_BUTTON_EVENT_H

//=============================================================================
// End of file IButtonEvent.h
//=============================================================================
