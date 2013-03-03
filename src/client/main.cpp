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

using namespace std;

/*****************************************************************************/
/**
 * Affiche sur la console les messages Qt (warnings, erreurs...)
 */
#ifndef QT_NO_DEBUG
void myMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QByteArray localMsg = msg.toLocal8Bit();
    switch (type) {
    case QtDebugMsg:
        fprintf(stderr, "Debug: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
        break;
    case QtWarningMsg:
        fprintf(stderr, "Warning: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
        break;
    case QtCriticalMsg:
        fprintf(stderr, "Critical: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
        break;
    case QtFatalMsg:
        fprintf(stderr, "Fatal: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
        abort();
    }
}
#endif
/*****************************************************************************/
/**
 * Entry point of the game
 */
int main( int argc, char** argv )
{
#ifndef QT_NO_DEBUG
    qInstallMessageHandler(myMessageOutput);
#endif

    QApplication app( argc, argv );

    QPixmap pixmap( ":/images/splash.png" );
    QSplashScreen splash(pixmap);
    splash.show();

    Game window;

    window.resize(1280, 770);
    QRect r = window.geometry();
    r.moveCenter(QApplication::desktop()->availableGeometry().center());
    window.setGeometry(r);
    window.show();

    splash.finish(&window);

   return app.exec();
}

//=============================================================================
// End of file main.cpp
//=============================================================================
