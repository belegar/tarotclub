#ifndef SERVICE_WEBSITE_CONNECTION_H
#define SERVICE_WEBSITE_CONNECTION_H

#include <vector>
#include "Value.h"
#include "DataBase.h"
#include "JsonValue.h"
#include "IScriptEngine.h"
#include "IService.h"
#include "Lobby.h"
#include "Server.h"
#include "ThreadQueue.h"

class ServiceWebsiteConnection : public IService
{
public:
    ServiceWebsiteConnection(Server &server, Lobby &lobby, const ServerOptions &opt);

    // From IService
    virtual std::string GetName();
    virtual void Initialize();
    virtual void Stop();

private:
    Server &mServer;
    Lobby &mLobby;
    bool mQuitThread = false;
    bool mRegistered = false;

    std::thread mWebThread;
    std::string mHost;
    ServerOptions mOptions;

    void WebThread();
    std::string UpdateServerStatus();

    void HandleCommand(const std::string &cmd, const JsonObject &json);
};

#endif // SERVICE_WEBSITE_CONNECTION_H
