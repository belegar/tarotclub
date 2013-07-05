/*=============================================================================
 * TarotClub - Tools.h
 *=============================================================================
 * Various place for utility classes and character strings
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

#ifndef _TOOLS_H
#define _TOOLS_H

#include <QtCore>
#include "defines.h"

#define STR_PASSE           QT_TR_NOOP("Pass")
#define STR_PRISE           QT_TR_NOOP("Take")
#define STR_GARDE           QT_TR_NOOP("Guard")
#define STR_GARDE_SANS      QT_TR_NOOP("Guard without")
#define STR_GARDE_CONTRE    QT_TR_NOOP("Guard against")

#define STR_SUD      QT_TR_NOOP("South")
#define STR_EST      QT_TR_NOOP("East")
#define STR_NORD     QT_TR_NOOP("North")
#define STR_OUEST    QT_TR_NOOP("West")

#define STR_WIN      QT_TR_NOOP("Contract succeded by ")
#define STR_LOSE     QT_TR_NOOP("Contract failed by ")
#define STR_POINTS   QT_TR_NOOP(" points")


class Util
{
public:

    static QString ToString(Place p)
    {
        QStringList places;
        places << STR_SUD << STR_EST << STR_NORD << STR_OUEST;
        return places[p];
    }

    static QString ToString(Contract c)
    {
        QStringList contracts;
        contracts << STR_PASSE << STR_PRISE << STR_GARDE << STR_GARDE_SANS << STR_GARDE_CONTRE;
        return contracts[c];
    }
};

#endif // _TAROT_STRINGS_H

//=============================================================================
// End of file Tools.h
//=============================================================================
