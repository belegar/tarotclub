#include "ServerManager.h"

ServerManager::ServerManager() : QThread()
{
   connect(&tarotServer,SIGNAL(readyReadStandardOutput()), this, SLOT() );
}

void ServerManager::run()
{

   tarotServer.start("TarotClubServer");
   tarotServer.waitForStarted();
   gzip.closeWriteChannel();
   QByteArray result = gzip.readAll();

}
