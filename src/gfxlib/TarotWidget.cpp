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
#include <QJsonDocument>

// TarotClub includes
#include "TarotWidget.h"
#include "Common.h"
#include "Log.h"
#include "Defines.h"

/*****************************************************************************/
TarotWidget::TarotWidget(QWidget *parent)
    : QWidget(parent)
    , mSession(*this)
    , mServer(mLobby)
    , mConnectionType(NO_CONNECTION)
    , mAutoPlay(false)
    , mShutdown(false)
    , mSequence(IDLE)
{
    setWindowTitle(QString(TAROT_TITLE.c_str()) + " " + QString(TAROT_VERSION.c_str()));

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mCanvas = new Canvas(this);
    mCanvas->show();

    mainLayout->addWidget(mCanvas);
    setLayout(mainLayout);

    qRegisterMetaType<Place>("Place");
    qRegisterMetaType<Contract>("Contract");
    qRegisterMetaType<Tarot::Distribution>("Tarot::Shuffle");
    qRegisterMetaType<std::string>("std::string");

    // Board click events
    connect(mCanvas, &Canvas::sigViewportClicked, this, &TarotWidget::slotClickBoard);
    connect(mCanvas, &Canvas::sigClickCard, this, &TarotWidget::slotClickCard);
    connect(mCanvas, &Canvas::sigCursorOverCard, this, &TarotWidget::slotMoveCursor);
    connect(mCanvas, &Canvas::sigContract, this, &TarotWidget::slotSetBid);
    connect(mCanvas, &Canvas::sigAcceptDiscard, this, &TarotWidget::slotAcceptDiscard);
    connect(mCanvas, &Canvas::sigAcceptHandle, this, &TarotWidget::slotAcceptHandle);
    connect(mCanvas, &Canvas::sigStartGame, this, &TarotWidget::slotNewQuickGame);
}
/*****************************************************************************/
TarotWidget::~TarotWidget()
{
    if (!mShutdown)
    {
        slotCleanBeforeExit();
    }
}
/*****************************************************************************/
/**
 * @brief One time game initialization
 */
