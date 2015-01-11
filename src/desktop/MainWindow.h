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

// Qt includes
#include <QMainWindow>
#include <QMenu>
#include <QAction>

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
#include "LobbyWindow.h"
#include "ui_QuickJoin.h"
#include "DealsWindow.h"

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
    void slotJoinNetworkGame();
    void slotQuickJoinNetworkGame();
    void slotShowOptions();
    void slotLaunchHelp();
    void slotNewAutoPlay();
    void slotAboutToQuit();
    void slotDisplayDeals();

    // Events from TarotWidget
    void slotNewGameEvent();
    void slotPlayersListEvent();
    void slotShowCardEvent(Place p, std::string cardName);
    void slotStartDealEvent();
    void slotWaitTrickEvent(Place winner);
    void slotMessageEvent(std::string message);
    void slotEndOfDeal();
    void slotNewDealEvent();
    void slotTableQuitEvent(std::uint32_t tableId);
    void slotTableJoinEvent(std::uint32_t tableId);
    void slotClientError(std::uint32_t errorId);
    void slotDisconnectedFromServer();
    void slotEnteredLobby();

    // Events from LobbyWindow
    void slotConnectToLobby(QString server, std::uint16_t port);
    void slotJoinTable(std::uint32_t tableId);
    void slotQuitTable(std::uint32_t tableId);
    void slotDisconnectFromLobby();
    void slotLobbyPlayersList();

private:
    void SetupDialogs();
    void SetupMenus();
    void SetupDocks();

    std::uint8_t mTrickCounter;
    bool mFirstPlayer;

    ClientConfig    mClientConfig;
    ServerConfig    mServerConfig;

    // Menus
    QMenu *gameMenu;
    QMenu *netMenu;
    QMenu *paramsMenu;
    QMenu *mDevMenu;
    QMenu *helpMenu;

    // Menu Jeu
    //----local
    QAction *newQuickGameAct;
    QAction *newTournamentAct;
    QAction *newNumberedDealAct;
    QAction *newCustomDealAct;

    //----network
//    QAction *netGameServerAct;
    QAction *onlineGameAct;
//    QAction *netQuickJoinAct;

    // Menu Paramètres
    QAction *dealEditorAct;
    QAction *optionsAct;
    QAction *dealsAct;

    // Menu développeur
    QAction *newAutoPlayAct;

    TarotWidget *tarotWidget;

    // Modal windows
    AboutWindow *about;
    OptionsWindow *optionsWindow;
    EditorWindow *editorWindow;
    QDialog *quickJoinWindow;
    Ui::quickJoin uiQuickJoin;
    DealsWindow *dealsWindow;

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
