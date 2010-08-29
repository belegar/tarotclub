#ifndef TAROTSERVER_H
#define TAROTSERVER_H

#include <QThread>
#include <QtNetwork>
#include "../defines.h"
#include "Saloon.h"
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
   Saloon playground;
   ServerConfig config;

public:
   TarotServer();

   void start();
   void incomingConnection(int socket);

public slots:
   void readClient();
   void discardClient();

};


#endif // TAROTSERVER_H
