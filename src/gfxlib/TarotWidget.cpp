/*=============================================================================
 * TarotClub - TarotWidget.cpp
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

// Qt includes
#include <QInputDialog>
#include <QMessageBox>
#include <QSysInfo>

// TarotClub includes
#include "TarotWidget.h"
#include "Common.h"
#include "Log.h"

/*****************************************************************************/
TarotWidget::TarotWidget(QWidget* parent = 0)
  : QWidget(parent)
  , mClient(*this)
  , mConnectionType(NO_CONNECTION)
{
    setWindowTitle(QString(TAROT_TITRE) + " " + QString(TAROT_VERSION));

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mCanvas = new Canvas(this);
    mCanvas->show();

    mainLayout->addWidget(mCanvas);
    setLayout(mainLayout);

    qRegisterMetaType<Place>("Place");
    qRegisterMetaType<Contract>("Contract");
    qRegisterMetaType<Game::Shuffle>("Game::Shuffle");
    qRegisterMetaType<std::string>("std::string");

    // Board click events
    connect(mCanvas, &Canvas::sigViewportClicked, this, &TarotWidget::slotClickTapis);
    connect(mCanvas, &Canvas::sigClickCard, this, &TarotWidget::slotClickCard);
    connect(mCanvas, &Canvas::sigMoveCursor, this, &TarotWidget::slotMoveCursor);
    connect(mCanvas, &Canvas::sigContract, this, &TarotWidget::slotSetEnchere);
    connect(mCanvas, &Canvas::sigAcceptDiscard, this, &TarotWidget::slotAcceptDiscard);
    connect(mCanvas, &Canvas::sigAcceptHandle, this, &TarotWidget::slotAcceptHandle);
    connect(mCanvas, &Canvas::sigStartGame, this, &TarotWidget::slotNewQuickGame);

    // Client events. This connection list allow to call Qt GUI from another thread (Client class thread)
    connect(this, &TarotWidget::sigReceiveCards, this, &TarotWidget::slotReceiveCards, Qt::QueuedConnection);
    connect(this, &TarotWidget::sigAssignedPlace, this, &TarotWidget::slotAssignedPlace, Qt::QueuedConnection);
    connect(this, &TarotWidget::sigPlayersList, this, &TarotWidget::slotPlayersList, Qt::QueuedConnection);
    connect(this, &TarotWidget::sigMessage, this, &TarotWidget::slotMessage, Qt::QueuedConnection);
    connect(this, &TarotWidget::sigSelectPlayer, this, &TarotWidget::slotSelectPlayer, Qt::QueuedConnection);
    connect(this, &TarotWidget::sigRequestBid, this, &TarotWidget::slotRequestBid, Qt::QueuedConnection);
    connect(this, &TarotWidget::sigShowBid, this, &TarotWidget::slotShowBid, Qt::QueuedConnection);
    connect(this, &TarotWidget::sigShowDog, this, &TarotWidget::slotShowDog, Qt::QueuedConnection);
    connect(this, &TarotWidget::sigStartDeal, this, &TarotWidget::slotStartDeal, Qt::QueuedConnection);
    connect(this, &TarotWidget::sigPlayCard, this, &TarotWidget::slotPlayCard, Qt::QueuedConnection);
    connect(this, &TarotWidget::sigBuildDiscard, this, &TarotWidget::slotBuildDiscard, Qt::QueuedConnection);
    connect(this, &TarotWidget::sigDealAgain, this, &TarotWidget::slotDealAgain, Qt::QueuedConnection);
    connect(this, &TarotWidget::sigEndOfDeal, this, &TarotWidget::slotEndOfDeal, Qt::QueuedConnection);
    connect(this, &TarotWidget::sigEndOfGame, this, &TarotWidget::slotEndOfGame, Qt::QueuedConnection);
    connect(this, &TarotWidget::sigShowCard, this, &TarotWidget::slotShowCard, Qt::QueuedConnection);
    connect(this, &TarotWidget::sigShowHandle, this, &TarotWidget::slotShowHandle, Qt::QueuedConnection);
    connect(this, &TarotWidget::sigWaitTrick, this, &TarotWidget::slotWaitTrick, Qt::QueuedConnection);
}
/*****************************************************************************/
/**
 * @brief One time game initialization
 */
