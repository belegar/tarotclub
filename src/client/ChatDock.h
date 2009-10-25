/*=============================================================================
 * TarotClub - ChatDock.h
 *=============================================================================
 * Online chat dock window
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

#ifndef CHATDOCK_H
#define CHATDOCK_H

// Includes Qt
#include <QDockWidget>
#include <QLineEdit>
#include <QTextBrowser>
#include <QPushButton>
#include <QListWidget>

// Includes locales
#include "../defines.h"

/*****************************************************************************/
class ChatDock : public QDockWidget
{
   Q_OBJECT

private:
   QTextBrowser    *chatWindow;
   QLineEdit       *textLine;
   QPushButton     *options;
   QListWidget     *playersList;

public:
   ChatDock( QWidget *parent );

   void clear();
   void setPlayersList( int nombre, Identity *idents );
   void clearPlayersList();

protected:
   void closeEvent( QCloseEvent * e );

public slots:
   void slotReturnPressed();
   void message(const QString &message);

signals:
   void sgnlEmitMessage(const QString&);
   void sgnlClose();
};

#endif // CHATDOCK_H

//=============================================================================
// Fin du fichier ChatDock.h
//=============================================================================
