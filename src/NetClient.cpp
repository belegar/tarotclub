#include "NetClient.h"
#include "Log.h"

/*****************************************************************************/
NetClient::NetClient(const std::shared_ptr<Protocol::IWorkItem> &item, IEvent &event)
    : mItem(item)
    , mEvent(event)
    , mConnected(false)
    , mInitialized(false)
{

}
/*****************************************************************************/
void NetClient::Initialize()
{
    if (!mInitialized)
    {
        mInitialized = mTcpClient.Start();
        if (mInitialized)
        {
            mThread = std::thread(NetClient::EntryPoint, this);
        }
        else
        {
            TLogError("Cannot initialize socket client");
        }
    }
}

/*
void NetClient::SendQuitTable(std::uint32_t tableId)
{
    ByteArray packet = Protocol::ClientQuitTable(mPlayer.GetUuid(), tableId);
    SendPacket(packet);
}

void NetClient::SendIdentity(const Identity &identity)
{
    ByteArray packet = Protocol::ClientReplyLogin(mPlayer.GetUuid(), identity);
    SendPacket(packet);
}

void NetClient::SendChangeIdentity(const Identity &identity)
{
    ByteArray packet = Protocol::ClientChangeIdentity(mPlayer.GetUuid(), identity);
    SendPacket(packet);
}

void NetClient::SendTableMessage(const std::string &message)
{
    ByteArray packet = Protocol::ClientLobbyMessage(message, mPlayer.GetUuid(), mTableId);
    SendPacket(packet);
}

void NetClient::SendLobbyMessage(const std::string &message)
{
    ByteArray packet = Protocol::ClientLobbyMessage(message, mPlayer.GetUuid(), Protocol::LOBBY_UID);
    SendPacket(packet);
}
*/

/*****************************************************************************/
void NetClient::SendPacket(const ByteArray &packet)
{
    std::uint8_t cmd = Protocol::GetCommand(packet);
    std::stringstream dbg;
    dbg << "Client sending packet: 0x" << std::hex << (int)cmd;
    TLogNetwork(dbg.str());

    if (IsConnected())
    {
        mTcpClient.Send(packet.ToSring());
    }
    else
    {
        TLogNetwork("WARNING! try to send packet without any connection.");
    }
}
/*****************************************************************************/
bool NetClient::IsConnected()
{
    return mConnected;
}
/*****************************************************************************/
void NetClient::Disconnect()
{
    mConnected = false;
    mTcpClient.Close();
}
/*****************************************************************************/
void NetClient::ConnectToHost(const std::string &hostName, std::uint16_t port)
{
    Disconnect();
    if (!mTcpClient.IsValid())
    {
        // Create a socket before connection
        mTcpClient.Create();
    }

    mHostName = hostName;
    mTcpPort = port;
    mQueue.Push(START);
}
/*****************************************************************************/
void NetClient::Close()
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
void NetClient::EntryPoint(void *pthis)
{
    NetClient *pt = static_cast<NetClient *>(pthis);
    pt->Run();
}
/*****************************************************************************/
void NetClient::Run()
{
    Command cmd;

    while (true)
    {
        mQueue.WaitAndPop(cmd);
        if (cmd == START)
        {
            if (mTcpClient.Connect(mHostName, mTcpPort) == true)
            {
                mConnected = true;
                while (mConnected)
                {
                    std::int32_t ret = mTcpClient.Recv(mItem.data);
                    if (ret > 0)
                    {
                        Protocol::GetInstance().Execute(mItem);
                    }
                    else if (ret == 0)
                    {
                        mConnected = false;
                        TLogNetwork("Lost connection!");
                    }
                    else if (ret == -2)
                    {
                        // try again, ignore and select again the socket
                        std::cout << "Sock: try again" << std::endl;
                    }
                    else
                    {
                        mConnected = false;
                        TLogNetwork("Connection reset by peer.");
                    }
                }
                mEvent.NetSignal(IEvent::ErrDisconnectedFromServer);
            }
            else
            {
                TLogError("Client cannot connect to server.");
                mEvent.NetSignal(IEvent::ErrCannotConnectToServer);
            }
        }
        else if (cmd == EXIT)
        {
            return;
        }
    }
}

