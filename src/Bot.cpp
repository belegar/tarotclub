/*=============================================================================
 * TarotClub - Bot.cpp
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

#include <sstream>
#include "Bot.h"
#include "Log.h"
#include "Util.h"
#include "System.h"
#include "JsonReader.h"

/*****************************************************************************/
Bot::Bot()
    : mClient(*this)
    , mTimeBeforeSend(0U)
    , mTableToJoin(0U)
{

}
/*****************************************************************************/
Bot::~Bot()
{

}
/*****************************************************************************/
void Bot::TableMessage(const std::string &message)
{
    (void)(message);
    // A bot cannot reply (yet :)
}
/*****************************************************************************/
void Bot::LobbyMessage(const std::string &message)
{
    (void)(message);
    // A bot cannot reply (yet :)
}
/*****************************************************************************/
void Bot::TableJoinEvent(std::uint32_t tableId)
{
    (void) tableId;
    // Nothing to do
}
/*****************************************************************************/
void Bot::TablePlayersList()
{
    // Nothing to do with the list of players
}
/*****************************************************************************/
void Bot::LobbyPlayersList()
{
    // Nothing to do with this event
}
/*****************************************************************************/
void Bot::NewDeal()
{
    JSEngine::StringList args;
    args.push_back(mClient.GetMyDeck().ToString());
    mBotEngine.Call("ReceiveCards", args);
}
/*****************************************************************************/
void Bot::SelectPlayer(Place p)
{
    (void)(p);
}
/*****************************************************************************/
void Bot::RequestBid(Contract highestBid)
{
    bool slam = false;
    JSEngine::StringList args;

    args.push_back(highestBid.ToString());
    Value result = mBotEngine.Call("AnnounceBid", args);

    if (!result.IsValid())
    {
        TLogError("Invalid script answer, requested string");
    }

    Contract botContract(result.GetString());

    // security test
    if ((botContract >= Contract::PASS) && (botContract <= Contract::GUARD_AGAINST))
    {
        // Ask to the bot if a slam has been announced
        args.clear();
        result = mBotEngine.Call("AnnounceSlam", args);
        if (result.IsValid())
        {
            slam = result.GetBool();
        }
        else
        {
            TLogError("Invalid script answer, requested boolean");
        }
    }
    else
    {
        botContract = mClient.CalculateBid(); // propose our algorithm if the user's one failed
    }

    // only bid over previous one is allowed
    if (botContract <= highestBid)
    {
        botContract = Contract::PASS;
    }

    mClient.SendBid(botContract, slam);
}
/*****************************************************************************/
void Bot::ShowBid(Place place, bool slam, Contract contract)
{
    (void)(place);
    (void)(contract);
    (void)(slam);
    mClient.SendSyncBid();
}
/*****************************************************************************/
void Bot::AllPassed()
{
    mClient.SendSyncAllPassed();
}
/*****************************************************************************/
void Bot::StartDeal()
{
    // FIXME: pass the game type to the script
    // FIXME: pass the slam declared bolean to the script
    JSEngine::StringList args;
    args.push_back(mClient.GetBid().taker.ToString());
    args.push_back(mClient.GetBid().contract.ToString());
    mBotEngine.Call("StartDeal", args);

    // We are ready, let's inform the server about that
    mClient.SendSyncStart();
}
/*****************************************************************************/
void Bot::ShowDog()
{
    mClient.SendSyncDog();
}
/*****************************************************************************/
void Bot::AskForHandle()
{
    bool valid = false;
    JSEngine::StringList args;

    TLogInfo("Ask for handle");
    Value ret = mBotEngine.Call("AskForHandle", args);
    Deck handle; // empty by default

    if (ret.IsValid())
    {
        if (ret.GetType() == Value::STRING)
        {
            std::string cards = ret.GetString();
            if (cards.size() > 0)
            {
                std::uint8_t count = handle.SetCards(cards);
                if (count > 0U)
                {
                    valid = mClient.TestHandle(handle);
                }
                else
                {
                    TLogError("Unknown cards in the handle");
                }
            }
            else
            {
                // Empty string means no handle to declare, it is valid!
                valid = true;
            }
        }
        else
        {
            TLogError("Bad format, requested a string");
        }
    }
    else
    {
        TLogError("Invalid script answer");
    }

    if (valid)
    {
        TLogInfo(std::string("Sending handle") + handle.ToString());
        mClient.SendHandle(handle);
    }
    else
    {
        TLogInfo("Invalid handle is: " + handle.ToString());
    }
}
/*****************************************************************************/
void Bot::ShowHandle()
{
    JSEngine::StringList args;

    // Send the handle to the bot
    args.push_back(mClient.GetHandleDeck().ToString());
    if (mClient.GetHandleDeck().GetOwner() == ATTACK)
    {
        args.push_back("0");
    }
    else
    {
        args.push_back("1");
    }
    mBotEngine.Call("ShowHandle", args);

    // We have seen the handle, let's inform the server about that
    mClient.SendSyncHandle();
}
/*****************************************************************************/
void Bot::BuildDiscard()
{
    bool valid = false;
    JSEngine::StringList args;
    Deck discard;

    args.push_back(mClient.GetDogDeck().ToString());
    Value ret = mBotEngine.Call("BuildDiscard", args);

    if (ret.IsValid())
    {
        if (ret.GetType() == Value::STRING)
        {
            std::uint8_t count = discard.SetCards(ret.GetString());
            if (count == Tarot::NumberOfDogCards(mClient.GetNumberOfPlayers()))
            {
                valid = mClient.TestDiscard(discard);
            }
            else
            {
                TLogError("Unknown cards in the discard");
            }
        }
        else
        {
            TLogError("Bad format, requested a string");
        }
    }
    else
    {
        TLogError("Invalid script answer");
    }

    if (valid)
    {
        mClient.SetDiscard(discard);

        TLogInfo("Player deck: " + mClient.GetMyDeck().ToString());
        TLogInfo("Discard: " + discard.ToString());
    }
    else
    {
        TLogInfo("Invalid discard is: " + discard.ToString());

        discard = mClient.AutoDiscard(); // build a random valid deck
        NewDeal(); // Resend cards to the bot!
    }

    mClient.SendDiscard(discard);
}
/*****************************************************************************/
void Bot::NewGame()
{
    // (re)inititialize script context
    if (InitializeScriptContext() == true)
    {
        JSEngine::StringList args;
        args.push_back(mClient.GetPlace().ToString());
        Tarot::GameMode mode = mClient.GetGameMode();
        std::string modeString;
        if (mode == Tarot::ONE_DEAL)
        {
            modeString = "one_deal";
        }
        else
        {
            modeString = "tournament";
        }
        args.push_back(modeString);
        mBotEngine.Call("EnterGame", args);

        mClient.SendSyncNewGame();
    }
    else
    {
        mClient.SendError();
    }
}
/*****************************************************************************/
void Bot::PlayCard()
{
    Card c;

    // Wait some time before playing
    std::this_thread::sleep_for(std::chrono::milliseconds(mTimeBeforeSend));

    JSEngine::StringList args;
    Value ret = mBotEngine.Call("PlayCard", args);

    if (!ret.IsValid())
    {
        TLogError("Invalid script answer");
    }

    // Test validity of card
    c = mClient.GetMyDeck().GetCard(ret.GetString());
    if (c.IsValid())
    {
        if (!mClient.IsValid(c))
        {
            std::stringstream message;
            message << mClient.GetPlace().ToString() << " played a non-valid card: " << ret.GetString() << "Deck is: " << mClient.GetMyDeck().ToString();
            TLogInfo(message.str());
            // The show must go on, play a random card
            c = mClient.Play();

            if (!c.IsValid())
            {
                TLogError("Panic!");
            }
        }
    }
    else
    {
        std::stringstream message;
        message << mClient.GetPlace().ToString() << " played an unknown card: " << ret.GetString()
                << " Client deck is: " << mClient.GetMyDeck().ToString();

        // The show must go on, play a random card
        c = mClient.Play();

        if (c.IsValid())
        {
            message << " Randomly chosen card is: " << c.GetName();
            TLogInfo(message.str());
        }
        else
        {
            TLogError("Panic!");
        }
    }

    mClient.SetPlayedCard(c);
    mClient.SendCard(c);
}
/*****************************************************************************/
void Bot::ShowCard(Place p, const std::string &name)
{
    JSEngine::StringList args;
    args.push_back(name);
    args.push_back(p.ToString());
    mBotEngine.Call("PlayedCard", args);

    // We have seen the card, let's inform the server about that
    mClient.SendSyncShowCard();
}
/*****************************************************************************/
void Bot::WaitTrick(Place winner)
{
    (void)(winner);
    // FIXME Call script and pass the winner

    mClient.SendSyncTrick();
}
/*****************************************************************************/
void Bot::EndOfDeal()
{
    // FIXME Call the script and pass the score?
    mClient.SendSyncEndOfDeal();
}
/*****************************************************************************/
void Bot::EndOfGame(Place winner)
{
    (void) winner;
    // FIXME What must we do?
    AdminGameFull(); // start a new game
}
/*****************************************************************************/
void Bot::SetTimeBeforeSend(std::uint16_t t)
{
    mTimeBeforeSend = t;
}
/*****************************************************************************/
void Bot::SetIdentity(const Identity &ident)
{
    mClient.SetMyIdentity(ident);
}
/*****************************************************************************/
void Bot::SetAiScriptConfigFile(const std::string &fileName)
{
    mScriptConf = fileName;
}
/*****************************************************************************/
void Bot::Initialize()
{
    mClient.Initialize();
}
/*****************************************************************************/
void Bot::ConnectToHost(const std::string &hostName, std::uint16_t port)
{
    mClient.ConnectToHost(hostName, port);
}
/*****************************************************************************/
bool Bot::InitializeScriptContext()
{
    bool retCode = true;
    std::string scriptRoot = Util::GetDirectoryPath(mScriptConf);

    TLogInfo(std::string("Opening: ") + mScriptConf + std::string(", script root path: ") + scriptRoot);

    // Open the configuration file to find the scripts
    JsonReader json;
    if (json.Open(mScriptConf))
    {
        std::vector<JsonValue> files = json.GetArray("files");

        mBotEngine.Initialize();

        // Load all Javascript files
        for (std::uint32_t i = 0U; i < files.size(); i++)
        {
            if (files[i].IsValid() && (files[i].GetTag() == IJsonNode::STRING))
            {

                std::string fileName = scriptRoot + Util::DIR_SEPARATOR + files[i].GetString();

#ifdef USE_WINDOWS_OS
                // Correct the path if needed
                Util::ReplaceCharacter(fileName, "/", "\\");
#endif

                if (!mBotEngine.Evaluate(fileName))
                {
                    std::stringstream message;
                    message << "Script error: could not open program file: " << fileName;
                    TLogError(message.str());
                    retCode = false;
                }
            }
            else
            {
                TLogError("Bad Json value in the array");
                retCode = false;
            }
        }
    }
    else
    {
        TLogError("Cannot open Json configuration file");
        retCode = false;
    }

    return retCode;
}
/*****************************************************************************/
void Bot::Error(std::uint32_t errorId)
{
    std::stringstream ss;
    ss << "Bot client error code: " << errorId;
    TLogError(ss.str());
}
/*****************************************************************************/
void Bot::EnteredLobby()
{
    // As soon as we have entered into the lobby, join the assigned table
    mClient.SendJoinTable(mTableToJoin);
}
/*****************************************************************************/
void Bot::AdminGameFull()
{
    // We are the admin on this table, let's start the game!
    Tarot::Shuffle sh;
    sh.type = Tarot::Shuffle::RANDOM_DEAL;
    mClient.AdminNewGame(Tarot::ONE_DEAL, sh, mClient.GetNumberOfTurns());
}
/*****************************************************************************/
void Bot::TableQuitEvent(std::uint32_t tableId)
{
    (void) tableId;
    // nothing to do except waiting for new orders
}

//=============================================================================
// End of file Bot.cpp
//=============================================================================
