/*=============================================================================
 * TarotClub - main.cpp
 *=============================================================================
 * Entry point of TarotClub dedicated server
 *=============================================================================
 * TarotClub ( http://www.tarotclub.fr ) - This file is part of TarotClub
 * Copyright (C) 2003-2999 - Anthony Rabine
 * anthony@tarotclub.fr
 *
 * TarotClub is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * TarotClub is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with TarotClub.  If not, see <http://www.gnu.org/licenses/>.
 *
 *=============================================================================
 */

#include "EventLoop.h"
#include "GetOptions.h"
#include "System.h"
#include "Log.h"
#include "Version.h"
#include "ServiceStats.h"
#include "JSEngine.h"
#include "Terminal.h"
#include "IService.h"
#include "Server.h"

#include <execinfo.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

/*****************************************************************************/
class Logger : public Observer<Log::Infos>
{
public:
    Logger()
        : Observer()
    {

    }

    void Update(const Log::Infos &info)
    {
        std::cout << info.message << std::endl;
    }
};
/*****************************************************************************/
void handler(int sig) {
  void *array[10];
  size_t size;

  // get void*'s for all entries on the stack
  size = backtrace(array, 10);

  // print out all the frames to stderr
  fprintf(stderr, "Error: signal %d:\n", sig);
  backtrace_symbols_fd(array, size, STDERR_FILENO);
  exit(1);
}
/*****************************************************************************/
/**
 * @brief Entry point of the dedicated game server
 */
int main(int argc, char *argv[])
{
    CommandLine cmd(argc, argv);

    signal(SIGSEGV, handler);   // install our handler
    signal(SIGPIPE, SIG_IGN); // ignore sigpipe for this process

    std::cout << "TarotClub dedicated server " << TCDS_VERSION << std::endl;

    if(cmd.Exists("-h"))
    {
        // Print help and exit:
        std::cout << std::endl << "Usage: " << argv[0] << "[option] [argument] ..." << std::endl;
        std::cout << "Options are:" << std::endl;
        std::cout << "\t" << "-h" << "\tPrints this help" << std::endl;
        std::cout << "\t" << "-w" << "\tSpecifies a workspace path where generated/configuration files are located" << std::endl;
        std::cout << "\t" << "-t" << "\tSpecifies a token for Web server registering" << std::endl;
        return 0;
    }

    std::string homePath;
    if(cmd.Exists("-w"))
    {
        homePath = cmd.GetOption("-w");
        System::Initialize(homePath);
    }
    else
    {
        System::Initialize(); // default home path
        homePath = System::HomePath();
    }
    std::cout << "Using home path: " << homePath << std::endl;

    Logger logger;
    Log::SetLogPath(System::LogPath());
    Log::RegisterListener(logger);

    // Load options
    ServerConfig srvConfig;
    TournamentConfig trnConfig;

    srvConfig.Load(System::HomePath() + ServerConfig::DEFAULT_SERVER_CONFIG_FILE);
    trnConfig.Load(System::HomePath() + TournamentConfig::DEFAULT_FILE_NAME);

    // Init lobby
    Lobby lobby;
    Server server(lobby);

    EventLoop loop;
    JSEngine js;

    // Instanciate all your services here
    ServiceStats stats(js, loop, lobby);
//    Terminal term(js, loop);

//    IService *cServices[] = {
//        &stats,
//        &term
//    };

    // Initialize everything
    lobby.Initialize("TarotServer", srvConfig.GetOptions().tables);
    server.Start(srvConfig.GetOptions());
    js.Initialize();

    // FIXME: services should be aware of command line configuration, think about a
    // solution (standardized structure like ServerOptions)
//    term.SetPort(ServerConfig::DEFAULT_CONSOLE_TCP_PORT);

    // Initialize services
//    for (auto *srv : cServices)
//    {
//        std::cout << "Loading service: " << srv->GetName() << std::endl;
//        srv->Initialize();
//    }

    static std::thread webServerRegister(WebServerRegisterThread);
    loop.Loop();

//    // Stop services
//    for (auto *srv : cServices)
//    {
//        std::cout << "Stopping service: " << srv->GetName() << std::endl;
//        srv->Stop();
//    }

//    loop.Stop();
    server.Stop();
    webServerRegister.join();

    return 0;
}

//=============================================================================
// End of file main.cpp
//=============================================================================
