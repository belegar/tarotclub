#include "Session.h"
#include "Log.h"
#include "Protocol.h"

/*****************************************************************************/
void Session::Initialize()
{
    if (!mInitialized)
    {
        mThread = std::thread(Session::EntryPoint, this);
        mInitialized = true;
    }
}
/*****************************************************************************/
void Session::Send(const std::vector<Reply> &out)
{
    // Send all messages
    for (std::uint32_t i = 0U; i < out.size(); i++)
    {
        // To all indicated peers
        for (std::uint32_t j = 0U; j < out[i].dest.size(); j++)
        {
            SendToHost(Protocol::Build(mListener.GetUuid(), out[i].dest[j], out[i].data.ToString(0U)));
        }
    }
}
/*****************************************************************************/
void Session::SendToHost(const std::string &data)
{
//    std::stringstream dbg;
//    dbg << "Client sending packet: 0x" << std::hex << (int)cmd;
///    TLogNetwork(dbg.str());

    if (IsConnected())
    {
        mTcpClient.Send(data);
    }
    else
    {
        TLogNetwork("WARNING! try to send packet without any connection.");
    }
}
/*****************************************************************************/
bool Session::IsConnected()
{
    return mTcpClient.IsConnected();
}
/*****************************************************************************/
void Session::Disconnect()
{
    mTcpClient.Close();
}
/*****************************************************************************/
void Session::ConnectToHost(const std::string &hostName, std::uint16_t port)
{
    Disconnect();
    if (!mTcpClient.IsValid())
    {
        // Create a socket before connection
        mTcpClient.Initialize();
    }

    mHostName = hostName;
    mTcpPort = port;
    mQueue.Push(START);
}
/*****************************************************************************/
void Session::Close()
{
    Disconnect();

    if (mInitialized)
    {
        mQueue.Push(EXIT);
        mThread.join();
        mInitialized = false;
    }
}
/*****************************************************************************/
void Session::EntryPoint(void *pthis)
{
    Session *pt = static_cast<Session *>(pthis);
    pt->Run();
}
/*****************************************************************************/
void Session::Run()
{
    Command cmd;

    while (true)
    {
        mQueue.WaitAndPop(cmd);
        if (cmd == START)
        {
            if (mTcpClient.Connect(mHostName, mTcpPort) == true)
            {
                bool connected = true;
                Protocol proto;

                while (connected)
                {
                    if (mTcpClient.DataWaiting(10U))
                    {
                        std::string payload;
                        if (mTcpClient.Recv(payload))
                        {
                            proto.Add(payload);
                            std::string data;
                            while (proto.Parse(data))
                            {
                            //    TLogNetwork("Found one packet with data: " + data);
                                std::vector<Reply> out;
                                std::uint32_t client_uuid = proto.GetDestUuid();

                                bool ret = mListener.Deliver(proto.GetSourceUuid(), client_uuid, data, out);
                                // Send synchronous data to the server
                                if (ret)
                                {
                                    Send(out);
                                }
                            }
                        }
                    }
                    else
                    {
                        //std::cout << "client wait timeout or failure" << std::endl;
                        connected = IsConnected(); // determine origine of failure
                    }
                }
                mListener.Signal(net::IEvent::ErrDisconnectedFromServer);
                TLogNetwork("Client connection closed.");
            }
            else
            {
                TLogError("Client cannot connect to server.");
                mListener.Signal(net::IEvent::ErrCannotConnectToServer);
            }
        }
        else if (cmd == EXIT)
        {
            return;
        }
    }
}
