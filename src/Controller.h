/*=============================================================================
 * TarotClub - Controller.h
 *=============================================================================
 * Manage TarotClub protocol requests within a Tarot context
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
#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <thread>
#include <map>
#include "Protocol.h"
#include "TarotEngine.h"
#include "Observer.h"
#include "ByteArray.h"
#include "ThreadQueue.h"

/*****************************************************************************/
/**
 * @brief The Controller class
 *
 * A server instance creates one controller thread. All requests to the server must be
 * performed using a request packet sent to the send for executions.
 *
 * All the requests are queued in the order of arrival and executed in a FIFO mode.
 *
 * The TarotEngine is not accessible to protect accesses and enforce all the
 * calls within the thread context. This mechanism allow a protection by design against
 * multi-threaded application.
 */
class Controller
{

public:
    enum SignalType
    {
        SIG_SEND_DATA,    //!< Emitted when there is something to send to a client
        SIG_GAME_FULL     //!< Emitted when there is enough players to start the game
    };

    struct Signal
    {
        SignalType type;
        ByteArray data;
    };

    Controller();

    void RegisterListener(Observer<Signal> &observer);
    void Start();
    void Stop();
    void ExecuteRequest(const ByteArray &packet);

private:
    TarotEngine engine;
    Subject<Signal> mSubject;
    std::thread mThread;
    ThreadQueue<ByteArray> mQueue; //!< Queue of network packets received
    bool mInitialized;

    /**
     * @brief Main server thread loop
     */
    void Run();
    static void EntryPoint(void *pthis);

    void NewDeal();
    bool DoAction(const ByteArray &data);
    void SendPacket(const ByteArray &block);
    void SignalGameFull();
    void BidSequence();
    void GameSequence();
};

#endif // CONTROLLER_H

//=============================================================================
// End of file Controller.h
//=============================================================================
