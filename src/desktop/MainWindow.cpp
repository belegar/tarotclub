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

// Game includes
#include "MainWindow.h"
#include "ui_NumberedDealUI.h"
#include "Util.h"
#include "System.h"
#include "Defines.h"
#include "JsonReader.h"
#include "JsonWriter.h"
//#include "MiniBrowser.h"
#include "CanvasElement.h"

// Qt includes
#include <QStatusBar>
#include <QMenuBar>
#include <QDateTime>
#include <QDir>
#include <QDesktopServices>

#include <iostream>


/*****************************************************************************/
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    mdiArea = new QMdiArea;
    mdiArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    mdiArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    setCentralWidget(mdiArea);

    SetupDialogs();
    SetupDocks();
    SetupMenus();
    SetupCanvas2D();

    setWindowTitle(QString(TAROT_TITLE.c_str()) + " " + QString(TAROT_VERSION.c_str()));

    // TarotWidget events
    connect(tarotWidget, &TarotWidget::sigTablePlayersList, this, &MainWindow::slotPlayersListEvent, Qt::QueuedConnection);
    connect(tarotWidget, &TarotWidget::sigShowCard, this, &MainWindow::slotShowCardEvent, Qt::QueuedConnection);
    connect(tarotWidget, &TarotWidget::sigStartDeal, this, &MainWindow::slotStartDealEvent, Qt::QueuedConnection);
    connect(tarotWidget, &TarotWidget::sigWaitTrick, this, &MainWindow::slotWaitTrickEvent, Qt::QueuedConnection);
    connect(tarotWidget, &TarotWidget::sigTableMessage, this, &MainWindow::slotMessageEvent, Qt::QueuedConnection);
    connect(tarotWidget, &TarotWidget::sigAddScore, this, &MainWindow::slotEndOfDeal, Qt::QueuedConnection);
    connect(tarotWidget, &TarotWidget::sigNewGame, this, &MainWindow::slotNewGameEvent, Qt::QueuedConnection);
    connect(tarotWidget, &TarotWidget::sigNewDeal, this, &MainWindow::slotNewDealEvent, Qt::QueuedConnection);
    connect(tarotWidget, &TarotWidget::sigLobbyMessage, mLobbyDock, &LobbyDock::slotChatMessage, Qt::QueuedConnection);
    connect(tarotWidget, &TarotWidget::sigLobbyPlayersList, this, &MainWindow::slotLobbyPlayersList, Qt::QueuedConnection);
    connect(tarotWidget, &TarotWidget::sigTableQuitEvent, this, &MainWindow::slotTableQuitEvent, Qt::QueuedConnection);
    connect(tarotWidget, &TarotWidget::sigTableJoinEvent, this, &MainWindow::slotTableJoinEvent, Qt::QueuedConnection);
    connect(tarotWidget, &TarotWidget::sigClientError, this, &MainWindow::slotClientError, Qt::QueuedConnection);
    connect(tarotWidget, &TarotWidget::sigEnteredLobby, this, &MainWindow::slotEnteredLobby, Qt::QueuedConnection);

    // Game menu specific to desktop version
    connect(newQuickGameAct, &QAction::triggered, tarotWidget, &TarotWidget::slotNewQuickGame);
    connect(newTournamentAct, &QAction::triggered, tarotWidget, &TarotWidget::slotNewTournamentGame);

    connect(newNumberedDealAct, &QAction::triggered, this, &MainWindow::slotNewNumberedDeal);
    connect(newCustomDealAct, &QAction::triggered, this, &MainWindow::slotNewCustomDeal);

    connect(netQuickJoinAct, &QAction::triggered, this, &MainWindow::slotQuickJoinNetworkGame);
    connect(netHostGameAct, &QAction::triggered, tarotWidget, &TarotWidget::slotCreateHostedGame);

    connect(optionsAct, &QAction::triggered, this, &MainWindow::slotShowOptions);
    connect(newAutoPlayAct, &QAction::triggered, tarotWidget, &TarotWidget::slotNewAutoPlay);
    connect(dealsAct, &QAction::triggered, this, &MainWindow::slotDisplayDeals);

    // Table chat
    connect(chatDock, &ChatDock::sigEmitMessage, tarotWidget, &TarotWidget::slotSendChatMessage);

    // Lobby
    connect(mLobbyDock, &LobbyDock::sigConnect, this, &MainWindow::slotConnectToLobby);
    connect(mLobbyDock, &LobbyDock::sigDisconnect, this, &MainWindow::slotDisconnectFromLobby);
    connect(mLobbyDock, &LobbyDock::sigJoinTable, this, &MainWindow::slotJoinTable);
    connect(mLobbyDock, &LobbyDock::sigQuitTable, this, &MainWindow::slotQuitTable);
    connect(mLobbyDock, &LobbyDock::sigEmitMessage, tarotWidget, &TarotWidget::slotSendLobbyMessage);

    // TarotStudio windows
    connect(editorWindow, &EditorWindow::sigHide, mdiEditor, &QMdiSubWindow::hide);
    connect(scoreCalcWindow, &ScoreCalculatorWindow::sigHide, mdiScoreCalc, &MainWindow::hide);

    // Exit catching to terminate the game properly
    connect(qApp, &QApplication::aboutToQuit, this, &MainWindow::slotAboutToQuit);
}
/*****************************************************************************/
void MainWindow::Initialize()
{
    QString fontPath = ":/fonts/kanzlei.ttf";
    int fontId = QFontDatabase::addApplicationFont(fontPath);
    if (fontId == -1)
    {
        TLogError("Cannot add custom font on your system");
    }

    dealsWindow->Initialize();

    mClientConfig.Load(System::HomePath() + ClientConfig::DEFAULT_CLIENT_CONFIG_FILE);
    mServerConfig.Load(System::HomePath() + ServerConfig::DEFAULT_SERVER_CONFIG_FILE);
    mTournamentConfig.Load(System::HomePath() + TournamentConfig::DEFAULT_FILE_NAME);

    tarotWidget->Initialize();
    tarotWidget->ApplyOptions(mClientConfig.GetOptions(),
                              mServerConfig.GetOptions(),
                              mTournamentConfig.GetOptions());

    mLobbyDock->SetServersList(mClientConfig.GetOptions().serverList);
    debugDock->Initialize();

    // Load previously saved settings
    QSettings settings("TarotCorp.", "TarotClub");
    restoreGeometry(settings.value("mainWindowGeometry").toByteArray());
    restoreState(settings.value("mainWindowState").toByteArray());
    subWindow->restoreGeometry(settings.value("tarotwidgetGeometry").toByteArray());
}
/*****************************************************************************/
void MainWindow::SetupCanvas2D()
{
    /*
    mView = new test::MiniBrowser(this);

    mView->setSource(QUrl::fromLocalFile("U:/tarotclub/assets/canvasjs/canvas2.qml"));
    mView->show();

  //  mView->Initialize();
    mCanvasMdiSubWindow = mdiArea->addSubWindow(mView, Qt::WindowMinMaxButtonsHint);
    mCanvasMdiSubWindow->show();

    */
/*
    test::MiniBrowser *view = new test::MiniBrowser;
    view->setSource(QUrl::fromLocalFile("U:/tarotclub/assets/canvasjs/canvas.qml"));
    view->show();

    */

    mEnv = new Environment(this);
 //   QObject::connect(mEnv, SIGNAL(scriptError(QJSValue)),
 //                    this, SLOT(slotReportScriptError(QJSValue)));

    QSize canvasSize(600, 500);
    CanvasElement *canvas = mEnv->createCanvas("canvas");
    canvas->setSize(canvasSize.width(), canvasSize.height());

    mCanvasWidget = new CanvasWidget(mEnv, this);
    mCanvasWidget->SetSize(canvasSize.width(), canvasSize.height());
    QObject::connect(mEnv, SIGNAL(sigContentsChanged(QImage)), mCanvasWidget, SLOT(contentsChanged(QImage)));

    mRunScriptButton = new QPushButton("RunMe", mCanvasWidget);
    connect (mRunScriptButton, &QPushButton::clicked, this, &MainWindow::slotRun);

    mCanvasMdiSubWindow = mdiArea->addSubWindow(mCanvasWidget, Qt::WindowMinMaxButtonsHint);
    mCanvasMdiSubWindow->setAttribute(Qt::WA_DeleteOnClose);
    mCanvasMdiSubWindow->show();
}
/*****************************************************************************/
void MainWindow::slotReportScriptError(const QJSValue &error)
{
    std::cout <<  tr("Line %0: %1")
                  .arg(error.property("lineNumber").toInt())
                  .arg(error.toString()).toStdString() << std::endl;
}
/*****************************************************************************/
void MainWindow::slotRun()
{
    mEnv->reset();

    RunScript("U:/tarotclub/assets/canvasjs/engine.js");
    RunScript("U:/tarotclub/assets/canvasjs/class.js");
    RunScript("U:/tarotclub/assets/canvasjs/widgets.js");
    RunScript("U:/tarotclub/assets/canvasjs/main.js");

     QJSValue ret = mEnv->evaluate("init();");
     if (ret.isError())
     {
        slotReportScriptError(ret);
     }
}
/*****************************************************************************/
void MainWindow::RunScript(const QString &fileName)
{
    QFile file(fileName);
    file.open(QIODevice::ReadOnly);
    QString contents = file.readAll();
    file.close();

    QJSValue ret = mEnv->evaluate(contents, fileName);

    if (ret.isError())
    {
       slotReportScriptError(ret);
    }
}
/*****************************************************************************/
void MainWindow::slotAboutToQuit()
{
    tarotWidget->slotCleanBeforeExit();

    // Save QMainWindow settings
    QSettings settings("TarotCorp.", "TarotClub");
    settings.setValue("mainWindowGeometry", saveGeometry());
    settings.setValue("mainWindowState", saveState());
    settings.setValue("tarotwidgetGeometry", subWindow->saveGeometry());
}
/*****************************************************************************/
void MainWindow::slotDisplayDeals()
{
    dealsWindow->RefreshDeals();
    dealsWindow->show();
}
/*****************************************************************************/
void MainWindow::slotNewNumberedDeal()
{
    QDialog *widget = new QDialog;
    Ui::NumberedDeal ui;
    ui.setupUi(widget);

    if (widget->exec() == QDialog::Accepted)
    {
        tarotWidget->NewNumberedDeal(ui.dealNumber->value());
    }
}
/*****************************************************************************/
void MainWindow::slotNewCustomDeal()
{
    QString fileName = QFileDialog::getOpenFileName(this);

    if (fileName.size() != 0)
    {
        tarotWidget->NewCustomDeal(fileName.toStdString());
    }
}
/*****************************************************************************/
void MainWindow::slotConnectToLobby(QString server, std::uint16_t port)
{
    tarotWidget->LaunchRemoteGame(server.toStdString(), port);
}
/*****************************************************************************/
void MainWindow::slotDisconnectFromLobby()
{
    tarotWidget->Disconnect();
}
/*****************************************************************************/
void MainWindow::slotJoinTable(std::uint32_t tableId)
{
    // join the game table
    if (tableId > 0U)
    {
        tarotWidget->JoinTable(tableId);
    }
}
/*****************************************************************************/
void MainWindow::slotQuitTable(std::uint32_t tableId)
{
    // quit the game
    if (tableId > 0U)
    {
        tarotWidget->QuitTable(tableId);
    }
}
/*****************************************************************************/
void MainWindow::slotTableQuitEvent(std::uint32_t tableId)
{
    if (tarotWidget->GetConnectionType() != TarotWidget::LOCAL)
    {
        mLobbyDock->SetTableStatus(tableId, false);
    }
    infosDock->Clear();
    scoresDock->Clear();
}
/*****************************************************************************/
void MainWindow::slotTableJoinEvent(std::uint32_t tableId)
{
    if (tarotWidget->GetConnectionType() != TarotWidget::LOCAL)
    {
        mLobbyDock->SetTableStatus(tableId, true);
    }
}
/*****************************************************************************/
void MainWindow::slotClientError(QString error, bool quitServer)
{
    mLobbyDock->SystemMessage(error);
    if (quitServer)
    {
        DisconnectedFromServer();
    }
}
/*****************************************************************************/
void MainWindow::DisconnectedFromServer()
{
    mLobbyDock->DisconnectedFromServer();
    infosDock->Clear();
    scoresDock->Clear();
}
/*****************************************************************************/
void MainWindow::slotEnteredLobby()
{
    if (tarotWidget->GetConnectionType() != TarotWidget::LOCAL)
    {
        mLobbyDock->SetTables(tarotWidget->GetTableList());
    }
}
/*****************************************************************************/
void MainWindow::slotLobbyPlayersList()
{
    if (tarotWidget->GetConnectionType() != TarotWidget::LOCAL)
    {
        mLobbyDock->SetPlayersNames(tarotWidget->GetLobbyPlayersList());
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
    optionsWindow->SetTournamentOptions(mTournamentConfig.GetOptions());
    optionsWindow->Refresh();

    if (optionsWindow->exec() == QDialog::Accepted)
    {
        mClientConfig.SetOptions(optionsWindow->GetClientOptions());
        mServerConfig.SetOptions(optionsWindow->GetServerOptions());
        mTournamentConfig.SetOptions(optionsWindow->GetTournamentOptions());

        mClientConfig.Save(System::HomePath() + ClientConfig::DEFAULT_CLIENT_CONFIG_FILE);
        mServerConfig.Save(System::HomePath() + ServerConfig::DEFAULT_SERVER_CONFIG_FILE);
        mTournamentConfig.Save(System::HomePath() + TournamentConfig::DEFAULT_FILE_NAME);

        tarotWidget->ApplyOptions(mClientConfig.GetOptions(),
                                  mServerConfig.GetOptions(),
                                  mTournamentConfig.GetOptions());
        mLobbyDock->SetServersList(mClientConfig.GetOptions().serverList);
    }
}
/*****************************************************************************/
void MainWindow::SetupDialogs()
{
    // TarotWidget window
    tarotWidget = new TarotWidget(this);
    tarotWidget->setObjectName("TarotWidget"); // for position & size saving
    tarotWidget->show();
    subWindow = mdiArea->addSubWindow(tarotWidget, Qt::WindowMinMaxButtonsHint);
    subWindow->setAttribute(Qt::WA_DeleteOnClose);

    // Deals Window
    dealsWindow = new DealsWindow(this);
    dealsWindow->hide();

    // About Window
    about = new AboutWindow(this);
    about->hide();

    // Options
    optionsWindow = new OptionsWindow(this);
    optionsWindow->setAttribute(Qt::WA_ShowModal, true);
    optionsWindow->hide();

    // Deal editor
    editorWindow = new EditorWindow(this);
    mdiEditor = mdiArea->addSubWindow(editorWindow, Qt::WindowMinMaxButtonsHint);
    mdiEditor->setAttribute(Qt::WA_DeleteOnClose);
    mdiEditor->hide();

    // Score calculcator window
    scoreCalcWindow = new ScoreCalculatorWindow(this);
    mdiScoreCalc = mdiArea->addSubWindow(scoreCalcWindow, Qt::WindowMinMaxButtonsHint);
    mdiScoreCalc->setAttribute(Qt::WA_DeleteOnClose);
    mdiScoreCalc->hide();

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
    debugDock->setObjectName("DebugDock"); // for dock saving
    addDockWidget(Qt::BottomDockWidgetArea, debugDock);
    debugDock->hide();

    // Debug, for scripts and Qt error redirection
    mLobbyDock = new LobbyDock(this);
    mLobbyDock->setObjectName("LobbyDock"); // for dock saving
    addDockWidget(Qt::BottomDockWidgetArea, mLobbyDock);
    mLobbyDock->hide();

    tabifyDockWidget(debugDock, mLobbyDock);

    // ----------  Right Docks ------------------------

    // Scores
    scoresDock = new ScoresDock(this);
    scoresDock->setObjectName("ScoresDock"); // for dock saving
    addDockWidget(Qt::RightDockWidgetArea, scoresDock);

    // Information
    infosDock = new InfosDock(this);
    infosDock->setObjectName("InfosDock"); // for dock saving
    addDockWidget(Qt::RightDockWidgetArea, infosDock);

    // Chat
    chatDock = new ChatDock(this);
    chatDock->setObjectName("ChatDock"); // for dock saving
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
    onlineGameAct = new QAction(tr("&Play online"), this);
    onlineGameAct->setShortcut(tr("Ctrl+P"));
    onlineGameAct->setStatusTip(tr("Join a TarotClub game server"));

    netHostGameAct = new QAction(tr("Create a network game serve&r"), this);
    netHostGameAct->setShortcut(tr("Ctrl+R"));
    netHostGameAct->setStatusTip(tr("Create a network game and invite friends to join the game"));

    netQuickJoinAct = new QAction(tr("&Quick join a network game"), this);
    netQuickJoinAct->setShortcut(tr("Ctrl+U"));
    netQuickJoinAct->setStatusTip(tr("Join a game server created using TarotClub"));

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

    gameMenu->addSeparator();
    gameMenu->addAction(mLobbyDock->toggleViewAction());
    gameMenu->addAction(netHostGameAct);
    gameMenu->addAction(netQuickJoinAct);

    gameMenu->addSeparator();
    gameMenu->addAction(exitAct);

    //---------------
    // Windows menu
    //---------------
    optionsAct = new QAction(tr("&Options"), this);
    optionsAct->setShortcut(tr("Ctrl+O"));
    optionsAct->setStatusTip(tr("Game options"));

    dealsAct = new QAction(tr("&Deals viewer"), this);
    dealsAct->setShortcut(tr("Ctrl+W"));
    dealsAct->setStatusTip(tr("Display previously played deals"));

    paramsMenu = menuBar()->addMenu(tr("Windows"));
    paramsMenu->addAction(optionsAct);
    paramsMenu->addSeparator();

    // Dock windows
    paramsMenu->addAction(scoresDock->toggleViewAction());
    paramsMenu->addAction(infosDock->toggleViewAction());
    paramsMenu->addAction(chatDock->toggleViewAction());

    // ----------------
    // TarotStudio menu
    // ----------------
    dealEditorAct = new QAction(tr("Deal e&ditor"), this);
    dealEditorAct->setShortcut(tr("Ctrl+D"));
    dealEditorAct->setStatusTip(tr("Create a pre-defined deal by choosing the cards of each player"));
    connect(dealEditorAct, &QAction::triggered, mdiEditor, &MainWindow::show);

    scoreCalcAct = new QAction(tr("Score calculator"), this);
    scoreCalcAct->setShortcut(tr("Ctrl+S"));
    scoreCalcAct->setStatusTip(tr("Tool to easily calculate a score of a deal"));
    connect(scoreCalcAct, &QAction::triggered, mdiScoreCalc, &QMdiSubWindow::show);

    mStudioMenu = menuBar()->addMenu(tr("TarotStudio"));

    mStudioMenu->addAction(dealEditorAct);
    mStudioMenu->addAction(dealsAct);
    mStudioMenu->addAction(scoreCalcAct);

    mStudioMenu->addSeparator();
    newAutoPlayAct = new QAction(tr("New auto& play"), this);
    newAutoPlayAct->setShortcut(tr("Ctrl+Y"));
    newAutoPlayAct->setStatusTip(tr("Auto play, to train your AI script!"));
    mStudioMenu->addAction(newAutoPlayAct);
    mStudioMenu->addAction(debugDock->toggleViewAction());

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
    QMap<Place, Identity> players = tarotWidget->GetTablePlayersList();
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
    infosDock->AddRound(p, cardName);
}
/*****************************************************************************/
void MainWindow::slotWaitTrickEvent(Place winner)
{
    infosDock->SelectWinner(winner);
}
/*****************************************************************************/
void MainWindow::slotEndOfDeal()
{
    if (tarotWidget->GetGameMode() == Tarot::Game::cSimpleTournament)
    {
        scoresDock->SetNewScore(tarotWidget->GetPoints(), tarotWidget->GetBid());
    }

    // Generate end of deal log
    std::string fileName = System::GamePath() + System::ProjectName() + "_" + Util::CurrentDateTime("%Y-%m-%d.%H%M%S") + ".json";
    JsonValue json;

    if (JsonReader::ParseString(json, tarotWidget->GetResult()))
    {
        if (!JsonWriter::SaveToFile(json.GetObject(), fileName))
        {
            TLogError("Cannot save JSON result file.");
        }
    }
    else
    {
        TLogError("Cannot parse JSON result file.");
    }
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
void MainWindow::slotMessageEvent(QString message)
{
    chatDock->message(message);
}
/*****************************************************************************/
void MainWindow::slotStartDealEvent()
{
    Tarot::Bid bid = tarotWidget->GetBid();
    Tarot::Distribution shuffle = tarotWidget->GetShuffle();
    QMap<Place, Identity> players = tarotWidget->GetTablePlayersList();

    infosDock->Clear();
    infosDock->SetContract(bid.contract);

    QString name = "ERROR";

    QMap<Place, Identity>::const_iterator iter = players.find(bid.taker);
    if (iter != players.end())
    {
        name = QString(iter.value().nickname.data());
    }
    infosDock->SetTaker(name, bid.taker);

    if (shuffle.mType != Tarot::Distribution::CUSTOM_DEAL)
    {
        infosDock->SetDealNumber(shuffle.mSeed);
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
    mdiEditor->show();
}


//=============================================================================
// End of file MainWindow.cpp
//=============================================================================
