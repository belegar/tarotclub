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
    Client()
    , statsObj(stats)
    , deckObj(myDeck, mainDeck)
{
    connect( this, SIGNAL(sgnlMessage(const QString &)), this, SLOT(slotMessage(const QString &)));
    connect( this, SIGNAL(sgnlAssignedPlace(Place)), this, SLOT(slotAssignedPlace(Place)));
    connect( this, SIGNAL(sgnlReceptionCartes()), this, SLOT(slotReceptionCartes()));
    connect( this, SIGNAL(sgnlAfficheSelection(Place)), this, SLOT(slotAfficheSelection(Place)));
    connect( this, SIGNAL(sgnlChoixEnchere(Contrat)), this, SLOT(slotChoixEnchere(Contrat)));
    connect( this, SIGNAL(sgnlAfficheEnchere(Place,Contrat)), this, SLOT(slotAfficheEnchere(Place,Contrat)));
    connect( this, SIGNAL(sgnlRedist()), this, SLOT(slotRedist()));
    connect( this, SIGNAL(sgnlAfficheChien()), this, SLOT(slotAfficheChien()));
    connect( this, SIGNAL(sgnlPrepareChien()), this, SLOT(slotPrepareChien()));
    connect( this, SIGNAL(sgnlDepartDonne(Place,Contrat)), this, SLOT(slotDepartDonne(Place,Contrat)));
    connect( this, SIGNAL(sgnlJoueCarte()), this, SLOT(slotJoueCarte()));
    connect( this, SIGNAL(sgnlAfficheCarte(int, Place)), this, SLOT(slotAfficheCarte(int, Place)));
    connect( this, SIGNAL(sgnlFinDonne(Place, float, bool)), this, SLOT(slotFinDonne(Place, float, bool)));
    connect( this, SIGNAL(sgnlWaitPli(Place, float)), this, SLOT(slotWaitPli(Place, float)));

    timeBeforeSend.setSingleShot(true);
    timeBeforeSend.setInterval(0);
    connect(&timeBeforeSend, SIGNAL(timeout()), this, SLOT(slotTimeBeforeSend()));

#ifndef QT_NO_DEBUG
    debugger.attachTo(&botEngine);
#endif
}
/*****************************************************************************/
Bot::~Bot()
{

}
/*****************************************************************************/
void Bot::setTimeBeforeSend(int t)
{
    timeBeforeSend.setInterval(t);
}
/*****************************************************************************/
void Bot::slotTimeBeforeSend()
{
    Card *c = NULL;
    QString ret;

    QScriptValue createFunc = botEngine.evaluate("PlayCard");

    if (botEngine.hasUncaughtException()) {
        QScriptValue exception = botEngine.uncaughtException();
        QMessageBox::critical(0, "Script error", QString("Script threw an uncaught exception while looking for create func: ") + exception.toString());
        return;
    }

    if (!createFunc.isFunction()) {
        QMessageBox::critical(0, "Script Error", "createFunc is not a function!");
    }

    QScriptValueList args;
    args << infos.gameCounter;
    ret = createFunc.call(QScriptValue(), args).toString();

    if (botEngine.hasUncaughtException()) {
        QScriptValue exception = botEngine.uncaughtException();
        qDebug() << QString("Script threw an uncaught exception while looking for create func: ") + exception.toString();
        return;
    }

    // Test validity of card
    c = GetCardByName(ret);
    if (c != NULL) {
        if (isValid(c) == false) {
            QString message = getName() + QString(" played a non-valid card: ") + ret;
            qDebug() << message.toLatin1().constData();
            // The show must go on, play a random card
            c = play();
        }
    } else {
        // The show must go on, play a random card
        c = play();
    }

    removeCard(c);
    sendCard(c);
}
/*****************************************************************************/
void Bot::slotMessage( const QString &text )
{
    Q_UNUSED(text);
    // A bot cannot reply (yet :)
}
/*****************************************************************************/
void Bot::slotAssignedPlace(Place p)
{
    initializeScriptContext();

    QScriptValue createFunc = botEngine.evaluate("EnterGame");

    if (botEngine.hasUncaughtException()) {
        QScriptValue exception = botEngine.uncaughtException();
        QMessageBox::critical(0, "Script error", QString("Script threw an uncaught exception while looking for create func: ") + exception.toString());
        return;
    }

    if (!createFunc.isFunction()) {
        QMessageBox::critical(0, "Script Error", "createFunc is not a function!");
    }

    QScriptValueList args;
    args << p;
    createFunc.call(QScriptValue(), args);

    if (botEngine.hasUncaughtException()) {
        QScriptValue exception = botEngine.uncaughtException();
        qDebug() << QString("Script threw an uncaught exception while looking for create func: ") + exception.toString();
        return;
    }
}
/*****************************************************************************/
void Bot::slotReceptionCartes()
{
    updateStats();
}
/*****************************************************************************/
void Bot::slotAfficheSelection( Place p )
{
    Q_UNUSED(p);
    // nada aussi
}
/*****************************************************************************/
QScriptValue myPrint( QScriptContext * context, QScriptEngine * eng )
{
    Q_UNUSED(eng);

    QScriptValue arg = context->argument(0);
    if (!arg.isString()) {
        return context->throwError(
            QScriptContext::TypeError,
            QString::fromLatin1("print(): expected string argument"));
    }
    QString toPrint = QString("Bot script: ") + arg.toString();
    qDebug() << toPrint.toLatin1().constData();
    return QScriptValue();
}
/*****************************************************************************/
bool Bot::initializeScriptContext()
{
#ifndef QT_NO_DEBUG
#ifdef Q_OS_WIN32
    QString fileName = "E:/Personnel/tarotclub/ai/beginner.js";
#else
    QString fileName = "/home/anthony/Documents/tarotclub/ai/beginner.js";
#endif
#else
    // Release
    QString fileName = qApp->applicationDirPath() + "/ai/beginner.js";
#endif

    // Give access to some objects from the JavaScript engine
    botEngine.globalObject().setProperty("TStats", botEngine.newQObject(&statsObj));
    botEngine.globalObject().setProperty("TDeck", botEngine.newQObject(&deckObj));
    botEngine.globalObject().setProperty("print", botEngine.newFunction( &myPrint ) );

    QFile scriptFile(fileName);

    if (! scriptFile.exists()) {
        QMessageBox::critical(0, "Error", "Could not find program file!");
        return false;
    }

    if (! scriptFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::critical(0, "Error", "Could not open program file!");
        return false;
    }

    QString strProgram = scriptFile.readAll();

    // do static check so far of code:
    if (! botEngine.canEvaluate(strProgram) ) {
        QMessageBox::critical(0, "Error", "canEvaluate returned false!");
        return false;
    }
#ifndef QT_NO_DEBUG
    debugger.action(QScriptEngineDebugger::InterruptAction);
#endif
    // actually do the eval:
    botEngine.evaluate(strProgram);

    // uncaught exception?
    if (botEngine.hasUncaughtException()) {
        QScriptValue exception = botEngine.uncaughtException();
        QMessageBox::critical(0, "Script error", QString("Script threw an uncaught exception: ") + exception.toString());
        return false;
    }

    return true;
}
/*****************************************************************************/
void Bot::slotChoixEnchere(Contrat highestBid)
{
    int ret;
    Contrat mon_contrat;

    QScriptValue createFunc = botEngine.evaluate("CalculateBid");

    if (botEngine.hasUncaughtException()) {
        QScriptValue exception = botEngine.uncaughtException();
        QMessageBox::critical(0, "Script error", QString("Script threw an uncaught exception while looking for create func: ") + exception.toString());
        return;
    }

    if (!createFunc.isFunction()) {
        QMessageBox::critical(0, "Script Error", "createFunc is not a function!");
    }

    QScriptValueList args;
    args << (int)highestBid;
    ret = createFunc.call(QScriptValue(), args).toInteger();

    if (botEngine.hasUncaughtException()) {
        QScriptValue exception = botEngine.uncaughtException();
        qDebug() << QString("Script threw an uncaught exception while looking for create func: ") + exception.toString();
        return;
    }

    // security test
    if (ret >= PASSE && ret <= GARDE_CONTRE) {
        mon_contrat = (Contrat)ret;
    } else {
        mon_contrat = calculEnchere(); // propose our algorithm if the user's one failed
    }

    // only bid over previous one is allowed
    if( mon_contrat <= highestBid ) {
        mon_contrat = PASSE;
    }
    sendEnchere(mon_contrat);
}
/*****************************************************************************/
void Bot::slotAfficheEnchere( Place place, Contrat contract)
{
    Q_UNUSED(place);
    Q_UNUSED(contract);
}
/*****************************************************************************/
void Bot::slotRedist()
{
    // Re-inititialize script context
    if (initializeScriptContext() == true) {
        sendReady();
    } else {
        sendError();
    }
}
/*****************************************************************************/
void Bot::slotAfficheChien()
{
    sendVuChien();
}
/*****************************************************************************/
void Bot::slotPrepareChien()
{
    choixChien(&chien);
    sendChien();
}
/*****************************************************************************/
void Bot::slotDepartDonne(Place i_taker,Contrat i_contract)
{
    QScriptValue createFunc = botEngine.evaluate("StartGame");

    if (botEngine.hasUncaughtException()) {
        QScriptValue exception = botEngine.uncaughtException();
        QMessageBox::critical(0, "Script error", QString("Script threw an uncaught exception while looking for create func: ") + exception.toString());
        return;
    }

    if (!createFunc.isFunction()) {
        QMessageBox::critical(0, "Script Error", "createFunc is not a function!");
    }

    QScriptValueList args;
    args << i_taker << i_contract;
    createFunc.call(QScriptValue(), args);

    if (botEngine.hasUncaughtException()) {
        QScriptValue exception = botEngine.uncaughtException();
        qDebug() << QString("Script threw an uncaught exception while looking for create func: ") + exception.toString();
        return;
    }
}
/*****************************************************************************/
void Bot::slotJoueCarte()
{
    timeBeforeSend.start();
}
/*****************************************************************************/
void Bot::slotAfficheCarte(int id, Place p)
{
    Q_UNUSED(id);
    Q_UNUSED(p);

    QScriptValue createFunc = botEngine.evaluate("PlayedCard");

    if (botEngine.hasUncaughtException()) {
        QScriptValue exception = botEngine.uncaughtException();
        QMessageBox::critical(0, "Script error", QString("Script threw an uncaught exception while looking for create func: ") + exception.toString());
        return;
    }

    if (!createFunc.isFunction()) {
        QMessageBox::critical(0, "Script Error", "createFunc is not a function!");
    }

    QScriptValueList args;
    args << Jeu::getCard(id)->getCardName() << (int)p;
    createFunc.call(QScriptValue(), args);

    if (botEngine.hasUncaughtException()) {
        QScriptValue exception = botEngine.uncaughtException();
        qDebug() << QString("Script threw an uncaught exception while looking for create func: ") + exception.toString();
        return;
    }
}
/*****************************************************************************/
void Bot::slotFinDonne(Place winner, float pointsTaker, bool lastDeal)
{
    Q_UNUSED(winner);
    Q_UNUSED(pointsTaker);
    if (lastDeal == false) {
        sendReady();
    }
}
/*****************************************************************************/
void Bot::slotWaitPli(Place winner, float pointsTaker)
{
    Q_UNUSED(winner);
    Q_UNUSED(pointsTaker);
    sendVuPli();
}

//=============================================================================
// End of file Bot.cpp
//=============================================================================

