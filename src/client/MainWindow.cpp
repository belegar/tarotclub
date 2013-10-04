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

#include "MainWindow.h"
#include <QStatusBar>
#include <QMenuBar>
#include <QDateTime>
#include <QDir>
#include "../Tools.h"

/*****************************************************************************/
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    SetupDialogs();
    SetupDocks();
    SetupMenus();

    setWindowTitle(QString(TAROT_TITRE) + " " + QString(TAROT_VERSION));
    tapis = new Canvas(this);
    setCentralWidget(tapis);
    tapis->show();
}
/*****************************************************************************/
void MainWindow::SetupDialogs()
{
    // About Window
    about = new AboutWindow(this);
    about->hide();

    // Help Window
    helpWindow = new HelpWindow(this);
    helpWindow->hide();

    // Scores
    resultWindow = new ResultWindow(this);
    resultWindow->setAttribute(Qt::WA_ShowModal, true);
    resultWindow->hide();

    // Options
    optionsWindow = new OptionsWindow(this);
    optionsWindow->setAttribute(Qt::WA_ShowModal, true);
    optionsWindow->hide();

    // Join a new network game wizard
    joinWizard = new JoinWizard(this);
    joinWizard->hide();

    // Deal editor
    editorWindow = new EditorWindow(this);
    editorWindow->setAttribute(Qt::WA_ShowModal, true);
    editorWindow->hide();

    // Quick join a network game
    quickJoinWindow = new QDialog(this);
    uiQuickJoin.setupUi(quickJoinWindow);
    quickJoinWindow->hide();
}
/*****************************************************************************/
/**
 * @brief MainWindow::SetupDocks
 */
void MainWindow::SetupDocks()
{
    // ----------  Bottom Docks ------------------------

    // Debug, for scripts and Qt error redirection
    debugDock = new DebugDock(this);
    addDockWidget(Qt::BottomDockWidgetArea, debugDock);
    debugDock->hide();

    // ----------  Right Docks ------------------------

    // Scores
    scoresDock = new ScoresDock(this);
    addDockWidget(Qt::RightDockWidgetArea, scoresDock);

    // Information
    infosDock = new InfosDock(this);
    addDockWidget(Qt::RightDockWidgetArea, infosDock);

    // Chat
    chatDock = new ChatDock(this);
    addDockWidget(Qt::RightDockWidgetArea, chatDock);

    // Right area belongs to right docks
    QMainWindow::setCorner(Qt::TopRightCorner, Qt::RightDockWidgetArea);
    QMainWindow::setCorner(Qt::BottomRightCorner, Qt::RightDockWidgetArea);
    tabifyDockWidget(chatDock, infosDock);

    scoresDock->show();
    infosDock->show();
    chatDock->show();
}
/*****************************************************************************/
/**
 * @brief MainWindow::SetupMenus
 */
