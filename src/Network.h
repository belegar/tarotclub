#ifndef NETWORK_H
#define NETWORK_H

#include <map>
#include <vector>

#include "JsonValue.h"
#include "Common.h"
#include "ThreadQueue.h"
#include "TcpClient.h"
#include "Identity.h"

struct Reply
{
    std::vector<std::uint32_t> dest;
    JsonObject data;

    Reply(std::uint32_t d, const JsonObject &obj)
        : data(obj)
    {
        dest.push_back(d);
    }

    Reply(std::vector<std::uint32_t> dlist, const JsonObject &obj)
        : dest(dlist)
        , data(obj)
    {

    }
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
    ident.avatar = obj.GetValue("avatar").GetInteger();
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
        std::uint32_t src;
        std::uint32_t dst;
        std::string msg;
    };

    struct Member
    {
        std::string nickname;
        std::uint32_t table;
        Place place;
    };

    virtual void Initialize() = 0;
    virtual void AddTable(const std::string &name, std::uint32_t uuid) = 0;
    virtual void AddMessage(const Message &msg) = 0;
    virtual void UpdateMember(std::uint32_t uuid, const Member &member, const std::string &event) = 0;
    virtual void SetResult(const JsonObject &result) = 0;
};

class EmptyContext : public IContext
{
public:
    virtual void Initialize() { }
    virtual void AddTable(const std::string &name, std::uint32_t uuid) { (void) name; (void) uuid; }
    virtual void AddMessage(const Message &msg) { (void) msg;}
    virtual void UpdateMember(std::uint32_t uuid, const Member &member, const std::string &event) { (void) uuid; (void) member; (void) event; }
    virtual void SetResult(const JsonObject &result) { (void) result; }
};


class Context : public IContext
{
public:
    virtual void Initialize()
    {
        mTables.clear();
        mMessages.clear();
    }

    virtual void AddTable(const std::string &name, std::uint32_t uuid)
    {
        mTables[uuid] = name;
    }

    virtual void AddMessage(const Message &msg)
    {
        mMessages.push_back(msg);
    }

    virtual void UpdateMember(std::uint32_t uuid, const Member &member, const std::string &event)
    {
        if (event == "Update")
        {
            mMembers[uuid] = member;
        }
        else
        {
            mMembers.erase(uuid);
        }
    }

    virtual void SetResult(const JsonObject &result)
    {
        (void) result;
    }

private:
    std::map<std::uint32_t, std::string> mTables;
    std::vector<IContext::Message> mMessages;
    std::map<std::uint32_t, IContext::Member> mMembers;
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
