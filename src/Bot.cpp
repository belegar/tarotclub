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
#include <thread>
#include <chrono>
#include "Bot.h"
#include "Log.h"
#include "Util.h"
#include "System.h"
#include "JsonReader.h"
#include "Zip.h"

/*****************************************************************************/
Bot::Bot()
    : mTimeBeforeSend(0U)
    , mTableToJoin(0U)
{

}
/*****************************************************************************/
Bot::~Bot()
{

}
/*****************************************************************************/
bool Bot::Decode(uint32_t src_uuid, uint32_t dest_uuid, const std::string &arg, std::vector<Reply> &out)
{
    bool ret = true;

    // Generic client decoder, fill the context and the client structure
    BasicClient::Event event = mClient.Decode(src_uuid, dest_uuid, arg, mCtx, out);

    switch (event)
    {
    case BasicClient::ACCESS_GRANTED:
    {
        // As soon as we have entered into the lobby, join the assigned table
        mClient.BuildJoinTable(mTableToJoin, out);
        break;
    }
    case BasicClient::NEW_DEAL:
    {
        JSEngine::StringList args;
        args.push_back(mClient.mDeck.ToString());
        mBotEngine.Call("ReceiveCards", args);
        mClient.Sync(Engine::WAIT_FOR_CARDS, out);
        break;
    }
    case BasicClient::REQ_BID:
    {
        // Only reply a bid if it is our place to anwser
        if (mClient.mBid.taker == mClient.mMyself.place)
        {
            TLogNetwork("Bot " + mClient.mMyself.place.ToString() + " is bidding");
            RequestBid(out);
        }
        break;
    }
    case BasicClient::SHOW_BID:
    {
        mClient.Sync(Engine::WAIT_FOR_SHOW_BID, out);
        break;
    }
    case BasicClient::BUILD_DISCARD:
    {
        BuildDiscard(out);
        break;
    }
    case BasicClient::SHOW_DOG:
    {
        mClient.Sync(Engine::WAIT_FOR_SHOW_DOG, out);
        break;
    }
    case BasicClient::START_DEAL:
    {
        StartDeal();
        mClient.Sync(Engine::WAIT_FOR_START_DEAL, out);
        break;
    }
    case BasicClient::SHOW_HANDLE:
    {
        ShowHandle();
        mClient.Sync(Engine::WAIT_FOR_SHOW_HANDLE, out);
        break;
    }
    case BasicClient::NEW_GAME:
    {
        NewGame();
        mClient.Sync(Engine::WAIT_FOR_READY, out);
        break;
    }
    case BasicClient::SHOW_CARD:
    {
        ShowCard();
        mClient.Sync(Engine::WAIT_FOR_SHOW_CARD, out);
        break;
    }
    case BasicClient::PLAY_CARD:
    {
        // Only reply a bid if it is our place to anwser
        if (mClient.IsMyTurn())
        {
            PlayCard(out);
        }
        break;
    }
    case BasicClient::ASK_FOR_HANDLE:
    {
        AskForHandle(out);
        break;
    }
    case BasicClient::END_OF_TRICK:
    {
        mClient.Sync(Engine::WAIT_FOR_END_OF_TRICK, out);
        break;
    }
    case BasicClient::END_OF_GAME:
    {
        mClient.Sync(Engine::WAIT_FOR_READY, out);
        break;
    }
    case BasicClient::END_OF_DEAL:
    {
        mClient.Sync(Engine::WAIT_FOR_END_OF_DEAL, out);
        break;
    }
    case BasicClient::JOIN_TABLE:
    {
        mClient.Sync(Engine::WAIT_FOR_PLAYERS, out);
        break;
    }
    case BasicClient::ALL_PASSED:
    {
        mClient.Sync(Engine::WAIT_FOR_ALL_PASSED, out);
        break;
    }

    case BasicClient::JSON_ERROR:
    case BasicClient::BAD_EVENT:
    case BasicClient::REQ_LOGIN:
    case BasicClient::MESSAGE:
    case BasicClient::PLAYER_LIST:
    case BasicClient::QUIT_TABLE:
    case BasicClient::SYNC:
    {
        // Nothing to do for that event
        break;
    }

    default:
        ret = false;
        break;
    }

    return ret;
}
/*****************************************************************************/
void Bot::RequestBid(std::vector<Reply> &out)
{
    bool slam = false;
    JSEngine::StringList args;
    Contract highestBid = mClient.mBid.contract;

    args.push_back(highestBid.ToString()); // Send the highest bid as argument: FIXME: not necessary, the Bot can remember it, to be deleted
    Value result = mBotEngine.Call("AnnounceBid", args);

    if (!result.IsValid())
    {
        TLogError("Invalid script answer, requested string");
    }

    Contract botContract(result.GetString());

    // security test
    if ((botContract >= Contract(Contract::PASS)) && (botContract <= Contract(Contract::GUARD_AGAINST)))
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

    mClient.BuildBid(botContract, slam, out);
}
/*****************************************************************************/
void Bot::StartDeal()
{
    // FIXME: pass the game type to the script
    // FIXME: pass the slam declared bolean to the script
    JSEngine::StringList args;
    args.push_back(mClient.mBid.taker.ToString());
    args.push_back(mClient.mBid.contract.ToString());
    mBotEngine.Call("StartDeal", args);
}
/*****************************************************************************/
void Bot::AskForHandle(std::vector<Reply> &out)
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
                    valid = mClient.mDeck.TestHandle(handle);
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

    if (!valid)
    {
        TLogInfo("Invalid handle is: " + handle.ToString());
        handle.Clear();
    }

    TLogInfo(std::string("Sending handle") + handle.ToString());
    mClient.BuildHandle(handle, out);
}
/*****************************************************************************/
void Bot::ShowHandle()
{
    JSEngine::StringList args;

    // Send the handle to the bot
    args.push_back(mClient.mHandle.ToString());
    if (mClient.mHandle.GetOwner() == Team::ATTACK)
    {
        args.push_back("0");
    }
    else
    {
        args.push_back("1");
    }
    mBotEngine.Call("ShowHandle", args);
}
/*****************************************************************************/
void Bot::BuildDiscard(std::vector<Reply> &out)
{
    bool valid = false;
    JSEngine::StringList args;
    Deck discard;

    args.push_back(mClient.mDog.ToString());
    Value ret = mBotEngine.Call("BuildDiscard", args);

    if (ret.IsValid())
    {
        if (ret.GetType() == Value::STRING)
        {
            std::uint8_t count = discard.SetCards(ret.GetString());
            if (count == Tarot::NumberOfDogCards(mClient.mNbPlayers))
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
        mClient.mDeck += mClient.mDog;
        mClient.mDeck.RemoveDuplicates(discard);

        TLogInfo("Player deck: " + mClient.mDeck.ToString());
        TLogInfo("Discard: " + discard.ToString());
    }
    else
    {
        TLogInfo("Invalid discard is: " + discard.ToString());

        discard = mClient.AutoDiscard(); // build a random valid deck
    }

    mClient.BuildDiscard(discard, out);
}
/*****************************************************************************/
void Bot::NewGame()
{
    // (re)inititialize script context
    if (InitializeScriptContext() == true)
    {
        JSEngine::StringList args;
        args.push_back(mClient.mMyself.place.ToString());
        Tarot::Game game = mClient.mGame;
        std::string modeString;
        if (game.mode == Tarot::Game::cQuickDeal)
        {
            modeString = "one_deal";
        }
        else
        {
            modeString = "simple_tournament";
        }
        args.push_back(modeString);
        mBotEngine.Call("EnterGame", args);
    }
    else
    {
        TLogError("Cannot initialize bot context");
    }
}
/*****************************************************************************/
void Bot::PlayCard(std::vector<Reply> &out)
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
    c = mClient.mDeck.GetCard(ret.GetString());
    if (c.IsValid())
    {
        if (!mClient.IsValid(c))
        {
            std::stringstream message;
            message << mClient.mMyself.place.ToString() << " played a non-valid card: " << ret.GetString() << "Deck is: " << mClient.mDeck.ToString();
            TLogError(message.str());
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
        message << mClient.mMyself.place.ToString() << " played an unknown card: " << ret.GetString()
                << " Client deck is: " << mClient.mDeck.ToString();

        // The show must go on, play a random card
        c = mClient.Play();

        if (c.IsValid())
        {
            message << " Randomly chosen card is: " << c.ToString();
            TLogInfo(message.str());
        }
        else
        {
            TLogError("Panic!");
        }
    }

    mClient.mDeck.Remove(c);
    if (!c.IsValid())
    {
        TLogError("Invalid card!");
    }

    mClient.BuildSendCard(c, out);
}
/*****************************************************************************/
void Bot::ShowCard()
{
    JSEngine::StringList args;
    args.push_back(mClient.mCurrentTrick.Last().ToString());
    args.push_back(mClient.mCurrentPlayer.ToString());
    mBotEngine.Call("PlayedCard", args);
}
/*****************************************************************************/
void Bot::SetTimeBeforeSend(std::uint16_t t)
{
    mTimeBeforeSend = t;
}
/*****************************************************************************/
void Bot::ChangeNickname(const std::string &nickname, std::vector<Reply> &out)
{
    mClient.mMyself.identity.nickname = nickname;
    mClient.BuildChangeNickname(out);
}
/*****************************************************************************/
void Bot::SetAiScript(const std::string &path)
{
    mScriptPath = path;
}
/*****************************************************************************/
void Bot::SetIdentity(const Identity &identity)
{
    mClient.mMyself.identity = identity;
}
/*****************************************************************************/
bool Bot::InitializeScriptContext()
{
    bool retCode = true;
    Zip zip;
    bool useZip = false;

    if (Util::FileExists(mScriptPath))
    {
        // Seems to be an archive file
        if (!zip.Open(mScriptPath, true))
        {
            TLogError("Invalid AI Zip file.");
            retCode = false;
        }
        useZip = true;
    }
    else
    {
        if (Util::FolderExists(mScriptPath))
        {
            TLogInfo("Using script root path: " + mScriptPath);
        }
        else
        {
            // Last try, maybe a zipped memory buffer
            if (!zip.Open(mScriptPath, false))
            {
                TLogError("Invalid AI Zip buffer.");
                retCode = false;
            }
            useZip = true;
        }
    }

    if (retCode)
    {
        // Open the configuration file to find the scripts
        JsonValue json;

        if (useZip)
        {
            std::string package;
            if (zip.GetFile("package.json", package))
            {
                retCode = JsonReader::ParseString(json, package);
            }
            else
            {
                retCode = false;
            }
        }
        else
        {
            retCode = JsonReader::ParseFile(json, mScriptPath + Util::DIR_SEPARATOR + "package.json");
        }

        if (retCode)
        {
            JsonValue files = json.FindValue("files");

            mBotEngine.Initialize();

            // Load all Javascript files
            for (JsonArray::Iterator iter = files.GetArray().Begin(); iter != files.GetArray().End(); ++iter)
            {
                if (iter->IsValid() && (iter->IsString()))
                {
                    if (useZip)
                    {
                        std::string script;
                        if (zip.GetFile(iter->GetString(), script))
                        {
                            std::string output;
                            if (!mBotEngine.EvaluateString(script, output))
                            {
                                TLogError("Script error: " + output);
                                retCode = false;
                            }
                        }
                        else
                        {
                            retCode = false;
                        }
                    }
                    else
                    {
                        std::string fileName = mScriptPath + iter->GetString();

        #ifdef USE_WINDOWS_OS
                        // Correct the path if needed
     //                   Util::ReplaceCharacter(fileName, "/", "\\");
        #endif

                        if (!mBotEngine.EvaluateFile(fileName))
                        {
                            std::stringstream message;
                            message << "Script error: could not open program file: " << fileName;
                            TLogError(message.str());
                            retCode = false;
                        }
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
    }

    return retCode;
}


//=============================================================================
// End of file Bot.cpp
//=============================================================================
