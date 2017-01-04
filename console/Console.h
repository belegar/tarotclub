#ifndef CONSOLE_H
#define CONSOLE_H

#include <cstdint>

#ifdef USE_WINDOWS_OS
#include <windows.h>
#endif

class Console
{
public:
    Console();
    std::int32_t WhereX();
    std::int32_t WhereY();
    void Cls();
    void GotoXY(std::int32_t x, std::int32_t y);
private:

#ifdef USE_WINDOWS_OS
    HANDLE mHandle;
#endif


};

#endif // CONSOLE_H
