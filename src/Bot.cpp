/*=============================================================================
 * TarotClub - Bot.cpp
 *=============================================================================
 * Modélise un robot joueur. Interface vers les scripts d'IA
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

#include "Bot.h"
#include "Log.h"
#include "Util.h"

using namespace std;

/*****************************************************************************/
Bot::Bot()
    : mClient(*this)
{
    timeBeforeSend.setSingleShot(true);
    timeBeforeSend.setInterval(0);
    //connect (&timeBeforeSend, SIGNAL(timeout()), this, SLOT(slotTimeBeforeSend()));
    connect(&timeBeforeSend, &QTimer::timeout, this, &Bot::slotTimeBeforeSend);
}
/*****************************************************************************/
Bot::~Bot()
{

}
/*****************************************************************************/
void Bot::Message(const string &message)
{
    Q_UNUSED(message);
    // A bot cannot reply (yet :)
}
/*****************************************************************************/
void Bot::AssignedPlace()
{
    InitializeScriptContext();

    QJSValueList args;
    args << mClient.GetPlace().Value();
    CallScript("EnterGame", args);
}
/*****************************************************************************/
void Bot::PlayersList(std::map<Place, Identity> &players)
{
    Q_UNUSED(players);
}
/*****************************************************************************/
void Bot::ReceiveCards()
{
    QJSValueList args;
    args << QString(mClient.GetMyDeck().GetCardList().data());
    CallScript("ReceiveCards", args);
}
/*****************************************************************************/
void Bot::SelectPlayer(Place p)
{
    Q_UNUSED(p);
}
/*****************************************************************************/
void Bot::RequestBid(Contract highestBid)
{
    qint32 ret;
    bool slam = false;
    Contract botContract;

    QJSValueList args;
    args << QString(highestBid.ToString().c_str());

    ret = CallScript("AnnounceBid", args).toInt();

    // security test
    if ((ret >= Contract::PASS) && (ret <= Contract::GUARD_AGAINST))
    {
        botContract = (Contract)ret;
        // Ask to the bot if a slam has been announced
        args.clear();
        slam = CallScript("AnnounceSlam", args).toBool();
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
    Q_UNUSED(place);
    Q_UNUSED(contract);
    Q_UNUSED(slam);
    mClient.SendSyncBid();
}
/*****************************************************************************/
void Bot::StartDeal(Place taker, Contract contract, const Game::Shuffle &sh)
{
    Q_UNUSED(sh);

    // FIXME: pass the game type to the script
    QJSValueList args;
    args << QString(taker.ToString().c_str()) << QString(contract.ToString().c_str());
    CallScript("StartDeal", args);

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
    QJSValueList args;
    args << QString(mClient.GetHandleDeck().GetCardList().data()) << (int)mClient.GetHandleDeck().GetOwner();
    CallScript("ShowHandle", args);

    // We have seen the handle, let's inform the server about that
    mClient.SendSyncHandle();
}
/*****************************************************************************/
void Bot::BuildDiscard()
{
    bool valid = true;
    QJSValueList args;
    Deck discard;

    args << QString(mClient.GetDogDeck().GetCardList().data());
    QString ret = CallScript("BuildDiscard", args).toString();

    int count = discard.SetCards(ret.toStdString());

    if (count == 6)
    {
        for (int i=0; i<6; i++)
        {
            Card *c = discard.at(i);

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
                if (discard.count(c) != 1)
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
void Bot::DealAgain()
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
    timeBeforeSend.start();
}
/*****************************************************************************/
void Bot::ShowCard(Place p, const std::string &name)
{
    QJSValueList args;
    args << QString(name.data()) << QString(p.ToString().c_str());
    CallScript("PlayedCard", args);

    // We have seen the card, let's inform the server about that
    mClient.SendSyncCard();
}
/*****************************************************************************/
void Bot::WaitTrick(Place winner)
{
    Q_UNUSED(winner);
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
void Bot::SetTimeBeforeSend(int t)
{
    timeBeforeSend.setInterval(t);
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
void Bot::ConnectToHost(const string &hostName, std::uint16_t port)
{
    mClient.ConnectToHost(hostName, port);
}
/*****************************************************************************/
void Bot::slotTimeBeforeSend()
{
    Card *c = NULL;

    QJSValueList args;
    QString ret = CallScript("PlayCard", args).toString();

    // Test validity of card
    c = mClient.GetMyDeck().GetCardByName(ret.toStdString());
    if (c != NULL)
    {
        if (mClient.IsValid(c) == false)
        {
            std::stringstream message;
            message << mClient.GetPlace().ToString() << " played a non-valid card: " << ret.toStdString();
            TLogInfo(message.str());
            // The show must go on, play a random card
            c = mClient.Play();
        }
    }
    else
    {
        std::stringstream message;
        message << mClient.GetPlace().ToString() << " played an unkown card: " << ret.toStdString();
        TLogInfo(message.str());

        message.flush();
        message << mClient.GetPlace().ToString() << " engine deck is: " << mClient.GetMyDeck().GetCardList();
        TLogInfo(message.str());

        // The show must go on, play a random card
        c = mClient.Play();
    }

    mClient.GetMyDeck().removeAll(c);
    mClient.SendCard(c);
}
/*****************************************************************************/
bool Bot::InitializeScriptContext()
{
    QStringList scriptFiles;
    QString appRoot;

#ifndef QT_NO_DEBUG
    // Debug, the binary is inside the build directory
    appRoot = qApp->applicationDirPath() + "/../../ai";
#else
    // Release
    appRoot = qApp->applicationDirPath() + "/ai";
#endif

    // TarotClub Javascript library files
    // Beware, the order is important, for global objects creation
    scriptFiles << appRoot + "/tarotlib/system.js";
    scriptFiles << appRoot + "/tarotlib/util.js";
    scriptFiles << appRoot + "/tarotlib/card.js";
    scriptFiles << appRoot + "/tarotlib/deck.js";
    scriptFiles << appRoot + "/tarotlib/player.js";
    scriptFiles << appRoot + "/tarotlib/bot.js";
    scriptFiles << appRoot + "/tarotlib/game.js";
    scriptFiles << appRoot + "/beginner.js";

    // Give access to some global objects from the JavaScript engine
    botEngine.globalObject().setProperty("TarotUtil", botEngine.newQObject(&utilObj));

    // Load all Javascript files
    QListIterator<QString> i(scriptFiles);
    while (i.hasNext())
    {
        QString fileName = i.next();
        QFile scriptFile(fileName);
        std::stringstream message;

        if (! scriptFile.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            message << "Script error: could not open program file: " << fileName.toStdString();
            TLogInfo(message.str());
            return false;
        }

        QString strProgram = scriptFile.readAll();
        scriptFile.close();

        QJSValue ret = botEngine.evaluate(strProgram, fileName);

        // uncaught exception?
        if (ret.isError())
        {
            message << "Evaluate uncaught exception: " << ret.toString().toStdString();
            TLogInfo(message.str());
            return false;
        }
    }

    return true;
}
/*****************************************************************************/
QJSValue Bot::CallScript(const QString &function, QJSValueList &args)
{
    QJSValue ret;
    std::stringstream message;
    QJSValue createFunc = botEngine.globalObject().property(function);

    if (createFunc.isError())
    {
        message << "Script error: script threw an uncaught exception while looking for create func: " << createFunc.toString().toStdString();
        TLogInfo(message.str());
        return ret;
    }

    ret = createFunc.call(args);

    if (ret.isError())
    {
        message << "Script threw an uncaught exception while looking for create func: " << ret.toString().toStdString();
        TLogInfo(message.str());
    }

    return ret;
}


//=============================================================================
// End of file Bot.cpp
//=============================================================================
