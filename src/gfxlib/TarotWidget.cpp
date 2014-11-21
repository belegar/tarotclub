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
TarotWidget::TarotWidget(QWidget *parent)
    : QWidget(parent)
    , mClient(*this)
    , mConnectionType(NO_CONNECTION)
    , mAutoPlay(false)
    , mGameMode(Tarot::ONE_DEAL)
    , mShutdown(false)
{
    setWindowTitle(QString(TAROT_TITLE.c_str()) + " " + QString(TAROT_VERSION.c_str()));

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
    connect(this, &TarotWidget::sigNewGame, this, &TarotWidget::slotNewGame, Qt::QueuedConnection);
    connect(this, &TarotWidget::sigNewDeal, this, &TarotWidget::slotNewDeal, Qt::QueuedConnection);
    connect(this, &TarotWidget::sigAssignedPlace, this, &TarotWidget::slotAssignedPlace, Qt::QueuedConnection);
    connect(this, &TarotWidget::sigTablePlayersList, this, &TarotWidget::slotPlayersList, Qt::QueuedConnection);
    connect(this, &TarotWidget::sigTableMessage, this, &TarotWidget::slotMessage, Qt::QueuedConnection);
    connect(this, &TarotWidget::sigSelectPlayer, this, &TarotWidget::slotSelectPlayer, Qt::QueuedConnection);
    connect(this, &TarotWidget::sigRequestBid, this, &TarotWidget::slotRequestBid, Qt::QueuedConnection);
    connect(this, &TarotWidget::sigShowBid, this, &TarotWidget::slotShowBid, Qt::QueuedConnection);
    connect(this, &TarotWidget::sigShowDog, this, &TarotWidget::slotShowDog, Qt::QueuedConnection);
    connect(this, &TarotWidget::sigStartDeal, this, &TarotWidget::slotStartDeal, Qt::QueuedConnection);
    connect(this, &TarotWidget::sigPlayCard, this, &TarotWidget::slotPlayCard, Qt::QueuedConnection);
    connect(this, &TarotWidget::sigBuildDiscard, this, &TarotWidget::slotBuildDiscard, Qt::QueuedConnection);
    connect(this, &TarotWidget::sigAllPassed, this, &TarotWidget::slotAllPassed, Qt::QueuedConnection);
    connect(this, &TarotWidget::sigEndOfDeal, this, &TarotWidget::slotEndOfDeal, Qt::QueuedConnection);
    connect(this, &TarotWidget::sigEndOfGame, this, &TarotWidget::slotEndOfGame, Qt::QueuedConnection);
    connect(this, &TarotWidget::sigShowCard, this, &TarotWidget::slotShowCard, Qt::QueuedConnection);
    connect(this, &TarotWidget::sigAskForHandle, this, &TarotWidget::slotAskForHandle, Qt::QueuedConnection);
    connect(this, &TarotWidget::sigShowHandle, this, &TarotWidget::slotShowHandle, Qt::QueuedConnection);
    connect(this, &TarotWidget::sigWaitTrick, this, &TarotWidget::slotWaitTrick, Qt::QueuedConnection);
    connect(this, &TarotWidget::sigGameFull, this, &TarotWidget::slotStartGame, Qt::QueuedConnection);
    connect(this, &TarotWidget::sigAutoJoinTable, this, &TarotWidget::slotAutoJoinTable, Qt::QueuedConnection);
    connect(this, &TarotWidget::sigTableQuitEvent, this, &TarotWidget::slotTableQuitEvent, Qt::QueuedConnection);
    connect(this, &TarotWidget::sigDisconnectedFromServer, this, &TarotWidget::slotDisconnectedFromServer, Qt::QueuedConnection);

}
/*****************************************************************************/
/**
 * @brief One time game initialization
 */
