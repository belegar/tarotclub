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

// Qt includes
#include <QStatusBar>
#include <QMenuBar>
#include <QDateTime>
#include <QDir>
#include <QDesktopServices>

// Game includes
#include "MainWindow.h"
#include "ui_NumberedDealUI.h"
#include "ui_WinUI.h"
#include "Util.h"

/*****************************************************************************/
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    SetupDialogs();
    SetupDocks();
    SetupMenus();

    setWindowTitle(QString(TAROT_TITRE) + " " + QString(TAROT_VERSION));
    tarotWidget = new TarotWidget(this);
    tarotWidget->show();
    setCentralWidget(tarotWidget);

    // Game menu to TarotWidget
    connect(newQuickGameAct, &QAction::triggered, tarotWidget, &TarotWidget::slotNewQuickGame);
    connect(newTournamentAct, &QAction::triggered, tarotWidget, &TarotWidget::slotNewTournamentGame);
    connect(netGameServerAct, &QAction::triggered, tarotWidget, &TarotWidget::slotCreateNetworkGame);

    // TarotWidget events
    connect (tarotWidget, &TarotWidget::sigPlayersList, this, &MainWindow::slotPlayersListEvent, Qt::QueuedConnection);
    connect (tarotWidget, &TarotWidget::sigShowCard, this, &MainWindow::slotShowCardEvent, Qt::QueuedConnection);
    connect (tarotWidget, &TarotWidget::sigStartDeal, this, &MainWindow::slotStartDealEvent, Qt::QueuedConnection);
    connect (tarotWidget, &TarotWidget::sigWaitTrick, this, &MainWindow::slotWaitTrickEvent, Qt::QueuedConnection);
    connect (tarotWidget, &TarotWidget::sigMessage, this, &MainWindow::slotMessageEvent, Qt::QueuedConnection);
    connect (tarotWidget, &TarotWidget::sigAddScore, this, &MainWindow::slotEndOfDeal, Qt::QueuedConnection);
    connect (tarotWidget, &TarotWidget::sigNewGame, this, &MainWindow::slotNewGameEvent, Qt::QueuedConnection);
    connect (tarotWidget, &TarotWidget::sigNewDeal, this, &MainWindow::slotNewDealEvent, Qt::QueuedConnection);

    // Game menu specific to desktop version
    connect(newNumberedDealAct, &QAction::triggered, this, &MainWindow::slotNewNumberedDeal);
    connect(newCustomDealAct, &QAction::triggered, this, &MainWindow::slotNewCustomDeal);
    connect(netGameClientAct, &QAction::triggered, this, &MainWindow::slotJoinNetworkGame);
    connect(netQuickJoinAct, &QAction::triggered, this, &MainWindow::slotQuickJoinNetworkGame);
    connect(optionsAct, &QAction::triggered, this, &MainWindow::slotShowOptions);
    connect(newAutoPlayAct, &QAction::triggered, this, &MainWindow::slotNewAutoPlay);

    // Network chat
    connect(chatDock, &ChatDock::sigEmitMessage, tarotWidget, &TarotWidget::slotSendChatMessage);

    // Exit catching to terminate the game properly
    connect(qApp, &QApplication::aboutToQuit, tarotWidget, &TarotWidget::slotCleanBeforeExit);
}
/*****************************************************************************/
void MainWindow::slotNewNumberedDeal()
{
    QDialog *widget = new QDialog;
    Ui::NumberedDeal ui;
    ui.setupUi(widget);

    if (widget->exec() == QDialog::Accepted)
    {
        Tarot::Shuffle sh;
        sh.type = Tarot::Shuffle::NUMBERED_DEAL;
        sh.seed = ui.dealNumber->value();

        tarotWidget->LaunchLocalGame(Tarot::ONE_DEAL, sh, false);
    }
}
/*****************************************************************************/
void MainWindow::slotNewAutoPlay()
{
    Tarot::Shuffle sh;
    sh.type = Tarot::Shuffle::RANDOM_DEAL;
    tarotWidget->LaunchLocalGame(Tarot::ONE_DEAL, sh, true);
}
/*****************************************************************************/
void MainWindow::slotNewCustomDeal()
{
    QString fileName = QFileDialog::getOpenFileName(this);

    if (fileName.size() != 0)
    {
        Tarot::Shuffle sh;
        sh.type = Tarot::Shuffle::CUSTOM_DEAL;
        sh.file = fileName.toStdString();

        tarotWidget->LaunchLocalGame(Tarot::ONE_DEAL, sh, false);
    }
}
/*****************************************************************************/
void MainWindow::slotJoinNetworkGame()
{
    lobbyWindow->Initialize();
    if (lobbyWindow->exec() == QDialog::Accepted)
    {
        // connect to table
        LobbyWindow::Connection cn = lobbyWindow->GetTableConnection();

        if (cn.isValid)
        {
            tarotWidget->LaunchRemoteGame(cn.ip.toStdString(), cn.port);
        }
    }
}
/*****************************************************************************/
void MainWindow::slotQuickJoinNetworkGame()
{
    if (quickJoinWindow->exec() == QDialog::Accepted)
    {
        QString ip = uiQuickJoin.ipAddress->text();
        quint16 port = uiQuickJoin.tcpPort->value();

        tarotWidget->LaunchRemoteGame(ip.toStdString(), port);
    }
}
/*****************************************************************************/
void MainWindow::slotShowOptions()
{
    optionsWindow->SetClientOptions(mClientConfig.GetOptions());
    optionsWindow->SetServerOptions(mServerConfig.GetOptions());

    if (optionsWindow->exec() == QDialog::Accepted)
    {
        mClientConfig.SetOptions(optionsWindow->GetClientOptions());
        mServerConfig.SetOptions(optionsWindow->GetServerOptions());

        mClientConfig.Save();
        mServerConfig.Save();
        tarotWidget->ApplyOptions(mClientConfig.GetOptions(),
                                  mServerConfig.GetOptions());
    }
}
/*****************************************************************************/
void MainWindow::Initialize()
{
    mClientConfig.Load();
    mServerConfig.Load();

    tarotWidget->Initialize();
    tarotWidget->ApplyOptions(mClientConfig.GetOptions(),
                              mServerConfig.GetOptions());

    debugDock->Initialize();
}
/*****************************************************************************/
void MainWindow::SetupDialogs()
{
    // About Window
    about = new AboutWindow(this);
    about->hide();

    // Options
    optionsWindow = new OptionsWindow(this);
    optionsWindow->setAttribute(Qt::WA_ShowModal, true);
    optionsWindow->hide();

    // Connect to a dedicated server
    lobbyWindow = new LobbyWindow(this);
    lobbyWindow->hide();

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

    newAutoPlayAct = new QAction(tr("New auto& play"), this);
    newAutoPlayAct->setShortcut(tr("Ctrl+P"));
    newAutoPlayAct->setStatusTip(tr("Auto play, to train your AI script!"));

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

    QAction *exitAct = new QAction(tr("&Quit"), this);
    exitAct->setShortcut(tr("Ctrl+Q"));
    exitAct->setStatusTip(tr("Quit the game"));
    connect(exitAct, &QAction::triggered, qApp, &QCoreApplication::quit);

    // Add action to the main menu
    gameMenu = menuBar()->addMenu(tr("Game"));
    gameMenu->addAction(newQuickGameAct);
    gameMenu->addAction(newTournamentAct);
    gameMenu->addAction(newNumberedDealAct);
    gameMenu->addAction(newCustomDealAct);
    gameMenu->addAction(newAutoPlayAct);
    gameMenu->addSeparator();
    gameMenu->addAction(netGameServerAct);
    gameMenu->addAction(netQuickJoinAct);
    gameMenu->addAction(netGameClientAct);
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
    connect(dealEditorAct, &QAction::triggered, this, &MainWindow::slotDealEditor);

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
    connect(aboutAct, &QAction::triggered, about, &AboutWindow::show);

    QAction *helpAct = new QAction(tr("&Help and manual"), this);
    helpAct->setShortcut(tr("Ctrl+H"));
    helpAct->setStatusTip(tr("Show game help and Tarot rules"));
    connect(helpAct, &QAction::triggered, this, &MainWindow::slotLaunchHelp);

    // Add actions to the Help menu
    helpMenu = menuBar()->addMenu(tr("Help"));
    helpMenu->addAction(aboutAct);
    helpMenu->addAction(helpAct);
}
/*****************************************************************************/
void MainWindow::slotLaunchHelp()
{
    QString path = QString(Util::ExecutablePath().c_str()) + "/doc/index.html";
    QDesktopServices::openUrl(QUrl("file:///" + path, QUrl::TolerantMode));
}
/*****************************************************************************/
void MainWindow::slotPlayersListEvent()
{
    QMap<Place, Identity> players = tarotWidget->GetPlayersList();
    scoresDock->SetPlayers(players);
    infosDock->SetPlayers(players);
}
/*****************************************************************************/
void MainWindow::slotNewGameEvent()
{
    // reset scores
    scoresDock->Clear();
}
/*****************************************************************************/
void MainWindow::slotShowCardEvent(Place p, std::string cardName)
{
    infosDock->AddRound(mTrickCounter, p, QString(cardName.data()));

    if (mFirstPlayer)
    {
        mFirstPlayer = false;
        infosDock->SelectFirstPlayer(mTrickCounter, p);
    }
}
/*****************************************************************************/
void MainWindow::slotWaitTrickEvent(Place winner)
{
    infosDock->SelectWinner(mTrickCounter, winner);
    mTrickCounter++;
    mFirstPlayer = true;
}
/*****************************************************************************/
void MainWindow::slotEndOfDeal()
{
    scoresDock->SetNewScore(tarotWidget->GetDeal());
}
/*****************************************************************************/
void MainWindow::slotNewDealEvent()
{
    Deck deck = tarotWidget->GetDeck();
    Deck::Statistics stats;
    stats.Reset();
    deck.AnalyzeSuits(stats);
    deck.AnalyzeTrumps(stats);
    infosDock->PrintStats(stats);
}
/*****************************************************************************/
void MainWindow::slotMessageEvent(std::string message)
{
    chatDock->message(message);
}
/*****************************************************************************/
void MainWindow::slotStartDealEvent()
{
    mTrickCounter = 0U;
    mFirstPlayer = true;
    Tarot::Bid bid = tarotWidget->GetBid();
    Tarot::Shuffle shuffle = tarotWidget->GetShuffle();
    QMap<Place, Identity> players = tarotWidget->GetPlayersList();

    infosDock->Clear();
    infosDock->SetContract(bid.contract);

    QString name = "ERROR";

    if (players.contains(bid.taker))
    {
        name = QString(players.value(bid.taker).name.data());
    }
    infosDock->SetTaker(name, bid.taker);

    if (shuffle.type != Tarot::Shuffle::CUSTOM_DEAL)
    {
        infosDock->SetDealNumber(shuffle.seed);
    }
    else
    {
        // Numbered deal
        infosDock->SetDealNumber(0U);
    }

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
