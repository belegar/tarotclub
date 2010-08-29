
#include "Saloon.h"

/*****************************************************************************/
Saloon::Saloon()
{
   saloonName = "Essai";
}
/*****************************************************************************/
void Saloon::startGames()
{
   for (int i=0; i<SERVER_MAX_TABLES; i++) {
      tables[i].start();
   }
}


