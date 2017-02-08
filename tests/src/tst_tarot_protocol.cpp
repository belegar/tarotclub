
#include <QString>
#include <QtTest>
#include <QCoreApplication>
#include <cstdint>
#include <iostream>

#include "tst_tarot_protocol.h"

#include "Bot.h"
#include "Lobby.h"
#include "Protocol.h"
#include "TcpServer.h"
#include "TcpClient.h"
#include "Semaphore.h"

TarotProtocol::TarotProtocol()
{
}


bool Dump(const std::vector<Reply> &reply)
{
    bool endOfGame = false;
    for (std::uint32_t i = 0U; i < reply.size(); i++)
    {
        std::cout << "------------------------------------" << std::endl;

        for (std::uint32_t j = 0U; j < reply[i].dest.size(); j++)
        {
            std::cout << "Destination #" << j << ": " << (int)reply[i].dest[j]  << std::endl;
        }
        std::string data = reply[i].data.ToString(0U);
        std::cout << data << std::endl;

        if (data.find("EndOfGame") != std::string::npos)
        {
            endOfGame = true;
        }
    }
    return endOfGame;
}


// Basic packet with no data
static const std::string test1 = "4F:8B5C:00AC:0000:QUIT:";

void TarotProtocol::TestPacketCodec()
{
    Protocol proto;
    std::string output;

    proto.Add(test1);

    QCOMPARE(proto.Parse(output), true);
    QCOMPARE(output, std::string());
    QCOMPARE(proto.GetOption(), (std::uint32_t)0x4F);
    QCOMPARE(proto.GetDestUuid(), (std::uint32_t)0x00AC);
    QCOMPARE(proto.GetSourceUuid(), (std::uint32_t)0x8B5C);
    QCOMPARE(proto.GetType(), std::string("QUIT"));
    QCOMPARE(proto.GetSize(), (std::uint32_t)0U);
}

// Test data streaming with fragmented packets, last packet is not complete
std::string gThreeAndHalf = test1 + test1 + test1 + test1.substr(0, 10);
std::string gEndAndTwo = test1.substr(10, test1.size() - 10) + test1 + test1;
std::string gBadPacket = test1.substr(5, 10) + test1.substr(2, 13) + "::56BFGTWF9076:4400";

Semaphore gSem;
int nb_packets = 0;

/**
 * @brief Fake server that only parses the packets
 */
class ProtoServer : public tcp::TcpServer::IEvent
{

public:

    virtual void NewConnection(const tcp::Conn &conn)
    {
        (void) conn;
    }

    virtual void ReadData(const tcp::Conn &conn)
    {
        std::string data;

        mProto.Add(conn.payload);

        while (mProto.Parse(data))
        {
        //    std::cout << "Found one packet with data: " << data << std::endl;
            nb_packets++;
        }

        gSem.Notify();
    }

    virtual void ClientClosed(const tcp::Conn &conn)
    {
        (void) conn;
    }

    virtual void ServerTerminated(tcp::TcpServer::IEvent::CloseType type)
    {
        (void) type;
    }

private:
    Protocol mProto;
};

void TarotProtocol::TestPacketStream()
{
    ProtoServer protoSrv;

    tcp::TcpServer server(protoSrv);

    // Init the whole Tcp stack
    tcp::TcpSocket::Initialize();

    server.Start(10U, false, 61617);

    // Then simulate the client sending multiple packets
    tcp::TcpClient client;

    client.Initialize();
    client.Connect("127.0.0.1", 61617);

    // Send packets and wait for them...
    QCOMPARE(client.Send(test1), true);
    QCOMPARE(gSem.Wait(2000), true);

    QCOMPARE(client.Send(gThreeAndHalf), true);
    QCOMPARE(gSem.Wait(2000), true);

    QCOMPARE(client.Send(gEndAndTwo), true);
    QCOMPARE(gSem.Wait(2000), true);

    QCOMPARE(client.Send(gBadPacket), true);
    QCOMPARE(gSem.Wait(2000), true);

    QCOMPARE(nb_packets, 7);

    server.Stop();
    server.Join();
}


void TarotProtocol::TestPlayerJoinAndQuit()
{
    std::vector<std::string> tables;
    tables.push_back("test");
    Lobby lobby;

    lobby.Initialize("test", tables);

    std::vector<Reply> reply;

    std::uint32_t src = Protocol::INVALID_UID;
    std::uint32_t dest = Protocol::LOBBY_UID;

    JsonObject json;

    src = lobby.AddUser(reply);
    Dump(reply);
    reply.clear();

    json.AddValue("cmd", "ReplyLogin");
    json.AddValue("nickname", "Casimir");

    QCOMPARE(lobby.Deliver(src, dest, json.ToString(0U), reply), true);
    Dump(reply);
    reply.clear();
}

void TarotProtocol::TestBotsFullGame()
{
    std::vector<std::string> tables;
    tables.push_back("test");
    Lobby lobby;

    lobby.Initialize("test", tables);

    std::vector<Reply> lobby_data;

    struct BotManager
    {
        std::vector<Reply> reply;
        Bot bot;
        std::uint32_t uuid;
    };

    BotManager bots[4];
    std::string names[4];
    names[0] = "riri";
    names[1] = "fifi";
    names[2] = "loulou";
    names[3] = "tata";

    std::uint32_t tableId = Protocol::TABLES_UID;

    // Initialize & Register the 4 bots on the lobby
    for (int i = 0; i < 4; i++)
    {
        bots[i].uuid = lobby.AddUser(lobby_data);
        bots[i].bot.SetUser(names[i], "");
        bots[i].bot.SetAiScript(Util::ExecutablePath() + "/../../../bin/aicontest/ai.zip");
        bots[i].bot.SetTableToJoin(tableId);
    }

    Dump(lobby_data);

    // We run N games in a loop
    int gameCounter = 0;
    while(gameCounter < 10)
    {
        // For all bots
        for (int i = 0; i < 4; i++)
        {
            bots[i].reply.clear();

            std::cout << "------------------------------------" << std::endl;
            std::cout << "Bot " << bots[i].bot.GetPlace().ToString() << " cards: " << bots[i].bot.GetDeck() << std::endl;

            // Send data to that bot if any
            for (std::uint32_t j = 0U; j < lobby_data.size(); j++)
            {

                for (std::uint32_t k = 0U; k < lobby_data[j].dest.size(); k++)
                {
                    if ((lobby_data[j].dest[k] == bots[i].uuid) ||
                        (lobby_data[j].dest[k] == Protocol::LOBBY_UID) ||
                        (lobby_data[j].dest[k] == bots[i].bot.GetCurrentTable()))
                    {
                        bots[i].bot.Decode(Protocol::LOBBY_UID, lobby_data[j].dest[k], lobby_data[j].data.ToString(0U), bots[i].reply);
                    }
                }
            }

            Dump(bots[i].reply);
        }

        lobby_data.clear();
        // For all bots
        for (int i = 0; i < 4; i++)
        {
            // Send bot replies to the lobby (destination should be always the lobby or a table)
            for (std::uint32_t j = 0U; j < bots[i].reply.size(); j++)
            {
                // No allowed broadcast for messages from clients
                QCOMPARE(bots[i].reply[j].dest.size(), 1U);
                QCOMPARE(lobby.Deliver(bots[i].uuid, bots[i].reply[j].dest[0], bots[i].reply[j].data.ToString(0U), lobby_data), true);
            }
        }
        if (Dump(lobby_data))
        {
            gameCounter++;
        }
    }
}
