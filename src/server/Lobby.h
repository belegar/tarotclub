/*=============================================================================
 * TarotClub - Lobby.h
 *=============================================================================
 * Manage temporary connections to join free game tables
 *=============================================================================
 * TarotClub ( http://www.tarotclub.fr ) - This file is part of TarotClub
 * Copyright (C) 2003-2999 - Anthony Rabine
 * anthony@tarotclub.fr
 *
 * This file must be used under the terms of the CeCILL.
 * This source file is licensed as described in the file COPYING, which
 * you should have received as part of this distribution.  The terms
 * are also available at
 * http://www.cecill.info/licences/Licence_CeCILL_V2-en.txt
 *
 *=============================================================================
 */

#ifndef LOBBY_H
#define LOBBY_H

#include <QtCore>
#include "../Table.h"
#include "ServerConfig.h"

/*****************************************************************************/
class Lobby : public QObject
{
    Q_OBJECT

private:

    struct TableInstance
    {
        QString name;
        Table table;    // A Tarot table, owns a thread, bots and a Tarot network engine game
        QThread thread;
    };

    struct Saloon
    {
        QString name; // name of
        TableInstance tables[SERVER_MAX_TABLES]; // available game tables in this saloon
    };

    QTcpServer socket;
    Saloon  saloons[SERVER_MAX_SALOONS];

public:
    Lobby();

    void SetupTables();
    void StartGames();

    void Initialize();
public slots:
    // Connections to the lobby
    void slotNewConnection();
    void slotClientClosed();
    void slotReadData();
};

#endif // LOBBY_H

//=============================================================================
// End of file Lobby.h
//=============================================================================
