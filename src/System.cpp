
#ifdef USE_ANDROID_OS
#include <QStandardPaths>
#include <QString>
#endif

#include "System.h"
#include "Util.h"
#include "TarotDeck.h"

/*****************************************************************************/
std::string System::mHomePath;
std::string System::mDeckPath;
#ifdef TAROT_DEBUG
std::string System::mScriptPath;
#endif

/*****************************************************************************/
std::string System::ScriptPath()
{
#ifdef TAROT_DEBUG
    return mScriptPath;
#else
    return mHomePath + "/ai";
#endif
}
/*****************************************************************************/
void System::Initialize()
{
// ----------- Android
#ifdef USE_ANDROID_OS
    QString path = QStandardPaths::writableLocation(QStandardPaths::DataLocation);
    mHomePath = path.toStdString();

    FIXME: Copy script files from the Qt resource file to the application internal memory

    mDeckPath = ":cards/default/";
// ----------- Linux, Windows ...
#else

#ifdef TAROT_DEBUG
    mHomePath  = "./";
    mScriptPath = Util::ExecutablePath() + "/../../assets/ai/";
    mDeckPath = Util::ExecutablePath() + "/../../assets/cards/default/";
#else
    mHomePath  = Util::HomePath() + "/.tarotclub/";
    mDeckPath = Util::ExecutablePath() + "/default/";
#endif
#endif

    // Check the user TarotClub directories and create them if necessary (all platforms)
    if (!Util::FolderExists(HomePath()))
    {
        Util::Mkdir(HomePath());
    }
    if (!Util::FolderExists(GamePath()))
    {
        Util::Mkdir(GamePath());
    }
    if (!Util::FolderExists(LogPath()))
    {
        Util::Mkdir(LogPath());
    }

    TarotDeck::Initialize();
}

/*****************************************************************************/
System::System()
{
}
