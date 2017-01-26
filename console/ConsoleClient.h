#ifndef CONSOLECLIENT_H
#define CONSOLECLIENT_H

#include "Log.h"
#include "BotManager.h"
#include "Console.h"
#include "BasicClient.h"

#include <list>
#include <deque>

/*****************************************************************************/
class ConsoleClient : public Observer<std::string>, public net::IEvent
{
public:
    ConsoleClient();

    // From ConsoleClient
    void Stop();
    void Run(uint16_t tcp_port);

private:
    void Start(uint16_t tcp_port);

    // From Observer
    void Update(const std::string &info);

    // From net::IEvent
    void Signal(std::uint32_t sig);
    bool Deliver(uint32_t src_uuid, uint32_t dest_uuid, const std::string &arg, std::vector<Reply> &out);
    std::uint32_t AddUser(std::vector<Reply> &/*out*/);
    void RemoveUser(std::uint32_t /* uuid */, std::vector<Reply> &/*out*/);
    std::uint32_t GetUuid() { return mClient.mUuid; }

    Session mSession;
    BasicClient mClient;
    Context mCtx;
    BotManager mBots;
    std::list<std::uint32_t> mIds;
    Console mConsole;
    std::deque<std::wstring> mLog;

    std::mutex mMutex;
    bool mCanPlay;
    std::uint32_t mArrowPosition;

    std::wstring ToString(const Card &c);
    void DrawLine(const std::wstring &left, const std::wstring &middle, const std::wstring &right);
    void BuildBoard();
    void DrawBox(uint32_t size, std::uint32_t middle_pos = 0U);
    void DrawHLine(const std::wstring &left, const std::wstring &fill, const std::wstring &right, const std::wstring &middle = L"", std::uint32_t middle_pos = 0U);
    void FillBox(std::uint32_t size, std::uint32_t middle_pos = 0U);
    void AppendToLog(const std::wstring &str);
    void DisplayDeck();
    void DisplayCard(Card c, Place p);
    void ClearBoard();
    void DisplayText(const std::wstring &txt, Place p);
    void DisplayArrow();
    void HandleEvent(Console::KeyEvent event);
    void DisplayUserInfos(const std::wstring &txt);
    void ClearUserInfos();
    void ClearArrow();
};

#endif // CONSOLECLIENT_H
