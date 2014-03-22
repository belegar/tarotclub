/*=============================================================================
 * TarotClub - TarotClub.cpp
 *=============================================================================
 * Graphical TarotClub client class, contains an embedded server.
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

#include <QStatusBar>
#include <QInputDialog>
#include <QMessageBox>
#include "TarotClub.h"
#include "TarotDeck.h"
#include "ui_NumberedDealUI.h"
#include "ui_WinUI.h"
#include "Common.h"
#include "Log.h"

/*****************************************************************************/
TarotClub::TarotClub()
  : MainWindow()
  , mClient(*this)
  , mConnectionType(NO_CONNECTION)
{
    qRegisterMetaType<Place>("Place");
    qRegisterMetaType<Contract>("Contract");
    qRegisterMetaType<Game::Shuffle>("Game::Shuffle");
    qRegisterMetaType<std::string>("std::string");

    // Board click events
    connect(tapis, &Canvas::sigViewportClicked, this, &TarotClub::slotClickTapis);
    connect(tapis, &Canvas::sigClickCard, this, &TarotClub::slotClickCard);
    connect(tapis, &Canvas::sigMoveCursor, this, &TarotClub::slotMoveCursor);
    connect(tapis, &Canvas::sigContract, this, &TarotClub::slotSetEnchere);
    connect(tapis, &Canvas::sigAcceptDiscard, this, &TarotClub::slotAcceptDiscard);
    connect(tapis, &Canvas::sigAcceptHandle, this, &TarotClub::slotAcceptHandle);

    // Game Menu
    connect(newQuickGameAct, SIGNAL(triggered()), this, SLOT(slotNewQuickGame()));
    connect(newTournamentAct, SIGNAL(triggered()), this, SLOT(slotNewTournamentGame()));
    connect(newNumberedDealAct, SIGNAL(triggered()), this, SLOT(slotNewNumberedDeal()));
    connect(newCustomDealAct, SIGNAL(triggered()), this, SLOT(slotNewCustomDeal()));
    connect(netGameClientAct, SIGNAL(triggered()), this, SLOT(slotJoinNetworkGame()));
    connect(netGameServerAct, SIGNAL(triggered()), this, SLOT(slotCreateNetworkGame()));
    connect(netQuickJoinAct, SIGNAL(triggered()), this, SLOT(slotQuickJoinNetworkGame()));

    // Client events. This connection list allow to call Qt GUI from another thread (Client class thread)
    connect(this, &TarotClub::sigReceiveCards, this, &TarotClub::slotReceiveCards, Qt::QueuedConnection);
    connect(this, &TarotClub::sigAssignedPlace, this, &TarotClub::slotAssignedPlace, Qt::QueuedConnection);
    connect(this, &TarotClub::sigPlayersList, this, &TarotClub::slotPlayersList, Qt::QueuedConnection);
    connect(this, &TarotClub::sigMessage, this, &TarotClub::slotMessage, Qt::QueuedConnection);
    connect(this, &TarotClub::sigSelectPlayer, this, &TarotClub::slotSelectPlayer, Qt::QueuedConnection);
    connect(this, &TarotClub::sigRequestBid, this, &TarotClub::slotRequestBid, Qt::QueuedConnection);
    connect(this, &TarotClub::sigShowBid, this, &TarotClub::slotShowBid, Qt::QueuedConnection);
    connect(this, &TarotClub::sigShowDog, this, &TarotClub::slotShowDog, Qt::QueuedConnection);
    connect(this, &TarotClub::sigStartDeal, this, &TarotClub::slotStartDeal, Qt::QueuedConnection);
    connect(this, &TarotClub::sigPlayCard, this, &TarotClub::slotPlayCard, Qt::QueuedConnection);
    connect(this, &TarotClub::sigBuildDiscard, this, &TarotClub::slotBuildDiscard, Qt::QueuedConnection);
    connect(this, &TarotClub::sigDealAgain, this, &TarotClub::slotDealAgain, Qt::QueuedConnection);
    connect(this, &TarotClub::sigEndOfDeal, this, &TarotClub::slotEndOfDeal, Qt::QueuedConnection);
    connect(this, &TarotClub::sigEndOfGame, this, &TarotClub::slotEndOfGame, Qt::QueuedConnection);
    connect(this, &TarotClub::sigShowCard, this, &TarotClub::slotShowCard, Qt::QueuedConnection);
    connect(this, &TarotClub::sigShowHandle, this, &TarotClub::slotShowHandle, Qt::QueuedConnection);
    connect(this, &TarotClub::sigWaitTrick, this, &TarotClub::slotWaitTrick, Qt::QueuedConnection);

    // Network chat
    connect(chatDock, &ChatDock::sigEmitMessage, this, &TarotClub::slotSendChatMessage);

    // Parameters Menu
    connect(optionsAct, SIGNAL(triggered()), this, SLOT(slotShowOptions()));

    // Exit catching to terminate the game properly
    connect(qApp, SIGNAL(aboutToQuit()), this, SLOT(slotQuitTarotClub()));
}
/*****************************************************************************/
/**
 * @brief One time game initialization
 */
