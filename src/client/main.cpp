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
#include "TarotClub.h"
#include "DebugDock.h"
#include "ClientConfig.h"
#include "../Log.h"

/*****************************************************************************/
QString GetLocale()
{
    ClientConfig conf;

    conf.Load();
    return conf.GetLocale();
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

    QTranslator translator;
    QString locale = GetLocale();

    // Install language translation files
    if (translator.load(QString("tarotclub_") + locale) == false)
    {
        TLogError("Cannot load translation file: tarotclub_xx.");
    }
    app.installTranslator(&translator);

    TarotClub tarot;
    tarot.Initialize(); // Init internal stuff before showing the interface

    tarot.resize(1280, 770);
    QRect r = tarot.geometry();
    r.moveCenter(QApplication::desktop()->availableGeometry().center());
    tarot.setGeometry(r);
    tarot.show();

    splash.finish(&tarot);

    return app.exec();
}

//=============================================================================
// End of file main.cpp
//=============================================================================
