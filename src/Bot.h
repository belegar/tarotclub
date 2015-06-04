/*=============================================================================
 * TarotClub - Bot.h
 *=============================================================================
 * Bot class player. Uses a Script Engine to execute IA scripts
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

#ifndef BOT_H
#define BOT_H

#include "Client.h"
#include "NetClient.h"
#include "JSEngine.h"
#include "Log.h"

/*****************************************************************************/
class Bot : public Client::IEvent, public NetClient::IEvent
{

public:
    Bot();
    virtual ~Bot();

    void SetTimeBeforeSend(std::uint16_t t);
    void SetIdentity(const Identity &ident);
    void SetAiScript(const std::string &path);
    void Initialize();
    void ConnectToHost(const std::string &hostName, std::uint16_t port);
    void Close()
    {
        mNet.Close();
    }
    void SetTableToJoin(std::uint32_t table) { mTableToJoin = table; }
    std::uint32_t GetUuid() { return mClient->mPlayer.GetUuid(); }
    bool IsConnected() { return mNet.IsConnected(); }
    void JoinTable(std::uint32_t tableId);

private:
    std::shared_ptr<Client> mClient;
    NetClient mNet;
    std::uint16_t  mTimeBeforeSend;
    JSEngine mBotEngine;
    std::uint32_t mTableToJoin;
    std::string mScriptPath;

    bool InitializeScriptContext();

    // Client events
    virtual void RequestLogin();
    virtual void Error(std::uint32_t errorId);
    virtual void EnteredLobby();
    virtual void KickedFromLobby();
    virtual void AdminGameFull();
    virtual void TableQuitEvent(std::uint32_t tableId);
    virtual void TableMessage(const std::string &message);
    virtual void LobbyMessage(const std::string &message);
    virtual void TableJoinEvent(std::uint32_t tableId);
    virtual void TablePlayersList();
    virtual void LobbyPlayersList();
    virtual void NewDeal();
    virtual void SelectPlayer(Place p);
    virtual void RequestBid(Contract highestBid);
    virtual void ShowBid(Place p, bool slam, Contract c);
    virtual void AllPassed();
    virtual void StartDeal();
    virtual void ShowDog();
    virtual void AskForHandle();
    virtual void ShowHandle();
    virtual void BuildDiscard();
    virtual void NewGame();
    virtual void PlayCard();
    virtual void ShowCard(Place p, const std::string &name);
    virtual void WaitTrick(Place winner);
    virtual void EndOfDeal();
    virtual void EndOfGame(Place winner);

    // From NetClient::IEvent
    virtual void NetSignal(std::uint32_t sig);

};

#endif // BOT_H

//=============================================================================
// End of file Bot.h
//=============================================================================
