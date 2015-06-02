#ifndef NETCLIENT_H
#define NETCLIENT_H

#include <thread>
#include "ThreadQueue.h"
#include "TcpClient.h"
#include "Protocol.h"

/*****************************************************************************/
class NetClient
{
public:
    enum Command
    {
        START,
        EXIT
    };

    class IEvent
    {
    public:
        static const std::uint32_t ErrDisconnectedFromServer    = 6000U;
        static const std::uint32_t ErrCannotConnectToServer     = 6001U;

        virtual void NetSignal(std::uint32_t sig) = 0;
    };

    NetClient(const std::shared_ptr<Protocol::IWorkItem> &item, IEvent &event);

    void Initialize();
    void ConnectToHost(const std::string &hostName, std::uint16_t port);
    bool IsConnected();
    void Disconnect();
    void Close();
    void SendPacket(const ByteArray &packet);

private:
    Protocol::IWorkItem::Data mItem;
    IEvent &mEvent;

    ThreadQueue<Command> mQueue;
    bool        mConnected;
    std::string mHostName;
    std::uint16_t mTcpPort;
    TcpClient   mTcpClient;
    std::thread mThread;
    bool        mInitialized;

    static void EntryPoint(void *pthis);
    void Run();
};


#endif // NETCLIENT_H
