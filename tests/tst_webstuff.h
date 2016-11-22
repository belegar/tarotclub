#ifndef TST_WEBSTUFF_H
#define TST_WEBSTUFF_H

#include <QString>
#include <QtTest>
#include <QCoreApplication>

class TstWebStuff : public QObject
{
    Q_OBJECT

public:
    TstWebStuff();

private Q_SLOTS:
    void Http();
    void WebSocketUpgrade();

private:

};

#endif // TST_WEBSTUFF_H
