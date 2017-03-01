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
#include "Log.h"
#include "TcpSocket.h"

/*****************************************************************************/
std::string System::mHomePath;
std::string System::mDeckPath;
std::string System::mScriptPath;
std::string System::mLocalePath;

/*****************************************************************************/
void System::Initialize()
{
    std::string homePath;
#ifdef TAROT_DEBUG
    homePath = Util::ExecutablePath() + "/.tarotclub/";
#else
    homePath = Util::HomePath() + "/.tarotclub/";
#endif

    Initialize(homePath);
}
/*****************************************************************************/
void System::Initialize(const std::string &homePath)
{
    mHomePath = homePath; 

    // ----------- Android
#ifdef USE_ANDROID_OS
    QString path = QStandardPaths::writableLocation(QStandardPaths::DataLocation);
    mHomePath = path.toStdString();

    //FIXME:
    //Copy script files from the Qt resource file to the application internal memory

    mDeckPath = ":cards/default/";

    // ----------- Linux, Windows ...
#else

#ifdef TAROT_DEBUG
    mScriptPath = Util::ExecutablePath() + "/../../../assets/ai/";
    mDeckPath = Util::ExecutablePath() + "/../../../assets/cards/default/";
    mLocalePath = Util::ExecutablePath() + "/../../../assets/ts/";
#else
    mScriptPath = Util::ExecutablePath() + "/ai/";
    mDeckPath = Util::ExecutablePath() + "/default/";
    mLocalePath = Util::ExecutablePath() + "/";
#endif
#endif

#ifdef USE_WINDOWS_OS
    Util::ReplaceCharacter(mHomePath, "/", "\\");
    Util::ReplaceCharacter(mScriptPath, "/", "\\");
    Util::ReplaceCharacter(mDeckPath, "/", "\\");
    Util::ReplaceCharacter(mLocalePath, "/", "\\");
#endif

    // Ensure final slash/backslash
    if (mHomePath.back() != Util::DIR_SEPARATOR)
    {
        mHomePath.push_back(Util::DIR_SEPARATOR);
    }

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
    if (!Util::FolderExists(AvatarPath()))
    {
        Util::Mkdir(AvatarPath());
    }

    if (!tcp::TcpSocket::Initialize())
    {
        TLogError("Cannot initialize TCP context");
    }
}
/*****************************************************************************/
System::System()
{
}

//=============================================================================
// End of file System.cpp
//=============================================================================
