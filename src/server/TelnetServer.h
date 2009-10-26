/*=============================================================================
 * TarotClub - TelnetServer.h
 *=============================================================================
 * Telnet server to add remote game control
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

#ifndef _TELNETSERVER_H
#define _TELNETSERVER_H

#include <QThread>
#include <QtNetwork>
#include "../defines.h"
#include <iostream>
using namespace std;

/*****************************************************************************/
class TelnetServer : public ServerConsole
{
   Q_OBJECT

private:
   QTcpServer  server;
   QString login;
   QString pass;

public:
   TelnetServer(QObject *o);

   void run();

public slots:
   void  newConnection();
   void  clientClosed();
   void  readData();
};

#endif // _TELNETSERVER_H

//=============================================================================
// End of file TelnetServer.h
//=============================================================================
