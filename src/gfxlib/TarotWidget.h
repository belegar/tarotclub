/*=============================================================================
 * TarotWidget - TarotWidget.h
 *=============================================================================
 * Graphical TarotWidget client class, contains an embedded server.
 *=============================================================================
 * TarotWidget ( http://www.tarotclub.fr ) - This file is part of TarotClub
 * Copyright (C) 2003-2999 - Anthony Rabine
 * anthony@TarotWidget.fr
 *
 * TarotWidget is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * TarotWidget is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with TarotWidget.  If not, see <http://www.gnu.org/licenses/>.
 *
 *=============================================================================
 */

#ifndef TAROT_WIDGET_H_
#define TAROT_WIDGET_H_

#include <QWidget>
#include <QEvent>

// Game includes
#include "JsonClient.h"
#include "NetClient.h"
#include "Common.h"
#include "LobbyServer.h"
#include "ClientConfig.h"
#include "ServerConfig.h"
#include "GfxCard.h"
#include "Canvas.h"
#include "BotManager.h"

/*****************************************************************************/
/**
 * joueur 0 : humain SUD
 * joueur 1 : ordi EST
 * joueur 2 : ordi NORD (ou NORD-EST)
 * joueur 3 : ordi OUEST
 */
class TarotWidget : public QWidget, public JsonClient::IEvent, public NetClient::IEvent
{
    Q_OBJECT

public:
    enum ConnectionType
    {
        NO_CONNECTION,
        LOCAL,
        REMOTE
    };

    enum Sequence
    {
        IDLE,
        SHOW_DOG,
        BUILD_DISCARD,
        BUILD_HANDLE,
        SHOW_HANDLE,
        PLAY_TRICK,
        SYNC_END_OF_TRICK,
        SHOW_SCORE
    };

    class TarotEvent : public QEvent
    {
    public:

        static const QEvent::Type staticType = static_cast<QEvent::Type>(QEvent::User + 462);
        TarotEvent()
            : QEvent(staticType)
        {

        }
        QByteArray data;
    };

    class ErrorEvent : public QEvent
    {
    public:

        static const QEvent::Type staticType = static_cast<QEvent::Type>(QEvent::User + 463);
        ErrorEvent()
            : QEvent(staticType)
            , quitServer(false)
        {

        }
        QString reason;
        bool quitServer;
    };

    TarotWidget(QWidget *parent = 0);
    ~TarotWidget();

    void Initialize();
    void NewNumberedDeal(std::uint32_t dealNumber);
    void NewCustomDeal(const std::string &file);
    void LaunchLocalGame(bool autoPlay);
    void LaunchRemoteGame(const std::string &ip, std::uint16_t port);
    void JoinTable(std::uint32_t tableId);
    void QuitTable(std::uint32_t tableId);
    void Disconnect();

    // Configuration management
    void ApplyOptions(const ClientOptions &i_clientOpt, const ServerOptions &i_servOpt);

    // Getters about various current game information, external usage
    QMap<Place, Identity> GetTablePlayersList()
    {
        QMap<Place, Identity> opponents;
        for (QMap<Place, std::uint32_t>::iterator iter = mTablePlayers.begin();
             iter != mTablePlayers.end();
             ++iter)
        {
            opponents[iter.key()] = mLobbyUsers[iter.value()];
        }
        return opponents;
    }

    QMap<std::uint32_t, Identity> GetLobbyPlayersList()
    {
        return mLobbyUsers;
    }

    Tarot::Bid GetBid()
    {
        return mClient.mBid;
    }
    Tarot::Distribution GetShuffle()
    {
        return mClient.mShuffle;
    }
    Points GetPoints()
    {
        return mPoints;
    }
    std::string GetResult()
    {
        return mClient.mResult;
    }
    Deck GetDeck()
    {
        return mClient;
    }

    std::uint8_t GetGameMode()
    {
        return mClient.mGame.mode;
    }

    QMap<QString, std::uint32_t> GetTableList()
    {
        return mTables;
    }

    ConnectionType GetConnectionType() { return mConnectionType; }

signals:
    // These signals are used internally and made accessible in public for any external entity
    void sigClientError(QString, bool);
    void sigDisconnectedFromServer();
    void sigEnteredLobby();
    void sigTableQuitEvent(std::uint32_t tableId);
    void sigTableJoinEvent(std::uint32_t tableId);
    void sigNewGame();
    void sigTablePlayersList();
    void sigLobbyPlayersList();
    void sigShowCard(Place, std::string);
    void sigWaitTrick(Place);
    void sigStartDeal();
    void sigTableMessage(QString);
    void sigLobbyMessage(QString);
    void sigAddScore();
    void sigNewDeal();

public slots:
    // These slots are made available to link them to any external widget
    void slotNewTournamentGame();
    void slotNewQuickGame();
    void slotCreateNetworkGame();
    void slotNewAutoPlay();
    void slotCleanBeforeExit();
    void slotSendChatMessage(const QString &message);
    void slotSendLobbyMessage(const QString &message);

private:
    std::shared_ptr<JsonClient> mDecoder;
    NetClient mNet;
    Lobby           mLobby;    // Embedded lobby into this executable
    LobbyServer     mLobbyServer;
    ClientOptions   mClientOptions;
    ServerOptions   mServerOptions;
    ClientHelper    mClient;
    Deck            mDiscard;
    ConnectionType  mConnectionType;
    Canvas          *mCanvas;
    Deck            mMyHandle;
    bool            mAutoPlay;
    Deck            mMySavedDeck;
    bool            mShutdown;
    BotManager      mBotManager;
    Sequence        mSequence;
    Points          mPoints;
    std::map<std::uint32_t, Identity> mBotIds; // bot id, identity
    QMap<Place, std::uint32_t> mTablePlayers;  // players around the table
    QMap<std::uint32_t, Identity> mLobbyUsers; // pair of uuid, names
    QMap<QString, std::uint32_t> mTables;

    // Helpers
    void ShowSouthCards();
    void HideTrick();
    void InitScreen(bool rawClear = false);
    bool HasLocalConnection();
    void AutoJoinTable();
    void AddBots();
    void AskForHandle();

    // From JsonClient::IEvent
    virtual void EmitEvent(const std::string &event);
    virtual void EmitError(std::uint32_t errorId);

    // From NetClient::IEvent
    virtual void NetSignal(std::uint32_t sig);

    void customEvent(QEvent *e);

private slots:
    // Board events
    void slotAcceptHandle();
    void slotAcceptDiscard();
    void slotSetEnchere(Contract cont);
    void slotClickCard(std::uint8_t value, std::uint8_t suit, bool selected);
    void slotClickBoard();
    void slotMoveCursor(std::uint8_t value, std::uint8_t suit);
};

#endif // TAROT_WIDGET_H_

//=============================================================================
// End of file TarotWidget.h
//=============================================================================
