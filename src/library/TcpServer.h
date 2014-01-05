#ifndef TCP_SERVER_H
#define TCP_SERVER_H

#include <string>
#include <thread>
#include <vector>
#include "TcpSocket.h"
#include "Observer.h"

class TcpServer : public TcpSocket
{
public:
    TcpServer();

    enum SignalType
    {
        NEW_CONNECTION,
        DATA_RECEIVED
    };

    struct Signal
    {
        SignalType type;
        int socket;
        std::string data;
    };

    virtual ~TcpServer(void) { }

    bool Start(std::uint16_t port, std::int32_t maxConnections);
    void Stop();

    void RegisterListener(Observer<Signal> &obs)
    {
        mSubject.Attach(obs);
    }

private:
    std::thread mThread;
    int  mMaxSd;
    fd_set mMasterSet;
    Subject<Signal> mSubject;
    std::vector<int> mClients;
    bool mInitialized;

    static void EntryPoint(void *pthis);
    void Run();
    void IncommingConnection();
    bool IncommingData(int in_sock);
    void UpdateMaxSocket();
};


#endif // TCP_SERVER_H
