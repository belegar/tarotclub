#include "ServiceWebsiteConnection.h"
#include "tls_client.h"
#include "HttpProtocol.h"
#include "JsonReader.h"

/*****************************************************************************/
std::string ServiceWebsiteConnection::UpdateRequest(const std::string &cmd, JsonObject &serverObj)
{
    JsonObject obj;
    HttpProtocol http;
    HttpRequest request;

    obj.AddValue("command", cmd);
    obj.AddValue("token", mToken);
    obj.AddValue("server", serverObj);

    std::string body = obj.ToString(0);

    request.method = "POST";
    request.protocol = "HTTP/1.1";
    request.query = "/api/v1/server/register";
    request.body = body;
    request.headers["Host"] = "www." + mHost;
    request.headers["Content-type"] = "application/json";
    request.headers["Content-length"] = std::to_string(body.size());

    return http.GenerateRequest(request);
}
/*****************************************************************************/
void ServiceWebsiteConnection::WebThread()
{
    JsonObject serverObj;

    serverObj.AddValue("nb_players", mLobby.GetNumberOfPlayers());
    serverObj.AddValue("name", mLobby.GetName());
    serverObj.AddValue("nb_tables", mLobby.GetNumberOfTables());
    serverObj.AddValue("nb_players", mLobby.GetNumberOfPlayers());

    /*
    { text: 'Server name', value: 'name' },
             { text: 'Number of players', value: 'nb_players' },
             { text: 'Number of tables', value: 'nb_tables' },
             { text: 'Region', value: 'region' },
             { text: 'Server type', value: 'server_type' },
             { text: 'Privacy', value: 'privacy' }
           ],
*/

    std::string request_string = UpdateRequest("register", serverObj);
//    std::cout << request_string << std::endl;

    SimpleTlsClient tls;
    bool connected = false;

    while(!mQuitThread)
    {
        if (!connected)
        {
            if (tls.Connect(mHost.c_str()))
            {
                connected = true;
            }
            else
            {
                TLogError("[CLOUD] Cannot join server");
            }
        }
        else
        {
            read_buff_t rb;
            TLogInfo("[WEBSITE] Sending registering request");
            bool req_success = tls.Request(reinterpret_cast<const uint8_t *>(request_string.c_str()), request_string.size(), &rb);
            if (req_success && (rb.size > 0))
            {
                HttpReply reply;
                HttpProtocol http;
                std::string raw_data(reinterpret_cast<char *>(rb.data), rb.size);
                if (http.ParseReplyHeader(raw_data, reply))
                {
                    JsonReader reader;
                    JsonValue json;

                    if (reader.ParseString(json, reply.body))
                    {
                        if (json.IsObject())
                        {
                            JsonObject replyObj = json.GetObj();

                            // Analyse de la réponse
                            if (mInitialRequest)
                            {
                                mInitialRequest = false;

                                // SSK == Server Session Key
                                if (replyObj.HasValue("ssk"))
                                {
                                  //  mServer.
                                }
                            }

                            // Update game information
                            serverObj.ReplaceValue("nb_players", mLobby.GetNumberOfPlayers());
                            serverObj.ReplaceValue("name", mLobby.GetName());
                            serverObj.ReplaceValue("nb_tables", mLobby.GetNumberOfTables());
                            serverObj.ReplaceValue("nb_players", mLobby.GetNumberOfPlayers());
                            request_string = UpdateRequest(mInitialRequest ? "register" : "status", serverObj);
                        }
                    }

                    TLogInfo("[CLOUD] Got reply: " + reply.body);
                }
                else
                {
                    TLogError("[CLOUD] Invalid reply");
                }
            }
            else
            {
                 // request failure
                connected = false;
                tls.Close();
            }
        } // if connected

        std::this_thread::sleep_for(std::chrono::seconds(2));
    } // while()
}

ServiceWebsiteConnection::ServiceWebsiteConnection(Server &server, Lobby &lobby, const std::string &token)
    : mServer(server)
    , mLobby(lobby)
    , mToken(token)
{

}

std::string ServiceWebsiteConnection::GetName()
{
    return "ServiceWebsiteConnection";
}

// Le protocole Server Send Event est lien permanent unidirectionnel du serveur vers le client
// Les données transférées sont de l'UTF8 uniquement, nickel pour du JSON
void ServiceWebsiteConnection::ServerSendEventThread()
{
    SimpleTlsClient tls;
    bool connected = false;

    HttpProtocol http;
    HttpRequest request;

    request.method = "GET";
    request.protocol = "HTTP/1.1";
    request.query = "/api/v1/server/events";
    request.headers["Host"] = "www." + mHost;
    request.headers["Accept"] = "text/event-stream";

    std::string sseInitialReq = http.GenerateRequest(request);

    while(!mQuitThread)
    {
        if (!connected)
        {
            if (tls.Connect(mHost.c_str()))
            {
                read_buff_t rb;
                tls.Request(reinterpret_cast<const uint8_t *>(sseInitialReq.c_str()), sseInitialReq.size(), &rb);
                connected = true;
            }
            else
            {
                TLogError("[CLOUD] Cannot join server");
            }
        }
        else
        {
            read_buff_t rb;
            TLogInfo("[WEBSITE] Wait for data");
            if (tls.Read(&rb))
            {
                if (rb.size > 0)
                {
                    HttpReply reply;
                    HttpProtocol http;

                    TLogInfo("[WEBSITE] Received SSE");
                }
            }
            else
            {
                connected = false;
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

 //   mWebThread = std::thread(&ServiceWebsiteConnection::WebThread, this);
    mSSEThread = std::thread(&ServiceWebsiteConnection::ServerSendEventThread, this);
}

void ServiceWebsiteConnection::Stop()
{

}