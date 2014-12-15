#include "Avatar.h"

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

bool Avatar::IsHttp()
{
    return mHttp;
}

bool Avatar::IsValid()
{
    return mValid;
}

QPixmap Avatar::GetPixmap()
{
    return mPixmap;
}