void TarotWidget::Initialize()
{
    mClientConfig.Load();
    mServerConfig.Load();

    ApplyOptions();

    table.Initialize();
    mClient.Initialize();
    deal.Initialize();
    mCanvas->Initialize();
}
/*****************************************************************************/
/**
 * This method allows a proper cleanup before closing the application
 */
void TarotWidget::slotCleanBeforeExit()
{
    // Close ourself
    mClient.Close();
    table.Stop();   
}
/*****************************************************************************/
void TarotWidget::slotNewTournamentGame()
{
    Game::Shuffle sh;
    sh.type = Game::RANDOM_DEAL;

    LaunchLocalGame(Game::TOURNAMENT, sh);
}
/*****************************************************************************/
void TarotWidget::slotNewQuickGame()
{
    Game::Shuffle sh;
    sh.type = Game::RANDOM_DEAL;

    LaunchLocalGame(Game::ONE_DEAL, sh);
}
/*****************************************************************************/
void TarotWidget::slotCreateNetworkGame()
{
    Game::Shuffle sh;
    sh.type = Game::RANDOM_DEAL;

    table.CreateGame(Game::ONE_DEAL, 4U, sh);
    // Connect us to the server
    mClient.ConnectToHost("127.0.0.1", DEFAULT_TCP_PORT);
}
/*****************************************************************************/
bool TarotWidget::HasLocalConnection()
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
void TarotWidget::LaunchRemoteGame(const std::string &ip, std::uint16_t port)
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
void TarotWidget::LaunchLocalGame(Game::Mode mode, const Game::Shuffle &sh)
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
void TarotWidget::InitScreen()
{
    // GUI initialization
    mCanvas->InitBoard();
    mCanvas->ResetCards();
    mCanvas->SetFilter(Canvas::BLOCK_ALL);
}
/*****************************************************************************/
void TarotWidget::ApplyOptions()
{
    mClientConfig.Save();
    mServerConfig.Save();

    ClientOptions options = mClientConfig.GetOptions();
    mClient.SetMyIdentity(options.identity);

    ServerOptions srvOpt = mServerConfig.GetOptions();
    table.SetBotParameters(srvOpt.bots, srvOpt.timer);

    mCanvas->ShowAvatars(options.showAvatars);
    mCanvas->SetBackground(options.backgroundColor);
}
/*****************************************************************************/
void TarotWidget::ShowVictoryWindow()
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
void TarotWidget::HideTrick()
{
    Card *c;
    GfxCard *gc;
    Deck &trick = mClient.GetCurrentTrick();

    for (Deck::ConstIterator i = trick.Begin(); i != trick.End(); ++i)
    {
        c = (*i);
        gc = mCanvas->GetGfxCard(TarotDeck::GetIndex(c->GetName()));
        gc->hide();
    }
}
/*****************************************************************************/
void TarotWidget::slotSetEnchere(Contract cont)
{
    mClient.SendBid(cont, mCanvas->GetSlamOption());
    mCanvas->HideBidsChoice();
}
/*****************************************************************************/
void TarotWidget::slotAcceptDiscard()
{
    // Clear canvas and forbid new actions
    mCanvas->DisplayDiscardMenu(false);
    mCanvas->SetFilter(Canvas::BLOCK_ALL);

    for (Deck::ConstIterator i = discard.Begin(); i != discard.End(); ++i)
    {
        Card *c = (*i);
        GfxCard *gc = mCanvas->GetGfxCard(TarotDeck::GetIndex(c->GetName()));
        gc->hide();
    }

    mClient.SetDiscard(discard);
    ShowSouthCards();
}
/*****************************************************************************/
void TarotWidget::slotAcceptHandle()
{
    if (mClient.TestHandle() == false)
    {
        QMessageBox::information(this, trUtf8("Information"),
                                 trUtf8("Your handle is not valid.\n"
                                        "Showing the fool means that you have no any more trumps in your deck."));
        return;
    }

    mCanvas->DisplayHandleMenu(false);
    mCanvas->SetFilter(Canvas::CARDS);
    mClient.SendHandle();
    ShowSouthCards();
    mClient.GetGameInfo().sequence = Game::PLAY_TRICK;
}
/*****************************************************************************/
/**
 * @brief TarotWidget::ShowSouthCards
 *
 * Show current player's cards
 *
 * @param pos = 0 for 18-cards in the hand, otherwise 1 (with cards from the chien)
 */
