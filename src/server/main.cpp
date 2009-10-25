/*=============================================================================
 * TarotClubServer - main.cpp
 *=============================================================================
 * Entry point for TarotClubServer
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

#include <QtCore>
#include <QString>
#include <QThread>
#include "ServerConsole.h"
#include "TarotEngine.h"

/*****************************************************************************/
/**
 * Affiche sur la console les messages Qt (warnings, erreurs...)
 */
void myMessageOutput(QtMsgType type, const char *msg)
{
   switch (type) {
      case QtDebugMsg:
         cout << "Debug: " << msg << endl;
         break;
      case QtWarningMsg:
         cout << "Warning: " << msg << endl;
         break;
      case QtCriticalMsg:
         cout << "Critical: " << msg << endl;
         break;
      case QtFatalMsg:
         cout << "Fatal: " << msg << endl;
         abort(); // deliberately core dump
         break;
      default:
         break;
   }
}
/*****************************************************************************/
/**
 * Point d'entrée de l'application
 */
int main(int argc, char* argv[])
{
#ifdef _DEBUG
   qInstallMsgHandler(myMessageOutput);
#endif

   QCoreApplication app(argc, argv);

   cout << "TarotClub server " TAROT_VERSION << endl;
   TarotEngine engine;
   ServerConsole console(&engine);
   console.start();
   engine.start();
   
   app.exec();

   console.exit();
   engine.exit();

   return 0;
}

//=============================================================================
// End of file main.cpp
//=============================================================================
