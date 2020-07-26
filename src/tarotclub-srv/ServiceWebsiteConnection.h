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

class ServiceWebsiteConnection : public IService
{
public:
    ServiceWebsiteConnection(Server &server, Lobby &lobby, const std::string &token);

    // From IService
    virtual std::string GetName();
    virtual void Initialize();
    virtual void Stop();

private:
    Server &mServer;
    Lobby &mLobby;
    bool mQuitThread = false;
    bool mInitialRequest = true;
    std::string mToken;
    std::thread mWebThread;
    std::string mHost;

    void WebThread();
    std::string UpdateRequest(const std::string &cmd, JsonObject &serverObj);
};

#endif // SERVICE_WEBSITE_CONNECTION_H
