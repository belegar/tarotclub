/*=============================================================================
 * TarotClub - Avatar.h
 *=============================================================================
 * Utility class to manage different source of avatar image files
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

#ifndef AVATAR_H
#define AVATAR_H

#include <QtGui/QPixmap>
#include <QtCore/QUrl>
#include <QtNetwork>

/*****************************************************************************/
class Avatar
{

public:
    Avatar();
    Avatar(const QString &filePath);

    bool IsLocal() const;
    bool IsPredefined() const;
    bool IsValid() const;
    QPixmap GetPixmap();
    QString GetFilePath() { return mFilePath; }
    void SetFilePath(const QString &filePath);
    bool LoadFile();
    bool LoadFromAvatarDirectory();
    bool SaveToAvatarDirectory();
    bool ExistsInAvatarDirectory();
    QString GetLocalPath();
    QString HashName();

private:
    QPixmap mPixmap;
    QString mFilePath;
    QNetworkAccessManager mNetworkManager;
};

#endif // AVATAR_H

//=============================================================================
// End of file Avatar.h
//=============================================================================
