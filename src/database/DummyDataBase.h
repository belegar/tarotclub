/*=============================================================================
 * TarotClub - DummyDataBase.h
 *=============================================================================
 * Empty IDataBase implementation used to stub where it is needed
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

#ifndef DUMMY_DATABASE_H
#define DUMMY_DATABASE_H

#include <vector>
#include "Value.h"
#include "IDataBase.h"

/*****************************************************************************/
class DummyDataBase : public IDataBase
{
public:
    DummyDataBase() {}
    ~DummyDataBase() {}

    void Stop() { }

    // From IDataBase
    void Initialize() {}
    void AddPlayer() {}
    void DecPlayer() {}
};


#endif // DUMMY_DATABASE_H

//=============================================================================
// End of file DummyDataBase.h
//=============================================================================
