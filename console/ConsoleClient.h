#ifndef CONSOLECLIENT_H
#define CONSOLECLIENT_H

#include "Log.h"
#include "BotManager.h"
#include "Console.h"
#include "BasicClient.h"

#include <list>

#include <fcntl.h>
#include <io.h>
#include <stdio.h>

/*****************************************************************************/
class ConsoleClient : public Observer<std::string>, public net::IEvent
{
public:
    ConsoleClient();

    // From ConsoleClient
    void Stop();
    void Run(uint16_t tcp_port);

private:
    void Start(uint16_t tcp_port);

    // From Observer
    void Update(const std::string &info);

    // From net::IEvent
    virtual void Signal(std::uint32_t sig);
    virtual bool Deliver(uint32_t src_uuid, uint32_t dest_uuid, const std::string &arg, std::vector<Reply> &out);
    virtual std::uint32_t AddUser(std::vector<Reply> &/*out*/);
    virtual void RemoveUser(std::uint32_t /* uuid */, std::vector<Reply> &/*out*/);

    Session mSession;
    BasicClient mClient;
    Context mCtx;
    BotManager mBots;
    std::list<std::uint32_t> mIds;
    Console mConsole;

};

#endif // CONSOLECLIENT_H
