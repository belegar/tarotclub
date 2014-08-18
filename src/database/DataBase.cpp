/*=============================================================================
 * TarotClub - DataBase.cpp
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

#include <cctype>
#include <iostream>
#include <string.h>

#include "DataBase.h"

/*
static int callback(void *NotUsed, int argc, char **argv, char **azColName)
{
   int i;

   (void) NotUsed;
   (void) argv;
   (void) azColName;

   for (i=0; i<argc; i++)
   {
      //printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
   }
   printf("\n");
   return 0;
}
*/

/*****************************************************************************/
DataBase::DataBase()
    : mDb(NULL)
    , mValid(false)
{


}
/*****************************************************************************/
DataBase::~DataBase()
{
    Close();

}
/*****************************************************************************/
void DataBase::Close()
{
    if (mDb != NULL)
    {
        sqlite3_close(mDb);
        mDb = NULL;
    }
    mValid = false;
}
/*****************************************************************************/
bool DataBase::Open(const std::string &fileName)
{
    int rc = sqlite3_open(fileName.c_str(), &mDb);
    if (rc == 0)
    {
        mValid = true;
    }
    return mValid;
}
/*****************************************************************************/
std::vector<std::vector<Value> > DataBase::Query(const char *query)
{
    sqlite3_stmt *statement;
    std::vector<std::vector<Value> > results;

    if (sqlite3_prepare_v2(mDb, query, -1, &statement, 0) == SQLITE_OK)
    {
        int cols = sqlite3_column_count(statement);

        while (true)
        {
            int result = sqlite3_step(statement);
            if (result == SQLITE_ROW)
            {
                std::vector<Value> values;

                for (int col = 0; col < cols; col++)
                {
                    int type = sqlite3_column_type(statement, col);

                    // Limitation here: we only support integer and strings
                    if (type == SQLITE_INTEGER)
                    {
                        std::int32_t intVal = sqlite3_column_int(statement, col);
                        values.push_back(intVal);
                    }
                    else if (type == SQLITE_TEXT)
                    {
                        std::string strVal((char *)sqlite3_column_text(statement, col));
                        values.push_back(strVal);
                    }
                    else
                    {
                        // Not supported, push invalid value
                        Value invalid;
                        values.push_back(invalid);
                    }
                }
                results.push_back(values);
            }
            else
            {
                break;
            }
        }
        sqlite3_finalize(statement);
    }
    std::string error = sqlite3_errmsg(mDb);
    if (error != "not an error")
    {
        std::cout << query << " " << error << std::endl;
    }

    return results;

}



//=============================================================================
// End of file DataBase.cpp
//=============================================================================
