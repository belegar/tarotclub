/*=============================================================================
 * TarotClub - DebugDock.h
 *=============================================================================
 * General purpose dock window
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

#ifndef _DEBUG_DOCK_H
#define _DEBUG_DOCK_H

// Includes Qt
#include <QDockWidget>
#include <QLineEdit>
#include <QTextBrowser>
#include <QPushButton>

// Includes locales
#include "../defines.h"
#include "../Identity.h"

/*****************************************************************************/
class DebugDock : public QDockWidget
{
    Q_OBJECT

private:
    QTextBrowser *logWindow;
    QPushButton  *clearButton;
    QPushButton  *saveButton;

    static DebugDock *instance;

public:
    DebugDock( QWidget *parent );

    static DebugDock *getInstance() { return instance; }

protected:
    void closeEvent( QCloseEvent * e );

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
