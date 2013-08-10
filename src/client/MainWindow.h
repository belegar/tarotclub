/*=============================================================================
 * TarotClub - MainWindow.h
 *=============================================================================
 * Main window of the game. Contains all the widgets.
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

#ifndef _MAINWINDOW_H
#define _MAINWINDOW_H

// Includes Qt
#include <QMainWindow>
#include <QMenu>
#include <QAction>

// Includes locales
#include "../defines.h"
#include "Canvas.h"
#include "TextBox.h"
#include "AboutWindow.h"
#include "ResultWindow.h"
#include "OptionsWindow.h"
#include "EditorWindow.h"
#include "HelpWindow.h"
#include "ScoresDock.h"
#include "InfosDock.h"
#include "ChatDock.h"
#include "DebugDock.h"
#include "JoinWizard.h"


/*****************************************************************************/
class MainWindow : public QMainWindow
{
    Q_OBJECT

protected:

    // Menus
    QMenu *gameMenu;
    QMenu *paramsMenu;
    QMenu *helpMenu;

    // Menu Jeu
    //----local
    QAction *newQuickGameAct;
    QAction *newTournamentAct;
    QAction *newNumberedDealAct;
    QAction *newCustomDealAct;
    //----network
    QAction *netGameServerAct;
    QAction *netGameClientAct;
    //----misc
    QAction *pliPrecAct;

    // Menu Paramètres
    QAction *dealEditorAct;
    QAction *optionsAct;

    Canvas *tapis;      // QCanvasView

    // Modal windows
    AboutWindow *about;
    ResultWindow *resultWindow;
    OptionsWindow *optionsWindow;
    EditorWindow *editorWindow;
    JoinWizard *joinWizard;
    HelpWindow *helpWindow;

    // Dock windows
    ScoresDock *scoresDock;
    InfosDock *infosDock;
    ChatDock *chatDock;
    DebugDock *debugDock;

public:
    MainWindow(QWidget *parent = 0);

public slots:
    void slotDealEditor();

private:
    void SetupDialogs();
    void SetupMenus();
    void SetupDocks();
};

#endif // _MAINWINDOW_H

//=============================================================================
// End of file MainWindow.h
//=============================================================================
