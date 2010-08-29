#ifndef SALOON_H
#define SALOON_H

#include <QtCore>
#include "../TarotEngine.h"
#include "ServerConfig.h"

class Saloon
{
private:
   QString saloonName;

   // available game tables
   TarotEngine tables[SERVER_MAX_TABLES];

public:
   Saloon();

   void startGames();

};


#endif // SALOON_H