void MainWindow::SetupMenus()
{
    //----------
    // Menu Jeu
    //----------
    //----- Local
    newQuickGameAct = new QAction(tr("&New quick game"), this);
    newQuickGameAct->setShortcut(tr("Ctrl+N"));
    newQuickGameAct->setStatusTip(tr("Start a one player game against computer"));

    newTournamentAct = new QAction(tr("New &tournament"), this);
    newTournamentAct->setShortcut(tr("Ctrl+T"));
    newTournamentAct->setStatusTip(tr("Play five deals against the computer"));

    newNumberedDealAct = new QAction(tr("New nu&mbered deal"), this);
    newNumberedDealAct->setShortcut(tr("Ctrl+M"));
    newNumberedDealAct->setStatusTip(tr("Deal cards with a predefined deal number"));

    newCustomDealAct = new QAction(tr("New manua&l deal"), this);
    newCustomDealAct->setShortcut(tr("Ctrl+L"));
    newCustomDealAct->setStatusTip(tr("Deal cards with a deal created with the editor"));

    //----- Network
    netGameServerAct = new QAction(tr("Create a new network game (serve&r)"), this);
    netGameServerAct->setShortcut(tr("Ctrl+R"));
    netGameServerAct->setStatusTip(tr("Create a network game and invite friends to join the game"));

    netQuickJoinAct = new QAction(tr("&Quick join a network game"), this);
    netQuickJoinAct->setShortcut(tr("Ctrl+U"));
    netQuickJoinAct->setStatusTip(tr("Join a game server created using TarotClub"));

    netGameClientAct = new QAction(tr("&Join a dedicated game server"), this);
    netGameClientAct->setShortcut(tr("Ctrl+J"));
    netGameClientAct->setStatusTip(tr("Join a game server created using TarotClubServer"));

    //----- Misc
    previousTrickAct = new QAction(tr("&Previous trick"), this);
    previousTrickAct->setShortcut(tr("Ctrl+P"));
    previousTrickAct->setStatusTip(tr("Show the previous trick played"));
    previousTrickAct->setEnabled(false);

    QAction *exitAct = new QAction(tr("&Quit"), this);
    exitAct->setShortcut(tr("Ctrl+Q"));
    exitAct->setStatusTip(tr("Quit the game"));
    connect(exitAct, SIGNAL(triggered()), qApp, SLOT(quit()));

    // Add action to the main menu
    gameMenu = menuBar()->addMenu(tr("Game"));
    gameMenu->addAction(newQuickGameAct);
    gameMenu->addAction(newTournamentAct);
    gameMenu->addAction(newNumberedDealAct);
    gameMenu->addAction(newCustomDealAct);
    gameMenu->addSeparator();
    gameMenu->addAction(netGameServerAct);
    gameMenu->addAction(netQuickJoinAct);
    gameMenu->addAction(netGameClientAct);
    gameMenu->addSeparator();
    gameMenu->addAction(previousTrickAct);
    gameMenu->addSeparator();
    gameMenu->addAction(exitAct);

    //---------------
    // Parameter menu
    //---------------
    optionsAct = new QAction(tr("&Options"), this);
    optionsAct->setShortcut(tr("Ctrl+O"));
    optionsAct->setStatusTip(tr("Game options"));

    dealEditorAct = new QAction(tr("Deal e&ditor"), this);
    dealEditorAct->setShortcut(tr("Ctrl+D"));
    dealEditorAct->setStatusTip(tr("Create a pre-defined deal by choosing the cards of each player"));
    connect(dealEditorAct, SIGNAL(triggered()), this, SLOT(slotDealEditor()));

    paramsMenu = menuBar()->addMenu(tr("Parameters"));
    paramsMenu->addAction(optionsAct);
    paramsMenu->addAction(dealEditorAct);
    paramsMenu->addSeparator();

    // Dock windows
    paramsMenu->addAction(scoresDock->toggleViewAction());
    paramsMenu->addAction(infosDock->toggleViewAction());
    paramsMenu->addAction(chatDock->toggleViewAction());
    paramsMenu->addAction(debugDock->toggleViewAction());
    paramsMenu->addAction(debugDock->toggleViewAction());

    //-----------
    // Menu Help
    //-----------
    QAction *aboutAct = new QAction(tr("&About..."), this);
    aboutAct->setShortcut(tr("Ctrl+A"));
    aboutAct->setStatusTip(tr("About TarotClub"));
    connect(aboutAct, SIGNAL(triggered()), about, SLOT(show()));

    QAction *helpAct = new QAction(tr("&Help and manual"), this);
    helpAct->setShortcut(tr("Ctrl+H"));
    helpAct->setStatusTip(tr("Show game help and Tarot rules"));
    connect(helpAct, SIGNAL(triggered()), helpWindow, SLOT(show()));

    // Add actions to the Help menu
    helpMenu = menuBar()->addMenu(tr("Help"));
    helpMenu->addAction(aboutAct);
    helpMenu->addAction(helpAct);

}
/*****************************************************************************/
void MainWindow::slotDealEditor()
{
    editorWindow->Initialize();
    editorWindow->exec();
}

//=============================================================================
// End of file MainWindow.cpp
//=============================================================================
