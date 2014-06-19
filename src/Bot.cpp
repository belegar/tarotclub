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

/*****************************************************************************/
Bot::Bot()
    : mClient(*this)
    , mTimeBeforeSend(0U)
{

}
/*****************************************************************************/
Bot::~Bot()
{

}
/*****************************************************************************/
void Bot::Message(const std::string &message)
{
    (void)(message);
    // A bot cannot reply (yet :)
}
/*****************************************************************************/
void Bot::AssignedPlace()
{
    InitializeScriptContext();

    JSEngine::StringList args;
    args.push_back(mClient.GetPlace().ToString());
    args.push_back(""); // FIXME replace by the game mode, string format
    mBotEngine.Call("EnterGame", args);
}
/*****************************************************************************/
void Bot::PlayersList()
{
    // Nothing to do with the list of players
}
/*****************************************************************************/
void Bot::ReceiveCards()
{
    JSEngine::StringList args;
    args.push_back(mClient.GetMyDeck().GetCardList());
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
    JSValue result = mBotEngine.Call("AnnounceBid", args);

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
void Bot::StartDeal(Place taker, Contract contract, const Game::Shuffle &sh)
{
    (void)(sh);

    // FIXME: pass the game type to the script
    JSEngine::StringList args;
    args.push_back(taker.ToString());
    args.push_back(contract.ToString());
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
void Bot::ShowHandle()
{
    JSEngine::StringList args;

    // Send the handle to the bot
    args.push_back(mClient.GetHandleDeck().GetCardList());
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
    bool valid = true;
    JSEngine::StringList args;
    Deck discard;

    args.push_back(mClient.GetDogDeck().GetCardList());
    JSValue ret = mBotEngine.Call("BuildDiscard", args);

    if (!ret.IsValid())
    {
        TLogError("Invalid script answer, requested string");
    }

    int count = discard.SetCards(ret.GetString());

    if (count == 6)
    {
        for (Deck::ConstIterator i = discard.Begin(); i != discard.End(); ++i)
        {
            Card *c = (*i);

            // Look if the card belongs to the dog or the player's deck
            if (mClient.GetDogDeck().HasCard(c) || mClient.GetMyDeck().HasCard(c))
            {
                // Look the card value against the Tarot rules
                if ((c->GetSuit() == Card::TRUMPS) ||
                    ((c->GetSuit() != Card::TRUMPS) && (c->GetValue() == 14)))
                {
                    valid = false;
                }

                // Look if this card is unique
                if (discard.Count(c) != 1)
                {
                    valid = false;
                }
            }
            else
            {
                valid = false;
            }
        }
    }
    else
    {
        valid = false;
    }

    if (!valid)
    {
        TLogInfo("Invalid discard: " + discard.GetCardList());

        discard = mClient.BuildDogDeck(); // build a random valid deck
        ReceiveCards(); // Resend cards to the bot!
    }
    mClient.SetDiscard(discard);
}
/*****************************************************************************/
void Bot::NewDeal()
{
    // Re-inititialize script context
    if (InitializeScriptContext() == true)
    {
        mClient.SendReady();
    }
    else
    {
        mClient.SendError();
    }
}
/*****************************************************************************/
void Bot::PlayCard()
{
    Card *c = NULL;

    // Wait some time before playing
    std::this_thread::sleep_for(std::chrono::milliseconds(mTimeBeforeSend));

    JSEngine::StringList args;
    JSValue ret = mBotEngine.Call("PlayCard", args);

    if (!ret.IsValid())
    {
        TLogError("Invalid script answer, requested string");
    }

    // Test validity of card
    c = mClient.GetMyDeck().GetCardByName(ret.GetString());
    if (c != NULL)
    {
        if (mClient.IsValid(c) == false)
        {
            std::stringstream message;
            message << mClient.GetPlace().ToString() << " played a non-valid card: " << ret.GetString();
            TLogInfo(message.str());
            // The show must go on, play a random card
            c = mClient.Play();
        }
    }
    else
    {
        std::stringstream message;
        message << mClient.GetPlace().ToString() << " played an unkown card: " << ret.GetString();
        TLogInfo(message.str());

        message.flush();
        message << mClient.GetPlace().ToString() << " engine deck is: " << mClient.GetMyDeck().GetCardList();
        TLogInfo(message.str());

        // The show must go on, play a random card
        c = mClient.Play();
    }

    mClient.GetMyDeck().Remove(c);
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
    mClient.SendSyncCard();
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
    mClient.SendReady();
}
/*****************************************************************************/
void Bot::EndOfGame()
{
    // FIXME What must we do?
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
    JSEngine::StringList scriptFiles;
    std::string appRoot;

    appRoot = System::ScriptPath();

    // TarotClub Javascript library files
    // Beware, the order is important, for global objects creation
    scriptFiles.push_back("tarotlib/system.js");
    scriptFiles.push_back("tarotlib/util.js");
    scriptFiles.push_back("tarotlib/card.js");
    scriptFiles.push_back("tarotlib/deck.js");
    scriptFiles.push_back("tarotlib/player.js");
    scriptFiles.push_back("tarotlib/bot.js");
    scriptFiles.push_back("tarotlib/game.js");
    scriptFiles.push_back("beginner.js");

    mBotEngine.Initialize();
    // FIXME: register print function TLogInfo

    // Load all Javascript files
    for (std::uint32_t i = 0; i < scriptFiles.size(); i++)
    {
        std::string fileName = appRoot + scriptFiles[i];
        if (!mBotEngine.Evaluate(fileName))
        {
            std::stringstream message;
            message << "Script error: could not open program file: " << fileName;
            TLogError(message.str());
            retCode = false;
        }
    }

    return retCode;
}

//=============================================================================
// End of file Bot.cpp
//=============================================================================
