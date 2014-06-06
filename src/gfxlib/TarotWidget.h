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

// Game includes
#include "Client.h"
#include "Common.h"
#include "Table.h"
#include "ClientConfig.h"
#include "ServerConfig.h"
#include "GfxCard.h"
#include "Canvas.h"

/*****************************************************************************/
/**
 * joueur 0 : humain SUD
 * joueur 1 : ordi EST
 * joueur 2 : ordi NORD (ou NORD-EST)
 * joueur 3 : ordi OUEST
 */
class TarotWidget : public QWidget, public Client::IEvent
{
    Q_OBJECT

public:
    enum ConnectionType
    {
        NO_CONNECTION,
        LOCAL,
        REMOTE
    };

    TarotWidget(QWidget *parent);

    void Initialize();
    void LaunchLocalGame(Game::Mode, const Game::Shuffle &sh);
    void LaunchRemoteGame(const std::string &ip, std::uint16_t port);

    // Configuration management
    void ApplyOptions(const ClientOptions &i_clientOpt, const ServerOptions &i_servOpt);

    // Getters about various current game information, external usage
    QMap<Place, Identity> GetPlayersList() { return mPlayers; }

signals:
    // These signals are used to indicate to an external widget any useful game event
    void sigPlayersListEvent();

public slots:
    // These slots are made available to link them to any external widget
    void slotNewTournamentGame();
    void slotNewQuickGame();
    void slotCreateNetworkGame();
    void slotCleanBeforeExit();
    void slotSendChatMessage(const QString &message);

private:
    Table           mTable;    // A Tarot table, owns a thread, bots and a Tarot network engine game
    ClientOptions   mClientOptions;
    ServerOptions   mServerOptions;
    Client          mClient; // The human player
    bool            firstTurn;
    QMap<Place, Identity> mPlayers;
    Deal            deal;
    Deck            discard;
    std::list<std::string> mMessages;
    ConnectionType  mConnectionType;
    Canvas          *mCanvas;

    // Helpers
    void ShowSouthCards();
    void HideTrick();
    void InitScreen();
    void ShowVictoryWindow();
    bool HasLocalConnection();

    // Client events
    virtual void Message(const std::string &message)
    {
        mMessages.push_back(message);
        emit sigMessage();
    }
    virtual void AssignedPlace() { emit sigAssignedPlace(); }
    virtual void PlayersList()
    {
        std::map<Place, Identity>::iterator iter;
        std::map<Place, Identity> pl = mClient.GetGame().GetPlayers();

        // Transform a std::map into a QMap
        mPlayers.clear();
        for(iter = pl.begin(); iter != pl.end(); iter++)
        {
            mPlayers[iter->first] = iter->second;
        }

        emit sigPlayersList();
    }
    virtual void ReceiveCards() { emit sigReceiveCards(); }
    virtual void SelectPlayer(Place p) { emit sigSelectPlayer(p); }
    virtual void RequestBid(Contract highestBid) { emit sigRequestBid(highestBid); }
    virtual void ShowBid(Place p, bool slam, Contract c) { emit sigShowBid(p, slam, c); }
    virtual void StartDeal(Place taker, Contract c, const Game::Shuffle &sh) { emit sigStartDeal(taker, c, sh); }
    virtual void ShowDog() { emit sigShowDog(); }
    virtual void ShowHandle() { emit sigShowHandle(); }
    virtual void BuildDiscard() { emit sigBuildDiscard(); }
    virtual void NewDeal() { emit sigDealAgain(); }
    virtual void PlayCard()  { emit sigPlayCard(); }
    virtual void ShowCard(Place p, const std::string &name)  { emit sigShowCard(p, name); }
    virtual void WaitTrick(Place winner) { emit sigWaitTrick(winner); }
    virtual void EndOfDeal() { emit sigEndOfDeal(); }
    virtual void EndOfGame() { emit sigEndOfGame(); }

signals:
   void sigReceiveCards();
   void sigAssignedPlace();
   void sigPlayersList();
   void sigMessage();
   void sigSelectPlayer(Place);
   void sigRequestBid(Contract);
   void sigShowBid(Place, bool, Contract);
   void sigShowDog();
   void sigStartDeal(Place, Contract, Game::Shuffle);
   void sigShowHandle();
   void sigBuildDiscard();
   void sigDealAgain();
   void sigPlayCard();
   void sigShowCard(Place, std::string);
   void sigWaitTrick(Place);
   void sigEndOfDeal();
   void sigEndOfGame();

private slots:
    // Client events
    void slotReceiveCards();
    void slotAssignedPlace();
    void slotPlayersList();
    void slotMessage();
    void slotSelectPlayer(Place p);
    void slotRequestBid(Contract highestBid);
    void slotShowBid(Place p, bool slam, Contract c);
    void slotShowDog();
    void slotStartDeal(Place taker, Contract c, Game::Shuffle sh);
    void slotShowHandle();
    void slotBuildDiscard();
    void slotDealAgain();
    void slotPlayCard();
    void slotShowCard(Place p, std::string name);
    void slotWaitTrick(Place p);
    void slotEndOfDeal();
    void slotEndOfGame();

    // Board events
    void slotAcceptHandle();
    void slotAcceptDiscard();
    void slotSetEnchere(Contract cont);
    void slotClickCard(std::uint8_t index, bool selected);
    void slotClickBoard();
    void slotMoveCursor(std::uint8_t index);
};

#endif // TAROT_WIDGET_H_

//=============================================================================
// End of file TarotWidget.h
//=============================================================================
