
// Tarot files
#include "BasicClient.h"
#include "Protocol.h"

// ICL files
#include "Log.h"
#include "JsonReader.h"
#include "Protocol.h"

/*****************************************************************************/
BasicClient::BasicClient()
    : mNbPlayers(0U)
{

}
/*****************************************************************************/
void BasicClient::Start()
{
    mMyself.uuid = Protocol::INVALID_UID;
}
/*****************************************************************************/
void BasicClient::Stop()
{
    mMyself.uuid = Protocol::INVALID_UID;
}
/*****************************************************************************/
bool BasicClient::TestDiscard(const Deck &discard)
{
    return mDeck.TestDiscard(discard, mDog, mNbPlayers);
}
/*****************************************************************************/
Contract BasicClient::CalculateBid()
{
    int total = 0;
    Contract cont;

    UpdateStatistics();

    // Set points according to the card values
    if (mStats.bigTrump == true)
    {
        total += 9;
    }
    if (mStats.fool == true)
    {
        total += 7;
    }
    if (mStats.littleTrump == true)
    {
        if (mStats.trumps == 5)
        {
            total += 5;
        }
        else if (mStats.trumps == 6 || mStats.trumps == 7)
        {
            total += 7;
        }
        else if (mStats.trumps > 7)
        {
            total += 8;
        }
    }

    // Each trump is 1 point
    // Each major trump is 1 more point
    total += mStats.trumps * 2;
    total += mStats.majorTrumps * 2;
    total += mStats.kings * 6;
    total += mStats.queens * 3;
    total += mStats.knights * 2;
    total += mStats.jacks;
    total += mStats.weddings;
    total += mStats.longSuits * 5;
    total += mStats.cuts * 5;
    total += mStats.singletons * 3;
    total += mStats.sequences * 4;

    // We can decide the bid
    if (total <= 35)
    {
        cont = Contract::PASS;
    }
    else if (total >= 36  && total <= 50)
    {
        cont = Contract::TAKE;
    }
    else if (total >= 51  && total <= 65)
    {
        cont = Contract::GUARD;
    }
    else if (total >= 66  && total <= 75)
    {
        cont = Contract::GUARD_WITHOUT;
    }
    else
    {
        cont = Contract::GUARD_AGAINST;
    }
    return cont;
}
/*****************************************************************************/
void BasicClient::UpdateStatistics()
{
    mStats.Reset();
    mDeck.AnalyzeTrumps(mStats);
    mDeck.AnalyzeSuits(mStats);
}
/*****************************************************************************/
Card BasicClient::Play()
{
    Card c;

    for (Deck::ConstIterator it = mDeck.Begin(); it != mDeck.End(); ++it)
    {
        c = (*it);
        if (IsValid(c) == true)
        {
            break;
        }
    }
    return c;
}
/*****************************************************************************/
bool BasicClient::IsValid(const Card &c)
{
    return mDeck.CanPlayCard(c, mCurrentTrick);
}
/*****************************************************************************/
Deck BasicClient::AutoDiscard()
{
    Deck discard = mDeck.AutoDiscard(mDog, mNbPlayers);
    mDeck.RemoveDuplicates(discard);
    return discard;
}
/*****************************************************************************/
void BasicClient::BuildBid(Contract c, bool slam, std::vector<Reply> &out)
{
    JsonObject obj;

    obj.AddValue("cmd", "ReplyBid");
    obj.AddValue("contract", c.ToString());
    obj.AddValue("slam", slam);

    out.push_back(Reply(mMyself.tableId, obj));
}
/*****************************************************************************/
void BasicClient::BuildHandle(const Deck &handle, std::vector<Reply> &out)
{
    JsonObject obj;

    obj.AddValue("cmd", "Handle");
    obj.AddValue("handle", handle.ToString());

    out.push_back(Reply(mMyself.tableId, obj));
}
/*****************************************************************************/
void BasicClient::BuildDiscard(const Deck &discard, std::vector<Reply> &out)
{
    JsonObject obj;

    obj.AddValue("cmd", "Discard");
    obj.AddValue("discard", discard.ToString());

    out.push_back(Reply(mMyself.tableId, obj));
}
/*****************************************************************************/
void BasicClient::BuildSendCard(Card c, std::vector<Reply> &out)
{
    JsonObject obj;

    obj.AddValue("cmd", "Card");
    obj.AddValue("card", c.ToString());

    out.push_back(Reply(mMyself.tableId, obj));
}
/*****************************************************************************/
void BasicClient::BuildQuitTable(std::uint32_t tableId, std::vector<Reply> &out)
{
    JsonObject obj;

    obj.AddValue("cmd", "RequestQuitTable");
    obj.AddValue("table_id", tableId);

    out.push_back(Reply(mMyself.tableId, obj));
}
/*****************************************************************************/
void BasicClient::BuildChangeNickname(std::vector<Reply> &out)
{
    JsonObject obj;

    obj.AddValue("cmd", "RequestChangeNickname");
    obj.AddValue("nickname", mMyself.identity.nickname);

    out.push_back(Reply(Protocol::LOBBY_UID, obj));
}
/*****************************************************************************/
void BasicClient::BuildJoinTable(uint32_t tableId, std::vector<Reply> &out)
{
    JsonObject obj;

    obj.AddValue("cmd", "RequestJoinTable");
    obj.AddValue("table_id", tableId);

    out.push_back(Reply(Protocol::LOBBY_UID, obj));
}
/*****************************************************************************/
void BasicClient::BuildNewGame(std::vector<Reply> &out)
{
    JsonObject obj;

    obj.AddValue("cmd", "RequestNewGame");
    // FIXME: add new game parameters
    out.push_back(Reply(Protocol::LOBBY_UID, obj));
}
/*****************************************************************************/
void BasicClient::Sync(Engine::Sequence sequence, std::vector<Reply> &out)
{
    JsonObject obj;

    obj.AddValue("cmd", "Ack");
    obj.AddValue("step", Ack::ToString(sequence));
    out.push_back(Reply(mMyself.tableId, obj));
}
/*****************************************************************************/
void BasicClient::GetPlayerStatus(Users::Entry &member, JsonObject &player)
{
    FromJson(member.identity, player);

    member.uuid = static_cast<std::uint32_t>(player.GetValue("uuid").GetInteger());
    member.tableId = static_cast<std::uint32_t>(player.GetValue("table").GetInteger());
    member.place = Place(player.GetValue("place").GetString());
}
/*****************************************************************************/
BasicClient::Event BasicClient::Decode(uint32_t src_uuid, uint32_t dest_uuid, const std::string &arg, IContext &ctx, std::vector<Reply> &out)
{
    (void) src_uuid;
    (void) dest_uuid;
    BasicClient::Event event = BAD_EVENT;
    JsonReader reader;
    JsonValue json;

    if (!reader.ParseString(json, arg))
    {
        TLogError("Not a JSON data" + arg);
        return JSON_ERROR;
    }

    std::string cmd = json.FindValue("cmd").GetString();

    if (cmd == "RequestLogin")
    {
        ctx.Initialize();

        mMyself.uuid = static_cast<std::uint32_t>(json.FindValue("uuid").GetInteger());

        JsonObject obj;

        obj.AddValue("cmd", "ReplyLogin");
        ToJson(mMyself.identity, obj);

        out.push_back(Reply(Protocol::LOBBY_UID, obj));

        event = REQ_LOGIN;
    }
    else if (cmd == "AccessGranted")
    {
        JsonArray tables = json.FindValue("tables").GetArray();

        for (std::uint32_t i = 0U; i < tables.Size(); i++)
        {
           JsonObject table = tables.GetEntry(i).GetObj();
           std::uint32_t uuid = static_cast<std::uint32_t>(table.GetValue("uuid").GetInteger());
           std::string name = table.GetValue("name").GetString();
           ctx.AddTable(name, uuid);
        }

        event = ACCESS_GRANTED;
    }
    else if (cmd == "ChatMessage")
    {
        IContext::Message msg;
        msg.src = static_cast<std::uint32_t>(json.FindValue("source").GetInteger());
        msg.dst = static_cast<std::uint32_t>(json.FindValue("target").GetInteger());
        msg.msg = json.FindValue("message").GetString();

        ctx.AddMessage(msg);

        event = MESSAGE;
    }
    else if (cmd == "PlayerList")
    {
        JsonArray players = json.FindValue("players").GetArray();

        ctx.ClearMembers();
        for (std::uint32_t i = 0U; i < players.Size(); i++)
        {
            Users::Entry member;
            JsonObject player = players.GetEntry(i).GetObj();

            GetPlayerStatus(member, player);
            ctx.UpdateMember(member, "New");
        }

        event = PLAYER_LIST;
    }
    else if (cmd == "Event")
    {
        Users::Entry member;
        std::string ev_type = json.FindValue("type").GetString();
        JsonObject player = json.FindValue("player").GetObj();

        GetPlayerStatus(member, player);
        ctx.UpdateMember(member, ev_type);

        event = PLAYER_EVENT;
    }
    else if (cmd == "ReplyJoinTable")
    {
        mMyself.place = Place(json.FindValue("place").GetString());
        mMyself.tableId = static_cast<std::uint32_t>(json.FindValue("table_id").GetInteger());
        mNbPlayers = static_cast<std::uint32_t>(json.FindValue("size").GetInteger());

        event = JOIN_TABLE;
    }
    else if (cmd == "RequestQuitTable")
    {
        event = QUIT_TABLE;
    }
    else if (cmd == "NewGame")
    {
        mGame.Set(json.FindValue("mode").GetString());
        event = NEW_GAME;
    }
    else if (cmd == "NewDeal")
    {
        mDeck.SetCards(json.FindValue("cards").GetString());
        event = NEW_DEAL;
    }
    else if (cmd == "RequestBid")
    {
        mBid.taker = Place(json.FindValue("place").GetString());
        mBid.contract = Contract(json.FindValue("contract").GetString());
        event = REQ_BID;
    }
    else if (cmd == "ShowBid")
    {
        mBid.taker = Place(json.FindValue("place").GetString());
        mBid.contract = Contract(json.FindValue("contract").GetString());
        mBid.slam = json.FindValue("slam").GetBool();
        event = SHOW_BID;
    }
    else if (cmd == "AllPassed")
    {
        event = ALL_PASSED;
    }
    else if (cmd == "ShowDog")
    {
        mDog.SetCards(json.FindValue("dog").GetString());
        event = SHOW_DOG;
    }
    else if (cmd == "BuildDiscard")
    {
        event = BUILD_DISCARD;
    }
    else if (cmd == "StartDeal")
    {
        mBid.taker = Place(json.FindValue("taker").GetString());
        mBid.contract = Contract(json.FindValue("contract").GetString());
        mBid.slam = json.FindValue("slam").GetBool();

        UpdateStatistics();
        mCurrentTrick.Clear();
        event = START_DEAL;
    }
    else if (cmd == "AskForHandle")
    {
        event = ASK_FOR_HANDLE;
    }
    else if (cmd == "ShowHandle")
    {
        Place place = Place(json.FindValue("place").GetString());
        mHandle.SetCards(json.FindValue("handle").GetString());

        Team team(Team::DEFENSE);
        if (place == mBid.taker)
        {
            team = Team::ATTACK;
        }
        mHandle.SetOwner(team);
        event = SHOW_HANDLE;
    }
    else if (cmd == "ShowCard")
    {
        mCurrentPlayer = Place(json.FindValue("place").GetString());
        Card card = Card(json.FindValue("card").GetString());

        mCurrentTrick.Append(card);
        event = SHOW_CARD;
    }
    else if (cmd == "PlayCard")
    {
        mCurrentPlayer = Place(json.FindValue("place").GetString());

        event = PLAY_CARD;
    }
    else if (cmd == "EndOfTrick")
    {
        mCurrentPlayer = Place(json.FindValue("place").GetString());
        mCurrentTrick.Clear();

        event = END_OF_TRICK;
    }
    else if (cmd == "EndOfDeal")
    {
        mPoints.pointsAttack = json.FindValue("points").GetInteger();
        mPoints.oudlers = json.FindValue("oudlers").GetInteger();
        mPoints.littleEndianOwner = static_cast<std::uint8_t>(json.FindValue("little_bonus").GetInteger());
        mPoints.handlePoints = json.FindValue("handle_bonus").GetInteger();
        mPoints.slamDone = json.FindValue("slam_bonus").GetBool();

        JsonObject deal = json.FindValue("deal").GetObj();

        ctx.SetResult(deal);
        event = END_OF_DEAL;
    }
    else if (cmd == "EndOfGame")
    {
        mCurrentPlayer = Place(json.FindValue("winner").GetString());
        event = END_OF_GAME;
    }
    else
    {
        TLogError("Command not supported: " + cmd);
    }

    return event;
}