void TarotWidget::ShowSouthCards()
{
    mClient.GetMyDeck().Sort(mClientConfig.GetOptions().cardsOrder);
    mCanvas->DrawSouthCards(mClient.GetMyDeck());
}
/*****************************************************************************/
void TarotWidget::slotClickTapis()
{
    if (mClient.GetGameInfo().sequence == Game::SHOW_DOG)
    {
        mCanvas->HidePopup();
        mClient.SendSyncDog(); // We have seen the dog, let's inform the server about that
    }
    else if (mClient.GetGameInfo().sequence == Game::SHOW_HANDLE)
    {
        mCanvas->HidePopup();
        mClient.SendSyncHandle(); // We have seen the handle, let's inform the server about that
    }
    else if (mClient.GetGameInfo().sequence == Game::SYNC_TRICK)
    {
        HideTrick();
        mClient.SendSyncTrick();
    }
}
/*****************************************************************************/
/**
 * @brief TarotWidget::slotMoveCursor
 *
 * This function is called every time the mouse cursor moves
 *
 * @param gc
 */
void TarotWidget::slotMoveCursor(GfxCard *gc)
{
    Card *c = mCanvas->GetObjectCard(gc);

    if (mClient.GetMyDeck().HasCard(c) == false)
    {
        return;
    }

    if (mClient.GetGameInfo().sequence == Game::BUILD_DOG)
    {
        if ((c->GetSuit() == Card::TRUMPS) ||
                ((c->GetSuit() != Card::TRUMPS) && (c->GetValue() == 14)))
        {
            mCanvas->SetCursorType(Canvas::FORBIDDEN);
        }
        else
        {
            mCanvas->SetCursorType(Canvas::ARROW);
        }
    }
    else if (mClient.GetGameInfo().sequence == Game::BUILD_HANDLE)
    {
        if (c->GetSuit() == Card::TRUMPS)
        {
            mCanvas->SetCursorType(Canvas::ARROW);
        }
        else
        {
            mCanvas->SetCursorType(Canvas::FORBIDDEN);
        }
    }
    else if (mClient.GetGameInfo().sequence == Game::PLAY_TRICK)
    {
        if (mClient.IsValid(c) == true)
        {
            mCanvas->SetCursorType(Canvas::ARROW);
        }
        else
        {
            mCanvas->SetCursorType(Canvas::FORBIDDEN);
        }
    }
}
/*****************************************************************************/
void TarotWidget::slotSendChatMessage(const QString &message)
{
    mClient.SendChatMessage(message.toStdString());
}
/*****************************************************************************/
void TarotWidget::slotClickCard(GfxCard *gc)
{
    Card *c = mCanvas->GetObjectCard(gc);

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
        mCanvas->SetFilter(Canvas::BLOCK_ALL);

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
                mCanvas->DisplayDiscardMenu(true);
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
            mCanvas->DisplayDiscardMenu(false);
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
                mCanvas->SetFilter(Canvas::MENU | Canvas::CARDS);
                mCanvas->DisplayHandleMenu(true);
            }
            else
            {
                mCanvas->SetFilter(Canvas::CARDS);
                mCanvas->DisplayHandleMenu(false);
            }
            gc->ToggleStatus();
        }
    }
}
/*****************************************************************************/
void TarotWidget::slotMessage()
{
    while (!mMessages.empty())
    {
        std::string msg = mMessages.front();
        mMessages.pop_front();
    }
}
/*****************************************************************************/
void TarotWidget::slotAssignedPlace()
{

}
/*****************************************************************************/
void TarotWidget::slotPlayersList()
{
    mCanvas->SetPlayerIdentity(players, mClient.GetPlace());
}
/*****************************************************************************/
void TarotWidget::slotReceiveCards()
{
    mCanvas->ResetCards();
    ShowSouthCards();
}
/*****************************************************************************/
void TarotWidget::slotSelectPlayer(Place p)
{
    mCanvas->ShowSelection(p, mClient.GetPlace());
}
/*****************************************************************************/
void TarotWidget::slotRequestBid(Contract highestBid)
{
    mCanvas->ShowBidsChoice(highestBid);
    mCanvas->SetFilter(Canvas::MENU);
}
/*****************************************************************************/
void TarotWidget::slotShowBid(Place p, bool slam, Contract c)
{
    // FIXME: show the announced slam on the screen
    Q_UNUSED(slam);

    mCanvas->ShowBid(p, c, mClient.GetPlace());
    mClient.SendSyncBid();
}
/*****************************************************************************/
void TarotWidget::slotStartDeal(Place taker, Contract c, Game::Shuffle sh)
{
    (void)sh;
    (void)c;
    firstTurn = true;
    QString name = "ERROR";

    if (players.contains(taker))
    {
        name = QString(players.value(taker).name.data());
    }

    mCanvas->SetFilter(Canvas::BLOCK_ALL);
    mCanvas->InitBoard();
    mCanvas->ShowTaker(taker, mClient.GetPlace());

    // We are ready, let's inform the server about that
    mClient.SendSyncStart();
}
/*****************************************************************************/
void TarotWidget::slotShowDog()
{
    QList<Card *> cards;

    for (Deck::ConstIterator i = mClient.GetDogDeck().Begin(); i != mClient.GetDogDeck().End(); ++i)
    {
        cards.append((*i));
    }
    mCanvas->DrawCardsInPopup(cards);
}
/*****************************************************************************/
void TarotWidget::slotShowHandle()
{
    QList<Card *> cards;

    for (Deck::ConstIterator i = mClient.GetHandleDeck().Begin(); i != mClient.GetHandleDeck().End(); ++i)
    {
        cards.append((*i));
    }
    mCanvas->DrawCardsInPopup(cards);
}
/*****************************************************************************/
void TarotWidget::slotDealAgain()
{
    mCanvas->SetFilter(Canvas::BLOCK_ALL);
    mCanvas->InitBoard();

    QMessageBox::information(this, trUtf8("Information"),
                             trUtf8("All the players have passed.\n"
                                    "New deal will begin."));
    mClient.SendReady();
}
/*****************************************************************************/
void TarotWidget::slotBuildDiscard()
{
    Card *c;

    // We add the dog to the player's deck
    for (Deck::ConstIterator i = mClient.GetDogDeck().Begin(); i != mClient.GetDogDeck().End(); ++i)
    {
        c = (*i);
        mClient.GetMyDeck().Append(c);
    }
    discard.Clear();
    mCanvas->SetFilter(Canvas::CARDS | Canvas::MENU);

    // Player's cards are shown
    ShowSouthCards();
}
/*****************************************************************************/
void TarotWidget::slotPlayCard()
{
    mCanvas->SetFilter(Canvas::CARDS);

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
            }
        }
    }
    else
    {

    }
}
/*****************************************************************************/
void TarotWidget::slotShowCard(Place p, std::string name)
{
    GfxCard *gc = mCanvas->GetGfxCard(TarotDeck::GetIndex(name));
    mCanvas->DrawCard(gc, p, mClient.GetPlace());

    // We have seen the card, let's inform the server about that
    mClient.SendSyncCard();
}
/*****************************************************************************/
void TarotWidget::slotEndOfDeal()
{
    mCanvas->SetFilter(Canvas::BLOCK_ALL);
    mCanvas->InitBoard();
    mCanvas->ResetCards();

 //   resultWindow->SetResult(mClient.GetScore(), mClient.GetGameInfo());

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

        if (continueGame == true)
        {
            mClient.SendReady();
        }
        else
        {
            // Continue next deal (FIXME: test if it is the last deal)
            ShowVictoryWindow();
        }
    }
}
/*****************************************************************************/
/**
 * @brief TarotWidget::slotWaitTrick
 *
 * This method is called at the end of each turn, when all the players have
 * played a card.
 */
void TarotWidget::slotWaitTrick(Place winner)
{
    (void)winner;
    mCanvas->SetFilter(Canvas::BLOCK_ALL);

    // launch timer to clean cards, if needed
    if (mClientConfig.GetOptions().enableDelayBeforeCleaning == true)
    {
        QTimer::singleShot(mClientConfig.GetOptions().delayBeforeCleaning, this, SLOT(slotClickTapis()));
    }
}
/*****************************************************************************/
void TarotWidget::slotEndOfGame()
{
    // TODO
}

//=============================================================================
// End of file TarotClub.cpp
//=============================================================================
