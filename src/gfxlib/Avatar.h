#ifndef AVATAR_H
#define AVATAR_H

#include <QtGui/QPixmap>
#include <QtCore/QUrl>
#include <QtNetwork>

class Avatar : public QObject
{
    Q_OBJECT

public:
    Avatar(const QString& fileName);

    bool IsHttp();
    bool IsValid();
    QPixmap GetPixmap();

private:
    QPixmap mPixmap;
    QNetworkAccessManager mNetworkManager;
    bool mHttp;
    bool mValid;

};

#endif // AVATAR_H
