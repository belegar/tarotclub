#ifndef BASICCLIENT_H
#define BASICCLIENT_H

#include "Network.h"
#include "Player.h"
#include "Score.h"

struct BasicClient
{
public:
    enum Event
    {
        JSON_ERROR,
        BAD_EVENT,
        REQ_LOGIN,
        ACCESS_GRANTED,
        MESSAGE,
        PLAYER_LIST,
        PLAYER_EVENT,
        JOIN_TABLE,
        SYNC,
        QUIT_TABLE,
        NEW_GAME,
        NEW_DEAL,
        REQ_BID,
        SHOW_BID,
        ALL_PASSED,
        SHOW_DOG,
        BUILD_DISCARD,
        START_DEAL,
        ASK_FOR_HANDLE,
        SHOW_HANDLE,
        PLAY_CARD,
        SHOW_CARD,
        END_OF_TRICK,
        END_OF_DEAL,
        END_OF_GAME
    };

    BasicClient();

    void Start();
    void Stop();
    bool TestDiscard(const Deck &discard);
    Contract CalculateBid();
    void UpdateStatistics();
    Card Play();
    bool IsValid(const Card &c);
    Deck AutoDiscard();
    bool IsMyTurn() { return mCurrentPlayer == mMyself.place; }

    // Network serializers
    void BuildNewGame(std::vector<Reply> &out);
    void BuildChangeNickname(std::vector<Reply> &out);
    void BuildBid(Contract c, bool slam, std::vector<Reply> &out);
    void BuildJoinTable(std::uint32_t tableId, std::vector<Reply> &out);
    void BuildHandle(const Deck &handle, std::vector<Reply> &out);
    void BuildDiscard(const Deck &discard, std::vector<Reply> &out);
    void BuildSendCard(Card c, std::vector<Reply> &out);
    void BuildQuitTable(std::uint32_t tableId, std::vector<Reply> &out);
    void Sync(Engine::Sequence sequence, std::vector<Reply> &out);

    Deck::Statistics   mStats;   // statistics on player's cards
    Tarot::Game mGame;
    Tarot::Bid  mBid;
    Tarot::Distribution mShuffle;
    Points mPoints;
    Deck mCurrentTrick;
    Deck mDog;
    Deck mHandle;
    Player mDeck; ///< player own deck
    Place mCurrentPlayer;
    std::uint8_t mNbPlayers;
    Users::Entry mMyself;

    Event Decode(uint32_t src_uuid, uint32_t dest_uuid, const std::string &arg, IContext &ctx, std::vector<Reply> &out);
private:
    void GetPlayerStatus(Users::Entry &member, JsonObject &player);
};

#endif // BASICCLIENT_H
