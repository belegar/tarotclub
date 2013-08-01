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
#include <QMessageBox>

using namespace std;

/*****************************************************************************/
Bot::Bot() :
     statsObj(GetStatistics())
{
    connect(this, &Client::sigPlayersList, this, &Bot::slotPlayersList);
    connect(this, &Client::sigMessage, this, &Bot::slotMessage);
    connect(this, &Client::sigAssignedPlace, this, &Bot::slotAssignedPlace);
    connect(this, &Client::sigReceiveCards, this, &Bot::slotReceiveCards);
    connect(this, &Client::sigSelectPlayer, this, &Bot::slotSelectPlayer);
    connect(this, &Client::sigRequestBid, this, &Bot::slotRequestBid);
    connect(this, &Client::sigShowBid, this, &Bot::slotShowBid);
    connect(this, &Client::sigStartDeal, this, &Bot::slotStartDeal);
    connect(this, &Client::sigShowDog, this, &Bot::slotShowDog);
    connect(this, &Client::sigBuildDiscard, this, &Bot::slotBuildDiscard);
    connect(this, &Client::sigDealAgain, this, &Bot::slotDealAgain);
    connect(this, &Client::sigPlayCard, this, &Bot::slotPlayCard);
    connect(this, &Client::sigShowCard, this, &Bot::slotShowCard);
    connect(this, &Client::sigShowHandle, this, &Bot::slotShowHandle);
    connect(this, &Client::sigWaitTrick, this, &Bot::slotWaitTrick);
    connect(this, &Client::sigEndOfDeal, this, &Bot::slotEndOfDeal);
    connect(this, &Client::sigEndOfGame, this, &Bot::slotEndOfGame);

    timeBeforeSend.setSingleShot(true);
    timeBeforeSend.setInterval(0);
    connect (&timeBeforeSend, SIGNAL(timeout()), this, SLOT(slotTimeBeforeSend()));
    //connect(&timeBeforeSend, &QTimer::timeout, this, &Bot::slotTimeBeforeSend);

#ifndef QT_NO_DEBUG
    debugger.attachTo(&botEngine);
#endif
}
/*****************************************************************************/
Bot::~Bot()
{

}
/*****************************************************************************/
void Bot::slotMessage(const QString &message)
{
    Q_UNUSED(message);
    // A bot cannot reply (yet :)
}
/*****************************************************************************/
void Bot::slotAssignedPlace(Place p)
{
    InitializeScriptContext();

    QScriptValueList args;
    args << p;
    CallScript("EnterGame", args);
}
/*****************************************************************************/
void Bot::slotPlayersList(QMap<Place, Identity> &players)
{
    Q_UNUSED(players);
}
/*****************************************************************************/
void Bot::slotReceiveCards()
{
    QScriptValueList args;
    args << GetMyDeck().GetCardList();
    CallScript("ReceiveCards", args);
}
/*****************************************************************************/
void Bot::slotSelectPlayer(Place p)
{
    Q_UNUSED(p);
    // nada aussi
}
/*****************************************************************************/
void Bot::slotRequestBid(Contract highestBid)
{
    int ret;
    Contract botContract;

    QScriptValueList args;
    args << (int)highestBid;
    ret = CallScript("AnnounceBid", args).toInteger();

    // security test
    if ((ret >= PASS) && (ret <= GUARD_AGAINST))
    {
        botContract = (Contract)ret;
    }
    else
    {
        botContract = CalculateBid(); // propose our algorithm if the user's one failed
    }

    // only bid over previous one is allowed
    if (botContract <= highestBid)
    {
        botContract = PASS;
    }
    SendBid(botContract);
}
/*****************************************************************************/
void Bot::slotShowBid(Place place, Contract contract)
{
    Q_UNUSED(place);
    Q_UNUSED(contract);
    SendSyncBid();
}
/*****************************************************************************/
void Bot::slotStartDeal(Place taker, Contract contract)
{
    QScriptValueList args;
    args << taker << contract;
    CallScript("StartGame", args);

    // We are ready, let's inform the server about that
    SendSyncStart();
}
/*****************************************************************************/
void Bot::slotShowDog()
{
    SendSyncDog();
}
/*****************************************************************************/
void Bot::slotBuildDiscard()
{
    QScriptValueList args;
    args << GetDogDeck().GetCardList();
    QString ret = CallScript("BuildDiscard", args).toString();

    // We receive the discard made by the bot script
    // FIXME: verify the validity of the deck. If not valid, build the
    // discard automatically

    BuildDogDeck();
    SendDog();
}
/*****************************************************************************/
void Bot::slotDealAgain()
{
    // Re-inititialize script context
    if (InitializeScriptContext() == true)
    {
        SendReady();
    }
    else
    {
        SendError();
    }
}
/*****************************************************************************/
void Bot::slotPlayCard()
{
    timeBeforeSend.start();
}
/*****************************************************************************/
void Bot::slotShowCard(int id, Place p)
{
    QScriptValueList args;
    args << TarotDeck::GetCard(id)->GetName() << (int)p;
    CallScript("PlayedCard", args);

    // We have seen the card, let's inform the server about that
    SendSyncCard();
}
/*****************************************************************************/
void Bot::slotShowHandle()
{
    QScriptValueList args;
    args << GetHandleDeck().GetCardList() << (int)GetHandleDeck().GetOwner();
    CallScript("ShowHandle", args);

    // We have seen the handle, let's inform the server about that
    SendSyncHandle();
}
/*****************************************************************************/
void Bot::slotWaitTrick(Place winner)
{
    Q_UNUSED(winner);
    // FIXME Call script and pass the winner

    SendSyncTrick();
}
/*****************************************************************************/
void Bot::slotEndOfDeal()
{
    // FIXME Call the script and pass the score?
    SendReady();
}
/*****************************************************************************/
void Bot::slotEndOfGame()
{
    // FIXME What must we do?
}
/*****************************************************************************/
void Bot::SetTimeBeforeSend(int t)
{
    timeBeforeSend.setInterval(t);
}
/*****************************************************************************/
void Bot::slotTimeBeforeSend()
{
    Card *c = NULL;

    QScriptValueList args;
    QString ret = CallScript("PlayCard", args).toString();

    // Test validity of card
    c = GetMyDeck().GetCardByName(ret);
    if (c != NULL)
    {
        if (IsValid(c) == false)
        {
            QString message = GetMyIdentity().name + QString(" played a non-valid card: ") + ret;
            qDebug() << message.toLatin1().constData();
            // The show must go on, play a random card
            c = Play();
        }
    }
    else
    {
        QString message = GetMyIdentity().name + QString(" played an unkown card: ") + ret;
        qDebug() << message.toLatin1().constData();
        // The show must go on, play a random card
        c = Play();
    }

    GetMyDeck().removeAll(c);
    SendCard(c);
}
/*****************************************************************************/
QScriptValue myPrint(QScriptContext *context, QScriptEngine *eng)
{
    Q_UNUSED(eng);

    QScriptValue arg = context->argument(0);
    if (!arg.isString())
    {
        return context->throwError(
                   QScriptContext::TypeError,
                   QString::fromLatin1("ScriptDebug(): expected string argument"));
    }
    QString toPrint = QString("Bot script: ") + arg.toString();
    qDebug() << toPrint.toLatin1().constData();
    return QScriptValue();
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
    scriptFiles << appRoot + "/tarotlib/game.js";
    scriptFiles << appRoot + "/beginner.js";

    // Give access to some global objects from the JavaScript engine
    botEngine.globalObject().setProperty("TStats", botEngine.newQObject(&statsObj));
    botEngine.globalObject().setProperty("scriptDebug", botEngine.newFunction(&myPrint));

    // Load all Javascript files
    QListIterator<QString> i(scriptFiles);
    while (i.hasNext())
    {
        QFile scriptFile(i.next());

        if (! scriptFile.exists())
        {
            qDebug() << "Script error: could not find program file!";
            return false;
        }

        if (! scriptFile.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            qDebug() <<  "Script error: could not open program file!";
            return false;
        }

        QString strProgram = scriptFile.readAll();

        // do static check so far of code:
        if (! botEngine.canEvaluate(strProgram))
        {
            qDebug() <<  "Script error: canEvaluate returned false!";
            return false;
        }
    #ifndef QT_NO_DEBUG
        debugger.action(QScriptEngineDebugger::InterruptAction);
    #endif
        // actually do the eval:
        botEngine.evaluate(strProgram);

        // uncaught exception?
        if (botEngine.hasUncaughtException())
        {
            QScriptValue exception = botEngine.uncaughtException();
            QMessageBox::critical(0, "Script error", QString("Script threw an uncaught exception: ") + exception.toString());
            return false;
        }
    }

    return true;
}
/*****************************************************************************/
QScriptValue Bot::CallScript(const QString &function, QScriptValueList &args)
{
    QScriptValue ret;
    QScriptValue createFunc = botEngine.globalObject().property(function);
    if (botEngine.hasUncaughtException())
    {
        QScriptValue exception = botEngine.uncaughtException();
        qDebug() <<  "Script error: script threw an uncaught exception while looking for create func: " << exception.toString();
        return ret;
    }

    if (!createFunc.isFunction())
    {
        qDebug() <<  "Script error: " << function << " is not a function!";
    }

    ret = createFunc.call(QScriptValue(), args);

    if (botEngine.hasUncaughtException())
    {
        QScriptValue exception = botEngine.uncaughtException();
        qDebug() << QString("Script threw an uncaught exception while looking for create func: ") + exception.toString();
    }

    return ret;
}


//=============================================================================
// End of file Bot.cpp
//=============================================================================
