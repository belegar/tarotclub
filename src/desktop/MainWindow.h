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

// Qt includes
#include <QMainWindow>
#include <QMenu>
#include <QAction>

// Game includes
#include "Common.h"
#include "TarotWidget.h"
#include "TextBox.h"
#include "AboutWindow.h"
#include "ResultWindow.h"
#include "OptionsWindow.h"
#include "EditorWindow.h"
#include "ScoresDock.h"
#include "InfosDock.h"
#include "ChatDock.h"
#include "DebugDock.h"
#include "LobbyWindow.h"
#include "ui_QuickJoin.h"

/*****************************************************************************/
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);

    void Initialize();

private slots:
    void slotDealEditor();
    void slotNewNumberedDeal();
    void slotNewCustomDeal();
    void slotJoinNetworkGame();
    void slotQuickJoinNetworkGame();
    void slotShowOptions();
    void slotLaunchHelp();

private:
    void SetupDialogs();
    void SetupMenus();
    void SetupDocks();

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
    QAction *netQuickJoinAct;

    // Menu Paramètres
    QAction *dealEditorAct;
    QAction *optionsAct;

    TarotWidget *tarotWidget;

    // Modal windows
    AboutWindow *about;
    ResultWindow *resultWindow;
    OptionsWindow *optionsWindow;
    EditorWindow *editorWindow;
    LobbyWindow *lobbyWindow;
    QDialog *quickJoinWindow;
    Ui::quickJoin uiQuickJoin;

    // Dock windows
    ScoresDock *scoresDock;
    InfosDock *infosDock;
    ChatDock *chatDock;
    DebugDock *debugDock;
};

#endif // _MAINWINDOW_H

//=============================================================================
// End of file MainWindow.h
//=============================================================================
