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
typedef void (ServerConsole::*console_function)(QStringList args);

typedef struct
{
    QString cmd;
    QString help;
    console_function func;

} Command_t;

/*****************************************************************************/
class ServerConsole : public QThread
{
private:
    char line[256];
    QList<Command_t> list;
    QObject *obj;

public:
    ServerConsole(QObject *o);

    void addCommand(QString cmd, QString help, console_function func);
    void parseCommand();
    void run();

    // Ci-dessous la liste des commandes supportées, toutes doivent commencer par le préfixe c_
    void c_exit(QStringList args);
    void c_help(QStringList args);
    void c_start(QStringList args);
    void c_stop(QStringList args);
};

#endif // _SERVERCONSOLE_H

//=============================================================================
// End of file ServerConsole.h
//=============================================================================
