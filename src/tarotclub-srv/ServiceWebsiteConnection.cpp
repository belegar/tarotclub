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

    return obj.ToString(0);
}
/*****************************************************************************/
void ServiceWebsiteConnection::HandleCommand(const std::string &cmd, const JsonObject &json)
{
    if (cmd == "register")
    {
        if (json.GetValue("success").GetBool())
        {
            TLogInfo("[WEBSITE] Register success!");
            mRegistered = true;
        }
        else
        {
            TLogError("[WEBSITE] Registering rejected");
            mRegistered = false;
        }
    }
    else if (cmd == "join")
    {
        TLogInfo("[WEBSITE] Grant access client to a server");
        std::int64_t playerId = json.GetValue("playerId").GetInteger64();
        std::string gek = json.GetValue("gek").GetString();
        std::string passPhrase = json.GetValue("passPhrase").GetString();
        mServer.AddClient(playerId, gek, passPhrase);
    }
}
/*****************************************************************************/
void ServiceWebsiteConnection::WebThread()
{
    tcp::TcpClient client;
    bool connected = false;

    client.Initialize();
    client.SetSecured(false);
    client.SetWebSocket(true);

    while(!mQuitThread)
    {
        if (!connected)
        {
            client.Initialize();
            if (client.Connect(mHost, 8989))
            {
                connected = true;
            }
            else
            {
                TLogError("[WEBSITE] Cannot join server");
            }
        }
        else
        {
//            TLogInfo("[WEBSITE] Sending register or status request");
            std::string response;
            if (client.RecvWithTimeout(response, 2048, 500))
            {
                JsonReader reader;
                JsonValue json;

                if (reader.ParseString(json, response))
                {
                    if (json.IsObject())
                    {
                        JsonObject replyObj = json.GetObj();

                        if (replyObj.HasValue("cmd"))
                        {
                            HandleCommand(replyObj.GetValue("cmd").GetString(), replyObj);
                        }
                        else
                        {
                            TLogError("[WEBSITE] Invalid reply. Error: " + replyObj.GetValue("message").GetString());
                        }
                    }
                    else
                    {
                        TLogError("[WEBSITE] Reply body is not an object");
                    }
                }
                else
                {
                    TLogError("[WEBSITE] Reply body is not JSON: " + response);
                }
            }
            else
            {
                bool req_success = client.Send(UpdateServerStatus());

                if (!req_success)
                {
                    client.Close();
                    connected = false;
                    TLogError("[WEBSITE] Sending request failure");
                }
            }


        } // if connected

        std::this_thread::sleep_for(std::chrono::seconds(1));
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
