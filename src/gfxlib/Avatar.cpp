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
#include <QCryptographicHash>

/*****************************************************************************/
Avatar::Avatar()
{

}
/*****************************************************************************/
Avatar::Avatar(const QString &filePath)
    : mFilePath(filePath)
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
        ret = mPixmap.save(GetLocalPath());
    }
    return ret;
}
/*****************************************************************************/
bool Avatar::ExistsInLocalDirectory()
{
    QFileInfo info(GetLocalPath());
    return info.exists();
}
/*****************************************************************************/
QString Avatar::GetLocalPath()
{
    return QString(System::AvatarPath().c_str()) + HashName();
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
/*****************************************************************************/
QString Avatar::HashName()
{
    return QString(QCryptographicHash::hash(mFilePath.toLocal8Bit(),QCryptographicHash::Md5).toHex()) + ".png";
}
//=============================================================================
// End of file Avatar.cpp
//=============================================================================
