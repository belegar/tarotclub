/*=============================================================================
 * Tarot Club - main.cpp
 *=============================================================================
 * Entry point of Tarot Club
 *=============================================================================
 * Tarot Club est un jeu de Tarot franï¿œais
 * Copyright (C) 2003-2005  Anthony Rabine
 * anthony@ooso.org
 * http://tarotclub.ooso.org
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
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
   splash.show();

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
