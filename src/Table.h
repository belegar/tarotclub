#ifndef TABLE_H
#define TABLE_H

#include <QThread>
#include "Server.h"
#include "TarotEngine.h"

enum
{
    MsgStartGame = QEvent::User + 1,
    MsgRandomDeal,
    MsgStopGame,
    MsgExitGame
};

/*****************************************************************************/
class EvStartGame : public QEvent
{
public:
    EvStartGame() : QEvent((QEvent::Type)MsgStartGame) {}
};
/*****************************************************************************/
class EvStopGame : public QEvent
{
public:
    EvStopGame() : QEvent((QEvent::Type)MsgStopGame) {}
};
/*****************************************************************************/
class EvExitGame : public QEvent
{
public:
    EvExitGame() : QEvent((QEvent::Type)MsgExitGame) {}
};
/*****************************************************************************/
class Table : public QThread
{
    Q_OBJECT


public:
    Table();

    void StartGame();
    void SetMaximumPlayers(int n);

    void run();

protected:
    void customEvent(QEvent *e);


private:

};

#endif // TABLE_H
