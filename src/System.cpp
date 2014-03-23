
#ifdef USE_ANDROID_OS
#include <QStandardPaths>
#include <QString>
#endif

#include "System.h"
#include "Util.h"
#include "TarotDeck.h"

/*****************************************************************************/
std::string System::mHomePath;
/*****************************************************************************/
void System::Initialize()
{

// ----------- Android
#ifdef USE_ANDROID_OS
    QString path = QStandardPaths::writableLocation(QStandardPaths::DataLocation);
    mHomePath = path.toStdString();

// ----------- Linux, Windows ...
#else

#ifdef TAROT_DEBUG
    mHomePath  = "./";
#else
    mHomePath  = Util::HomePath() + "/.tarotclub/";
#endif
#endif

    // Check user TarotClub directories and create them if necessary (all platforms)

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
