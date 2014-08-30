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
    class IEvent
    {
    public:
        virtual void SendData(const ByteArray &block) = 0;
    };

    Controller(IEvent &handler);

    void Start();
    void Stop();
    void ExecuteRequest(const ByteArray &packet);

private:
    IEvent     &mEventHandler;
    TarotEngine engine;
    std::thread mThread;
    ThreadQueue<ByteArray> mQueue; //!< Queue of network packets received
    bool mInitialized;
    bool mFull;
    std::vector<std::uint32_t> mAdmins; //!< A list of admin players (default is the first player to be connected

    /**
     * @brief Main server thread loop
     */
    void Run();
    static void EntryPoint(void *pthis);

    void NewDeal();
     void StartDeal();
    bool DoAction(const ByteArray &data);
    void BidSequence();
    void GameSequence();
    bool IsAdmin(std::uint32_t unique_id);

};

#endif // CONTROLLER_H

//=============================================================================
// End of file Controller.h
//=============================================================================
