/*=============================================================================
 * TarotClub - Server.h
 *=============================================================================
 * Host a Tarot game and manage network clients
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
#ifndef SERVER_H
#define SERVER_H

#include <thread>
#include <map>
#include "Protocol.h"
#include "TarotEngine.h"
#include "Observer.h"
#include "ByteArray.h"
#include "ThreadQueue.h"

/*****************************************************************************/
/**
 * @brief The Server class
 *
 * A server instance creates a thread. All requests to the server must be
 * performed using a request packet sent to the send for executions.
 *
 * All the requests are queued in the order of arrival and executed in a FIFO mode.
 *
 * The TarotEngine is not accessible to protect accesses and enforce all the
 * calls within the thread context.
 *
 */
class Server : public Observer<TarotEngine::SignalInfo>
{

public:
    struct Signal
    {
        ByteArray data;
    };

    Server();

    void RegisterListener(Observer<Signal> &sig);
    void Start();
    void ExecuteRequest(const ByteArray &packet);

private:
    TarotEngine engine;
    Subject<Signal> mSubject;
    std::thread mThread;
    ThreadQueue<ByteArray> mQueue; //!< Queue of network packets received

    /**
     * @brief Main server thread loop
     */
    void Run();
    static void EntryPoint(void * pthis);

    void NewServerGame(Game::Mode mode);

    void Update(const TarotEngine::SignalInfo &info);
    bool DoAction(const ByteArray &data);
    void Broadcast(ByteArray &block);
    void SendPacket(const ByteArray &block);

    // Packets sent to clients (players)

    void SendDisconnect();
    void SendShowBid(Contract c, bool slam, Place p);
    void SendErrorServerFull(QTcpSocket *cnx);

    void SendPlayersList();
    void SendBuildDiscard();
    void SendShowCard(Card *c);
    void SendShowHandle(Deck &handle, Place p);

    // TarotClub engine callbacks
    void slotSendCards();
    void slotSendWaitTrick(Place winner);
    void slotSendStartDeal();
    void slotSendPlayCard(Place p);
    void slotSendRequestBid(Contract c, Place p);
    void slotSendShowDog();
    void slotSendDealAgain();
    void slotSendEndOfDeal();
};

#endif // SERVER_H

//=============================================================================
// End of file Server.h
//=============================================================================
