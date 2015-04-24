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

#ifndef I_REMOTE_DB_H
#define I_REMOTE_DB_H

#include <cstdint>
#include <string>
#include "JsonValue.h"

/*****************************************************************************/
class IRemoteDb
{
public:

    virtual ~IRemoteDb() { /* nothing to do */ }

    virtual bool Connect() = 0;
    virtual void Close() = 0;
    virtual bool StoreAiGame(const JsonObject &game, const std::string &username, const std::string &botname) = 0;

};


#endif // I_REMOTE_DB_H

//=============================================================================
// End of file IRemoteDb.h
//=============================================================================
