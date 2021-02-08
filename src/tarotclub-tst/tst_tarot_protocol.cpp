
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
static const std::string test1 = "4F:8B5C:00AC:0000:0001:";

void TarotProtocol::TestEmptyPacket()
{
    Protocol proto;
    std::string output;
    Protocol::Header h;

    proto.Add(test1);

    QCOMPARE(proto.Parse(output, h), true);
    QCOMPARE(output, std::string());
    QCOMPARE(h.option, (std::uint32_t)0x4F);
    QCOMPARE(h.dst_uid, (std::uint32_t)0x00AC);
    QCOMPARE(h.src_uid, (std::uint32_t)0x8B5C);
    QCOMPARE(h.payload_size, (std::uint32_t)0U);
}

static const std::string testData1 = "{ coucou: 2 }";
static const std::string testKey = "ABCDEFGHIJKLMNOP";
static const uint32_t testDestUuid = 34;
static const uint32_t testSourceUuid = 1807;

void TarotProtocol::TestCiphering()
{
    Protocol protoSnd;
    Protocol protoRcv;
    std::string output;
    Protocol::Header h;

    protoSnd.SetSecurty(testKey);
    protoRcv.SetSecurty(testKey);

    std::string sendString = protoSnd.Build(testSourceUuid, testDestUuid, testData1);
    protoRcv.Add(sendString);

    QCOMPARE(protoRcv.Parse(output, h), true);
    QCOMPARE(output, testData1);
    QCOMPARE(h.option, (std::uint32_t)0);
    QCOMPARE(h.dst_uid, testDestUuid);
    QCOMPARE(h.src_uid, testSourceUuid);
    QCOMPARE(h.payload_size, testData1.size());
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

    ProtoServer() {
        mProto.SetSecurty("1234567890123456");
    }

    virtual void NewConnection(const tcp::Conn &conn)
    {
        (void) conn;
    }

    virtual void ReadData(const tcp::Conn &conn)
    {
        std::string data;
        Protocol::Header h;
        mProto.Add(conn.payload);

        while (mProto.Parse(data, h))
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

bool AddUser(const std::string &nickname, Lobby &lobby, std::vector<Reply> &reply)
{
    bool success = false;
    std::uint32_t src = Protocol::INVALID_UID;
    std::uint32_t dest = Protocol::LOBBY_UID;

    JsonObject json;

    src = lobby.AddUser(reply);
    Dump(reply);
    reply.clear();

    json.AddValue("cmd", "ReplyLogin");
    json.AddValue("nickname", nickname);

    success = lobby.Deliver(src, dest, json.ToString(0U), reply);

    return success;
}


void TarotProtocol::TestPlayerJoinQuitAndPlayerList()
{
    std::vector<std::string> tables;
    tables.push_back("test");
    Lobby lobby;

    lobby.Initialize("test", tables);

    std::vector<Reply> reply;

    QCOMPARE(AddUser("Casimir", lobby, reply), true);
    Dump(reply);
    reply.clear();

    // Add a second user with the same nickname to the lobby
    QCOMPARE(AddUser("Casimir", lobby, reply), true);
    Dump(reply);

    // We should have only one player in the lobby
    std::uint32_t expected = 1U;
    QCOMPARE(lobby.GetNumberOfPlayers(), expected);

    reply.clear();

    // Add a second user to the lobby
    QCOMPARE(AddUser("Popeye", lobby, reply), true);
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
    Identity names[4] =
    {
        {"riri", "", Identity::cGenderRobot},
        {"fifi", "", Identity::cGenderRobot},
        {"loulou", "", Identity::cGenderRobot},
        {"toto", "", Identity::cGenderRobot}
    };

    std::uint32_t tableId = Protocol::TABLES_UID;

    // Initialize & Register the 4 bots on the lobby
    for (int i = 0; i < 4; i++)
    {
        bots[i].uuid = lobby.AddUser(lobby_data);
        bots[i].bot.SetIdentity(names[i]);
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
                std::uint32_t expected = 1UL;
                std::uint32_t value = static_cast<std::uint32_t>(bots[i].reply[j].dest.size());
                QCOMPARE(value, expected);
                QCOMPARE(lobby.Deliver(bots[i].uuid, bots[i].reply[j].dest[0], bots[i].reply[j].data.ToString(0U), lobby_data), true);
            }
        }
        if (Dump(lobby_data))
        {
            gameCounter++;
        }
    }
}
