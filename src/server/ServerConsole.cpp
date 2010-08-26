/*=============================================================================
 * TarotClub - ServerConsole.cpp
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

#include <QCoreApplication>
#include <QStringList>
#include "ServerConsole.h"
#include "../TarotEngine.h"

/*****************************************************************************/
ServerConsole::ServerConsole( QObject *eventObject ) : QThread()
{
   obj = eventObject;

   addCommand("exit","Terminate any pending game and exit program.",&ServerConsole::c_exit);
   addCommand("quit","Terminate any pending game and exit program.",&ServerConsole::c_exit);
   addCommand("help","Show this help.",&ServerConsole::c_help);
   addCommand("start","Start a new game with default values.",&ServerConsole::c_start);
   addCommand("stop","Kick clients and close server game.",&ServerConsole::c_stop);
}
/*****************************************************************************/
void ServerConsole::addCommand( QString cmd, QString help, console_function func )
{
   Command_t new_cmd;

   new_cmd.cmd = cmd;
   new_cmd.help = help;
   new_cmd.func = func;
   list.append(new_cmd);
}
/*****************************************************************************/
void ServerConsole::parseCommand()
{
   QString cmdLine(line);
   QStringList args;
   QString cmd;
   bool command_ok = false;

   args = cmdLine.split(" ",QString::SkipEmptyParts);
   if( args.size() == 0 ) {
      return;
   }
   cmd = args.at(0);
   if( cmd.isEmpty() ) {
      return;
   }
   args.removeAt(0);

   for(int i = 0; i < list.size(); ++i) {
      if( list.at(i).cmd == cmd ) {
         (this->*list.at(i).func)(args);
         command_ok = true;
      }
   }

   if( command_ok == false ) {
      cout << endl << "Unknown command.";
   }
/**
 * Code de test pour imprimer les arguments
   cout << endl;
   for( int i=0; i<arguments.size(); ++i ) {
      cout << arguments.at(i).toLocal8Bit().constData() << endl;
   }
*/

}
/*****************************************************************************/
void ServerConsole::run()
{
   cout << "> ";

   for(;;) {
      cin.getline(line,sizeof(line));
      parseCommand();
      cout << endl << "> ";
   }
}
/*****************************************************************************/
void ServerConsole::c_exit(QStringList args)
{
   EvExitGame *ev = new EvExitGame();
   QCoreApplication::postEvent( obj, ev );
}
/*****************************************************************************/
void ServerConsole::c_help(QStringList args)
{
   cout << "TarotClub server " TAROT_VERSION << endl;
   cout << "List of commands :" << endl;
   for(int i = 0; i < list.size(); ++i) {
      cout << "\t" << list.at(i).cmd.toLocal8Bit().data() << "\t" << list.at(i).help.toLocal8Bit().data() << endl;
   }
}
/*****************************************************************************/
void ServerConsole::c_start(QStringList args)
{
   EvStartGame *ev = new EvStartGame();
   QCoreApplication::postEvent( obj, ev );
}
/*****************************************************************************/
void ServerConsole::c_stop(QStringList args)
{
   EvStopGame *ev = new EvStopGame();
   QCoreApplication::postEvent( obj, ev );
}

//=============================================================================
// End of file ServerConsole.cpp
//=============================================================================
