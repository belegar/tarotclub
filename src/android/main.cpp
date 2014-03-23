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
#include <QDesktopWidget>
#include <QtGlobal>
#include <QTranslator>

// Specific game includes
#include "TarotClub.h"
#include "Log.h"
#include "System.h"

/*****************************************************************************/
/**
 * Entry point of the game
 */
int main(int argc, char **argv)
{
    QApplication app(argc, argv);

    System::Initialize(); // Initialize TarotClub context

    TarotClub tarot;
    tarot.Initialize(); // Init GUI internal stuff before showing the interface

    tarot.showFullScreen();

    return app.exec();
}

//=============================================================================
// End of file main.cpp
//=============================================================================
