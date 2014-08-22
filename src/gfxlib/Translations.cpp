/*=============================================================================
 * TarotClub - Translations.cpp
 *=============================================================================
 * Unique place to store shared strings among various widgets
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

// Qt includes
#include <QObject>
#include "Translations.h"

QString ContractToString(Contract c)
{
    QString labels[] =
    {
        QObject::tr("Pass"),
        QObject::tr("Take"),
        QObject::tr("Guard"),
        QObject::tr("Guard without"),
        QObject::tr("Guard against")
    };

    return labels[c.Value()];
}
/*****************************************************************************/
QString PlaceToString(Place p)
{
    QString labels[] =
    {
        QObject::tr("South"),
        QObject::tr("East"),
        QObject::tr("North"),
        QObject::tr("West"),
        QObject::tr("Fifth")
    };
    return labels[p.Value()];
}

//=============================================================================
// End of file Translations.cpp
//=============================================================================
