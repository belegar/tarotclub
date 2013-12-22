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
        std::string toPrint = "Bot script: " + message.toStdString();
        TLogInfo(toPrint);
    }
};
/*****************************************************************************/
class Bot : public QObject, public Client::IEvent
{
    Q_OBJECT

public:
    Bot();
    ~Bot();

    void SetTimeBeforeSend(int t);
    void SetIdentity(const Identity &ident);
    void Initialize();
    void ConnectToHost(const std::string &hostName, std::uint16_t port);

private:
    Client  mClient;
    QTimer  timeBeforeSend;
    QJSEngine botEngine;
    // Exposed object to the Javascript
    TarotUtil utilObj;

    // Javascript stuff
    QJSValue CallScript(const QString &function, QJSValueList &args);
    bool InitializeScriptContext();

    // Client events
    virtual void Message(const std::string &message);
    virtual void AssignedPlace();
    virtual void PlayersList(std::map<Place, Identity> &players);
    virtual void ReceiveCards();
    virtual void SelectPlayer(Place p);
    virtual void RequestBid(Contract highestBid);
    virtual void ShowBid(Place p, bool slam, Contract c);
    virtual void StartDeal(Place taker, Contract c, const Game::Shuffle &sh);
    virtual void ShowDog();
    virtual void ShowHandle();
    virtual void BuildDiscard();
    virtual void DealAgain();
    virtual void PlayCard();
    virtual void ShowCard(Place p, const std::string &name);
    virtual void WaitTrick(Place winner);
    virtual void EndOfDeal();
    virtual void EndOfGame();

private slots:
    void slotTimeBeforeSend();
};

#endif // _BOT_H

//=============================================================================
// End of file Bot.h
//=============================================================================
