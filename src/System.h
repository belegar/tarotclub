#ifndef SYSTEM_H
#define SYSTEM_H

#include <string>

/*****************************************************************************/
class System
{
public:
    static std::string HomePath() { return mHomePath; }
    static std::string GamePath() { return mHomePath + "/games/"; }
    static std::string LogPath() { return mHomePath + "/logs/"; }
    static std::string ScriptPath() { return  mHomePath + "/ai"; }

    static void Initialize();

private:
    System(); // forbid any instance

    static std::string mHomePath;
};

#endif // SYSTEM_H
