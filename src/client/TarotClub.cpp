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
#include "Tools.h"
#include "Log.h"

/*****************************************************************************/
TarotClub::TarotClub()
  : MainWindow()
  , mClient(*this)
{
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

    //   connect(pliPrecAct, SIGNAL(triggered()), this, SLOT(slotAffichePliPrecedent()));

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
    table.Stop();
}
/*****************************************************************************/
void TarotClub::slotNewTournamentGame()
{
    Game::Shuffle sh;
    sh.type = Game::RANDOM_DEAL;

    table.CreateGame(Game::TOURNAMENT, 4U, sh);
    NewGame("127.0.0.1", DEFAULT_PORT);
    // start game
    table.ConnectBots();
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

        table.CreateGame(Game::ONE_DEAL, 4U, sh);
        NewGame("127.0.0.1", DEFAULT_PORT);
        // start game
        table.ConnectBots();
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

        table.CreateGame(Game::ONE_DEAL, 4U, sh);
        NewGame("127.0.0.1", DEFAULT_PORT);
        // start game
        table.ConnectBots();
    }
}
/*****************************************************************************/
void TarotClub::slotNewQuickGame()
{
    Game::Shuffle sh;
    sh.type = Game::RANDOM_DEAL;

    table.CreateGame(Game::ONE_DEAL, 4U, sh);
    NewGame("127.0.0.1", DEFAULT_PORT);
    // start game
    table.ConnectBots();
}
/*****************************************************************************/
void TarotClub::NewGame(const QString &address, int port)
{
    // GUI initialization
    scoresDock->clear();
    infosDock->Clear();
    tapis->InitBoard();
    tapis->ResetCards();
    tapis->SetFilter(Canvas::BLOCK_ALL);

    // Connect us to the server
    mClient.ConnectToHost(address.toStdString(), port);
}
/*****************************************************************************/
void TarotClub::slotJoinNetworkGame()
{
    table.Stop();
    if (joinWizard->exec() == QDialog::Accepted)
    {
        // connect to table
        JoinWizard::Connection cn = joinWizard->GetTableConnection();

        if (cn.isValid)
        {
            NewGame(cn.ip, cn.port);
        }
    }
}
/*****************************************************************************/
void TarotClub::slotQuickJoinNetworkGame()
{
    table.Stop();
    if (quickJoinWindow->exec() == QDialog::Accepted)
    {
        QString ip = uiQuickJoin.ipAddress->text();
        quint16 port = uiQuickJoin.tcpPort->value();

        NewGame(ip, port);
    }
}
/*****************************************************************************/
void TarotClub::slotCreateNetworkGame()
{
    Game::Shuffle sh;
    sh.type = Game::RANDOM_DEAL;

    table.CreateGame(Game::ONE_DEAL, 4U, sh);
    NewGame("127.0.0.1", DEFAULT_PORT);
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
    QList<Place> podium = deal.GetPodium();

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
    int i;
    Card *c;
    GfxCard *gc;
    Deck &trick = mClient.GetCurrentTrick();

    for (i = 0; i < trick.size(); i++)
    {
        c = trick.at(i);
        gc = tapis->GetGfxCard(c->GetId());
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
    for (int i = 0; i < discard.size(); i++)
    {
        Card *c = discard.at(i);
        GfxCard *gc = tapis->GetGfxCard(c->GetId());
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
    mClient.GetMyDeck().Sort();
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
    else if (mClient.GetGameInfo().sequence == Game::SHOW_DOG)
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

    if (mClient.GetMyDeck().contains(c) == false)
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

    if (mClient.GetMyDeck().contains(c) == false)
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

        mClient.GetMyDeck().removeAll(c);
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
            if (discard.size() == 6)
            {
                return;
            }
            discard.append(c);
            if (discard.size() == 6)
            {
                tapis->DisplayDiscardMenu(true);
            }
        }
        // Un-select card
        else if (gc->GetStatus() == GfxCard::SELECTED)
        {
            if (discard.size() == 0)
            {
                return;
            }
            discard.removeAll(c);
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
                mClient.GetHandleDeck().append(c);
            }
            else if (gc->GetStatus() == GfxCard::SELECTED)
            {
                mClient.GetHandleDeck().removeAll(c);
            }
            if ((mClient.GetHandleDeck().size() == 10) ||
                    (mClient.GetHandleDeck().size() == 13) ||
                    (mClient.GetHandleDeck().size() == 15))
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
void TarotClub::Message(const std::string &message)
{
    chatDock->message(QString(message.data()));
}
/*****************************************************************************/
void TarotClub::AssignedPlace(Place p)
{
    chatDock->message("The server grant you access to the table in place " + QString(Util::ToString(p).data()));
}
/*****************************************************************************/
void TarotClub::PlayersList(std::map<Place, Identity> &pl)
{
    std::map<Place, Identity>::iterator iter;

    players.clear();
    for(iter = pl.begin(); iter != pl.end(); iter++)
    {
        players[iter->first] = iter->second;
    }

    tapis->SetPlayerIdentity(players, mClient.GetPlace());
    scoresDock->setPlayers(players);
}
/*****************************************************************************/
void TarotClub::ReceiveCards()
{
    infosDock->PrintStats(mClient.GetStatistics());
    tapis->ResetCards();
    ShowSouthCards();
}
/*****************************************************************************/
void TarotClub::SelectPlayer(Place p)
{
    tapis->ShowSelection(p, mClient.GetPlace());
}
/*****************************************************************************/
void TarotClub::RequestBid(Contract highestBid)
{
    tapis->ShowBidsChoice(highestBid);
    tapis->SetFilter(Canvas::MENU);
}
/*****************************************************************************/
void TarotClub::ShowBid(Place p, bool slam, Contract c)
{
    // FIXME: show the announced slam on the screen
    Q_UNUSED(slam);

    tapis->ShowBid(p, c, mClient.GetPlace());
    mClient.SendSyncBid();
}
/*****************************************************************************/
void TarotClub::StartDeal(Place taker, Contract c, const Game::Shuffle &sh)
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
        infosDock->SetDealNumber(-1);
    }
    tapis->SetFilter(Canvas::BLOCK_ALL);
    tapis->InitBoard();
    tapis->ShowTaker(taker, mClient.GetPlace());

    // We are ready, let's inform the server about that
    mClient.SendSyncStart();
}
/*****************************************************************************/
void TarotClub::ShowDog()
{
    QList<Card *> cards;

    for (int i = 0; i < mClient.GetDogDeck().size(); i++)
    {
        cards.append(mClient.GetDogDeck().at(i));
    }
    tapis->DrawCardsInPopup(cards);
    statusBar()->showMessage(trUtf8("Click on the board once you have seen the dog."));
}
/*****************************************************************************/
void TarotClub::ShowHandle()
{
    QList<Card *> cards;

    for (int i = 0; i < mClient.GetHandleDeck().size(); i++)
    {
        cards.append(mClient.GetHandleDeck().at(i));
    }
    tapis->DrawCardsInPopup(cards);
    statusBar()->showMessage(trUtf8("Click on the board once you have seen the handle."));
}
/*****************************************************************************/
void TarotClub::DealAgain()
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
void TarotClub::BuildDiscard()
{
    Card *c;

    // We add the dog to the player's deck
    for (int i = 0; i < mClient.GetDogDeck().size(); i++)
    {
        c = mClient.GetDogDeck().at(i);
        mClient.GetMyDeck().append(c);
    }
    discard.clear();
    tapis->SetFilter(Canvas::CARDS | Canvas::MENU);

    // Player's cards are shown
    ShowSouthCards();
    statusBar()->showMessage(trUtf8("Select cards to build your discard."));
}
/*****************************************************************************/
void TarotClub::PlayCard()
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
                mClient.GetHandleDeck().clear();
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
void TarotClub::ShowCard(Place p, const std::string &name)
{
    GfxCard *gc = tapis->GetGfxCard(TarotDeck::GetIndex(name));
    tapis->DrawCard(gc, p, mClient.GetPlace());
    infosDock->AddRound(mClient.GetGameInfo(), p, QString(name.data()));

    // We have seen the card, let's inform the server about that
    mClient.SendSyncCard();
}
/*****************************************************************************/
void TarotClub::EndOfDeal()
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
void TarotClub::WaitTrick(Place winner)
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
void TarotClub::EndOfGame()
{
}

//=============================================================================
// End of file TarotClub.cpp
//=============================================================================