void TarotClub::Initialize()
{
    debugDock->Initialize();

    // Check user TarotClub directories and create them if necessary
    if (!QDir(QString(Config::HomePath.data())).exists())
    {
        QDir().mkdir(QString(Config::HomePath.data()));
    }
    if (!QDir(QString(Config::GamePath.data())).exists())
    {
        QDir().mkdir(QString(Config::GamePath.data()));
    }
    if (!QDir(QString(Config::LogPath.data())).exists())
    {
        QDir().mkdir(QString(Config::LogPath.data()));
    }

    clientConfig.Load();

    TarotDeck::Initialize();
    if (tapis->Initialize(clientConfig.GetOptions()) == false)
    {
        TLogError("Cannot load SVG images, exiting...");
    }
    ApplyOptions();

    table.Initialize();
    mClient.Initialize();
    deal.Initialize();

    statusBar()->showMessage(trUtf8("Welcome to TarotClub!"));;
}
/*****************************************************************************/
/**
 * This method allows a proper cleanup before closing the application
 */
void TarotClub::slotQuitTarotClub()
{
    // Close ourself
    mClient.Close();

    table.Stop();   
}
/*****************************************************************************/
void TarotClub::slotNewNumberedDeal()
{
    QDialog *widget = new QDialog;
    Ui::NumberedDeal ui;
    ui.setupUi(widget);

    if (widget->exec() == QDialog::Accepted)
    {
        Game::Shuffle sh;
        sh.type = Game::NUMBERED_DEAL;
        sh.seed = ui.dealNumber->value();

        LaunchLocalGame(Game::ONE_DEAL, sh);
    }
}
/*****************************************************************************/
void TarotClub::slotNewCustomDeal()
{
    QString fileName = QFileDialog::getOpenFileName(this);

    if (fileName.size() != 0)
    {
        Game::Shuffle sh;
        sh.type = Game::CUSTOM_DEAL;
        sh.file = fileName.toStdString();

        LaunchLocalGame(Game::ONE_DEAL, sh);
    }
}
/*****************************************************************************/
void TarotClub::slotNewTournamentGame()
{
    Game::Shuffle sh;
    sh.type = Game::RANDOM_DEAL;

    LaunchLocalGame(Game::TOURNAMENT, sh);
}
/*****************************************************************************/
void TarotClub::slotNewQuickGame()
{
    Game::Shuffle sh;
    sh.type = Game::RANDOM_DEAL;

    LaunchLocalGame(Game::ONE_DEAL, sh);
}
/*****************************************************************************/
void TarotClub::slotJoinNetworkGame()
{
    lobbyWindow->Initialize();
    if (lobbyWindow->exec() == QDialog::Accepted)
    {
        // connect to table
        LobbyWindow::Connection cn = lobbyWindow->GetTableConnection();

        if (cn.isValid)
        {
            LaunchRemoteGame(cn.ip.toStdString(), cn.port);
        }
    }
}
/*****************************************************************************/
void TarotClub::slotQuickJoinNetworkGame()
{
    if (quickJoinWindow->exec() == QDialog::Accepted)
    {
        QString ip = uiQuickJoin.ipAddress->text();
        quint16 port = uiQuickJoin.tcpPort->value();

        LaunchRemoteGame(ip.toStdString(), port);
    }
}
/*****************************************************************************/
void TarotClub::slotCreateNetworkGame()
{
    Game::Shuffle sh;
    sh.type = Game::RANDOM_DEAL;

    table.CreateGame(Game::ONE_DEAL, 4U, sh);
    // Connect us to the server
    mClient.ConnectToHost("127.0.0.1", DEFAULT_TCP_PORT);
}
/*****************************************************************************/
bool TarotClub::HasLocalConnection()
{
    if ((mConnectionType == LOCAL) &&
        (mClient.IsConnected() == true))
    {
        return true;
    }
    else
    {
        return false;
    }
}
/*****************************************************************************/
void TarotClub::LaunchRemoteGame(const std::string &ip, std::uint16_t port)
{
    // Close ourself from any previous connection
    mClient.Close();
    mClient.Initialize();

    InitScreen();

    // Connect us to the server
    mClient.ConnectToHost(ip, port);
    mConnectionType = REMOTE;
}
/*****************************************************************************/
void TarotClub::LaunchLocalGame(Game::Mode mode, const Game::Shuffle &sh)
{
    table.CreateGame(mode, 4U, sh);
    InitScreen();

    if (HasLocalConnection())
    {
        table.StartDeal();
    }
    else
    {
        mConnectionType = LOCAL;
        // Connect us to the server
        mClient.ConnectToHost("127.0.0.1", DEFAULT_TCP_PORT);
        // Connect the other players
        table.ConnectBots();
    }
}
/*****************************************************************************/
void TarotClub::InitScreen()
{
    // GUI initialization
    scoresDock->clear();
    infosDock->Clear();
    tapis->InitBoard();
    tapis->ResetCards();
    tapis->SetFilter(Canvas::BLOCK_ALL);
}
/*****************************************************************************/
void TarotClub::ApplyOptions()
{
    ClientOptions &options = clientConfig.GetOptions();
    mClient.SetMyIdentity(options.identity);

    table.LoadConfiguration();

    tapis->ShowAvatars(options.showAvatars);
    tapis->SetBackground(options.backgroundColor);
}
/*****************************************************************************/
void TarotClub::slotShowOptions()
{
    optionsWindow->SetClientOptions(clientConfig.GetOptions());
    optionsWindow->SetServerOptions(table.GetOptions());

    if (optionsWindow->exec() == QDialog::Accepted)
    {
        clientConfig.SetOptions(optionsWindow->GetClientOptions());
        clientConfig.Save();

        table.SaveConfiguration(optionsWindow->GetServerOptions());

        ApplyOptions();
    }
}
/*****************************************************************************/
void TarotClub::showVictoryWindow()
{

    /*
    QGraphicsTextItem *txt;

    QDialog *widget = new QDialog;
    Ui::WinUI ui;
    ui.setupUi(widget);

    QGraphicsScene *scene = new QGraphicsScene();
    QGraphicsSvgItem *victory = new QGraphicsSvgItem(":images/podium.svg");

    ui.tournamentGraph->setScene(scene);
    scene->addItem(victory);
    ui.tournamentGraph->centerOn(victory);
    */
    std::map<int, Place> podium = deal.GetPodium();

    QMessageBox::information(this, trUtf8("Tournament result"),
                             trUtf8("The winner of the tournament is ") + QString(players[podium[0]].name.data()),
                             QMessageBox::Ok);

    /*
    // add the three best players to the image
    txt = scene->addText(players[podium[0]].name);
    txt->setPos(20+150, 450);
    txt = scene->addText(players[podium[1]].name);
    txt->setPos(20, 450);
    txt = scene->addText(players[podium[2]].name);
    txt->setPos(20+300, 450);

    widget->exec();
    */
}
/*****************************************************************************/
void TarotClub::HideTrick()
{
    Card *c;
    GfxCard *gc;
    Deck &trick = mClient.GetCurrentTrick();

    for (Deck::ConstIterator i = trick.Begin(); i != trick.End(); ++i)
    {
        c = (*i);
        gc = tapis->GetGfxCard(TarotDeck::GetIndex(c->GetName()));
        gc->hide();
    }
}
/*****************************************************************************/
void TarotClub::slotSetEnchere(Contract cont)
{
    mClient.SendBid(cont, tapis->GetSlamOption());
    tapis->HideBidsChoice();
}
/*****************************************************************************/
void TarotClub::slotAcceptDiscard()
{
    // Clear canvas and forbid new actions
    tapis->DisplayDiscardMenu(false);
    tapis->SetFilter(Canvas::BLOCK_ALL);

    for (Deck::ConstIterator i = discard.Begin(); i != discard.End(); ++i)
    {
        Card *c = (*i);
        GfxCard *gc = tapis->GetGfxCard(TarotDeck::GetIndex(c->GetName()));
        gc->hide();
    }

    mClient.SetDiscard(discard);
    ShowSouthCards();
}
/*****************************************************************************/
void TarotClub::slotAcceptHandle()
{
    if (mClient.TestHandle() == false)
    {
        QMessageBox::information(this, trUtf8("Information"),
                                 trUtf8("Your handle is not valid.\n"
                                        "Showing the fool means that you have no any more trumps in your deck."));
        return;
    }

    tapis->DisplayHandleMenu(false);
    tapis->SetFilter(Canvas::CARDS);
    mClient.SendHandle();
    ShowSouthCards();
    mClient.GetGameInfo().sequence = Game::PLAY_TRICK;
    statusBar()->showMessage(trUtf8("Please, play a card, it's your turn!"));
}
/*****************************************************************************/
/**
 * @brief TarotClub::ShowSouthCards
 *
 * Show current player's cards
 *
 * @param pos = 0 for 18-cards in the hand, otherwise 1 (with cards from the chien)
 */
