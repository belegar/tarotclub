#include "ServiceWebsiteConnection.h"
#include "tls_client.h"
#include "HttpProtocol.h"
#include "JsonReader.h"

/*****************************************************************************/
void ServiceWebsiteConnection::WebServerRegisterThread()
{
    JsonObject obj;
    HttpProtocol http;
    HttpRequest request;
    JsonObject serverObj;

    obj.AddValue("token", mToken);
    obj.AddValue("server", serverObj);

    std::string body = obj.ToString(0);
    std::string host = "tarotclub.fr";

    request.method = "POST";
    request.protocol = "HTTP/1.1";
    request.query = "/api/v1/machines/upstream/data";
    request.body = body;
    request.headers["Host"] = "www." + host;
    request.headers["Content-type"] = "application/json";
    request.headers["Content-length"] = std::to_string(body.size());

    std::string request_string = http.GenerateRequest(request);

//    std::cout << request_string << std::endl;

    while(!mQuitThread)
    {
        read_buff_t rb;
        int exit_code = tls_client(reinterpret_cast<const uint8_t *>(request_string.c_str()), request_string.size(), host.c_str(), &rb);

        if (rb.size > 0)
        {
            HttpReply reply;
            std::string raw_data(reinterpret_cast<char *>(rb.data), rb.size);
            if (http.ParseReplyHeader(raw_data, reply))
            {
                JsonReader reader;
                JsonValue json;

                if (reader.ParseString(json, reply.body))
                {
                    if (json.IsObject())
                    {
                        obj = json.GetObj();
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

ServiceWebsiteConnection::ServiceWebsiteConnection(Lobby &lobby, const std::string &token)
    : mLobby(lobby)
    , mToken(token)
{

}

std::string ServiceWebsiteConnection::GetName()
{

}

void ServiceWebsiteConnection::Initialize()
{

}

void ServiceWebsiteConnection::Stop()
{

}
