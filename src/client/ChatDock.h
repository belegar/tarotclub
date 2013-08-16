/*=============================================================================
 * TarotClub - ChatDock.h
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
#include "../Identity.h"

/*****************************************************************************/
class ChatDock : public QDockWidget
{
    Q_OBJECT

private:
    QTextBrowser    *chatWindow;
    QLineEdit       *textLine;

public:
    ChatDock(QWidget *parent);
    void clear();

signals:
    void sigEmitMessage(const QString &);

public slots:
    void slotReturnPressed();
    void message(const QString &message);

};

#endif // CHATDOCK_H

//=============================================================================
// End of file ChatDock.h
//=============================================================================
