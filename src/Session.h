#ifndef SESSION_H
#define SESSION_H

#include "Network.h"
#include "ThreadQueue.h"
#include "TcpClient.h"

class Session
{
public:
    enum Command
    {
        NO_CMD,
        START,
        EXIT
    };

    explicit Session(net::IEvent &client)
        : mListener(client)
        , mInitialized(false)
        , mTcpPort(0U)
    {

    }

    void Initialize(const std::string &key);
    void Send(const std::vector<Reply> &out);
    bool IsConnected();
    void Disconnect();
    void ConnectToHost(const std::string &hostName, std::uint16_t port);
    void Close();
private:
    void Sorter();

    net::IEvent &mListener;

    // Client management over the TCP network
    ThreadQueue<Command> mQueue;
    tcp::TcpClient   mTcpClient;
    std::thread mThread;
    bool        mInitialized;
    std::string mHostName;
    std::uint16_t mTcpPort;
    Protocol mProto;

    static void EntryPoint(void *pthis);
    void SendToHost(const std::string &data);
    void Run();
};


#endif // SESSION_H
