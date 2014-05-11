/*=============================================================================
 * TarotClub - System.cpp
 *=============================================================================
 * Manages system path for resources and generated files
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

#ifdef USE_ANDROID_OS
#include <QStandardPaths>
#include <QString>
#endif

#include "System.h"
#include "Util.h"
#include "TarotDeck.h"

/*****************************************************************************/
std::string System::mHomePath;
std::string System::mDeckPath;
std::string System::mScriptPath;

/*****************************************************************************/
void System::Initialize()
{
// ----------- Android
#ifdef USE_ANDROID_OS
    QString path = QStandardPaths::writableLocation(QStandardPaths::DataLocation);
    mHomePath = path.toStdString();

    FIXME: Copy script files from the Qt resource file to the application internal memory

    mDeckPath = ":cards/default/";
// ----------- Linux, Windows ...
#else

#ifdef TAROT_DEBUG
    mHomePath  = "./";
    mScriptPath = Util::ExecutablePath() + "/../../assets/ai/";
    mDeckPath = Util::ExecutablePath() + "/../../assets/cards/default/";
#else
    mHomePath  = Util::HomePath() + "/.tarotclub/";
    mScriptPath = Util::ExecutablePath() + "/ai/";
    mDeckPath = Util::ExecutablePath() + "/default/";
#endif
#endif

    // Check the user TarotClub directories and create them if necessary (all platforms)
    if (!Util::FolderExists(HomePath()))
    {
        Util::Mkdir(HomePath());
    }
    if (!Util::FolderExists(GamePath()))
    {
        Util::Mkdir(GamePath());
    }
    if (!Util::FolderExists(LogPath()))
    {
        Util::Mkdir(LogPath());
    }

    TarotDeck::Initialize();
}

/*****************************************************************************/
System::System()
{
}

//=============================================================================
// End of file System.cpp
//=============================================================================
