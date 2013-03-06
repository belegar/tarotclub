/*=============================================================================
 * TarotClub - Bot.cpp
 *=============================================================================
 * Modélise un robot joueur. Interface vers les scripts d'IA
 *=============================================================================
 * TarotClub ( http://www.tarotclub.fr ) - This file is part of TarotClub
 * Copyright (C) 2003-2999 - Anthony Rabine
 * anthony@tarotclub.fr
 *
 * This file must be used under the terms of the CeCILL.
 * This source file is licensed as described in the file COPYING, which
 * you should have received as part of this distribution.  The terms
 * are also available at
 * http://www.cecill.info/licences/Licence_CeCILL_V2-en.txt
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
{
    connect( this, SIGNAL(sgnlMessage(const QString &)), this, SLOT(slotMessage(const QString &)));
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

    debugger.attachTo(&botEngine);

    initializeScriptContext();

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
    Card *c;
    c = play();
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
   // return QScriptEngine();

    QScriptValue arg = context->argument(0);
    if (!arg.isString()) {
        return context->throwError(
            QScriptContext::TypeError,
            QString::fromLatin1("print(): expected string argument"));
    }
    QString toPrint = arg.toString();
    qDebug(toPrint.toLatin1().constData());
    return QScriptValue();
}

bool Bot::initializeScriptContext()
{
#ifndef QT_NO_DEBUG
#ifdef Q_OS_WIN32
    QString fileName = "E:/Personnel/tarotclub/ai/bid.js";
#else
    QString fileName = "/home/anthony/tarotclub/ai/bid.qs";
#endif
#endif

    botEngine.globalObject().setProperty("TStats", botEngine.newQObject(&statsObj));
   // botEngine.globalObject().setProperty("TDbg", botEngine.newQObject(&dbgObj));

    botEngine.globalObject().setProperty( "print", botEngine.newFunction( &myPrint ) );

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

    debugger.action(QScriptEngineDebugger::InterruptAction);
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
void Bot::slotChoixEnchere( Contrat c )
{
    int ret;
    Contrat mon_contrat;

    QScriptValue createFunc = botEngine.evaluate("calculateBid");

    if (botEngine.hasUncaughtException()) {
        QScriptValue exception = botEngine.uncaughtException();
        QMessageBox::critical(0, "Script error", QString("Script threw an uncaught exception while looking for create func: ") + exception.toString());
        return;
    }

    if (!createFunc.isFunction()) {
        QMessageBox::critical(0, "Script Error", "createFunc is not a function!");
    }

    ret = createFunc.call().toInteger();

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
    if( mon_contrat <= c ) {
        mon_contrat = PASSE;
    }
    sendEnchere(mon_contrat);
}
/*****************************************************************************/
void Bot::slotAfficheEnchere( Place, Contrat )
{
    // nada
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
void Bot::slotDepartDonne(Place p,Contrat c)
{
    Q_UNUSED(p);
    Q_UNUSED(c);
    // nada
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
    // nada
}
/*****************************************************************************/
void Bot::slotFinDonne(Place winner, float pointsTaker, bool lastDeal)
{
    Q_UNUSED(winner);
    Q_UNUSED(pointsTaker);
    Q_UNUSED(lastDeal);
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

