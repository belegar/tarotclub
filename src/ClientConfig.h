/*=============================================================================
 * TarotClub - ClientConfig.h
 *=============================================================================
 * Option file parameters management for the client configuration
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
#ifndef _CLIENTCONFIG_H
#define _CLIENTCONFIG_H

// Standard library
#include <cstdint>
#include <vector>
#include <string>

// Game includes
#include "defines.h"
#include "Identity.h"

// default values
#define AVATARS_DEF         true
#define CLIENT_TIMER_DEF    1500

typedef struct
{
    std::string     deckFilePath;
    bool            showAvatars;
    std::uint8_t    language;
    std::string     backgroundColor;
    Identity        identity;
    std::uint16_t   delayBeforeCleaning;  // in milliseconds
    bool            enableDelayBeforeCleaning;
    std::string     cardsOrder;
} ClientOptions;


/*****************************************************************************/
class ClientConfig
{
public:
    ClientConfig();

    bool     Load();
    bool     Save();

    ClientOptions &GetOptions();
    std::string GetLocale();

    void        SetOptions(ClientOptions &newOptions);
    static void SetDefault(ClientOptions &opt);

private:
    ClientOptions mOptions;
    bool mLoaded;
    std::vector<std::string> mLang;
};

#endif // _CLIENTCONFIG_H

//=============================================================================
// End of file ClientConfig.h
//=============================================================================