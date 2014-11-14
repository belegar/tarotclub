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

/*****************************************************************************/
struct ServerOptions
{
    std::uint16_t timer;      // between players, in milliseconds
    std::uint16_t game_tcp_port;
    std::uint16_t web_tcp_port;
    std::int32_t lobby_max_conn;    // Max number of simultaneous connected clients
    std::uint8_t    tournamentTurns;
    std::map<Place, Identity>   bots;
    std::vector<std::string> tables; // table list names
};

/*****************************************************************************/
class ServerConfig
{

public:
    static const std::uint16_t  DEFAULT_DELAY               = 500U;     // in ms
    static const std::uint16_t  DEFAULT_GAME_TCP_PORT       = 4269U;
    static const std::uint16_t  DEFAULT_WEB_TCP_PORT        = 8080U;
    static const std::uint32_t  DEFAULT_LOBBY_MAX_CONN      = 250U;
    static const std::string    DEFAULT_SERVER_CONFIG_FILE;
    static const std::uint8_t   DEFAULT_NUMBER_OF_TURNS     = 5U;
    static const std::uint8_t   MAX_NUMBER_OF_TURNS         = 20U;

    ServerConfig();

    // Helpers
    bool     Load(const std::string &fileName);
    bool     Save(const std::string &fileName);

    // Getters
    ServerOptions &GetOptions();

    // Setters
    void        SetOptions(const ServerOptions &newOptions);
    static ServerOptions GetDefault();

private:
    ServerOptions mOptions;
    bool mLoaded;

};

#endif // SERVER_CONFIG_H

//=============================================================================
// End of file ServerConfig.h
//=============================================================================
