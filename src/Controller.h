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

#include <vector>
#include "Protocol.h"
#include "TarotEngine.h"
#include "Observer.h"

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
class Controller : public Protocol::IWorkItem
{

public:
    class IEvent
    {
    public:
        virtual void SendData(const ByteArray &block) = 0;
    };

    Controller(IEvent &handler);
    virtual ~Controller () { /* nothing to do */ }

    void Initialize();
    void ExecuteRequest(const ByteArray &packet);

    std::string GetName() { return mName; }
    void SetName(const std::string &name) { mName = name; }

    std::uint32_t GetId() { return mId; }
    void SetId(std::uint32_t id) { mId = id; }

private:
    IEvent     &mEventHandler;
    TarotEngine engine;
    ThreadQueue<ByteArray> mQueue;      //!< Queue of network packets received
    bool mFull;
    std::vector<std::uint32_t> mAdmins; //!< A list of admin players (default is the first player to be connected
    std::string mName; //!< Name of this table
    std::uint32_t mId; //!< Table ID

    // From Protocol::WorkItem
    bool DoAction(std::uint8_t cmd, std::uint32_t src_uuid, std::uint32_t dest_uuid, const ByteArray &data);
    ByteArray GetPacket();

    void NewDeal();
    void StartDeal();
    void BidSequence();
    void GameSequence();
    bool IsAdmin(std::uint32_t unique_id);

};

#endif // CONTROLLER_H

//=============================================================================
// End of file Controller.h
//=============================================================================
