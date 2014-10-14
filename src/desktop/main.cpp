/*=============================================================================
 * TarotClub - main.cpp
 *=============================================================================
 * Entry point of Tarot Club
 *=============================================================================
 * TarotClub ( http://www.tarotclub.fr ) - This file is part of TarotClub
 * Copyright (C) 2003-2999 - Anthony Rabine
 * anthony@tarotclub.fr
 *
 * TarotClub is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * TarotClub is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with TarotClub.  If not, see <http://www.gnu.org/licenses/>.
 *
 *=============================================================================
 */

// Qt includes
#include <QApplication>
#include <QSplashScreen>
#include <QDesktopWidget>
#include <QtGlobal>
#include <QTranslator>

// Specific game includes
#include "MainWindow.h"
#include "DebugDock.h"
#include "ClientConfig.h"
#include "Log.h"
#include "System.h"
#include "Util.h"

/*****************************************************************************/
QString GetLocale()
{
    ClientConfig conf;

    conf.Load(System::HomePath() + ClientConfig::DEFAULT_CLIENT_CONFIG_FILE);
    return conf.GetLocale().c_str();
}
/*****************************************************************************/
/**
 * Entry point of the game
 */
int main(int argc, char **argv)
{
    QApplication app(argc, argv);

    QPixmap pixmap(":/images/splash.png");
    QSplashScreen splash(pixmap);
    splash.show();

    System::Initialize();
    Log::SetLogPath(System::LogPath());

    QTranslator translator;
    QString locale = GetLocale();

    // Install language translation files, except if locale is english (native software language)
    if (locale != "en")
    {
        QString langPath = QString(System::LocalePath().c_str()) + QString("tarotclub_") + locale;
        if (translator.load(langPath))
        {
            app.installTranslator(&translator);
        }
        else
        {
            TLogError("Cannot load translation file: tarotclub_xx.");
        }
    }

    MainWindow tarot;
    tarot.Initialize(); // Init internal stuff before showing the interface
    tarot.show();

    splash.finish(&tarot);

    return app.exec();
}

//=============================================================================
// End of file main.cpp
//=============================================================================
