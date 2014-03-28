#ifndef SYSTEM_H
#define SYSTEM_H

#include <string>

/*****************************************************************************/
class System
{
public:
    static std::string HomePath()   { return mHomePath; }
    static std::string GamePath()   { return mHomePath + "/games/"; }
    static std::string LogPath()    { return mHomePath + "/logs/"; }

    /**
     * @brief DeckPath, where the card images are located
     * @return The path
     */
    static std::string DeckPath()   { return mDeckPath; }
    static std::string ScriptPath();

    static void Initialize();

private:
    System(); // forbid any instance

    static std::string mHomePath;
    static std::string mDeckPath;

#ifdef TAROT_DEBUG
    static std::string mScriptPath;
#endif
};

#endif // SYSTEM_H
