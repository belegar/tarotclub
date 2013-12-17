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

#ifndef _DEBUG_DOCK_H
#define _DEBUG_DOCK_H

// Includes Qt
#include <QDockWidget>
#include <QLineEdit>
#include <QTextBrowser>
#include <QPushButton>

// Includes locales
#include "defines.h"
#include "Identity.h"
#include "Log.h"

/*****************************************************************************/
class DebugDock : public QDockWidget, public Observer<std::string>
{
    Q_OBJECT

private:
    QTextBrowser *logWindow;
    QPushButton  *clearButton;
    QPushButton  *saveButton;

    static DebugDock *instance;

public:
    DebugDock(QWidget *parent);

    static DebugDock *getInstance()
    {
        return instance;
    }

    void Update(const std::string &info) { message(QString(info.data())); }

    void Initialize() { Log::RegisterListener(*this); }

protected:
    void closeEvent(QCloseEvent *e);

public:
    void message(const QString &message);

public slots:
    void save();
    void clear();
};

#endif // _DEBUG_DOCK_H

//=============================================================================
// Fin du fichier DebugDock.h
//=============================================================================
