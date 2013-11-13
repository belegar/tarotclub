/*=============================================================================
 * TarotClub - Bot.h
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

#ifndef _BOT_H
#define _BOT_H

#include "Client.h"
#include <QJSEngine>
#include "Log.h"

class TarotUtil : public QObject
{
    Q_OBJECT

public:

public slots:

    void Print(const QString &message)
    {
        QString toPrint = QString("Bot script: ") + message;
        qDebug() << toPrint.toLatin1().constData();
    }

    void Log(const QString &key, const QString &value)
    {
        Log::AddEntry(Log::Bot, key, value);
    }
};
/*****************************************************************************/
class Bot : public Client
{
    Q_OBJECT

public:
    Bot();
    ~Bot();

    void SetTimeBeforeSend(int t);

private:
    QTimer  timeBeforeSend;
    QJSEngine botEngine;
    // Exposed object to the Javascript
    TarotUtil utilObj;

    QJSValue CallScript(const QString &function, QJSValueList &args);
    bool InitializeScriptContext();

private slots:
    void slotTimeBeforeSend();

    // client events
    void slotMessage(const QString &message);
    void slotAssignedPlace(Place p);
    void slotPlayersList(QMap<Place, Identity> &players);
    void slotReceiveCards();
    void slotSelectPlayer(Place p);
    void slotRequestBid(Contract highestBid);
    void slotShowBid(Place, bool slam, Contract contract);
    void slotStartDeal(Place, Contract contract);
    void slotShowDog();
    void slotBuildDiscard();
    void slotDealAgain();
    void slotPlayCard();
    void slotShowCard(int id, Place p);
    void slotShowHandle();
    void slotWaitTrick(Place winner);
    void slotEndOfDeal();
    void slotEndOfGame();


};

#endif // _BOT_H

//=============================================================================
// End of file Bot.h
//=============================================================================
