/*=============================================================================
 * TarotClub - TarotDeck.h
 *=============================================================================
 * Static class in memory to store cards of the game.
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

#ifndef _TAROT_DECK_H
#define _TAROT_DECK_H

#include "Card.h"

/*****************************************************************************/
class TarotDeck
{
public:
    TarotDeck();

    static void Initialize();
    static Card *GetCard(int index);
    static Card *GetCard(const std::string &name);
    static int GetIndex(const std::string &name);

private:
    static Card mCards[78];
};

#endif // _TAROT_DECK_H

//=============================================================================
// End of file TarotDeck.h
//=============================================================================
