#ifndef UTIL_H
#define UTIL_H

#include <ctime>
#include <sstream>
#include <iomanip>
#include <string>
#include <vector>


/*****************************************************************************/
class Util
{
public:
    // Get current date/time, format is YYYY-MM-DD.HH:mm:ss
    static std::string CurrentDateTime(const std::string &format);
    static std::string ExecutablePath();
    static std::string HomePath();
    static bool FolderExists(const std::string &foldername);
    static int Mkdir(const char *path);
};

#endif // UTIL_H