void TarotClub::ShowSouthCards()
{
    mClient.GetMyDeck().Sort(clientConfig.GetOptions().cardsOrder);
    tapis->DrawSouthCards(mClient.GetMyDeck());
}
/*****************************************************************************/
void TarotClub::slotClickTapis()
{
    if (mClient.GetGameInfo().sequence == Game::SHOW_DOG)
    {
        statusBar()->clearMessage();
        tapis->HidePopup();
        mClient.SendSyncDog(); // We have seen the dog, let's inform the server about that
    }
    else if (mClient.GetGameInfo().sequence == Game::SHOW_HANDLE)
    {
        statusBar()->clearMessage();
        tapis->HidePopup();
        mClient.SendSyncHandle(); // We have seen the handle, let's inform the server about that
    }
    else if (mClient.GetGameInfo().sequence == Game::SYNC_TRICK)
    {
        HideTrick();
        statusBar()->clearMessage();
        mClient.SendSyncTrick();
    }
}
/*****************************************************************************/
/**
 * @brief TarotClub::slotMoveCursor
 *
 * This function is called every time the mouse cursor moves
 *
 * @param gc
 */
void TarotClub::slotMoveCursor(GfxCard *gc)
{
    Card *c = tapis->GetObjectCard(gc);

    if (mClient.GetMyDeck().HasCard(c) == false)
    {
        return;
    }

    if (mClient.GetGameInfo().sequence == Game::BUILD_DOG)
    {
        if ((c->GetSuit() == Card::TRUMPS) ||
                ((c->GetSuit() != Card::TRUMPS) && (c->GetValue() == 14)))
        {
            tapis->SetCursorType(Canvas::FORBIDDEN);
        }
        else
        {
            tapis->SetCursorType(Canvas::ARROW);
        }
    }
    else if (mClient.GetGameInfo().sequence == Game::BUILD_HANDLE)
    {
        if (c->GetSuit() == Card::TRUMPS)
        {
            tapis->SetCursorType(Canvas::ARROW);
        }
        else
        {
            tapis->SetCursorType(Canvas::FORBIDDEN);
        }
    }
    else if (mClient.GetGameInfo().sequence == Game::PLAY_TRICK)
    {
        if (mClient.IsValid(c) == true)
        {
            tapis->SetCursorType(Canvas::ARROW);
        }
        else
        {
            tapis->SetCursorType(Canvas::FORBIDDEN);
        }
    }
}
/*****************************************************************************/
void TarotClub::slotSendChatMessage(const QString &message)
{
    mClient.SendChatMessage(message.toStdString());
}
/*****************************************************************************/
void TarotClub::slotClickCard(GfxCard *gc)
{
    Card *c = tapis->GetObjectCard(gc);

    if (mClient.GetMyDeck().HasCard(c) == false)
    {
        return;
    }

    if (mClient.GetGameInfo().sequence == Game::PLAY_TRICK)
    {
        if (mClient.IsValid(c) == false)
        {
            return;
        }
        tapis->SetFilter(Canvas::BLOCK_ALL);
        statusBar()->clearMessage();

        mClient.GetMyDeck().Remove(c);
        mClient.SendCard(c);

        ShowSouthCards();

    }
    else if (mClient.GetGameInfo().sequence == Game::BUILD_DOG)
    {

        if ((c->GetSuit() == Card::TRUMPS) ||
                ((c->GetSuit() != Card::TRUMPS) && (c->GetValue() == 14)))
        {
            return;
        }

        // select one card
        if (gc->GetStatus() == GfxCard::NORMAL)
        {
            if (discard.Size() == 6U)
            {
                return;
            }
            discard.Append(c);
            if (discard.Size() == 6U)
            {
                tapis->DisplayDiscardMenu(true);
            }
        }
        // Un-select card
        else if (gc->GetStatus() == GfxCard::SELECTED)
        {
            if (discard.Size() == 0U)
            {
                return;
            }
            discard.Remove(c);
            tapis->DisplayDiscardMenu(false);
        }
        gc->ToggleStatus();
    }
    else if (mClient.GetGameInfo().sequence == Game::BUILD_HANDLE)
    {
        if (c->GetSuit() == Card::TRUMPS)
        {
            // select one card
            if (gc->GetStatus() == GfxCard::NORMAL)
            {
                mClient.GetHandleDeck().Append(c);
            }
            else if (gc->GetStatus() == GfxCard::SELECTED)
            {
                mClient.GetHandleDeck().Remove(c);
            }
            if ((mClient.GetHandleDeck().Size() == 10U) ||
                    (mClient.GetHandleDeck().Size() == 13U) ||
                    (mClient.GetHandleDeck().Size() == 15U))
            {
                tapis->SetFilter(Canvas::MENU | Canvas::CARDS);
                tapis->DisplayHandleMenu(true);
            }
            else
            {
                tapis->SetFilter(Canvas::CARDS);
                tapis->DisplayHandleMenu(false);
            }
            gc->ToggleStatus();
        }
    }
}
/*****************************************************************************/
void TarotClub::slotMessage()
{
    while (!mMessages.empty())
    {
        std::string msg = mMessages.front();
        mMessages.pop_front();
        chatDock->message(msg);
    }
}
/*****************************************************************************/
void TarotClub::slotAssignedPlace()
{
    chatDock->message("The server grant you access to the table in place " + mClient.GetPlace().ToString());
}
/*****************************************************************************/
void TarotClub::slotPlayersList()
{
    tapis->SetPlayerIdentity(players, mClient.GetPlace());
    scoresDock->setPlayers(players);
}
/*****************************************************************************/
void TarotClub::slotReceiveCards()
{
    infosDock->PrintStats(mClient.GetStatistics());
    tapis->ResetCards();
    ShowSouthCards();
}
/*****************************************************************************/
void TarotClub::slotSelectPlayer(Place p)
{
    tapis->ShowSelection(p, mClient.GetPlace());
}
/*****************************************************************************/
void TarotClub::slotRequestBid(Contract highestBid)
{
    tapis->ShowBidsChoice(highestBid);
    tapis->SetFilter(Canvas::MENU);
}
/*****************************************************************************/
void TarotClub::slotShowBid(Place p, bool slam, Contract c)
{
    // FIXME: show the announced slam on the screen
    Q_UNUSED(slam);

    tapis->ShowBid(p, c, mClient.GetPlace());
    mClient.SendSyncBid();
}
/*****************************************************************************/
void TarotClub::slotStartDeal(Place taker, Contract c, Game::Shuffle sh)
{
    firstTurn = true;
    infosDock->Clear();
    infosDock->SetContract(c);

    QString name = "ERROR";

    if (players.contains(taker))
    {
        name = QString(players.value(taker).name.data());
    }
    infosDock->SetTaker(name, taker);

    if (sh.type != Game::CUSTOM_DEAL)
    {
        infosDock->SetDealNumber(sh.seed);
    }
    else
    {
        // Numbered deal
        infosDock->SetDealNumber(0U);
    }
    tapis->SetFilter(Canvas::BLOCK_ALL);
    tapis->InitBoard();
    tapis->ShowTaker(taker, mClient.GetPlace());

    // We are ready, let's inform the server about that
    mClient.SendSyncStart();
}
/*****************************************************************************/
void TarotClub::slotShowDog()
{
    QList<Card *> cards;

    for (Deck::ConstIterator i = mClient.GetDogDeck().Begin(); i != mClient.GetDogDeck().End(); ++i)
    {
        cards.append((*i));
    }
    tapis->DrawCardsInPopup(cards);
    statusBar()->showMessage(trUtf8("Click on the board once you have seen the dog."));
}
/*****************************************************************************/
void TarotClub::slotShowHandle()
{
    QList<Card *> cards;

    for (Deck::ConstIterator i = mClient.GetHandleDeck().Begin(); i != mClient.GetHandleDeck().End(); ++i)
    {
        cards.append((*i));
    }
    tapis->DrawCardsInPopup(cards);
    statusBar()->showMessage(trUtf8("Click on the board once you have seen the handle."));
}
/*****************************************************************************/
void TarotClub::slotDealAgain()
{
    infosDock->Clear();
    tapis->SetFilter(Canvas::BLOCK_ALL);
    tapis->InitBoard();

    QMessageBox::information(this, trUtf8("Information"),
                             trUtf8("All the players have passed.\n"
                                    "New deal will begin."));
    mClient.SendReady();
}
/*****************************************************************************/
void TarotClub::slotBuildDiscard()
{
    Card *c;

    // We add the dog to the player's deck
    for (Deck::ConstIterator i = mClient.GetDogDeck().Begin(); i != mClient.GetDogDeck().End(); ++i)
    {
        c = (*i);
        mClient.GetMyDeck().Append(c);
    }
    discard.Clear();
    tapis->SetFilter(Canvas::CARDS | Canvas::MENU);

    // Player's cards are shown
    ShowSouthCards();
    statusBar()->showMessage(trUtf8("Select cards to build your discard."));
}
/*****************************************************************************/
void TarotClub::slotPlayCard()
{
    tapis->SetFilter(Canvas::CARDS);

    // If we're about to play the first card, the Player is allowed to declare a handle
    if (firstTurn == true)
    {
        firstTurn = false;
        // If a handle exists in the player's deck, we propose to declare it
        if (mClient.GetStatistics().trumps >= 10)
        {
            int ret = QMessageBox::question(this, trUtf8("Handle"),
                                            trUtf8("You have a handle.\n"
                                                   "Do you want to declare it?"),
                                            QMessageBox::Yes | QMessageBox::No);
            if (ret == QMessageBox::Yes)
            {
                mClient.GetGameInfo().sequence = Game::BUILD_HANDLE;
                mClient.GetHandleDeck().Clear();
                statusBar()->showMessage(trUtf8("Build your handle."));
            }
        }
    }
    else
    {
        statusBar()->showMessage(trUtf8("It's your turn to play a card."));
    }
}
/*****************************************************************************/
void TarotClub::slotShowCard(Place p, std::string name)
{
    GfxCard *gc = tapis->GetGfxCard(TarotDeck::GetIndex(name));
    tapis->DrawCard(gc, p, mClient.GetPlace());
    infosDock->AddRound(mClient.GetGameInfo(), p, QString(name.data()));

    // We have seen the card, let's inform the server about that
    mClient.SendSyncCard();
}
/*****************************************************************************/
void TarotClub::slotEndOfDeal()
{
    statusBar()->showMessage(trUtf8("End of the deal."));
    tapis->SetFilter(Canvas::BLOCK_ALL);
    tapis->InitBoard();
    tapis->ResetCards();

    resultWindow->SetResult(mClient.GetScore(), mClient.GetGameInfo());
    resultWindow->exec();

    /*
     * FIXME:
        - If tournament mode, show the deal winner, then send a sync on window closing
        - If the last turn of a tournament, show the deal result, then sho the podem
        - Otherwise, show the deal winner
     */

    if (mClient.GetGameInfo().gameMode == Game::TOURNAMENT)
    {
        bool continueGame;

        deal.SetScore(mClient.GetScore());
        continueGame = deal.AddScore(mClient.GetGameInfo());
        scoresDock->SetNewScore(deal);

        if (continueGame == true)
        {
            mClient.SendReady();
        }
        else
        {
            // Continue next deal (FIXME: test if it is the last deal)
            showVictoryWindow();
        }
    }
}
/*****************************************************************************/
/**
 * @brief TarotClub::slotWaitTrick
 *
 * This method is called at the end of each turn, when all the players have
 * played a card.
 */
void TarotClub::slotWaitTrick(Place winner)
{
    infosDock->SelectWinner(mClient.GetGameInfo(), winner);
    tapis->SetFilter(Canvas::BLOCK_ALL);
    statusBar()->showMessage(trUtf8("Click on the board to continue."));

    // launch timer to clean cards, if needed
    if (clientConfig.GetOptions().enableDelayBeforeCleaning == true)
    {
        QTimer::singleShot(clientConfig.GetOptions().delayBeforeCleaning, this, SLOT(slotClickTapis()));
    }
}
/*****************************************************************************/
void TarotClub::slotEndOfGame()
{
    // TODO
}

//=============================================================================
// End of file TarotClub.cpp
//=============================================================================
