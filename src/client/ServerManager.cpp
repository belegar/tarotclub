#include "ServerManager.h"


/*****************************************************************************/
ServerManager::ServerManager() : QThread()
{
  // connect(&tarotServer,SIGNAL(readyReadStandardOutput()), this, SLOT() );
}
/*****************************************************************************/
void ServerManager::run()
{
   tarotServer.start("TarotClubServer");
   tarotServer.waitForStarted();
   tarotServer.waitForFinished();
}

//=============================================================================
// End of file ServerManager.cpp
//=============================================================================
