/*=============================================================================
 * TarotClub - DataBase.h
 *=============================================================================
 * A simple DataBase class abstraction layer
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

#ifndef DATABASE_H
#define DATABASE_H

#include <vector>
#include "Value.h"
#include "sqlite3.h"

/*****************************************************************************/
class DataBase
{
public:
    DataBase();
    ~DataBase();

    // Local SQLite Database Helpers
    bool Open(const std::string &fileName);
    void Close();
    std::vector<std::vector<Value> > Query(const std::string &query);

private:
    sqlite3 *mDb;
};


#endif // DATABASE_H

//=============================================================================
// End of file DataBase.h
//=============================================================================
