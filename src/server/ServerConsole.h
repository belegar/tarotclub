/*=============================================================================
 * TarotClub - ServerConsole.h
 *=============================================================================
 * Console avec commandes permettant de contrôler le serveur de jeu
 *=============================================================================
 * TarotClub ( http://www.tarotclub.fr ) - This file is part of TarotClub
 * Copyright (C) 2003-2999 - Anthony Rabine
 * anthony@tarotclub.fr
 *
 * This file must be used under the terms of the CeCILL.
 * This source file is licensed as described in the file COPYING, which
 * you should have received as part of this distribution.  The terms
 * are also available at
 * http://www.cecill.info/licences/Licence_CeCILL_V2-en.txt
 *
 *=============================================================================
 */

#ifndef _SERVERCONSOLE_H
#define _SERVERCONSOLE_H

#include <QThread>
#include "../defines.h"
#include <iostream>
using namespace std;

/*****************************************************************************/
class ServerConsole;
typedef void (ServerConsole::*console_function)(const QStringList &args);

struct Command
{
    QString cmd;
    QString help;
    console_function func;
};

/*****************************************************************************/
class ServerConsole : public QObject
{
    Q_OBJECT
public:
    ServerConsole();

    void AddCommand(const QString &cmd, const QString &help, console_function func);
    void ParseCommand(QString &line);
    void Initialize();

    // All supported commands by the server
    void CmdExit(const QStringList &args);
    void CmdHelp(const QStringList &args);
    void CmdStart(const QStringList &args);
    void CmdStop(const QStringList &args);

signals:
    void Exit();
    void TextReceived(QString &message);

public slots:
    void Run();
    void Text();

private:
    QSocketNotifier *notifier;
    QList<Command> list;
};

#endif // _SERVERCONSOLE_H

//=============================================================================
// End of file ServerConsole.h
//=============================================================================
