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
#include "../TarotEngine.h"
#include "ServerConfig.h"

/*****************************************************************************/
class Lobby : public QObject
{
   Q_OBJECT

private:
   QString saloonName;
   QTcpServer socket;

   // available game tables
   TarotEngine tables[SERVER_MAX_TABLES];

public:
   Lobby();

   void setupTables(ServerOptions &opt);
   void startGames();

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
