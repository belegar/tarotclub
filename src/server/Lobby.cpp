/*=============================================================================
 * TarotClub - Lobby.cpp
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

#include "Lobby.h"
#include <iostream>
using namespace std;

/*****************************************************************************/
Lobby::Lobby()
{
   saloonName = "Essai";

   socket.listen(QHostAddress::Any, 4242);
   if (!socket.isListening()) {
      cerr << "Failed to bind to port 4242";
      qApp->quit();
   }
   connect(&socket, SIGNAL(newConnection()), this, SLOT(slotNewConnection()));
}
/*****************************************************************************/
void Lobby::startGames()
{
   for (int i=0; i<SERVER_MAX_TABLES; i++) {
      tables[i].newServerGame();
      tables[i].start();
   }
}
/*****************************************************************************/
void Lobby::setupTables(ServerOptions &opt)
{
   for (int i=0; i<SERVER_MAX_TABLES; i++) {
      tables[i].setOptions(opt);
      tables[i].setGameType(LOC_ONEDEAL);
      tables[i].setDealType(RANDOM_DEAL);
      opt.port++;
   }

}
/*****************************************************************************/
void Lobby::slotClientClosed()
{
    QTcpSocket* s = qobject_cast<QTcpSocket *>(sender());
    s->deleteLater();
}
/*****************************************************************************/
void Lobby::slotReadData()
{
   // This slot is called when the client sent data to the server. The
   // server looks if it was a get request and sends a very simple HTML
   // document back.
   QTcpSocket* s = qobject_cast<QTcpSocket *>(sender());
   if (s->canReadLine()) {
      QStringList tokens = QString(s->readLine()).split(':', QString::SkipEmptyParts, Qt::CaseSensitive);
      QTextStream os(s);
      if (tokens[0] == "GET") {
         if (tokens[1] == "SALOONS") {
            os.setAutoDetectUnicode(true);
            os << "SALOONS:" << saloonName;
            os << ":\r\n";
            os.flush();
         }
      }
   }
}
/*****************************************************************************/
void Lobby::slotNewConnection()
{
   QTcpSocket *s = socket.nextPendingConnection();
   connect( s, SIGNAL(disconnected()), this, SLOT(slotClientClosed()));
   connect( s, SIGNAL(readyRead()), this, SLOT(slotReadData()));
}

//=============================================================================
// End of file Lobby.cpp
//=============================================================================
