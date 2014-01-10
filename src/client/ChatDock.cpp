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
ChatDock::ChatDock(QWidget *parent)
    : QDockWidget(trUtf8("Chat"), parent)
{
    chatWindow = new QTextBrowser();
    textLine = new QLineEdit();


    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(chatWindow);
    layout->addWidget(textLine);

    QFrame *fr = new QFrame(this);
    fr->setLayout(layout);
    setWidget(fr);

    connect(textLine, SIGNAL(returnPressed()), this, SLOT(slotReturnPressed()));
}
/*****************************************************************************/
void ChatDock::clear()
{
    chatWindow->clear();
}
/*****************************************************************************/
void ChatDock::message(const std::string &message)
{
    chatWindow->append(message.c_str());
}
/*****************************************************************************/
void ChatDock::slotReturnPressed()
{
    QString message = textLine->text();
    if (message.length() == 0)
    {
        return;
    }
    emit sigEmitMessage(message);
    textLine->clear();
}

//=============================================================================
// End of file ChatDock.cpp
//=============================================================================
