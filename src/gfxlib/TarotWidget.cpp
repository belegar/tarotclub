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
  , mAutoPlay(false)
{
    setWindowTitle(QString(TAROT_TITRE) + " " + QString(TAROT_VERSION));

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mCanvas = new Canvas(this);
    mCanvas->show();

    mainLayout->addWidget(mCanvas);
    setLayout(mainLayout);

    qRegisterMetaType<Place>("Place");
    qRegisterMetaType<Contract>("Contract");
    qRegisterMetaType<Tarot::Shuffle>("Tarot::Shuffle");
    qRegisterMetaType<std::string>("std::string");

    // Board click events
    connect(mCanvas, &Canvas::sigViewportClicked, this, &TarotWidget::slotClickBoard);
    connect(mCanvas, &Canvas::sigClickCard, this, &TarotWidget::slotClickCard);
    connect(mCanvas, &Canvas::sigCursorOverCard, this, &TarotWidget::slotMoveCursor);
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
    connect(this, &TarotWidget::sigAskForHandle, this, &TarotWidget::slotAskForHandle, Qt::QueuedConnection);
    connect(this, &TarotWidget::sigShowHandle, this, &TarotWidget::slotShowHandle, Qt::QueuedConnection);
    connect(this, &TarotWidget::sigWaitTrick, this, &TarotWidget::slotWaitTrick, Qt::QueuedConnection);
}
/*****************************************************************************/
/**
 * @brief One time game initialization
 */
void TarotWidget::Initialize()
{
    mTable.Initialize();
    mClient.Initialize();
    deal.Initialize();
    mCanvas->Initialize();
    mCanvas->SetFilter(Canvas::MENU);
    mCanvas->DisplayMainMenu(true);
}
/*****************************************************************************/
/**
 * This method allows a proper cleanup before closing the application
 */
