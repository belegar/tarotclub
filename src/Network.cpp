#include "Network.h"
#include "Log.h"

// This source code has been intentionnaly left blank

Context::Context()
{

}

void Context::Initialize()
{
    mTables.clear();
    mMessages.clear();
    mUsers.Clear();
}

void Context::AddTable(const std::string &name, uint32_t uuid)
{
    mTables[uuid] = name;
}

void Context::AddMessage(const IContext::Message &msg)
{
    mMessages.push_back(msg);
}

void Context::ClearMembers()
{
    mUsers.Clear();
}

void Context::UpdateMember(Users::Entry &member, const std::string &event)
{
    if (event == "Quit")
    {
        mUsers.Remove(member.uuid);
    }
    else
    {
        if (!mUsers.IsHere(member.uuid))
        {
            if (event == "New")
            {
                // Create user if not exists
                member.uuid = mUsers.CreateEntry(Protocol::INVALID_UID);
            }
            else
            {
                TLogError("User uuid should be in the list");
            }
        }
        mUsers.Update(member.uuid, member.identity);
    }
    mLastEvent = Event(event, member.uuid);
}

IContext::Event Context::LastEvent()
{
    return mLastEvent;
}

void Context::SetResult(const JsonObject &result)
{
    (void) result;
}

bool Context::GetMember(uint32_t uuid, Users::Entry &entry)
{
    return mUsers.GetEntry(uuid, entry);
}
