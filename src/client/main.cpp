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

// Std C++
#include <iostream>

// Specific game includes
#include "TarotClub.h"
#include "DebugDock.h"

using namespace std;

/*****************************************************************************/
/**
 * Redirect debug messages to the console (FIXME: or Log file?)
 */
#ifndef QT_NO_DEBUG
void myMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    DebugDock *output = DebugDock::getInstance();
    QString infos = QString(": ") + QString(context.file) + QString(":") + QString().setNum(context.line) + QString(", ") + QString(context.function);

    if (output != NULL)
    {
        switch (type)
        {
            case QtDebugMsg:
                output->message(msg);
                break;
            case QtWarningMsg:
                output->message(msg + infos);
                break;
            case QtCriticalMsg:
                output->message(msg + infos);
                break;
            case QtFatalMsg:
                output->message(msg + infos);
                abort();
        }
    }
    else
    {
        cout << msg.toLatin1().constData() << infos.toLatin1().constData() << endl;
    }
}
#endif
/*****************************************************************************/
/**
 * Entry point of the game
 */
int main(int argc, char **argv)
{
#ifndef QT_NO_DEBUG
    qInstallMessageHandler(myMessageOutput);
#endif

    QApplication app(argc, argv);

    QPixmap pixmap(":/images/splash.png");
    QSplashScreen splash(pixmap);
    splash.show();

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
