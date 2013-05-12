/*=============================================================================
 * TarotClub - ChatDock.cpp
 *=============================================================================
 * Online chat dock window
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

// Includes Qt
#include <QDockWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QSplitter>

// Includes locales
#include "ChatDock.h"

/*****************************************************************************/
ChatDock::ChatDock( QWidget *parent )
    : QDockWidget( trUtf8("Discussion"), parent )
{
   chatWindow = new QTextBrowser();
   textLine = new QLineEdit();
   options = new QPushButton(trUtf8("Commandes"));
   playersList = new QListWidget();

   QHBoxLayout *lay1 = new QHBoxLayout();
   lay1->addWidget(textLine);
   lay1->addWidget(options);

   QSplitter *lay2 = new QSplitter();
   lay2->addWidget(chatWindow);
   lay2->addWidget(playersList);

   QVBoxLayout *principal = new QVBoxLayout();
   principal->addLayout(lay1);
   principal->addWidget(lay2);

   QFrame *fr = new QFrame(this);
   fr->setLayout(principal);
   setWidget(fr);

   connect( textLine,SIGNAL( returnPressed() ), this, SLOT(slotReturnPressed()) );
}
/*****************************************************************************/
void ChatDock::closeEvent( QCloseEvent * e )
{
    Q_UNUSED(e);
    emit sgnlClose();
}
/*****************************************************************************/
void ChatDock::clear()
{
   playersList->clear();
   chatWindow->clear();
}
/*****************************************************************************/
void ChatDock::clearPlayersList()
{
   chatWindow->clear();
}
/*****************************************************************************/
void ChatDock::message(const QString &message)
{
   chatWindow->append( message );
}
/*****************************************************************************/
void ChatDock::slotReturnPressed()
{
   QString message = textLine->text();
   if( message.length() == 0 ) {
      return;
   }
   emit sgnlEmitMessage(message);
   textLine->clear();
}
/*****************************************************************************/
void ChatDock::setPlayersList( QList<Identity> &idents )
{
   QString server;
   QStringList nicks;

   for( int i=0; i<idents.size(); i++ ) {
      nicks += idents.at(i).name;
   }
   playersList->clear();
   // On marque le premier joueur comme étant le serveur
   playersList->addItems( nicks );
   QListWidgetItem *it = playersList->item(0);
   server = it->text();
   server.prepend( '@' );
   it->setText( server );
}

//=============================================================================
// Fin du fichier ChatDock.cpp
//=============================================================================
