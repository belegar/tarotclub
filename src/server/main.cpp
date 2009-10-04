
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
