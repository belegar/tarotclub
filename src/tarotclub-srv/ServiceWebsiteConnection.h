#ifndef SERVICE_WEBSITE_CONNECTION_H
#define SERVICE_WEBSITE_CONNECTION_H

#include <vector>
#include "Value.h"
#include "DataBase.h"
#include "JsonValue.h"
#include "IScriptEngine.h"
#include "IService.h"
#include "Lobby.h"

class ServiceWebsiteConnection : public IService
{
public:
    ServiceWebsiteConnection(Lobby &lobby, const std::string &token);

    // From IService
    virtual std::string GetName();
    virtual void Initialize();
    virtual void Stop();

private:
    Lobby &mLobby;
    bool mQuitThread = false;
    std::string mToken;

    void WebServerRegisterThread();
};

#endif // SERVICE_WEBSITE_CONNECTION_H
