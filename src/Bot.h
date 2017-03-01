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

// Tarot files
#include "Network.h"
#include "BasicClient.h"

// ICL files
#include "JSEngine.h"
#include "Log.h"

/*****************************************************************************/
class Bot
{

public:
    Bot();
    virtual ~Bot();

    bool Decode(uint32_t src_uuid, uint32_t dest_uuid, const std::string &arg, std::vector<Reply> &out);

    std::uint32_t GetUuid() { return mClient.mMyself.uuid; }
    std::uint32_t GetCurrentTable() { return mClient.mMyself.tableId; }
    std::string GetDeck() { return mClient.mDeck.ToString(); }
    Place GetPlace() { return mClient.mMyself.place; }

    void SetTimeBeforeSend(std::uint16_t t);
    void ChangeNickname(const std::string &nickname, std::vector<Reply> &out);
    void SetAiScript(const std::string &path);
    void SetTableToJoin(std::uint32_t table) { mTableToJoin = table; }
    void SetUuid(std::uint32_t uuid) { mClient.mMyself.uuid = uuid; }
    void SetIdentity(const Identity &identity);

private:
    BasicClient mClient;
    EmptyContext mCtx;  ///< The bot does not need some network informations
    std::uint16_t  mTimeBeforeSend;
    JSEngine mBotEngine;
    std::uint32_t mTableToJoin;
    std::string mScriptPath;

    bool InitializeScriptContext();
    void StartDeal();
    void RequestBid(std::vector<Reply> &out);
    void AskForHandle(std::vector<Reply> &out);
    void ShowHandle();
    void BuildDiscard(std::vector<Reply> &out);
    void NewGame();
    void ShowCard();
    void PlayCard(std::vector<Reply> &out);
};

#endif // BOT_H

//=============================================================================
// End of file Bot.h
//=============================================================================
