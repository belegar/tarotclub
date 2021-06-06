#include "ServiceWebsiteConnection.h"
#include "HttpProtocol.h"
#include "JsonReader.h"

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
    obj.AddValue("auth", mSSK);

    return obj.ToString(0);
}
/*****************************************************************************/
void ServiceWebsiteConnection::WebThread()
{
    tcp::TcpClient client;
    bool connected = false;

    client.Initialize();
    client.SetSecured(true);
    client.SetWebSocket(false);
   // client.SetWebSocketUri("/servers");

    while(!mQuitThread)
    {
        if (!connected)
        {
            client.Initialize();
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
            TLogInfo("[REGISTER] Sending register or status request");

            HttpRequest request;

            request.body = UpdateServerStatus();
            request.method = "POST";
            std::string path = mRegistered ? "status" : "register";
            request.query = "/api/v1/servers/" + path;
            request.protocol = "HTTP/1.1";
            request.headers["Host"] = "www." + mHost;
            request.headers["Content-type"] = "application/json";
            request.headers["Content-length"] = std::to_string(request.body.size());
            request.headers["Authorization"] = "Bearer " + mSSK;

            bool req_success = client.Send(HttpProtocol::GenerateRequest(request));

            if (req_success)
            {
                std::string response;
                if (client.RecvWithTimeout(response, 2048, 500))
                {
                    HttpReply reply;
                    if (HttpProtocol::ParseReplyHeader(response, reply))
                    {
                        JsonReader reader;
                        JsonValue json;

                        if (reader.ParseString(json, reply.body))
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
                                            mSSK = replyObj.GetValue("data:ssk").GetString();
                                            mRegistered = true;
                                        }
                                        else
                                        {
                                            TLogError("[REGISTER] Registering rejected");
                                        }
                                    }
                                    else
                                    {
                                        TLogError("[REGISTER] Invalid reply. Error: " + replyObj.GetValue("message").GetString());
                                    }
                                }
                            }
                        }
                        else
                        {
                            TLogError("[REGISTER] Reply body is not JSON: " + reply.body);
                        }
                    }
                    else
                    {
                        TLogError("[REGISTER] Reply is not HTTP");
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


void ServiceWebsiteConnection::Initialize()
{
#ifdef TAROT_DEBUG
    mHost = "127.0.0.1";
#else
    mHost = "tarotclub.fr";
#endif
    mWebThread = std::thread(&ServiceWebsiteConnection::WebThread, this);
}

void ServiceWebsiteConnection::Stop()
{

}