void TarotWidget::Initialize()
{
    mSession.Initialize();
    mCanvas->Initialize();
    mSequence = IDLE;
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

    mServer.Stop();

    // Close ourself
    mSession.Close();
    mBotManager.Close();
    mBotManager.KillBots();
}
/*****************************************************************************/
void TarotWidget::slotNewTournamentGame()
{
    mClient.mGame.mode = Tarot::Game::cSimpleTournament;
    mClient.mGame.deals = mTournamentOptions.turns;

    LaunchLocalGame(false);
}
/*****************************************************************************/
void TarotWidget::slotNewQuickGame()
{
    mClient.mGame.mode = Tarot::Game::cQuickDeal;
    mClient.mGame.deals.clear();
    mClient.mGame.deals.push_back(Tarot::Distribution(Tarot::Distribution::RANDOM_DEAL));

    LaunchLocalGame(false);
}
/*****************************************************************************/
void TarotWidget::NewNumberedDeal(std::uint32_t dealNumber)
{
    mClient.mGame.mode = Tarot::Game::cQuickDeal;
    mClient.mGame.deals.clear();
    mClient.mGame.deals.push_back(Tarot::Distribution(Tarot::Distribution::NUMBERED_DEAL, "", dealNumber));

    LaunchLocalGame(false);
}
/*****************************************************************************/
void TarotWidget::NewCustomDeal(const std::string &file)
{
    mClient.mGame.mode = Tarot::Game::cQuickDeal;
    mClient.mGame.deals.clear();
    mClient.mGame.deals.push_back(Tarot::Distribution(Tarot::Distribution::CUSTOM_DEAL, file));

    LaunchLocalGame(false);
}
/*****************************************************************************/
void TarotWidget::slotCreateHostedGame()
{   
    LaunchHostedGame();
}
/*****************************************************************************/
void TarotWidget::slotNewAutoPlay()
{
    mClient.mGame.mode = Tarot::Game::cQuickDeal;
    mClient.mGame.deals.clear();
    mClient.mGame.deals.push_back(Tarot::Distribution(Tarot::Distribution::RANDOM_DEAL));

    LaunchLocalGame(true);
}
/*****************************************************************************/
bool TarotWidget::HasLocalConnection()
{
    if ((mConnectionType == LOCAL) &&
            (mSession.IsConnected() == true))
    {
        return true;
    }
    else
    {
        return false;
    }
}
/*****************************************************************************/
void TarotWidget::customEvent(QEvent *e)
{
    std::vector<Reply> out;

    if (e->type() == TarotEvent::staticType)
    {
        TarotEvent *ev = dynamic_cast<TarotEvent *>(e);

        // Generic client decoder, fill the context and the client structure
        BasicClient::Event event = mClient.Decode(ev->src_uuid, ev->dst_uuid, ev->arg, mCtx, out);

        switch (event)
        {
        case BasicClient::ACCESS_GRANTED:
        {
            emit sigEnteredLobby();
            AutoJoinTable();
            break;
        }
        case BasicClient::JOIN_TABLE:
        {
            emit sigTableJoinEvent(mClient.mMyself.tableId);
            AddBots(); // if needed
            mClient.Sync(Engine::WAIT_FOR_PLAYERS, out);
            break;
        }
        case BasicClient::NEW_DEAL:
        {
            mClient.UpdateStatistics();
            emit sigNewDeal();
            mCanvas->ResetCards();
            ShowSouthCards();
            mClient.Sync(Engine::WAIT_FOR_CARDS, out);
            break;
        }
        case BasicClient::REQ_BID:
        {
            mCanvas->ShowSelection(mClient.mBid.taker, mClient.mMyself.place);
            if (mClient.mBid.taker == mClient.mMyself.place)
            {
                TLogNetwork("My turn to bid");
                if (mAutoPlay)
                {
                    slotSetBid(Contract(Contract::PASS));
                }
                else
                {
                    mCanvas->ShowBidsChoice(mClient.mBid.contract);
                    mCanvas->SetFilter(Canvas::MENU);
                }
            }
            break;
        }
        case BasicClient::SHOW_BID:
        {
            mCanvas->ShowBid(mClient.mBid.taker, mClient.mBid.contract, mClient.mMyself.place);
            mClient.Sync(Engine::WAIT_FOR_SHOW_BID, out);
            break;
        }
        case BasicClient::SHOW_DOG:
        {
            if (mAutoPlay)
            {
                mClient.Sync(Engine::WAIT_FOR_SHOW_DOG, out);
            }
            else
            {
                mSequence = SHOW_DOG;
                mCanvas->DrawCardsInPopup(mClient.mDog);
                mCanvas->SetFilter(Canvas::BOARD);
            }
            break;
        }

        case BasicClient::START_DEAL:
        {
            mClient.UpdateStatistics();
            mClient.mCurrentTrick.Clear();

            emit sigStartDeal();

            mSequence = IDLE;
            mCanvas->SetFilter(Canvas::BLOCK_ALL);
            mCanvas->ShowTaker(mClient.mBid.taker, mClient.mMyself.place);

            mClient.Sync(Engine::WAIT_FOR_START_DEAL, out);
            break;
        }
        case BasicClient::SHOW_HANDLE:
        {
            mMyHandle = mClient.mHandle;
            mCanvas->DrawCardsInPopup(mMyHandle);
            mSequence = SHOW_HANDLE;
            mCanvas->SetFilter(Canvas::BOARD);
            mClient.Sync(Engine::WAIT_FOR_SHOW_HANDLE, out);
            break;
        }
        case BasicClient::BUILD_DISCARD:
        {
            // We add the dog to the player's deck for discard building
            mMySavedDeck = mClient.mDeck;  // locally save the legacy player's deck
            mClient.mDeck.Append(mClient.mDog); // Add the dog

            mDiscard.Clear();
            mSequence = BUILD_DISCARD;
            mCanvas->SetFilter(Canvas::CARDS | Canvas::MENU);

            // Player's cards are shown
            ShowSouthCards();
            break;
        }
        case BasicClient::NEW_GAME:
        {
            emit sigNewGame();
            mClient.Sync(Engine::WAIT_FOR_READY, out);
            break;
        }
        case BasicClient::SHOW_CARD:
        {
            Card card = mClient.mCurrentTrick.Last();
            emit sigShowCard(mClient.mCurrentPlayer, card.ToString());

            mCanvas->DrawCard(card, mClient.mCurrentPlayer, mClient.mMyself.place);
            mClient.mCurrentTrick.Append(card);
            mClient.Sync(Engine::WAIT_FOR_SHOW_CARD, out);
            break;
        }
        case BasicClient::PLAY_CARD:
        {
            mCanvas->ShowSelection(mClient.mCurrentPlayer, mClient.mMyself.place);
            if (mClient.IsMyTurn())
            {
                if (mAutoPlay)
                {
                    mSequence = IDLE;
                    mCanvas->SetFilter(Canvas::BLOCK_ALL);

                    Card c = mClient.Play();
                    mClient.mDeck.Remove(c);
                    mClient.BuildSendCard(c, out);
                    ShowSouthCards();
                }
                else
                {
                    mSequence = PLAY_TRICK;
                    mCanvas->SetFilter(Canvas::CARDS);
                }
            }
            break;
        }
        case BasicClient::ASK_FOR_HANDLE:
        {
            AskForHandle();
            break;
        }
        case BasicClient::END_OF_TRICK:
        {          
            emit sigWaitTrick(mClient.mCurrentPlayer);

            mSequence = SYNC_END_OF_TRICK;
            mCanvas->SetFilter(Canvas::BOARD);

            // launch timer to clean cards, if needed
            if (mClientOptions.clickToClean == true)
            {
                QTimer::singleShot(mClientOptions.delayBeforeCleaning, this, SLOT(slotClickBoard()));
            }
            else
            {
                mClient.Sync(Engine::WAIT_FOR_END_OF_TRICK, out);
            }

            break;
        }
        case BasicClient::END_OF_GAME:
        {
            // FIXME: deserialize the winner of the game
            /*
            QMessageBox::information(this, trUtf8("Game result"),
                                     trUtf8("The winner is ") + QString(winner.ToString().c_str()),
                                     QMessageBox::Ok);
            */
            mCanvas->SetFilter(Canvas::MENU);
            mCanvas->DisplayMainMenu(true);

            mClient.Sync(Engine::WAIT_FOR_READY, out);
            break;
        }
        case BasicClient::ALL_PASSED:
        {
            mSequence = IDLE;
            mCanvas->SetFilter(Canvas::BLOCK_ALL);
            mCanvas->InitBoard();

            if (!mAutoPlay)
            {
                QMessageBox::information(this, trUtf8("Information"),
                                         trUtf8("All the players have passed.\n"
                                                "New deal will start."));
            }
            mClient.Sync(Engine::WAIT_FOR_ALL_PASSED, out);
            break;
        }

        case BasicClient::MESSAGE:
        {
            // FIXME
            /*
            if (target == mClient.mTableId)
            {
                emit sigTableMessage(message);
            }
            else
            {
                emit sigLobbyMessage(message);
            }
            */

            break;
        }
        case BasicClient::PLAYER_LIST:
            emit sigLobbyPlayersList();
            emit sigTablePlayersList();
           // mCanvas->SetPlayerIdentity(GetTablePlayersList(), mClient.mPlace);
            break;

        case BasicClient::QUIT_TABLE:
            // Clean canvas
            InitScreen(true);
            break;
        case BasicClient::END_OF_DEAL:
        {
            mCanvas->InitBoard();
            mCanvas->ResetCards();
            mCanvas->SetResult(mPoints, mClient.mBid);

            mSequence = SHOW_SCORE;
            mCanvas->SetFilter(Canvas::BOARD);

            emit sigAddScore();
            mClient.Sync(Engine::WAIT_FOR_END_OF_DEAL, out);
            break;
        }

        case BasicClient::JSON_ERROR:
        case BasicClient::BAD_EVENT:
        case BasicClient::REQ_LOGIN:
        case BasicClient::SYNC:
        {
            // Nothing to do for that event
            break;
        }

        default:
            break;
        }
    }
    else if (e->type() == ErrorEvent::staticType)
    {
        ErrorEvent *err = dynamic_cast<ErrorEvent *>(e);

        // Progagate the error code only if the software is not in exit process
        if ((!mShutdown) && err != nullptr)
        {
            if (err->signal == net::IEvent::ErrDisconnectedFromServer)
            {
                mConnectionType = NO_CONNECTION;
                mSession.Close();
                emit sigClientError(tr("Kicked from lobby"), false);
                InitScreen(true);
            }
        }
    }

    mSession.Send(out);
}
/*****************************************************************************/
void TarotWidget::AutoJoinTable()
{
    if (HasLocalConnection())
    {
        // Auto join the embedded server in case of local game only
        std::vector<Reply> out;
        mClient.BuildJoinTable(Protocol::TABLES_UID, out);
        mSession.Send(out);
    }
}
/*****************************************************************************/
void TarotWidget::AddBots()
{
    if (HasLocalConnection())
    {
        bool addBots = true;
        // Now that we have joined the table, we can connect the bots
        // FIXME: add/remove bots depending of the number of players (3, 4 or 5)
        if (mBotIds.size() != 0U)
        {
            // If some bots are stuck in the lobby, make them join the table
            std::map<std::uint32_t, Identity>::iterator iter;
            for (iter = mBotIds.begin(); iter != mBotIds.end(); ++iter)
            {
                QThread::msleep(50U);
                mBotManager.JoinTable(iter->first, Protocol::TABLES_UID);
            }
            addBots = false;
        }

        if (addBots)
        {
            std::map<Place, BotConf>::iterator iter;
            for (iter = mClientOptions.bots.begin(); iter != mClientOptions.bots.end(); ++iter)
            {
                QThread::msleep(50U);
                std::uint32_t botId = mBotManager.AddBot(Protocol::TABLES_UID, iter->second.identity, mClientOptions.timer, iter->second.scriptFilePath);
                mBotIds[botId] = iter->second.identity;
                mBotManager.ConnectBot(botId, "127.0.0.1", mServerOptions.game_tcp_port);
            }
        }
    }
}
/*****************************************************************************/
void TarotWidget::LaunchLocalGame(bool autoPlay)
{    
    // Save game config
    mAutoPlay = autoPlay;

    if (!HasLocalConnection())
    {
        InitScreen(true);
        mConnectionType = LOCAL;
        // Connect us to the server
        mSession.ConnectToHost("127.0.0.1", ServerConfig::DEFAULT_GAME_TCP_PORT);
    }
    else
    {
        InitScreen();

        std::vector<Reply> out;
        mClient.BuildNewGame(out);
        mSession.Send(out);
    }
}
/*****************************************************************************/
void TarotWidget::LaunchRemoteGame(const std::string &ip, std::uint16_t port)
{
    if (mConnectionType != HOSTED)
    {
        // Destroy any local stuff before connecting elsewhere
        mSession.Disconnect();
        mServer.Stop();
        InitScreen(true);
        mConnectionType = REMOTE;
    }

    // Connect us to the server
    mSession.ConnectToHost(ip, port);
}
/*****************************************************************************/
void TarotWidget::LaunchHostedGame()
{
    // Destroy any local stuff before connecting elsewhere
    mSession.Disconnect();
    mServer.Stop();
    InitScreen(true);
    mConnectionType = HOSTED;

    ServerOptions opt = mServerOptions;
    opt.localHostOnly = false;
}
/*****************************************************************************/
void TarotWidget::JoinTable(std::uint32_t tableId)
{
    std::vector<Reply> out;
    mClient.BuildJoinTable(tableId, out);
    mSession.Send(out);
}
/*****************************************************************************/
void TarotWidget::QuitTable(std::uint32_t tableId)
{
    std::vector<Reply> out;
    mClient.BuildQuitTable(tableId, out);
    mSession.Send(out);
}
/*****************************************************************************/
void TarotWidget::Disconnect()
{
    mSession.Disconnect();
}
/*****************************************************************************/
void TarotWidget::InitScreen(bool rawClear)
{
    // GUI initialization
    mCanvas->InitBoard(rawClear);
    mCanvas->ResetCards();
    mSequence = IDLE;
    mCanvas->SetFilter(Canvas::BLOCK_ALL);
}
/*****************************************************************************/
void TarotWidget::ApplyOptions(const ClientOptions &i_clientOpt, const ServerOptions &i_servOpt, const TournamentOptions &i_tournamentOpt)
{
    // Save option structures locally
    mClientOptions = i_clientOpt;
    mServerOptions = i_servOpt;
    mTournamentOptions = i_tournamentOpt;

    // Launch local server if needed
    // Ensure that we only have one table (embedded lobby, not the dedicated one!)
    if (mServerOptions.tables.size() != 1)
    {
        mServerOptions.tables.clear();
        mServerOptions.tables.push_back("Default");
    }

    mLobby.DeleteTables();
    mLobby.Initialize("Local", mServerOptions.tables);

    // Initialize all the objects with the user preferences
    if (mSession.IsConnected())
    {
        // Send the new client identity to the identity server
        // FIXME
      //  mNet.SendPacket(Protocol::ClientChangeIdentity(mClient.GetUuid(), mClientOptions.identity));
    }

    mCanvas->ShowAvatars(mClientOptions.showAvatars);
    mCanvas->SetBackground(mClientOptions.backgroundColor);

    if (mConnectionType != REMOTE)
    {
        mServer.Start(mServerOptions); // Blocking call. On exit, quit the executable


        /* FIXME !!!!!!!!
        // Update bot identities if required
        for (std::map<std::uint32_t, Identity>::iterator iter = mBotIds.begin(); iter != mBotIds.end(); ++iter)
        {
            for (std::map<Place, BotConf>::iterator iter2 = mClientOptions.bots.begin();
                 iter2 != mClientOptions.bots.end(); ++iter2)
            {
                if (iter->second.username == iter2->second.identity.nickname)
                {
                    mBotManager.ChangeBotIdentity(iter->first, iter2->second.identity);
                }
            }
        }
        */
    }

}
/*****************************************************************************/
void TarotWidget::HideTrick()
{
    for (auto &card : mClient.mCurrentTrick)
    {
        mCanvas->HideCard(card);
    }

    mClient.mCurrentTrick.Clear();
}
/*****************************************************************************/
void TarotWidget::slotSetBid(const Contract cont)
{
    std::vector<Reply> out;
    mClient.BuildBid(cont, mCanvas->GetSlamOption(), out);
    mSession.Send(out);
    mCanvas->HideBidsChoice();
}
/*****************************************************************************/
void TarotWidget::slotAcceptDiscard()
{
    // Clear canvas and forbid new actions
    mCanvas->DisplayDiscardMenu(false);
    mCanvas->SetFilter(Canvas::BLOCK_ALL);

    for (Deck::ConstIterator it = mDiscard.begin(); it != mDiscard.end(); ++it)
    {
        mCanvas->HideCard(*it);
    }

    TLogInfo("Hide discard: " + mDiscard.ToString());

    // Re-set previous player deck
    mClient.mDeck.Set(mMySavedDeck);
    // Then filter out the discard
    mClient.mDeck += mClient.mDog;
    mClient.mDeck.RemoveDuplicates(mDiscard);
    ShowSouthCards();

    std::vector<Reply> out;
    mClient.BuildDiscard(mDiscard, out);
    mSession.Send(out);
}
/*****************************************************************************/
void TarotWidget::slotAcceptHandle()
{
    if (mClient.mDeck.TestHandle(mMyHandle) == false)
    {
        QMessageBox::information(this, trUtf8("Information"),
                                 trUtf8("Your handle is not valid.\n"
                                        "Showing the fool means that you have no any more trumps in your deck."));
        return;
    }

    mCanvas->DisplayHandleMenu(false);
    mSequence = PLAY_TRICK;
    mCanvas->SetFilter(Canvas::CARDS);

    std::vector<Reply> out;
    mClient.BuildHandle(mMyHandle, out);
    mSession.Send(out);
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
    mClient.mDeck.Sort(mClientOptions.cardsOrder);
    mCanvas->DrawSouthCards(mClient.mDeck);
}
/*****************************************************************************/
void TarotWidget::slotClickBoard()
{
    std::vector<Reply> out;

    if (mSequence == SHOW_DOG)
    {
        mCanvas->HidePopup();
        // Forbid any further clicks
        mSequence = IDLE;
        mCanvas->SetFilter(Canvas::BLOCK_ALL);

        mClient.Sync(Engine::WAIT_FOR_SHOW_DOG, out);
    }
    else if (mSequence == SHOW_HANDLE)
    {
        mCanvas->HidePopup();
        ShowSouthCards(); // refresh our cards if the handle displayed is our own one
        // Forbid any further clicks
        mSequence = IDLE;
        mCanvas->SetFilter(Canvas::BLOCK_ALL);

        mClient.Sync(Engine::WAIT_FOR_SHOW_HANDLE, out);
    }
    else if (mSequence == SYNC_END_OF_TRICK)
    {
        HideTrick();
        mClient.mCurrentTrick.Clear();
        // Forbid any further clicks
        mSequence = IDLE;
        mCanvas->SetFilter(Canvas::BLOCK_ALL);

        mClient.Sync(Engine::WAIT_FOR_END_OF_TRICK, out);
    }
    else if (mSequence == SHOW_SCORE)
    {
        mCanvas->HideMessageBox();
        // Forbid any further clicks
        mSequence = IDLE;
        mCanvas->SetFilter(Canvas::BLOCK_ALL);

        mClient.Sync(Engine::WAIT_FOR_END_OF_DEAL, out);
    }

    mSession.Send(out);
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

    if (mClient.mDeck.HasCard(c) == false)
    {
        return;
    }

    if (mSequence == BUILD_DISCARD)
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
    else if (mSequence == BUILD_HANDLE)
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
    else if (mSequence == PLAY_TRICK)
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

    if (mClient.mDeck.HasCard(c) == false)
    {
        return;
    }

    if (mSequence == PLAY_TRICK)
    {
        if (mClient.IsValid(c) == false)
        {
            return;
        }
        mCanvas->SetFilter(Canvas::BLOCK_ALL);

        std::vector<Reply> out;
        mClient.BuildSendCard(c, out);
        mClient.mDeck.Remove(c);
        mSession.Send(out);

        ShowSouthCards();
    }
    else if (mSequence == BUILD_DISCARD)
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
    else if (mSequence == BUILD_HANDLE)
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
            if (mClient.mDeck.TestHandle(mMyHandle))
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
//    mNet.SendPacket(Protocol::ClientLobbyMessage(message.toStdString(), mClient.GetUuid(), mClient.mTableId));
}
/*****************************************************************************/
void TarotWidget::slotSendLobbyMessage(const QString &message)
{
//    mNet.SendPacket(Protocol::ClientLobbyMessage(message.toStdString(), mClient.GetUuid(), Protocol::LOBBY_UID));
}
/*****************************************************************************/
void TarotWidget::AskForHandle()
{
    mMyHandle.Clear();
    bool declareHandle = false;

    // If we're about to play the first card, the Player is allowed to declare a handle
    // If a handle exists in the player's deck, we propose to declare it
    if (mClient.mStats.trumps >= 10)
    {
        int ret = QMessageBox::question(this, trUtf8("Handle"),
                                        trUtf8("You have a handle.\n"
                                               "Do you want to declare it?"),
                                        QMessageBox::Yes | QMessageBox::No);
        if (ret == QMessageBox::Yes)
        {
            declareHandle = true;
        }
    }

    if (declareHandle)
    {
        mSequence = BUILD_HANDLE;
        mCanvas->SetFilter(Canvas::CARDS);
    }
    else
    {
        // no available handle, send an empty deck
        std::vector<Reply> out;
        mClient.BuildHandle(Deck(), out);
        mSession.Send(out);
    }
}
/*****************************************************************************/
void TarotWidget::Signal(std::uint32_t sig)
{
    ErrorEvent *err = new ErrorEvent();

    switch(sig)
    {
    // Filter on possible reasons
    case net::IEvent::ErrCannotConnectToServer:
    case net::IEvent::ErrDisconnectedFromServer:
        err->signal = sig;
        QApplication::postEvent(this, reinterpret_cast<QEvent*>(err));
        break;
    default:
        break;
    }
}
/*****************************************************************************/
bool TarotWidget::Deliver(uint32_t src_uuid, uint32_t dest_uuid, const std::string &arg, std::vector<Reply> &out)
{
    bool ret = true;
    (void) out;

    if (mClient.mMyself.uuid != Protocol::INVALID_UID)
    {
        if (dest_uuid != mClient.mMyself.uuid)
        {
            ret = false;
        }
    }

    if (ret)
    {
        TarotEvent *tev = new TarotEvent();

        tev->src_uuid = src_uuid;
        tev->dst_uuid = dest_uuid;
        tev->arg = arg;

        QApplication::postEvent(this, reinterpret_cast<QEvent*>(tev));
    }
    return ret;
}
/*****************************************************************************/
uint32_t TarotWidget::AddUser(std::vector<Reply> &)
{
    return 0U;
}
/*****************************************************************************/
void TarotWidget::RemoveUser(uint32_t, std::vector<Reply> &)
{

}
/*****************************************************************************/
uint32_t TarotWidget::GetUuid()
{
    return mClient.mMyself.uuid;
}

//=============================================================================
// End of file TarotClub.cpp
//=============================================================================
