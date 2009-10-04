/*=============================================================================
 * Tarot Club - ServerConsole.h
 *=============================================================================
 * Console avec commandes permettant de contrôler le serveur de jeu
 *=============================================================================
 * Tarot Club est un jeu de Tarot français
 * Copyright (C) 2003-2005  Anthony Rabine
 * anthony@ooso.org
 * http://tarotclub.ooso.org
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
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

typedef struct {
   QString cmd;
   QString help;
   console_function func;
   
} Command_t;

enum {
   MsgStartGame = QEvent::User+1,
   MsgRandomDeal,
   MsgStopGame,
   MsgExitGame
};
/*****************************************************************************/
class EvStartGame : public QEvent
{
public:
   EvStartGame() : QEvent( (QEvent::Type)MsgStartGame ) {}
};
/*****************************************************************************/
class EvStopGame : public QEvent
{
public:
   EvStopGame() : QEvent( (QEvent::Type)MsgStopGame ) {}
};
/*****************************************************************************/
class EvExitGame : public QEvent
{
public:
   EvExitGame() : QEvent( (QEvent::Type)MsgExitGame ) {}
};
/*****************************************************************************/
class ServerConsole : public QThread
{
private:
   char line[256];
   QList<Command_t> list;
   QObject *obj;

public:
   ServerConsole( QObject *o );

   void addCommand( QString cmd, QString help, console_function func );
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
