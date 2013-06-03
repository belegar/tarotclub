/*=============================================================================
 * TarotClub - DealFile.h
 *=============================================================================
 * Manage saving and loading XML files of custom deals
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
#ifndef DEALFILE_H
#define DEALFILE_H

#include <QString>
#include "Deck.h"

/*****************************************************************************/
class DealFile
{
public:
    DealFile();

    bool LoadFile(QString fileName);
    void SaveFile(QString fileName);

    Deck  southDeck;
    Deck  northDeck;
    Deck  westDeck;
    Deck  eastDeck;
    Deck  dogDeck;

private:
    bool FillDeck(Deck &deck, QDomElement &child);

};

#endif // DEALFILE_H

//=============================================================================
// End of file DealFile.h
//=============================================================================
