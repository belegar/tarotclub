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

#define STR_PASS            QObject::tr("Pass")
#define STR_TAKE            QObject::tr("Take")
#define STR_GUARD           QObject::tr("Guard")
#define STR_GUARD_WITHOUT   QObject::tr("Guard without")
#define STR_GUARD_AGAINST   QObject::tr("Guard against")

#define STR_SUD      QObject::tr("South")
#define STR_EST      QObject::tr("East")
#define STR_NORD     QObject::tr("North")
#define STR_OUEST    QObject::tr("West")

#define STR_WIN      QObject::tr("Contract succeded by ")
#define STR_LOSE     QObject::tr("Contract failed by ")
#define STR_POINTS   QObject::tr(" points")


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
        contracts << STR_PASS << STR_TAKE << STR_GUARD << STR_GUARD_WITHOUT << STR_GUARD_AGAINST;
        return contracts[c];
    }
};

#endif // _TAROT_STRINGS_H

//=============================================================================
// End of file Tools.h
//=============================================================================
