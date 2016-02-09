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

#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

// Game includes
#include "Common.h"
#include "TarotWidget.h"
#include "TextBox.h"
#include "AboutWindow.h"
#include "OptionsWindow.h"
#include "EditorWindow.h"
#include "ScoresDock.h"
#include "InfosDock.h"
#include "ChatDock.h"
#include "DebugDock.h"
#include "LobbyDock.h"
#include "ui_QuickJoin.h"
#include "DealsWindow.h"
#include "ScoreCalculatorWindow.h"
#include "MiniBrowser.h"
#include "QCanvas.h"
#include "Environment.h"

// Qt includes
#include <QMainWindow>
#include <QMenu>
#include <QAction>
#include <QMdiArea>

/*****************************************************************************/
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);

    void Initialize();

private slots:
    // Events from MainWindow
    void slotDealEditor();
    void slotNewNumberedDeal();
    void slotNewCustomDeal();
    void slotQuickJoinNetworkGame();
    void slotShowOptions();
    void slotLaunchHelp();
    void slotAboutToQuit();
    void slotDisplayDeals();

    // Events from TarotWidget
    void slotNewGameEvent();
    void slotPlayersListEvent();
    void slotShowCardEvent(Place p, std::string cardName);
    void slotStartDealEvent();
    void slotWaitTrickEvent(Place winner);
    void slotMessageEvent(QString message);
    void slotEndOfDeal();
    void slotNewDealEvent();
    void slotTableQuitEvent(std::uint32_t tableId);
    void slotTableJoinEvent(std::uint32_t tableId);
    void slotClientError(QString error, bool quitServer);
    void DisconnectedFromServer();
    void slotEnteredLobby();

    // Events from LobbyWindow
    void slotConnectToLobby(QString server, std::uint16_t port);
    void slotJoinTable(std::uint32_t tableId);
    void slotQuitTable(std::uint32_t tableId);
    void slotDisconnectFromLobby();
    void slotLobbyPlayersList();

    // Canvas
    void slotRun();
    void slotReportScriptError(const QJSValue &error);

private:
    void SetupDialogs();
    void SetupMenus();
    void SetupDocks();
    void SetupCanvas2D();
    void RunScript(const QString &fileName);

    QMdiArea *mdiArea;
    QMdiSubWindow *subWindow;
    QMdiSubWindow *mdiEditor;
    QMdiSubWindow *mdiScoreCalc;

    ClientConfig    mClientConfig;
    ServerConfig    mServerConfig;
    TournamentConfig mTournamentConfig;

    // Menus
    QMenu *gameMenu;
    QMenu *paramsMenu;
    QMenu *mStudioMenu;
    QMenu *helpMenu;

    // Menu Jeu
    //----local
    QAction *newQuickGameAct;
    QAction *newTournamentAct;
    QAction *newNumberedDealAct;
    QAction *newCustomDealAct;

    //----network
    QAction *netHostGameAct;
    QAction *onlineGameAct;
    QAction *netQuickJoinAct;

    // Menu Windows
    QAction *optionsAct;

    //---- TarotStudio
    QAction *newAutoPlayAct;
    QAction *dealEditorAct;
    QAction *dealsAct;
    QAction *scoreCalcAct;

    TarotWidget *tarotWidget;

    // Experimental Canvas2D back-end
    QMdiSubWindow *mCanvasMdiSubWindow;
    test::MiniBrowser *mView;

    Environment *mEnv;
    QContext2DCanvas *mCanvas;
    QPushButton *mRunScriptButton;

    // Modal windows
    AboutWindow *about;
    OptionsWindow *optionsWindow;
    EditorWindow *editorWindow;
    QDialog *quickJoinWindow;
    Ui::quickJoin uiQuickJoin;
    DealsWindow *dealsWindow;
    ScoreCalculatorWindow *scoreCalcWindow;

    // Dock windows
    ScoresDock *scoresDock;
    InfosDock *infosDock;
    ChatDock *chatDock;
    DebugDock *debugDock;
    LobbyDock *mLobbyDock;

};

#endif // MAIN_WINDOW_H

//=============================================================================
// End of file MainWindow.h
//=============================================================================
