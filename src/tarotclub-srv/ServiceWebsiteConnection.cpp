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
    request.query = "/api/v1/servers/register";
    request.body = body;
    request.headers["Host"] = "www." + mHost;
    request.headers["Content-type"] = "application/json";
    request.headers["Content-length"] = std::to_string(body.size());

    return http.GenerateRequest(request);
}
/*****************************************************************************/
void ServiceWebsiteConnection::WebThread()
{
#ifdef TAROT_DEBUG
    mHost = "127.0.0.1";
#else
    mHost = "tarotclub.fr";
#endif
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

    while(!mQuitThread)
    {
        read_buff_t rb;
        TLogInfo("[WEBSITE] Sending registering request");
        int exit_code = tls_client(reinterpret_cast<const uint8_t *>(request_string.c_str()), request_string.size(), mHost.c_str(), &rb);

        if (rb.size > 0)
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
            TLogError("[CLOUD] Cannot join, error code: " + std::to_string(exit_code));
        }
        std::this_thread::sleep_for(std::chrono::seconds(5));
    }
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

void ServiceWebsiteConnection::Initialize()
{
    mWebThread = std::thread(&ServiceWebsiteConnection::WebThread, this);
}

void ServiceWebsiteConnection::Stop()
{

}
