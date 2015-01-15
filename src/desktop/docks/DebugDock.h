/*=============================================================================
 * TarotClub - DebugDock.h
 *=============================================================================
 * General purpose dock window
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

#ifndef DEBUG_DOCK_H
#define DEBUG_DOCK_H

// Includes Qt
#include <QDockWidget>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QPushButton>

// Includes locales
#include "Common.h"
#include "Identity.h"
#include "Log.h"

/*****************************************************************************/
class DebugDock : public QDockWidget, public Observer<std::string>
{
    Q_OBJECT

private:
    QPlainTextEdit *logWindow;
    QPushButton  *clearButton;
    QPushButton  *saveButton;

public:
    DebugDock(QWidget *parent);

    void Update(const std::string &info)
    {
        emit sigMessage(QString(info.data()));
    }
    void Initialize()
    {
        Log::RegisterListener(*this);
    }

protected:
    void closeEvent(QCloseEvent *e);

signals:
    void sigMessage(QString);


public slots:
    void slotMessage(QString message);
    void save();
    void clear();
};

#endif // DEBUG_DOCK_H

//=============================================================================
// Fin du fichier DebugDock.h
//=============================================================================
