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
