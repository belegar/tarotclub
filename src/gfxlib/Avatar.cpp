/*=============================================================================
 * TarotClub - Avatar.cpp
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

#include "Avatar.h"
#include "System.h"

/*****************************************************************************/
Avatar::Avatar()
{

}
/*****************************************************************************/
void Avatar::SetFilePath(const QString &filePath)
{
    mFilePath = filePath;
}
/*****************************************************************************/
bool Avatar::LoadFile()
{
    bool success = false;

    if (IsLocal())
    {
        success = mPixmap.load(mFilePath);
    }
    else
    {
        if (IsValid())
        {
            QNetworkReply *reply = mNetworkManager.get(QNetworkRequest(QUrl::fromUserInput(mFilePath)));

            QEventLoop loop;
            connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
            loop.exec();

            const QByteArray data(reply->readAll());
            if (data.size() > 0)
            {
                success = mPixmap.loadFromData(data);
            }
            reply->deleteLater();
        }
    }

    return success;
}
/*****************************************************************************/
/**
 * @brief Save the Pixmap to the TarotClub avatars directory
 * @return
 */
bool Avatar::SaveToLocalDirectory()
{
    bool ret = false;
    if (IsValid())
    {
        ret = mPixmap.save(QString(System::AvatarPath().c_str()) + QFileInfo(mFilePath).fileName());
    }
    return ret;
}
/*****************************************************************************/
bool Avatar::ExistsInLocalDirectory(const QString &filePath)
{
    QFileInfo info(GetLocalPath(filePath));
    return info.exists();
}
/*****************************************************************************/
QString Avatar::GetLocalPath(const QString &filePath)
{
    return QString(System::AvatarPath().c_str()) + QFileInfo(filePath).fileName();
}
/*****************************************************************************/
bool Avatar::IsLocal() const
{
    return QUrl::fromUserInput(mFilePath).isLocalFile();
}
/*****************************************************************************/
bool Avatar::IsValid() const
{
    return QUrl::fromUserInput(mFilePath).isValid();
}
/*****************************************************************************/
QPixmap Avatar::GetPixmap()
{
    return mPixmap;
}

//=============================================================================
// End of file Avatar.cpp
//=============================================================================
