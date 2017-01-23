#ifndef CONSOLE_H
#define CONSOLE_H

#include <cstdint>

#ifdef USE_WINDOWS_OS
#include <windows.h>
#endif

#include <iostream>
#include <string>
#include <locale>
#include <fstream>
#include <codecvt>
#include <fcntl.h>
#include <io.h>
#include <stdio.h>

class Console
{
public:
    Console();
    std::int32_t WhereX();
    std::int32_t WhereY();
    void Cls();
    void GotoXY(std::int32_t x, std::int32_t y);
    void Write(const std::wstring &s);
private:

#ifdef USE_WINDOWS_OS
    HANDLE mHandle;
#endif


};

#endif // CONSOLE_H
