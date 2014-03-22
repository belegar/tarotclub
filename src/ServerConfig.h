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
#ifndef SERVER_CONFIG_H
#define SERVER_CONFIG_H

// Game includes
#include "Common.h"
#include "Identity.h"

#define SERVER_MAX_TABLES     10
#define SERVER_MAX_SALOONS    2

static const std::uint16_t DEFAULT_DELAY        = 500U;     // in ms
static const std::uint16_t DEFAULT_TCP_PORT     = 4269U;

/*****************************************************************************/
typedef struct
{
    std::uint16_t   timer;      // between players, in milliseconds
    std::uint16_t   tcp_port;   // TCP/IP port
    std::map<Place, Identity>   bots;
} ServerOptions;

/*****************************************************************************/
class ServerConfig
{

public:
    ServerConfig();

    // Helpers
    bool     Load();
    bool     Save();

    // Getters
    ServerOptions &GetOptions();

    // Setters
    void        SetOptions(const ServerOptions &newOptions);
    static void SetDefault(ServerOptions &opt);

private:
    ServerOptions options;

};

#endif // SERVER_CONFIG_H

//=============================================================================
// End of file ServerConfig.h
//=============================================================================
