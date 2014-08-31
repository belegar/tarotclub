#ifndef GETOPTIONS_H
#define GETOPTIONS_H

#include <algorithm>

char* GetCmdOption(char ** begin, char ** end, const std::string & option)
{
    char ** itr = std::find(begin, end, option);
    if (itr != end && ++itr != end)
    {
        return *itr;
    }
    return 0;
}

bool CmdOptionExists(char** begin, char** end, const std::string& option)
{
    return std::find(begin, end, option) != end;
}

#endif // GETOPTIONS_H
