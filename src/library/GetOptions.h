/*=============================================================================
 * TarotClub - GetOptions.h
 *=============================================================================
 * Retrieve the command line option parameters
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

#ifndef GETOPTIONS_H
#define GETOPTIONS_H

#include <algorithm>

class CommandLine
{

public:
    CommandLine(int argc, char *argv[])
        : mArgc(argc)
        , mArgv(argv)
        , mEnd(mArgv + mArgc)
    {

    }

    CommandLine()
        : mArgc(0)
        , mArgv(NULL)
        , mEnd(NULL)
    {

    }

    std::string GetOption(const std::string & option)
    {
        if (mArgc > 1)
        {
            char ** itr = std::find(mArgv, mEnd, option);
            if (itr != mEnd && ++itr != mEnd)
            {
                return *itr;
            }
        }
        return "";
    }

    std::string GetLastOption()
    {
        if (mArgc > 1)
        {
            return (mArgv[mArgc-1]);
        }
        return "";
    }

    bool Exists(const std::string& option)
    {
        bool exists = false;

        if (mArgc > 1)
        {
            exists = std::find(mArgv, mEnd, option) != mEnd;
        }
        return exists;
    }

private:
    int mArgc;
    char **mArgv;
    char **mEnd;
};

#endif // GETOPTIONS_H

//=============================================================================
// End of file GetOptions.h
//=============================================================================
