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

/*****************************************************************************/
Avatar::Avatar(const QString &fileName)
    : mHttp(false)
    , mValid(false)
{
    if (fileName.size() > 0)
    {
        if (fileName[0] != ':')
        {
            QNetworkReply *reply = mNetworkManager.get(QNetworkRequest(QUrl(fileName)));

            QEventLoop loop;
            connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
            loop.exec();

            const QByteArray data(reply->readAll());
            if (data.size() > 0)
            {
                mValid = mPixmap.loadFromData(data);
                mHttp = true;
            }
            reply->deleteLater();
        }
        else
        {
            mValid = mPixmap.load(fileName);
        }
    }

}
/*****************************************************************************/
bool Avatar::IsHttp()
{
    return mHttp;
}
/*****************************************************************************/
bool Avatar::IsValid()
{
    return mValid;
}
/*****************************************************************************/
QPixmap Avatar::GetPixmap()
{
    return mPixmap;
}

//=============================================================================
// End of file Avatar.cpp
//=============================================================================
