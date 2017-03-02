#include "Network.h"
#include "Log.h"


static const Ack gAckList[] = {
    {"AckJoinTable", Engine::WAIT_FOR_PLAYERS},
    {"Ready", Engine::WAIT_FOR_READY },
    {"NewDeal", Engine::WAIT_FOR_CARDS },
    {"ShowBid", Engine::WAIT_FOR_SHOW_BID },
    {"AllPassed", Engine::WAIT_FOR_ALL_PASSED },
    {"ShowDog", Engine::WAIT_FOR_SHOW_DOG },
    {"StartDeal", Engine::WAIT_FOR_START_DEAL },
    {"ShowHandle", Engine::WAIT_FOR_SHOW_HANDLE },
    {"Card", Engine::WAIT_FOR_SHOW_CARD },
    {"EndOfTrick", Engine::WAIT_FOR_END_OF_TRICK },
    {"EndOfDeal", Engine::WAIT_FOR_END_OF_DEAL }
};

static const std::uint32_t gAckListSize = sizeof(gAckList) / sizeof(gAckList[0]);

Engine::Sequence Ack::FromString(const std::string &step)
{
    Engine::Sequence seq = Engine::BAD_STEP;
    for (std::uint32_t i = 0U; i < gAckListSize; i++)
    {
        if (gAckList[i].step == step)
        {
            seq = gAckList[i].sequence;
            break;
        }
    }
    return seq;
}

std::string Ack::ToString(Engine::Sequence sequence)
{
    std::string step = "BadSeq";
    for (std::uint32_t i = 0U; i < gAckListSize; i++)
    {
        if (gAckList[i].sequence == sequence)
        {
            step = gAckList[i].step;
            break;
        }
    }
    return step;
}




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
                if (!mUsers.AddEntry(member))
                {
                    TLogError("AddEntry should not fail, the list is managed by the server");
                }
            }
            else
            {
                TLogError("User uuid should be in the list");
            }
        }
        else
        {
            if (event == "Nick")
            {
                mUsers.ChangeNickName(member.uuid, member.identity.nickname);
            }
            // FIXME: support the other events
        }
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
