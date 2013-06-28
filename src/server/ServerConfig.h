/*=============================================================================
 * TarotClub - ServerConfig.h
 *=============================================================================
 * Classe de gestion du fichier d'options en XML (serveur)
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
#ifndef _CONFIGFILE_H
#define _CONFIGFILE_H

// Qt includes
#include <QString>
#include <QFile>
#include <QStringList>

// Game includes
#include "../defines.h"
#include "../Identity.h"

#define SERVER_XML_VERSION    "1.5"
#define SERVER_CONFIG_FILE    "tarotclubd.xml"
#define SERVER_MAX_TABLES     10

// default values
#define TIMER1_DEF      500
#define DEFAULT_PORT    4269

/*****************************************************************************/
typedef struct
{
    int      timer;  // between players, in milliseconds
    int      port;   // TCP/IP port
    Identity bots[3];
} ServerOptions;

/*****************************************************************************/
class ServerConfig
{

public:
    enum BotPlace
    {
        BOT_EAST = 0,
        BOT_NORTH = 1,
        BOT_WEST
    };

    ServerConfig();

    // Helpers
    bool     Load();
    bool     Save();

    // Getters
    ServerOptions &GetOptions();

    // Setters
    void        SetOptions(ServerOptions &newOptions);
    static void SetDefault(ServerOptions &opt);

private:
    ServerOptions options;

};

#endif // _CONFIGFILE_H

//=============================================================================
// End of file ServerConfig.h
//=============================================================================
