/*=============================================================================
 * TarotClub - DummyRemoteDb.h
 *=============================================================================
 * Dummy implementation of a remote DB connection.
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

#ifndef DUMMY_REMOTE_DB_H
#define DUMMY_REMOTE_DB_H

#include "IRemoteDb.h"

/*****************************************************************************/
class DummyRemoteDb : public IRemoteDb
{
public:

    virtual ~DummyRemoteDb() { /* nothing to do */ }

    virtual bool Connect() { return true; }
    virtual void Close() {}
    virtual bool StoreAiGame(const JsonObject &game, const std::string &username, const std::string &botname)
	{
		(void) game;
        (void) username;
        (void) botname;
		return true;
	}

};


#endif // DUMMY_REMOTE_DB_H

//=============================================================================
// End of file DummyRemoteDb.h
//=============================================================================
