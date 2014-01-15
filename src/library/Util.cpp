

#ifdef USE_WINDOWS_OS
#include <windows.h>
#include <direct.h>
#include <sys/stat.h>
#endif

#ifdef USE_UNIX_OS
#include <cstdio>
#include <unistd.h>
#include <sys/stat.h>
#endif

#include "Util.h"

/*****************************************************************************/
// Get current date/time, format is YYYY-MM-DD.HH:mm:ss
std::string Util::CurrentDateTime(const std::string &format)
{
    std::stringstream datetime;

    time_t rawtime;
    struct tm * timeinfo;
    char buffer [80];

    time (&rawtime);
    timeinfo = localtime(&rawtime);

    strftime (buffer, sizeof(buffer), format.c_str(), timeinfo);

    datetime << buffer;
/*
 * This code if the C++0x11 way of formating date, but GCC does not support it yet :(
    std::stringstream datetime;
    std::time_t t = std::time(nullptr);
    std::tm tm = *std::localtime(&t);

    datetime << std::put_time(&tm, format);
*/
    return datetime.str();
}
/*****************************************************************************/
std::string Util::ExecutablePath()
{
    std::string path;
    std::uint32_t found;

#ifdef USE_WINDOWS_OS
    wchar_t buf[MAX_PATH];

    // Will contain exe path
    HMODULE hModule = GetModuleHandle(NULL);
    if (hModule != NULL)
    {
        // When passing NULL to GetModuleHandle, it returns handle of exe itself
        GetModuleFileName(hModule, buf, (sizeof(buf)));
    }
    std::wstring wstr(buf);
    path = std::string(wstr.begin(), wstr.end());
    found = path.find_last_of("\\");

#elif defined(USE_UNIX_OS)
    char buf[FILENAME_MAX];
    readlink("/proc/self/exe", buf, sizeof(buf));
    path = buf;
    found = path.find_last_of("/");
#elif defined(USE_APPLE_OS)
    _NSGetExecutablePath(path, &size); // make it compile
#else
#error "A portable code is needed here"
#endif
    return(path.substr(0, found));
}
/*****************************************************************************/
std::string Util::HomePath()
{
    std::string homedir;

#if defined(USE_WINDOWS_OS)
    homedir = getenv("HOMEDRIVE");
    homedir += getenv("HOMEPATH");
#else
    homedir = getenv("HOME");
#endif
    return homedir;
}
/*****************************************************************************/
/**
 * Checks if a folder exists
 * @param foldername path to the folder to check.
 * @return true if the folder exists, false otherwise.
 */
bool Util::FolderExists(const std::string &foldername)
{
    struct stat st;
    ::stat(foldername.c_str(), &st);
    return st.st_mode & S_IFDIR;
}
/*****************************************************************************/
/**
 * Portable wrapper for mkdir. Internally used by mkdir()
 * @param[in] path the full path of the directory to create.
 * @return zero on success, otherwise -1.
 */
int my_mkdir(const char *path)
{
#if defined(USE_WINDOWS_OS)
    return ::_mkdir(path);
#else
    return ::mkdir(path, 0755); // not sure if this works on mac
#endif
}
/*****************************************************************************/
/**
 * Recursive, portable wrapper for mkdir.
 * @param[in] path the full path of the directory to create.
 * @return zero on success, otherwise -1.
 */
bool Util::Mkdir(const char *path)
{
    std::string current_level = "";
    std::string level;
    std::stringstream ss(path);

    // split path using slash as a separator
    while (std::getline(ss, level, '/'))
    {
        current_level += level; // append folder to the current level

        if (FolderExists(current_level))
        {
            continue;
        }
        else if ((current_level[1] == ':') && (current_level.size() == 2))
        {
            // not a directory but a disk letter (windows only)
            continue;
        }
        else
        {
            // create current level
            if (my_mkdir(current_level.c_str()) != 0)
            {
                return false;
            }
        }
        current_level += "/"; // don't forget to append a slash
    }

    return true;
}
