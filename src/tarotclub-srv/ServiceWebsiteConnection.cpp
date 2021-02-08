#include "ServiceWebsiteConnection.h"
#include "tls_client.h"
#include "HttpProtocol.h"
#include "JsonReader.h"

const std::uint32_t ServiceWebsiteConnection::cSSEStart = 0x1234;
const std::uint32_t ServiceWebsiteConnection::cSSEStop = 0x4567;

ServiceWebsiteConnection::ServiceWebsiteConnection(Server &server, Lobby &lobby, const ServerOptions &opt)
    : mServer(server)
    , mLobby(lobby)
    , mOptions(opt)
{

}
/*****************************************************************************/
std::string ServiceWebsiteConnection::UpdateServerStatus()
{
    JsonObject obj;
    JsonObject serverObj;

    serverObj.AddValue("nb_players", mLobby.GetNumberOfPlayers());
    serverObj.AddValue("name", mLobby.GetName());
    serverObj.AddValue("nb_tables", mLobby.GetNumberOfTables());
    serverObj.AddValue("nb_players", mLobby.GetNumberOfPlayers());
    serverObj.AddValue("tcp_port", mOptions.game_tcp_port);
    serverObj.AddValue("ws_port", mOptions.websocket_tcp_port);

    obj.AddValue("server", serverObj);
    obj.AddValue("token", mOptions.token);

    if (mRegistered)
    {
        obj.AddValue("auth", mSSK);
        obj.AddValue("command", "server_status");
    }
    else
    {
        obj.AddValue("command", "server_register");
    }

    return obj.ToString(0);
}
/*****************************************************************************/
void ServiceWebsiteConnection::WebThread()
{
    tcp::TcpClient client;
    bool connected = false;

    client.Initialize();
    client.SetSecured(true);
    client.SetWebSocket(true);
    client.SetWebSocketUri("/servers");

    while(!mQuitThread)
    {
        if (!connected)
        {
            if (client.Connect(mHost, 443))
            {
                connected = true;
            }
            else
            {
                TLogError("[REGISTER] Cannot join server");
            }
        }
        else
        {
            std::string request_string = UpdateServerStatus();
            TLogInfo("[REGISTER] Sending register or status request");
            bool req_success = client.Send(request_string);

            if (req_success)
            {
                std::string reply;
                if (client.RecvWithTimeout(reply, 2048, 500))
                {
                    JsonReader reader;
                    JsonValue json;

                    if (reader.ParseString(json, reply))
                    {
                        if (json.IsObject())
                        {
                            JsonObject replyObj = json.GetObj();

                            // Analyse de la réponse
                            if (!mRegistered)
                            {
                                if (replyObj.HasValue("success") && replyObj.HasValue("data:ssk"))
                                {
                                    // SSK == Server Session Key
                                    // basically, the SSK is a JWT format
                                    if (replyObj.GetValue("success").GetBool())
                                    {
                                        SSEItem item;
                                        item.event = cSSEStart;
                                        mSSK = replyObj.GetValue("data:ssk").GetString();
                                        mSSEQueue.Push(item);
                                        mRegistered = true;
                                    }
                                    else
                                    {
                                        TLogError("[REGISTER] Registering rejected");
                                    }
                                }
                                else
                                {
                                    TLogError("[REGISTER] Invalid reply, missing status");
                                }
                            }
                        }
                    }

                }
                else
                {
                    TLogError("[REGISTER] Rcv timeout :(");
                }
            }
            else
            {
                 // request failure
                connected = false;
                client.Close();
            }
        } // if connected

        std::this_thread::sleep_for(std::chrono::seconds(2));
    } // while()
}

std::string ServiceWebsiteConnection::GetName()
{
    return "ServiceWebsiteConnection";
}

// Le protocole Server Send Event est lien permanent unidirectionnel du serveur vers le client
// Les données transférées sont de l'UTF8 uniquement, nickel pour du JSON
void ServiceWebsiteConnection::ServerSentEventsThread()
{
    SimpleTlsClient tls;
    bool connected = false;
    bool started = false;

    HttpProtocol http;
    HttpRequest request;

    request.method = "GET";
    request.protocol = "HTTP/1.1";
    request.query = "/api/v1/server/events";
    request.headers["Host"] = "www." + mHost;
    request.headers["Accept"] = "text/event-stream";

    while(!mQuitThread)
    {
        SSEItem item;
        if (mSSEQueue.TryPop(item))
        {
            if (item.event == cSSEStart)
            {
                // arg is the token
                request.headers["Authorization"] = "Bearer " + mSSK;
                started = true;
                connected = false;
            }
            else
            {
                started = false;
                connected = false;
                tls.Close();
            }
        }

        if (started)
        {
            if (!connected)
            {
                if (tls.Connect(mHost.c_str()))
                {
                    std::string sseInitialReq = http.GenerateRequest(request);
                    tls.Write(reinterpret_cast<const uint8_t *>(sseInitialReq.c_str()), sseInitialReq.size());
                    connected = true;
                    TLogInfo("[SSE] Thread started");
                }
                else
                {
                    TLogError("[SSE] Cannot join server");
                }
            }
            else
            {
                read_buff_t rb;
                TLogInfo("[SSE] Wait for data");
                if (tls.Read(&rb))
                {
                    if (rb.size > 0)
                    {
                        HttpReply reply;
                        HttpProtocol http;

                        TLogInfo("[SSE] Received data");
                    }
                }
                else
                {
                    connected = false;
                }
            }
        }
    }
}

void ServiceWebsiteConnection::Initialize()
{
#ifdef TAROT_DEBUG
    mHost = "127.0.0.1";
#else
    mHost = "tarotclub.fr";
#endif


//    c.Initialize();
//    c.SetWebSocketUri("/clients");
//    c.Connect("127.0.0.1", 6000);

//    c.Send("coucou");


    mWebThread = std::thread(&ServiceWebsiteConnection::WebThread, this);
//    mSSEThread = std::thread(&ServiceWebsiteConnection::ServerSentEventsThread, this);
}

void ServiceWebsiteConnection::Stop()
{

}
