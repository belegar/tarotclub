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

// Qt includes
#include <QString>
#include <QFile>
#include <QStringList>

// Game includes
#include "../defines.h"
#include "../Identity.h"

#define CLIENT_XML_VERSION    "1.5"
#define CLIENT_CONFIG_FILE    "tarotclub.xml"

// default values
#define AVATARS_DEF         true
#define CLIENT_TIMER_DEF    1500

typedef struct
{
    QString         deckFilePath;
    bool            showAvatars;
    int             language;
    QString         backgroundColor;
    Identity        identity;
    unsigned int    delayBeforeCleaning;  // in milliseconds
    bool            enableDelayBeforeCleaning;
} ClientOptions;


/*****************************************************************************/
class ClientConfig
{
public:
    ClientConfig();

    bool     Load();
    bool     Save();

    ClientOptions &GetOptions();
    QString GetLocale();

    void        SetOptions(ClientOptions &newOptions);
    static void SetDefault(ClientOptions &opt);

private:
    ClientOptions options;
    bool loaded;
    QStringList lang;
};

#endif // _CLIENTCONFIG_H

//=============================================================================
// End of file ClientConfig.h
//=============================================================================
