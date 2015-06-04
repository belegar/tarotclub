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
    , mDecoder(new JsonClient(*this))
    , mNet(std::shared_ptr<Protocol::IWorkItem>(mDecoder), *this)
    , mLobbyServer(mLobby)
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
    connect(mCanvas, &Canvas::sigContract, this, &TarotWidget::slotSetEnchere);
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
    Protocol::GetInstance().Initialize();
    mNet.Initialize();
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

    // Stop work item thread
    Protocol::GetInstance().Stop();

    // Close ourself
    mNet.Close();
    mLobbyServer.Stop();
}
/*****************************************************************************/
void TarotWidget::slotNewTournamentGame()
{
    mClient.mGame.mode = Tarot::Game::cSimpleTournament;
    mClient.mGame.deals = mServerOptions.tournament;

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
void TarotWidget::slotCreateNetworkGame()
{
    // Connect us to the server
    mNet.ConnectToHost("127.0.0.1", ServerConfig::DEFAULT_GAME_TCP_PORT);
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
            (mNet.IsConnected() == true))
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
    if (e->type() == TarotEvent::staticType)
    {
        TarotEvent *ev = dynamic_cast<TarotEvent *>(e);

        QJsonParseError error;
        QJsonDocument json = QJsonDocument::fromJson(ev->data, &error);

        if (error.error != QJsonParseError::NoError)
        {
            TLogError("Json parse error: " + error.errorString().toStdString());
        }

        QJsonObject object = json.object();
        QString cmd = object["cmd"].toString();

        if (cmd == "RequestLogin")
        {
            int uuid = object["uuid"].toInt();
            mClient.SetUuid(static_cast<std::uint32_t>(uuid));
            mNet.SendPacket(Protocol::ClientReplyLogin(mClient.GetUuid(), mClientOptions.identity));
        }
        else if (cmd == "EnteredLobby")
        {
            QJsonArray tables = object["tables"].toArray();
            mTables.clear();
            for (int i = 0; i < tables.size(); i++)
            {
               QJsonObject table = tables[i].toObject();
               std::uint32_t uid = static_cast<std::uint32_t>(table["id"].toInt());
               QString name = table["name"].toString();
               mTables[name] = uid;
            }
            emit sigEnteredLobby();
            AutoJoinTable();
        }
        else if (cmd == "ChatMessage")
        {
            std::uint32_t src_uuid = static_cast<std::uint32_t>(object["origin"].toInt());
            QString message = object["message"].toString();
            if (src_uuid == mClient.mTableId)
            {
                emit sigTableMessage(message);
            }
            else
            {
                emit sigLobbyMessage(message);
            }
        }
        else if (cmd == "KickedFromLobby")
        {
            mConnectionType = NO_CONNECTION;
            mNet.Close();
            emit sigClientError(tr("Kicked from lobby"));
            InitScreen(true);
        }
        else if (cmd == "LobbyPlayersList")
        {
            QJsonArray players = object["players"].toArray();
            mLobbyUsers.clear();
            for (int i = 0; i < players.size(); i++)
            {
               QJsonObject player = players[i].toObject();
               std::uint32_t uuid = static_cast<std::uint32_t>(player["uuid"].toInt());
               Identity identity;

               identity.avatar = player["avatar"].toString().toStdString();
               identity.nickname = player["nickname"].toString().toStdString();
               identity.gender = static_cast<std::uint8_t>(player["gender"].toInt());
               mLobbyUsers[uuid] = identity;
            }

            emit sigLobbyPlayersList();
        }
        else if (cmd == "AdminGameFull")
        {
            // FIXME: parameter "full" can be used to display specific things on the GUI
            mNet.SendPacket(Protocol::AdminNewGame(mClient.mGame, mClient.GetUuid(), mClient.mTableId));
        }
        else if (cmd == "TableJoinEvent")
        {
            mClient.mPlace = Place(object["place"].toString().toStdString());
            mClient.mTableId = static_cast<std::uint32_t>(object["table"].toInt());
            mClient.mNbPlayers = static_cast<std::uint8_t>(object["table"].toInt());

            emit sigTableJoinEvent(mClient.mTableId);
            AddBots();
            mNet.SendPacket(Protocol::ClientSyncJoinTable(mClient.GetUuid(), mClient.mTableId));
        }
        else if (cmd == "TableQuitEvent")
        {
            std::uint32_t tableId = static_cast<std::uint32_t>(object["table"].toInt());
            emit sigTableQuitEvent(tableId);
            // We have been kicked from the table OR this is a simple acknowlegment message
            // Clean canvas
            InitScreen(true);
        }
        else if (cmd == "TablePlayersList")
        {
            QJsonArray players = object["players"].toArray();
            mTablePlayers.clear();
            for (int i = 0; i < players.size(); i++)
            {
               QJsonObject player = players[i].toObject();

               std::uint32_t uuid = static_cast<std::uint32_t>(player["uuid"].toInt());
               Place place = Place(player["place"].toString().toStdString());
               mTablePlayers[place] = uuid;
            }

            emit sigTablePlayersList();
            mCanvas->SetPlayerIdentity(GetTablePlayersList(), mClient.mPlace);
        }
        else if (cmd == "NewGame")
        {
            mClient.mGame.mode = static_cast<std::uint8_t>(object["mode"].toInt());

            emit sigNewGame();
            mNet.SendPacket(Protocol::ClientSyncNewGame(mClient.GetUuid(), mClient.mTableId));
        }
        else if (cmd == "NewDeal")
        {
            mClient.SetCards(object["cards"].toString().toStdString());
            emit sigNewDeal();
            mCanvas->ResetCards();
            ShowSouthCards();
            mNet.SendPacket(Protocol::ClientSyncNewDeal(mClient.GetUuid(), mClient.mTableId));
        }
        else if (cmd == "RequestBid")
        {
            Place place = Place(object["place"].toString().toStdString());
            Contract contract = Contract(object["contract"].toString().toStdString());

            mCanvas->ShowSelection(place, mClient.mPlace);
            if (place == mClient.mPlace)
            {
                if (mAutoPlay)
                {
                    mNet.SendPacket(Protocol::ClientBid(Contract::PASS, false, mClient.GetUuid(), mClient.mTableId));
                }
                else
                {
                    mCanvas->ShowBidsChoice(contract);
                    mCanvas->SetFilter(Canvas::MENU);
                }
            }
        }
        else if (cmd == "ShowBid")
        {
            Place place = Place(object["place"].toString().toStdString());
            Contract contract = Contract(object["contract"].toString().toStdString());
            bool slam = object["slam"].toBool();

            // FIXME: show the announced slam on the screen
            Q_UNUSED(slam);

            mCanvas->ShowBid(place, contract, mClient.mPlace);
            mNet.SendPacket(Protocol::ClientSyncBid(mClient.GetUuid(), mClient.mTableId));
        }
        else if (cmd == "AllPassed")
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
            mNet.SendPacket(Protocol::ClientSyncAllPassed(mClient.GetUuid(), mClient.mTableId));
        }
        else if (cmd == "ShowDog")
        {
            mClient.mDog.SetCards(object["dog"].toString().toStdString());
            if (mAutoPlay)
            {
                mNet.SendPacket(Protocol::ClientSyncDog(mClient.GetUuid(), mClient.mTableId));
            }
            else
            {
                mSequence = SHOW_DOG;
                mCanvas->DrawCardsInPopup(mClient.mDog);
                mCanvas->SetFilter(Canvas::BOARD);
            }
        }
        else if (cmd == "BuildDiscard")
        {
            // We add the dog to the player's deck for discard building
            mMySavedDeck = mClient;  // locally save the legacy player's deck
            mClient.Append(mClient.mDog); // Add the dog

            mDiscard.Clear();
            mSequence = BUILD_DISCARD;
            mCanvas->SetFilter(Canvas::CARDS | Canvas::MENU);

            // Player's cards are shown
            ShowSouthCards();
        }
        else if (cmd == "StartDeal")
        {
            mClient.mBid.taker = Place(object["taker"].toString().toStdString());
            mClient.mBid.contract = Contract(object["contract"].toString().toStdString());
            mClient.mBid.slam = object["slam"].toBool();

            emit sigStartDeal();

            mSequence = IDLE;
            mCanvas->SetFilter(Canvas::BLOCK_ALL);
            mCanvas->ShowTaker(mClient.mBid.taker, mClient.mPlace);
            mClient.mCurrentTrick.Clear();

            // We are ready, let's inform the server about that
            mNet.SendPacket(Protocol::ClientSyncStart(mClient.GetUuid(), mClient.mTableId));

        }
        else if (cmd == "AskForHandle")
        {
            AskForHandle();
        }
        else if (cmd == "ShowHandle")
        {
            Place place = Place(object["place"].toString().toStdString());
            mMyHandle.SetCards(object["handle"].toString().toStdString());

            Team team = DEFENSE;
            if (place == mClient.mBid.taker)
            {
                team = ATTACK;
            }
            mMyHandle.SetOwner(team);

            mCanvas->DrawCardsInPopup(mMyHandle);
            mSequence = SHOW_HANDLE;
            mCanvas->SetFilter(Canvas::BOARD);
        }
        else if (cmd == "ShowCard")
        {
            Place place = Place(object["place"].toString().toStdString());
            Card card = Card(object["card"].toString().toStdString());
            emit sigShowCard(place, card.GetName());

            mCanvas->DrawCard(card, place, mClient.mPlace);
            mClient.mCurrentTrick.Append(card);

            // We have seen the card, let's inform the server about that
            mNet.SendPacket(Protocol::ClientSyncShowCard(mClient.GetUuid(), mClient.mTableId));
        }
        else if (cmd == "PlayCard")
        {
            Place place = Place(object["place"].toString().toStdString());

            mCanvas->ShowSelection(place, mClient.mPlace);
            if (place == mClient.mPlace)
            {
                if (mAutoPlay)
                {
                    mSequence = IDLE;
                    mCanvas->SetFilter(Canvas::BLOCK_ALL);

                    Card c = mClient.Play();
                    mClient.Remove(c);
                    mNet.SendPacket(Protocol::ClientCard(c.GetName(), mClient.GetUuid(), mClient.mTableId));
                    ShowSouthCards();
                }
                else
                {
                    mSequence = PLAY_TRICK;
                    mCanvas->SetFilter(Canvas::CARDS);
                }
            }
        }
        else if (cmd == "EndOfTrick")
        {
            Place winner = Place(object["place"].toString().toStdString());
            emit sigWaitTrick(winner);

            mSequence = SYNC_END_OF_TRICK;
            mCanvas->SetFilter(Canvas::BOARD);

            // launch timer to clean cards, if needed
            if (mClientOptions.clickToClean == true)
            {
                QTimer::singleShot(mClientOptions.delayBeforeCleaning, this, SLOT(slotClickBoard()));
            }
        }
        else if (cmd == "EndOfDeal")
        {
            mPoints.pointsAttack = object["points"].toInt();
            mPoints.scoreAttack = object["score"].toInt();
            mPoints.oudlers = object["oudlers"].toInt();
            mPoints.littleEndianPoints = object["little_bonus"].toInt();
            mPoints.handlePoints = object["handle_bonus"].toInt();
            mPoints.slamPoints = object["slam_bonus"].toInt();

            mCanvas->InitBoard();
            mCanvas->ResetCards();
            mCanvas->SetResult(mPoints, mClient.mBid);

            mSequence = SHOW_SCORE;
            mCanvas->SetFilter(Canvas::BOARD);

            if (mClient.mGame.mode == Tarot::Game::cSimpleTournament)
            {
                emit sigAddScore();
            }
        }
        else if (cmd == "EndOfGame")
        {
            // FIXME: deserialize the winner of the game
            /*
            QMessageBox::information(this, trUtf8("Game result"),
                                     trUtf8("The winner is ") + QString(winner.ToString().c_str()),
                                     QMessageBox::Ok);
            */
            mCanvas->SetFilter(Canvas::MENU);
            mCanvas->DisplayMainMenu(true);
        }
        else
        {
            TLogError("Command not supported.");
        }
    }
    if (e->type() == ErrorEvent::staticType)
    {
        ErrorEvent *err = dynamic_cast<ErrorEvent *>(e);

        // Progagate the error code only if the software is not in exit process
        if (!mShutdown)
        {
            emit sigClientError(err->reason);
        }
    }
}
/*****************************************************************************/
void TarotWidget::AutoJoinTable()
{
    if (HasLocalConnection())
    {
        // Auto join the embedded server in case of local game only
        mNet.SendPacket(Protocol::ClientJoinTable(mClient.GetUuid(), Protocol::TABLES_UID));
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

    // Launch local server if needed
    // Ensure that we only have one table (embedded lobby, not the dedicated one!)
    if (mServerOptions.tables.size() != 1)
    {
        mServerOptions.tables.clear();
        mServerOptions.tables.push_back("Default");
    }

    mLobbyServer.Initialize(mServerOptions);

    if (!HasLocalConnection())
    {
        InitScreen(true);
        mConnectionType = LOCAL;
        // Connect us to the server
        mNet.ConnectToHost("127.0.0.1", ServerConfig::DEFAULT_GAME_TCP_PORT);
    }
    else
    {
        InitScreen();
        mNet.SendPacket(Protocol::AdminNewGame(mClient.mGame, mClient.GetUuid(), mClient.mTableId));
    }
}
/*****************************************************************************/
void TarotWidget::LaunchRemoteGame(const std::string &ip, std::uint16_t port)
{
    // Destroy any local stuff before connecting elsewhere
    mNet.Disconnect();
    mLobbyServer.Stop();
    mLobbyServer.WaitForEnd();
    InitScreen(true);
    mConnectionType = REMOTE;

    // Connect us to the server
    mNet.ConnectToHost(ip, port);
}
/*****************************************************************************/
void TarotWidget::JoinTable(std::uint32_t tableId)
{
    mNet.SendPacket(Protocol::ClientJoinTable(mClient.GetUuid(), tableId));
}
/*****************************************************************************/
void TarotWidget::QuitTable(std::uint32_t tableId)
{
    mNet.SendPacket(Protocol::ClientQuitTable(mClient.GetUuid(), tableId));
}
/*****************************************************************************/
void TarotWidget::Disconnect()
{
    mNet.Disconnect();
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
void TarotWidget::ApplyOptions(const ClientOptions &i_clientOpt, const ServerOptions &i_servOpt)
{
    // Save option structures locally
    mClientOptions = i_clientOpt;
    mServerOptions = i_servOpt;

    // Initialize all the objects with the user preferences
    if (mNet.IsConnected())
    {
        // Send the new client identity to the server
        mNet.SendPacket(Protocol::ClientChangeIdentity(mClient.GetUuid(), mClientOptions.identity));
    }

    mCanvas->ShowAvatars(mClientOptions.showAvatars);
    mCanvas->SetBackground(mClientOptions.backgroundColor);

    if (mConnectionType == LOCAL)
    {
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
    Deck trick = mClient.mCurrentTrick;

    for (Deck::ConstIterator it = trick.Begin(); it != trick.End(); ++it)
    {
        mCanvas->HideCard(*it);
    }
}
/*****************************************************************************/
void TarotWidget::slotSetEnchere(Contract cont)
{
    mNet.SendPacket(Protocol::ClientBid(cont, mCanvas->GetSlamOption(), mClient.GetUuid(),  mClient.mTableId));
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

    // Re-set previous player deck
    mClient = mMySavedDeck;
    // Then filter out the discard
    mClient += mClient.mDog;
    mClient.RemoveDuplicates(mDiscard);
    ShowSouthCards();
    mNet.SendPacket(Protocol::ClientDiscard(mDiscard, mClient.GetUuid(), mClient.mTableId));
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
    mSequence = PLAY_TRICK;
    mCanvas->SetFilter(Canvas::CARDS);
    mNet.SendPacket(Protocol::ClientHandle(mMyHandle, mClient.GetUuid(), mClient.mTableId));
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
    mClient.Sort(mClientOptions.cardsOrder);
    mCanvas->DrawSouthCards(mClient);
}
/*****************************************************************************/
void TarotWidget::slotClickBoard()
{
    if (mSequence == SHOW_DOG)
    {
        mCanvas->HidePopup();
        // We have seen the dog, let's inform the server about that
        mNet.SendPacket(Protocol::ClientSyncDog(mClient.GetUuid(), mClient.mTableId));
        // Forbid any further clicks
        mSequence = IDLE;
        mCanvas->SetFilter(Canvas::BLOCK_ALL);
    }
    else if (mSequence == SHOW_HANDLE)
    {
        mCanvas->HidePopup();
        ShowSouthCards(); // refresh our cards if the handle displayed is our own one
        // We have seen the handle, let's inform the server about that
        mNet.SendPacket(Protocol::ClientSyncHandle(mClient.GetUuid(), mClient.mTableId));
        // Forbid any further clicks
        mSequence = IDLE;
        mCanvas->SetFilter(Canvas::BLOCK_ALL);
    }
    else if (mSequence == SYNC_END_OF_TRICK)
    {
        HideTrick();
        mClient.mCurrentTrick.Clear();
        mNet.SendPacket(Protocol::ClientSyncTrick(mClient.GetUuid(), mClient.mTableId));
        // Forbid any further clicks
        mSequence = IDLE;
        mCanvas->SetFilter(Canvas::BLOCK_ALL);
    }
    else if (mSequence == SHOW_SCORE)
    {
        mCanvas->HideMessageBox();
        mNet.SendPacket(Protocol::ClientSyncEndOfDeal(mClient.GetUuid(), mClient.mTableId));
        // Forbid any further clicks
        mSequence = IDLE;
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

    if (mClient.HasCard(c) == false)
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

    if (mClient.HasCard(c) == false)
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
        mClient.Remove(c);
        mNet.SendPacket(Protocol::ClientCard(c.GetName(), mClient.GetUuid(), mClient.mTableId));
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
    mNet.SendPacket(Protocol::ClientLobbyMessage(message.toStdString(), mClient.GetUuid(), mClient.mTableId));
}
/*****************************************************************************/
void TarotWidget::slotSendLobbyMessage(const QString &message)
{
    mNet.SendPacket(Protocol::ClientLobbyMessage(message.toStdString(), mClient.GetUuid(), Protocol::LOBBY_UID));
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
        mNet.SendPacket(Protocol::ClientHandle(mMyHandle, mClient.GetUuid(), mClient.mTableId));
    }
}
/*****************************************************************************/
void TarotWidget::NetSignal(uint32_t sig)
{
    ErrorEvent *err = new ErrorEvent();

    switch(sig)
    {
    case NetClient::IEvent::ErrCannotConnectToServer:
        err->reason = tr("Cannot connect to server");
        break;
    case NetClient::IEvent::ErrDisconnectedFromServer:
        err->reason = tr("Disconnected from server");
        break;
    default:
        err->reason = tr("Unknown error");
        break;
    }
    QApplication::postEvent(this, reinterpret_cast<QEvent*>(err));
}
/*****************************************************************************/
void TarotWidget::EmitEvent(const std::string &event)
{
    TarotEvent *tev = new TarotEvent();
    tev->data.append(event.c_str());
    QApplication::postEvent(this, reinterpret_cast<QEvent*>(tev));
}
/*****************************************************************************/
void TarotWidget::EmitError(uint32_t errorId)
{
    ErrorEvent *err = new ErrorEvent();

    switch(errorId)
    {
    case JsonClient::IEvent::ErrLobbyAccessRefused:
        err->reason = tr("Lobby access refused");
        break;
    case JsonClient::IEvent::ErrTableAccessRefused:
        err->reason = tr("Table access refused");
        break;
    case JsonClient::IEvent::ErrTableFull:
        err->reason = tr("Table is full, cannot join the game");
        break;
    default:
        err->reason = tr("Unknown error");
        break;
    }

    QApplication::postEvent(this, reinterpret_cast<QEvent*>(err));
}

//=============================================================================
// End of file TarotClub.cpp
//=============================================================================