void TarotWidget::Initialize(const ServerOptions &opt)
{
    Protocol::GetInstance().Initialize();

    // Ensure that we only have one table (embedded lobby, not the dedicated one!)
    ServerOptions defaultOpt = opt;
    if (defaultOpt.tables.size() != 1)
    {
        defaultOpt.tables.clear();
        defaultOpt.tables.push_back("Default");
    }

    mLobby.Initialize(defaultOpt);
    mClient.Initialize();
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
    // Raise a flag
    mShutdown = true;

    // Close ourself
    mClient.Close();
    mLobby.Stop();
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

    // Connect us to the server
    mClient.ConnectToHost("127.0.0.1", ServerConfig::DEFAULT_GAME_TCP_PORT);
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
void TarotWidget::slotAutoJoinTable()
{
    if (HasLocalConnection())
    {
        // Auto join the embedded server in case of local game only
        mClient.SendJoinTable(1U);
    }
}
/*****************************************************************************/
void TarotWidget::slotAssignedPlace()
{
    if (HasLocalConnection())
    {
        // Now that we have joined the table, we can connect the bots
        // FIXME: add/remove bots depending of the number of players (3, 4 or 5)
        if (mLobby.GetNumberOfBots(1U) == 0U)
        {
            std::map<Place, BotConf>::iterator iter;
            for (iter = mServerOptions.bots.begin(); iter != mServerOptions.bots.end(); ++iter)
            {
                QThread::msleep(50U);
                mLobby.AddBot(1U, iter->second.identity, mServerOptions.timer, iter->second.scriptFilePath);
            }
        }
    }
}
/*****************************************************************************/
void TarotWidget::LaunchLocalGame(Tarot::GameMode mode, const Tarot::Shuffle &sh, bool autoPlay)
{
    // Save game config
    mAutoPlay = autoPlay;
    mGameMode = mode;
    mShuffle = sh;

    InitScreen();

    if (!HasLocalConnection())
    {
        mConnectionType = LOCAL;
        // Connect us to the server
        mClient.ConnectToHost("127.0.0.1", ServerConfig::DEFAULT_LOBBY_TCP_PORT);
    }
    else
    {
        slotStartGame();
    }
}
/*****************************************************************************/
void TarotWidget::LaunchRemoteGame(const std::string &ip, std::uint16_t port)
{
    InitScreen();

    // Connect us to the server
    mClient.ConnectToHost(ip, port);
    if (mClient.IsConnected())
    {
        mConnectionType = REMOTE;
    }
    else
    {
        mConnectionType = NO_CONNECTION;
        emit sigRemoteConnectionFailure();
    }
}
/*****************************************************************************/
void TarotWidget::JoinTable(std::uint32_t tableId)
{
    mClient.SendJoinTable(tableId);
}
/*****************************************************************************/
void TarotWidget::QuitTable(std::uint32_t tableId)
{
    mClient.SendQuitTable(tableId);
}
/*****************************************************************************/
void TarotWidget::slotTableQuitEvent(std::uint32_t tableId)
{
    (void) tableId;
    // We have been kicked from the table OR this is a simple acknowlegment message
    // Clean canvas
    InitScreen(true);
}
/*****************************************************************************/
void TarotWidget::slotDisconnectedFromServer()
{
    InitScreen(true);
}
/*****************************************************************************/
void TarotWidget::Disconnect()
{
    mClient.Disconnect();
}
/*****************************************************************************/
void TarotWidget::LaunchLocalGame(Tarot::GameMode mode, const Tarot::Shuffle &sh, bool autoPlay)
{
    // Save game config
    mAutoPlay = autoPlay;
    mGameMode = mode;
    mShuffle = sh;

    InitScreen();

    if (!HasLocalConnection())
    {
        mConnectionType = LOCAL;
        // Connect us to the server
        mClient.ConnectToHost("127.0.0.1", ServerConfig::DEFAULT_GAME_TCP_PORT);
    }
    else
    {
        slotStartGame();
    }
}
/*****************************************************************************/
void TarotWidget::slotStartGame()
{
    std::uint8_t  numberOfTurns = 1U;
    if (mGameMode == Tarot::TOURNAMENT)
    {
        numberOfTurns = mServerOptions.tournamentTurns;
    }

    mClient.AdminNewGame(mGameMode, mShuffle, numberOfTurns);
}
/*****************************************************************************/
void TarotWidget::InitScreen(bool rawClear)
{
    // GUI initialization
    mCanvas->InitBoard(rawClear);
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
    mCanvas->ShowAvatars(mClientOptions.showAvatars);
    mCanvas->SetBackground(mClientOptions.backgroundColor);
}
/*****************************************************************************/
void TarotWidget::HideTrick()
{
    Deck trick = mClient.GetCurrentTrick();

    for (Deck::ConstIterator it = trick.Begin(); it != trick.End(); ++it)
    {
        mCanvas->HideCard(*it);
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

    for (Deck::ConstIterator it = mDiscard.Begin(); it != mDiscard.End(); ++it)
    {
        mCanvas->HideCard(*it);
    }

    TLogInfo("Hide discard: " + mDiscard.ToString());


    mClient.SetMyDeck(mMySavedDeck); // Re-set previous player deck
    mClient.SetDiscard(mDiscard);    // Then filter out the discard
    ShowSouthCards();
    mClient.SendDiscard(mDiscard);
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
    Deck deck = mClient.GetMyDeck();
    deck.Sort(mClientOptions.cardsOrder);
    mCanvas->DrawSouthCards(deck);
}
/*****************************************************************************/
void TarotWidget::slotClickBoard()
{
    if (mClient.GetSequence() == Client::SHOW_DOG)
    {
        mCanvas->HidePopup();
        mClient.SendSyncDog(); // We have seen the dog, let's inform the server about that
        // Forbid any further clicks
        mCanvas->SetFilter(Canvas::BLOCK_ALL);
    }
    else if (mClient.GetSequence() == Client::SHOW_HANDLE)
    {
        mCanvas->HidePopup();
        ShowSouthCards(); // refresh our cards if the handle displayed is our own one
        mClient.SendSyncHandle(); // We have seen the handle, let's inform the server about that
        // Forbid any further clicks
        mCanvas->SetFilter(Canvas::BLOCK_ALL);
    }
    else if (mClient.GetSequence() == Client::SYNC_TRICK)
    {
        HideTrick();
        mClient.SendSyncTrick();
        // Forbid any further clicks
        mCanvas->SetFilter(Canvas::BLOCK_ALL);
    }
    else if (mClient.GetSequence() == Client::SHOW_SCORE)
    {
        mCanvas->HideMessageBox();
        mClient.SendSyncEndOfDeal();
        // Forbid any further clicks
        mCanvas->SetFilter(Canvas::BLOCK_ALL);
    }

}
/*****************************************************************************/
/**
 * @brief TarotWidget::slotMoveCursor
 *
 * This function is called every time the mouse cursor moves
 *
 * @param index is the card index under the mouse cursor
 */
void TarotWidget::slotMoveCursor(std::uint8_t value, std::uint8_t suit)
{
    Card c(value, suit);

    if (mClient.GetMyDeck().HasCard(c) == false)
    {
        return;
    }

    if (mClient.GetSequence() == Client::BUILD_DISCARD)
    {
        if ((c.GetSuit() == Card::TRUMPS) ||
                ((c.GetSuit() != Card::TRUMPS) && (c.GetValue() == 14)))
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
        if (c.GetSuit() == Card::TRUMPS)
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
void TarotWidget::slotClickCard(std::uint8_t value, std::uint8_t suit, bool selected)
{
    Card c(value, suit);

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

        mClient.SetPlayedCard(c);
        mClient.SendCard(c);

        ShowSouthCards();
    }
    else if (mClient.GetSequence() == Client::BUILD_DISCARD)
    {

        if ((c.GetSuit() == Card::TRUMPS) ||
                ((c.GetSuit() != Card::TRUMPS) && (c.GetValue() == 14)))
        {
            return;
        }

        // select one card
        if (!selected)
        {
            if (mDiscard.Size() == 6U)
            {
                return;
            }
            mDiscard.Append(c);
            if (mDiscard.Size() == 6U)
            {
                mCanvas->DisplayDiscardMenu(true);
            }
        }
        // Un-select card
        else if (selected)
        {
            if (mDiscard.Size() == 0U)
            {
                return;
            }
            mDiscard.Remove(c);
            mCanvas->DisplayDiscardMenu(false);
        }
        mCanvas->ToggleCardSelection(c);
    }
    else if (mClient.GetSequence() == Client::BUILD_HANDLE)
    {
        if (c.GetSuit() == Card::TRUMPS)
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
            mCanvas->ToggleCardSelection(c);
        }
    }
}
/*****************************************************************************/
void TarotWidget::slotSendChatMessage(const QString &message)
{
    mClient.SendTableMessage(message.toStdString());
}
/*****************************************************************************/
void TarotWidget::slotSendLobbyMessage(const QString &message)
{
    mClient.SendLobbyMessage(message.toStdString());
}
/*****************************************************************************/
void TarotWidget::slotMessage(std::string message)
{
    // Nothing to do here!
    (void) message;
}
/*****************************************************************************/
void TarotWidget::slotPlayersList()
{
    mCanvas->SetPlayerIdentity(mClient.GetTablePlayersList(), mClient.GetPlace());
}
/*****************************************************************************/
void TarotWidget::slotNewGame()
{
    mClient.SendSyncNewGame();
}
/*****************************************************************************/
void TarotWidget::slotNewDeal()
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
    if (mAutoPlay)
    {
        mClient.SendBid(Contract::PASS, false);
    }
    else
    {
        mCanvas->ShowBidsChoice(highestBid);
        mCanvas->SetFilter(Canvas::MENU);
    }
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
    Place taker = mClient.GetBid().taker;

    mCanvas->SetFilter(Canvas::BLOCK_ALL);
    mCanvas->ShowTaker(taker, mClient.GetPlace());

    // We are ready, let's inform the server about that
    mClient.SendSyncStart();
}
/*****************************************************************************/
void TarotWidget::slotShowDog()
{
    if (mAutoPlay)
    {
        mClient.SendSyncDog();
    }
    else
    {
        mCanvas->DrawCardsInPopup(mClient.GetDogDeck());
        mCanvas->SetFilter(Canvas::BOARD);
    }
}
/*****************************************************************************/
void TarotWidget::slotShowHandle()
{
    mCanvas->DrawCardsInPopup(mMyHandle);
    mCanvas->SetFilter(Canvas::BOARD);
}
/*****************************************************************************/
void TarotWidget::slotAllPassed()
{
    mCanvas->SetFilter(Canvas::BLOCK_ALL);
    mCanvas->InitBoard();

    if (!mAutoPlay)
    {
        QMessageBox::information(this, trUtf8("Information"),
                                 trUtf8("All the players have passed.\n"
                                        "New deal will start."));
    }
    mClient.SendSyncAllPassed();
}
/*****************************************************************************/
void TarotWidget::slotBuildDiscard()
{
    // We add the dog to the player's deck for discard building
    Deck deck = mClient.GetMyDeck();
    mMySavedDeck = deck;  // locally save the legacy player's deck
    deck.Append(mClient.GetDogDeck()); // Add the dog
    mClient.SetMyDeck(deck);

    mDiscard.Clear();
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

        Card c = mClient.Play();
        mClient.SetPlayedCard(c);
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
    mCanvas->DrawCard(Card(name), p, mClient.GetPlace());

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
    mCanvas->SetResult(mClient.GetDeal().GetScore(), mClient.GetBid());
    mCanvas->SetFilter(Canvas::BOARD);

    if (mClient.GetGameMode() == Tarot::TOURNAMENT)
    {
        emit sigAddScore();
    }
}
/*****************************************************************************/
void TarotWidget::slotEndOfGame(Place winner)
{
    (void) winner;
    /*
    QMessageBox::information(this, trUtf8("Game result"),
                             trUtf8("The winner is ") + QString(winner.ToString().c_str()),
                             QMessageBox::Ok);
    */
    mCanvas->SetFilter(Canvas::MENU);
    mCanvas->DisplayMainMenu(true);
}

//=============================================================================
// End of file TarotClub.cpp
//=============================================================================
