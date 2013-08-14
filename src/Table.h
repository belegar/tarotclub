/*=============================================================================
 * TarotClub - Table.h
 *=============================================================================
 * The table manages the Tarot thread server
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
#ifndef TABLE_H
#define TABLE_H

#include <QObject>
#include "Server.h"

/*****************************************************************************/
class Table : public QObject
{

public:
    enum Mode
    {
        USE_BOTS,
        MULTIPLAYERS
    };

    Table();

    // Helpers
    void LoadConfiguration(int tcpPort = DEFAULT_PORT);
    void CreateGame(Game::Mode gameMode, Table::Mode tableMode, int nbPlayers = 4);
    void Start();
    void Stop();

    // Getters
    Server &GetServer();
    ServerOptions &GetOptions();
    TarotEngine::Shuffle GetShuffle();

    // Setters
    void SetShuffle(const TarotEngine::Shuffle &s);
    void SaveConfiguration(const ServerOptions &opt);

private:
    Server server;
    ServerConfig serverConfig;
    Mode mode;
    Bot bots[3];

    void ConnectBots();

};

#endif // TABLE_H

//=============================================================================
// End of file Table.h
//=============================================================================
