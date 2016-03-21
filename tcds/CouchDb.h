/*=============================================================================
 * TarotClub - CouchDb.h
 *=============================================================================
 * CouchDb utility class (uses the REST API)
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


#ifndef COUCH_DB_H
#define COUCH_DB_H

#include "Http.h"
#include "JsonReader.h"
#include "JsonWriter.h"

class CouchDb : public JsonValue, public http::IEvent
{

public:
    CouchDb();

    // Raw CouchDB interface, get a document
    void Get(const std::string &url);

    // Work on a document, provide an id
    void Put(const std::string &url);
    void Put(const std::string &url, const std::string &body);

    // Post document, without id
    void Post(const std::string &url, const std::string &body);

    // Delete a document or a database
    void Delete(const std::string &url);

    bool Connect();
    void Close();
    bool IsSuccess();

    // Utility methods
    std::string GetRawData() { return mData; }

private:
    std::string mData;
    http::Connection mConn;
    std::string mLastError;

    // Http::IEvent interface
    virtual void ResponseBegin( const http::Response* r);
    virtual void ResponseData( const http::Response* r, const char* data, int n );
    virtual void ResponseComplete( const http::Response* r );
};

#endif // COUCH_DB_H

//=============================================================================
// End of file CouchDb.h
//=============================================================================
