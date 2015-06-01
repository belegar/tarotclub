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
#include "Zip.h"

/*****************************************************************************/
Bot::Bot()
    : mClient(new Client(*this))
    , mNet(std::shared_ptr<Protocol::IWorkItem>(mClient), *this)
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
    mNet.SendPacket(Protocol::ClientSyncJoinTable(mClient->mPlayer.GetUuid(), tableId));
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
    args.push_back(mClient->mPlayer.ToString());
    mBotEngine.Call("ReceiveCards", args);
    mNet.SendPacket(Protocol::ClientSyncCards(mClient->mPlayer.GetUuid(), mClient->mTableId));
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
        botContract = mClient->CalculateBid(); // propose our algorithm if the user's one failed
    }

    // only bid over previous one is allowed
    if (botContract <= highestBid)
    {
        botContract = Contract::PASS;
    }

    mNet.SendPacket(Protocol::ClientBid(botContract, slam, mClient->mPlayer.GetUuid(),  mClient->mTableId));
}
/*****************************************************************************/
void Bot::ShowBid(Place place, bool slam, Contract contract)
{
    (void)(place);
    (void)(contract);
    (void)(slam);
    mNet.SendPacket(Protocol::ClientSyncBid(mClient->mPlayer.GetUuid(), mClient->mTableId));
}
/*****************************************************************************/
void Bot::AllPassed()
{
    mNet.SendPacket(Protocol::ClientSyncAllPassed(mClient->mPlayer.GetUuid(), mClient->mTableId));
}
/*****************************************************************************/
void Bot::StartDeal()
{
    // FIXME: pass the game type to the script
    // FIXME: pass the slam declared bolean to the script
    JSEngine::StringList args;
    args.push_back(mClient->mBid.taker.ToString());
    args.push_back(mClient->mBid.contract.ToString());
    mBotEngine.Call("StartDeal", args);

    // We are ready, let's inform the server about that
    mNet.SendPacket(Protocol::ClientSyncStart(mClient->mPlayer.GetUuid(), mClient->mTableId));
}
/*****************************************************************************/
void Bot::ShowDog()
{
    mNet.SendPacket(Protocol::ClientSyncDog(mClient->mPlayer.GetUuid(), mClient->mTableId));
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
                    valid = mClient->TestHandle(handle);
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
        mNet.SendPacket(Protocol::ClientHandle(handle, mClient->mPlayer.GetUuid(), mClient->mTableId));

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
    args.push_back(mClient->handleDeck.ToString());
    if (mClient->handleDeck.GetOwner() == ATTACK)
    {
        args.push_back("0");
    }
    else
    {
        args.push_back("1");
    }
    mBotEngine.Call("ShowHandle", args);

    // We have seen the handle, let's inform the server about that
    mNet.SendPacket(Protocol::ClientSyncHandle(mClient->mPlayer.GetUuid(), mClient->mTableId));
}
/*****************************************************************************/
void Bot::BuildDiscard()
{
    bool valid = false;
    JSEngine::StringList args;
    Deck discard;

    args.push_back(mClient->mDog.ToString());
    Value ret = mBotEngine.Call("BuildDiscard", args);

    if (ret.IsValid())
    {
        if (ret.GetType() == Value::STRING)
        {
            std::uint8_t count = discard.SetCards(ret.GetString());
            if (count == Tarot::NumberOfDogCards(mClient->mNbPlayers))
            {
                valid = mClient->TestDiscard(discard);
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
        mClient->mPlayer += mClient->mDog;
        mClient->mPlayer.RemoveDuplicates(discard);

        TLogInfo("Player deck: " + mClient->mPlayer.ToString());
        TLogInfo("Discard: " + discard.ToString());
    }
    else
    {
        TLogInfo("Invalid discard is: " + discard.ToString());

        discard = mClient->AutoDiscard(); // build a random valid deck
        NewDeal(); // Resend cards to the bot!
    }

    mNet.SendPacket(Protocol::ClientDiscard(discard, mClient->mPlayer.GetUuid(), mClient->mTableId));
}
/*****************************************************************************/
void Bot::NewGame()
{
    // (re)inititialize script context
    if (InitializeScriptContext() == true)
    {
        JSEngine::StringList args;
        args.push_back(mClient->mPlace.ToString());
        Tarot::Game game = mClient->mGame;
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

        mNet.SendPacket(Protocol::ClientSyncNewGame(mClient->mPlayer.GetUuid(), mClient->mTableId));
    }
    else
    {
        mNet.SendPacket(Protocol::ClientError(mClient->mPlayer.GetUuid()));
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
    c = mClient->mPlayer.GetCard(ret.GetString());
    if (c.IsValid())
    {
        if (!mClient->IsValid(c))
        {
            std::stringstream message;
            message << mClient->mPlace.ToString() << " played a non-valid card: " << ret.GetString() << "Deck is: " << mClient->mPlayer.ToString();
            TLogError(message.str());
            // The show must go on, play a random card
            c = mClient->Play();

            if (!c.IsValid())
            {
                TLogError("Panic!");
            }
        }
    }
    else
    {
        std::stringstream message;
        message << mClient->mPlace.ToString() << " played an unknown card: " << ret.GetString()
                << " Client deck is: " << mClient->mPlayer.ToString();

        // The show must go on, play a random card
        c = mClient->Play();

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

    mClient->mPlayer.Remove(c);
    if (c.IsValid())
    {
        mNet.SendPacket(Protocol::ClientCard(c.GetName(), mClient->mPlayer.GetUuid(), mClient->mTableId));
    }
    else
    {
        mNet.SendPacket(Protocol::ClientError(mClient->mPlayer.GetUuid()));
    }
}
/*****************************************************************************/
void Bot::ShowCard(Place p, const std::string &name)
{
    JSEngine::StringList args;
    args.push_back(name);
    args.push_back(p.ToString());
    mBotEngine.Call("PlayedCard", args);

    // We have seen the card, let's inform the server about that
    mNet.SendPacket(Protocol::ClientSyncShowCard(mClient->mPlayer.GetUuid(), mClient->mTableId));
}
/*****************************************************************************/
void Bot::WaitTrick(Place winner)
{
    (void)(winner);
    // FIXME Call script and pass the winner
    mNet.SendPacket(Protocol::ClientSyncTrick(mClient->mPlayer.GetUuid(), mClient->mTableId));
}
/*****************************************************************************/
void Bot::EndOfDeal()
{
    // FIXME Call the script and pass the score?
    mNet.SendPacket(Protocol::ClientSyncEndOfDeal(mClient->mPlayer.GetUuid(), mClient->mTableId));
}
/*****************************************************************************/
void Bot::EndOfGame(Place winner)
{
    (void) winner;
}
/*****************************************************************************/
void Bot::NetSignal(uint32_t sig)
{
    (void) sig;
    mClient->mPlayer.SetUuid(Protocol::INVALID_UID);
}
/*****************************************************************************/
void Bot::SetTimeBeforeSend(std::uint16_t t)
{
    mTimeBeforeSend = t;
}
/*****************************************************************************/
void Bot::SetIdentity(const Identity &ident)
{
    mClient->mIdentity = ident;

    if (mNet.IsConnected())
    {
        // Send the new client identity to the server
        mNet.SendPacket(Protocol::ClientChangeIdentity(mClient->mPlayer.GetUuid(), ident));
    }
}
/*****************************************************************************/
void Bot::SetAiScript(const std::string &path)
{
    mScriptPath = path;
}
/*****************************************************************************/
void Bot::Initialize()
{
    mClient->Initialize();
    mNet.Initialize();
}
/*****************************************************************************/
void Bot::ConnectToHost(const std::string &hostName, std::uint16_t port)
{
    mNet.ConnectToHost(hostName, port);
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
        if (!zip.Open(mScriptPath))
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
            TLogError("Invalid AI script path " + mScriptPath);
            retCode = false;
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
                            if (!mBotEngine.EvaluateString(script))
                            {
                                TLogError("Script error: could not parse ZIP filename: " + iter->GetString());
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
                        std::string fileName = mScriptPath + Util::DIR_SEPARATOR + iter->GetString();

        #ifdef USE_WINDOWS_OS
                        // Correct the path if needed
                        Util::ReplaceCharacter(fileName, "/", "\\");
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
/*****************************************************************************/
void Bot::RequestLogin()
{
    mNet.SendPacket(Protocol::ClientReplyLogin(mClient->mPlayer.GetUuid(), mClient->mIdentity));
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
    mNet.SendPacket(Protocol::ClientJoinTable(mClient->mPlayer.GetUuid(), mTableToJoin));
}
/*****************************************************************************/
void Bot::KickedFromLobby()
{
    TLogInfo("Bot has been kicked from lobby.");
    mNet.Close();
}
/*****************************************************************************/
void Bot::AdminGameFull()
{
    // We are the admin on this table, let's start the game!
    Tarot::Game game;
    game.mode = Tarot::Game::cQuickDeal;
    game.deals.push_back(Tarot::Distribution());
    mNet.SendPacket(Protocol::AdminNewGame(game, mClient->mPlayer.GetUuid(), mClient->mTableId));
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
