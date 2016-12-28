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

    // Network serializers
    void BuildChangeNickname(std::vector<Reply> &out);
    void BuildBid(Contract c, bool slam, std::vector<Reply> &out);
    void JoinTable(std::uint32_t tableId, std::vector<Reply> &out);
    void SendHandle(const Deck &handle, std::vector<Reply> &out);
    void SendDiscard(const Deck &discard, std::vector<Reply> &out);
    void SendCard(Card c, std::vector<Reply> &out);
    void Sync(const std::string &step, std::vector<Reply> &out);

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
    std::uint32_t mTableId;
    std::string mNickName;
    std::string mUserName;
    Place mPlace;           ///< Assigned Place
    std::uint32_t mUuid;    ///< Assigned UUID

    Event Decode(uint32_t src_uuid, uint32_t dest_uuid, const std::string &arg, IContext &ctx, std::vector<Reply> &out);

};

#endif // BASICCLIENT_H
