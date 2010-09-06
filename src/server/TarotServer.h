/*=============================================================================
 * TarotClub - TarotServer.h
 *=============================================================================
 * Little web server to manage Tarot tables
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

#ifndef TAROTSERVER_H
#define TAROTSERVER_H

#include <QThread>
#include <QtNetwork>
#include "../defines.h"
#include "Lobby.h"
#include "ServerConfig.h"

/*****************************************************************************/
/**
 * --- Definitions ---
 * Saloon: group of tables, usually gather players of same level
 * Table: a game composed by four players
 */
class TarotServer : public QTcpServer
{
   Q_OBJECT

private:
   Lobby lobby;
   ServerConfig config;

public:
   TarotServer();

   void start();

protected:
   void incomingConnection(int socket);

public slots:
   void readClient();
   void discardClient();

};

#endif // TAROTSERVER_H

//=============================================================================
// End of file TarotServer.h
//=============================================================================
