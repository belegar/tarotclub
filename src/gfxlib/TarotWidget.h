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
        STOPPED,
        IDLE,
        SHOW_DOG,
        BUILD_DISCARD,
        BUILD_HANDLE,
        SYNC_START,
        SHOW_HANDLE,
        SYNC_CARD,
        PLAY_TRICK,
        SYNC_TRICK,
        SYNC_READY,
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
        std::string event;
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
    std::map<Place, Identity> GetTablePlayersList()
    {
        return mClient.mPlayersIdent;
    }

    std::map<std::uint32_t, std::string> GetLobbyPlayersList()
    {
        return mClient.mLobbyUsers;
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
        return mClient.mPoints;
    }
    std::string GetResult()
    {
        return mClient.mResult;
    }
    Deck GetDeck()
    {
        return mClient;
    }

    std::map<std::string, std::uint32_t> GetTableList()
    {
        return mClient.mTableList;
    }

    ConnectionType GetConnectionType() { return mConnectionType; }

signals:
    // These signals are used internally and made accessible in public for any external entity
    void sigClientError(std::uint32_t errorId);
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
    void sigTableMessage(std::string);
    void sigLobbyMessage(std::string);
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
    void slotStartGame();

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
    std::map<std::uint32_t, Identity> mBotIds; // bot id, identity

    // Helpers
    void ShowSouthCards();
    void HideTrick();
    void InitScreen(bool rawClear = false);
    bool HasLocalConnection();

    // From JsonClient::IEvent
    virtual void EmitEvent(const std::string &event);

    // From NetClient::IEvent
    virtual void NetSignal(std::uint32_t sig);

/*
    // Client events
    virtual void Error(std::uint32_t errorId)
    {
        switch(errorId)
        {
        default:
        case Client::IEvent::ErrCannotConnectToServer:
        case Client::IEvent::ErrDisconnectedFromServer:
        case Client::IEvent::ErrLobbyAccessRefused:
            mConnectionType = NO_CONNECTION;
            emit sigDisconnectedFromServer();
            break;
        }

        // Progagate the error code only if the software is not in exit process
        if (!mShutdown)
        {
            emit sigClientError(errorId);
        }
    }

    virtual void EnteredLobby()
    {
        emit sigEnteredLobby();
        emit sigAutoJoinTable();
    }

    virtual void TableMessage(const std::string &message)
    {
        emit sigTableMessage(message);
    }
    virtual void LobbyMessage(const std::string &message)
    {
        emit sigLobbyMessage(message);
    }

    virtual void TableQuitEvent(std::uint32_t tableId)
    {
        emit sigTableQuitEvent(tableId);
    }

    virtual void TableJoinEvent(std::uint32_t tableId)
    {
        emit sigTableJoinEvent(tableId);
        emit sigAssignedPlace();
    }
    virtual void TablePlayersList()
    {
        emit sigTablePlayersList();
    }

    virtual void LobbyPlayersList()
    {
        emit sigLobbyPlayersList();
    }

    virtual void AdminGameFull()
    {
        emit sigGameFull();
    }
    virtual void NewGame()
    {
        emit sigNewGame();
    }
    virtual void NewDeal()
    {
        emit sigNewDeal();
    }
    virtual void SelectPlayer(Place p)
    {
        emit sigSelectPlayer(p);
    }
    virtual void RequestBid(Contract highestBid)
    {
        emit sigRequestBid(highestBid);
    }
    virtual void ShowBid(Place p, bool slam, Contract c)
    {
        emit sigShowBid(p, slam, c);
    }
    virtual void StartDeal()
    {
        emit sigStartDeal();
    }
    virtual void ShowDog()
    {
        emit sigShowDog();
    }
    virtual void AskForHandle()
    {
        emit sigAskForHandle();
    }
    virtual void ShowHandle()
    {
        emit sigShowHandle();
    }
    virtual void BuildDiscard()
    {
        emit sigBuildDiscard();
    }
    virtual void AllPassed()
    {
        emit sigAllPassed();
    }
    virtual void PlayCard()
    {
        emit sigPlayCard();
    }
    virtual void ShowCard(Place p, const std::string &name)
    {
        emit sigShowCard(p, name);
    }
    virtual void WaitTrick(Place winner)
    {
        emit sigWaitTrick(winner);
    }
    virtual void EndOfDeal()
    {
        emit sigEndOfDeal();
    }
    virtual void EndOfGame(Place winner)
    {
        emit sigEndOfGame(winner);
    }
    */

    void customEvent(QEvent *e);

private slots:
    // Client events
    void slotAutoJoinTable();
    void slotNewGame();
    void slotNewDeal();
    void slotAssignedPlace();
    void slotPlayersList();
    void slotMessage(std::string message);
    void slotSelectPlayer(Place p);
    void slotRequestBid(Contract highestBid);
    void slotShowBid(Place p, bool slam, Contract c);
    void slotShowDog();
    void slotStartDeal();
    void slotAskForHandle();
    void slotShowHandle();
    void slotBuildDiscard();
    void slotAllPassed();
    void slotPlayCard();
    void slotShowCard(Place p, std::string name);
    void slotWaitTrick(Place p);
    void slotEndOfDeal();
    void slotEndOfGame(Place winner);
    void slotTableQuitEvent(std::uint32_t tableId);
    void slotDisconnectedFromServer();

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