void TarotWidget::slotCleanBeforeExit()
{
    // Close ourself
    mClient.Close();
    mTable.Stop();
}
/*****************************************************************************/
void TarotWidget::slotNewTournamentGame()
{
    Tarot::Shuffle sh;
    sh.type = Tarot::Shuffle::RANDOM_DEAL;

    LaunchLocalGame(Tarot::TOURNAMENT, sh, false);
}
/*****************************************************************************/
void TarotWidget::slotNewQuickGame()
{
    Tarot::Shuffle sh;
    sh.type = Tarot::Shuffle::RANDOM_DEAL;

    LaunchLocalGame(Tarot::ONE_DEAL, sh, false);
}
/*****************************************************************************/
void TarotWidget::slotCreateNetworkGame()
{
    Tarot::Shuffle sh;
    sh.type = Tarot::Shuffle::RANDOM_DEAL;

    mTable.CreateGame(Tarot::ONE_DEAL, 4U, sh);
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
void TarotWidget::LaunchLocalGame(Tarot::GameMode mode, const Tarot::Shuffle &sh, bool autoPlay)
{
    mAutoPlay = autoPlay;
    mTable.CreateGame(mode, 4U, sh);
    InitScreen();

    if (HasLocalConnection())
    {
        mTable.NewDeal();
    }
    else
    {
        mConnectionType = LOCAL;
        // Connect us to the server
        mClient.ConnectToHost("127.0.0.1", DEFAULT_TCP_PORT);
        // Connect the other players
        mTable.ConnectBots();
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
void TarotWidget::ApplyOptions(const ClientOptions &i_clientOpt, const ServerOptions &i_servOpt)
{
    // Save option structures locally
    mClientOptions = i_clientOpt;
    mServerOptions = i_servOpt;

    // Initialize all the objects with the user preferences
    mClient.SetMyIdentity(mClientOptions.identity);
    mTable.SetBotParameters(mServerOptions.bots, mServerOptions.timer);
    mCanvas->ShowAvatars(mClientOptions.showAvatars);
    mCanvas->SetBackground(mClientOptions.backgroundColor);
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
                             trUtf8("The winner of the tournament is ") + QString(mPlayers[podium[0]].name.data()),
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
    Deck &trick = mClient.GetCurrentTrick();

    for (Deck::ConstIterator i = trick.Begin(); i != trick.End(); ++i)
    {
        Card *c = (*i);
        mCanvas->ShowCard(TarotDeck::GetIndex(c->GetName()), false);
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
        mCanvas->ShowCard(TarotDeck::GetIndex(c->GetName()), false);
    }

    mClient.SetDiscard(discard);
    ShowSouthCards();
}
/*****************************************************************************/
void TarotWidget::slotAcceptHandle()
{
    if (mClient.TestHandle(mMyHandle) == false)
    {
        QMessageBox::information(this, trUtf8("Information"),
                                 trUtf8("Your handle is not valid.\n"
                                        "Showing the fool means that you have no any more trumps in your deck."));
        return;
    }

    mCanvas->DisplayHandleMenu(false);
    mCanvas->SetFilter(Canvas::CARDS);
    mClient.SendHandle(mMyHandle);
    ShowSouthCards();
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
    mClient.GetMyDeck().Sort(mClientOptions.cardsOrder);
    mCanvas->DrawSouthCards(mClient.GetMyDeck());
}
/*****************************************************************************/
void TarotWidget::slotClickBoard()
{
    if (mClient.GetSequence() == Client::SHOW_DOG)
    {
        mCanvas->HidePopup();
        mClient.SendSyncDog(); // We have seen the dog, let's inform the server about that
    }
    else if (mClient.GetSequence() == Client::SHOW_HANDLE)
    {
        mCanvas->HidePopup();
        mClient.SendSyncHandle(); // We have seen the handle, let's inform the server about that
    }
    else if (mClient.GetSequence() == Client::SYNC_TRICK)
    {
        HideTrick();
        mClient.SendSyncTrick();
    }
    else if (mClient.GetSequence() == Client::SHOW_SCORE)
    {
        mCanvas->HideMessageBox();
        mClient.SendSyncEndOfDeal();
    }
    // Forbid any further clicks
    mCanvas->SetFilter(Canvas::BLOCK_ALL);
}
/*****************************************************************************/
/**
 * @brief TarotWidget::slotMoveCursor
 *
 * This function is called every time the mouse cursor moves
 *
 * @param index is the card index under the mouse cursor
 */
void TarotWidget::slotMoveCursor(std::uint8_t index)
{
    Card *c = TarotDeck::GetCard(index);

    if (mClient.GetMyDeck().HasCard(c) == false)
    {
        return;
    }

    if (mClient.GetSequence() == Client::BUILD_DISCARD)
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
    else if (mClient.GetSequence() == Client::BUILD_HANDLE)
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
    else if (mClient.GetSequence() == Client::PLAY_TRICK)
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
void TarotWidget::slotClickCard(std::uint8_t index, bool selected)
{
    Card *c = TarotDeck::GetCard(index);

    if (mClient.GetMyDeck().HasCard(c) == false)
    {
        return;
    }

    if (mClient.GetSequence() == Client::PLAY_TRICK)
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
    else if (mClient.GetSequence() == Client::BUILD_DISCARD)
    {

        if ((c->GetSuit() == Card::TRUMPS) ||
                ((c->GetSuit() != Card::TRUMPS) && (c->GetValue() == 14)))
        {
            return;
        }

        // select one card
        if (!selected)
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
        else if (selected)
        {
            if (discard.Size() == 0U)
            {
                return;
            }
            discard.Remove(c);
            mCanvas->DisplayDiscardMenu(false);
        }
        mCanvas->ToggleCardSelection(index);
    }
    else if (mClient.GetSequence() == Client::BUILD_HANDLE)
    {
        if (c->GetSuit() == Card::TRUMPS)
        {
            // select one card
            if (!selected)
            {
                mMyHandle.Append(c);
            }
            else if (selected)
            {
                mMyHandle.Remove(c);
            }
            if (mClient.TestHandle(mMyHandle))
            {
                mCanvas->SetFilter(Canvas::MENU | Canvas::CARDS);
                mCanvas->DisplayHandleMenu(true);
            }
            else
            {
                mCanvas->SetFilter(Canvas::CARDS);
                mCanvas->DisplayHandleMenu(false);
            }
            mCanvas->ToggleCardSelection(index);
        }
    }
}
/*****************************************************************************/
void TarotWidget::slotSendChatMessage(const QString &message)
{
    mClient.SendChatMessage(message.toStdString());
}
/*****************************************************************************/
void TarotWidget::slotMessage(std::string message)
{
    // Nothing to do here!
    (void) message;
}
/*****************************************************************************/
void TarotWidget::slotAssignedPlace()
{
    // Nothing to do here!
}
/*****************************************************************************/
void TarotWidget::slotPlayersList()
{
    mCanvas->SetPlayerIdentity(mPlayers, mClient.GetPlace());
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
void TarotWidget::slotStartDeal()
{
    QString name = "ERROR";
    Place taker = mClient.GetBid().taker;

    if (mPlayers.contains(taker))
    {
        name = QString(mPlayers.value(taker).name.data());
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
    mCanvas->SetFilter(Canvas::BOARD);
}
/*****************************************************************************/
void TarotWidget::slotShowHandle()
{
    QList<Card *> cards;

    for (Deck::ConstIterator i = mMyHandle.Begin(); i != mMyHandle.End(); ++i)
    {
        cards.append((*i));
    }
    mCanvas->DrawCardsInPopup(cards);
    mCanvas->SetFilter(Canvas::BOARD);
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
    // We add the dog to the player's deck
    mClient.GetMyDeck().Append(mClient.GetDogDeck());
    discard.Clear();
    mCanvas->SetFilter(Canvas::CARDS | Canvas::MENU);

    // Player's cards are shown
    ShowSouthCards();
}
/*****************************************************************************/
void TarotWidget::slotAskForHandle()
{
    mMyHandle.Clear();

    // If we're about to play the first card, the Player is allowed to declare a handle
    // If a handle exists in the player's deck, we propose to declare it
    if (mClient.GetStatistics().trumps >= 10)
    {
        int ret = QMessageBox::question(this, trUtf8("Handle"),
                                        trUtf8("You have a handle.\n"
                                               "Do you want to declare it?"),
                                        QMessageBox::Yes | QMessageBox::No);
        if (ret == QMessageBox::Yes)
        {
            mCanvas->SetFilter(Canvas::CARDS);
        }
    }
    else
    {
        // no available handle, send an empty deck
        mClient.SendHandle(mMyHandle);
    }
}
/*****************************************************************************/
void TarotWidget::slotPlayCard()
{
    if (mAutoPlay)
    {
        mCanvas->SetFilter(Canvas::BLOCK_ALL);

        Card *c = mClient.Play();
        mClient.GetMyDeck().Remove(c);
        mClient.SendCard(c);

        ShowSouthCards();
    }
    else
    {
        mCanvas->SetFilter(Canvas::CARDS);
    }
}
/*****************************************************************************/
/**
 * @brief TarotWidget::slotShowCard
 * @param p
 * @param name Not passed as reference to avoid signal/slots problems
 */
void TarotWidget::slotShowCard(Place p, std::string name)
{
    mCanvas->DrawCard(TarotDeck::GetIndex(name), p, mClient.GetPlace());

    // We have seen the card, let's inform the server about that
    mClient.SendSyncShowCard();
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
    mCanvas->SetFilter(Canvas::BOARD);

    // launch timer to clean cards, if needed
    if (mClientOptions.clickToClean == true)
    {
        QTimer::singleShot(mClientOptions.delayBeforeCleaning, this, SLOT(slotClickBoard()));
    }
}
/*****************************************************************************/
void TarotWidget::slotEndOfDeal()
{
    mCanvas->InitBoard();
    mCanvas->ResetCards();
    mCanvas->SetResult(mClient.GetScore(), mClient.GetBid());
    mCanvas->SetFilter(Canvas::BOARD);

    if (mClient.GetGameMode() == Tarot::TOURNAMENT)
    {
        deal.SetScore(mClient.GetScore());
        (void)deal.AddScore(mClient.GetBid(), mClient.GetNumberOfPlayers());

        emit sigAddScore();
    }
}
/*****************************************************************************/
void TarotWidget::slotEndOfGame(Place winner)
{
    (void) winner; // FIXME: usage?

    ShowVictoryWindow();

    mCanvas->SetFilter(Canvas::MENU);
    mCanvas->DisplayMainMenu(true);
}

//=============================================================================
// End of file TarotClub.cpp
//=============================================================================
