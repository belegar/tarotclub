/*=============================================================================
 * TarotClub - main.cpp
 *=============================================================================
 * Entry point of Tarot Club
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

// Inclusions Qt
#include <QApplication>
#include <QSplashScreen>
#include <QDesktopWidget>
#include <QtGlobal>
#include <QTranslator>

// Inclusions C++
#include <iostream>

// Inclusions du jeu
#include "Game.h"
#include "ConfigFile.h"

using namespace std;

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
    }
}
/*****************************************************************************/
/**
 * Point d'entrée de l'application
 */
int main( int argc, char** argv )
{
   qInstallMsgHandler(myMessageOutput);

   QApplication app( argc, argv );
   QDir b = qApp->applicationDirPath();
   b.cdUp();
   QString path = b.path();

   QPixmap pixmap( ":/images/splash.png" );
   QSplashScreen splash(pixmap);
//   splash.show();

   ConfigFile config(path);
   if( config.load(CONFIG_FILE) == false ) {
      cout <<  "Unable to find the user preferences file.\n"
               "The factory default will be used instead." << endl;
      app.quit();
   }

   /*
   QTranslator translator;
   GameOptions *opt = config->getGameOptions();
   translator.load( b.path() + "/bin/" + langList[opt->langue] );
   qApp->installTranslator(&translator);
*/
   Game window( &config );
   window.resize(1000, 770);
   if ( QApplication::desktop()->width() > 1010 && QApplication::desktop()->height() > 780 ) {
      window.showMaximized();
   } else {
      window.show();
   }

   splash.finish(&window);
   return app.exec();
}

//=============================================================================
// End of file main.cpp
//=============================================================================
