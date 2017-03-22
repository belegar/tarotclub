#ifndef SRVAICONTEST_H
#define SRVAICONTEST_H


class SrvAiContest
{
public:
    SrvAiContest();

    /*
    struct BotMatch
    {
        std::uint32_t tableId;
        Identity identity;
        std::chrono::high_resolution_clock::time_point startTime;
        JsonArray deals;
        Score score;
        std::vector<std::uint32_t> botIds;
        bool finished;
    };



    void Start(const ServerOptions &options, const TournamentOptions &tournamentOpt);
    void Stop() { mStopRequested = true; mThread.join(); }

    // From Lobby::Event
    virtual void Update(const LobbyServer::Event &event);

    // From Protocol::IWorkItem
    virtual bool DoAction(std::uint8_t cmd, std::uint32_t src_uuid, std::uint32_t dest_uuid, const ByteArray &data);
*/


private:
    /*
    BotManager mBotManager;
    std::thread mThread;
    bool mInitialized;
    bool mStopRequested;
    std::uint16_t mGamePort;

    // AI Contest variables
    std::vector<BotMatch> mPendingAi; // Pending matches
    std::mutex mAiMutex;
    std::vector<Tarot::Distribution> mAiContest;

    */

    /*
    // Thread
    static void EntryPoint(void *pthis);
    void Run();

    void CheckNewAIBots();
    bool IsPending(const std::string &username);
    bool StoreBotScore(const BotMatch &match, const std::string &docId);
    void CheckFinishedGames();
    */

};

#endif // SRVAICONTEST_H
