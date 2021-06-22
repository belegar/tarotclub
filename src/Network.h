#ifndef NETWORK_H
#define NETWORK_H

#include <map>
#include <vector>

#include "JsonValue.h"
#include "Common.h"
#include "Identity.h"
#include "Protocol.h"
#include "Users.h"
#include "Engine.h"

struct Reply
{
    std::vector<std::uint32_t> dest;
    JsonObject data;

    Reply(std::uint32_t d, const JsonObject &obj)
        : data(obj)
    {
        dest.push_back(d);
    }

    Reply(const std::vector<std::uint32_t> &dlist, const JsonObject &obj)
        : dest(dlist)
        , data(obj)
    {

    }
};

// Ask steps translation from/into integer
struct Ack
{
    std::string step; // Textual value
    Engine::Sequence sequence; // Engine sequence

    static Engine::Sequence FromString(const std::string &step);
    static std::string ToString(Engine::Sequence sequence);
};


inline bool FromJson(Tarot::Distribution &dist, const JsonObject &obj)
{
    bool ret = true;

    dist.mFile = obj.GetValue("file").GetString();
    dist.mSeed = obj.GetValue("seed").GetInteger();
    dist.TypeFromString(obj.GetValue("type").GetString());

    return ret;
}

inline void ToJson(const Tarot::Distribution &dist, JsonObject &obj)
{
    obj.AddValue("file", dist.mFile);
    obj.AddValue("seed", dist.mSeed);
    obj.AddValue("type", dist.TypeToString());
}

inline bool FromJson(Identity &ident, JsonObject &obj)
{
    bool ret = true;

    ident.nickname = obj.GetValue("nickname").GetString();
    ident.avatar = obj.GetValue("avatar").GetString();
    ident.GenderFromString(obj.GetValue("gender").GetString());

    return ret;
}

inline void ToJson(Identity &ident, JsonObject &obj)
{
    obj.AddValue("nickname", ident.nickname);
    obj.AddValue("avatar", ident.avatar);
    obj.AddValue("gender", ident.GenderToString());
}

// Network context storage interface
// Used to store "optional" information that is not necessary for some implementations (eg: bots)
class IContext
{
public:
    struct Message
    {
        Message()
            : src(Protocol::INVALID_UID)
            , dst(Protocol::INVALID_UID)
        {

        }

        std::uint32_t src;
        std::uint32_t dst;
        std::string msg;
    };

    struct Event
    {
        Event ()
            : uuid(Protocol::INVALID_UID)
        {

        }

        Event(const std::string &t, std::uint32_t u)
            : type(t)
            , uuid(u)
        {

        }

        std::string type;
        std::uint32_t uuid;
    };

    virtual void Initialize() = 0;
    virtual void AddTable(const std::string &name, std::uint32_t uuid) = 0;
    virtual void AddMessage(const Message &msg) = 0;
    virtual void ClearMembers() = 0;
    virtual void UpdateMember(Users::Entry &member, const std::string &event) = 0;
    virtual Event LastEvent() = 0;
    virtual void SetResult(const JsonObject &result) = 0;
};

/**
 * @brief The EmptyContext class
 *
 * Mainly used by bot implementation that do not need any lobby/users information
 */
class EmptyContext : public IContext
{
public:
    void Initialize() { }
    void AddTable(const std::string &name, std::uint32_t uuid) { (void) name; (void) uuid; }
    void AddMessage(const Message &msg) { (void) msg;}
    void ClearMembers() {}
    void UpdateMember(Users::Entry &member, const std::string &event) { (void) member; (void) event; }
    Event LastEvent() { return Event(); }
    void SetResult(const JsonObject &result) { (void) result; }
};

/**
 * @brief The Context class
 *
 * Real implementation that can be used by human clients (console, desktop ...)
 */
class Context : public IContext
{
public:

    Context();

    // From IContext
    virtual void Initialize();
    virtual void AddTable(const std::string &name, std::uint32_t uuid);
    virtual void AddMessage(const Message &msg);
    virtual void ClearMembers();
    virtual void UpdateMember(Users::Entry &member, const std::string &event);
    virtual Event LastEvent();
    virtual void SetResult(const JsonObject &result);

    // From Context
    bool GetMember(uint32_t uuid, Users::Entry &entry);
    std::vector<Users::Entry> GetUsers(std::uint32_t tableId) { return mUsers.Get(tableId); }

private:
    std::map<std::uint32_t, std::string> mTables;
    std::vector<IContext::Message> mMessages;
    Users mUsers;
    IContext::Event mLastEvent;
};

namespace net
{
class IEvent
{
public:
    static const std::uint32_t ErrDisconnectedFromServer    = 6000U;
    static const std::uint32_t ErrCannotConnectToServer     = 6001U;

    virtual ~IEvent() { }

    virtual void Signal(std::uint32_t sig) = 0;
    virtual bool Deliver(std::uint32_t src_uuid, std::uint32_t dest_uuid, const std::string &arg, std::vector<Reply> &out) = 0;
    virtual std::uint32_t AddUser(std::vector<Reply> &out) = 0;
    virtual void RemoveUser(std::uint32_t uuid, std::vector<Reply> &out) = 0;
    virtual std::uint32_t GetUuid() = 0;
};

} // end of namespace net


#endif // NETWORK_H
