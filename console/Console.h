#ifndef CONSOLE_H
#define CONSOLE_H

#include <cstdint>

#ifdef USE_WINDOWS_OS
#include <windows.h>
#include <io.h>
#endif

#include <iostream>
#include <string>
#include <locale>
#include <fstream>
#include <codecvt>
#include <fcntl.h>
#include <stdio.h>

class Console
{
public:
    enum KeyEvent
    {
        KB_NONE,
        KB_SPACE,
        KB_LEFT,
        KB_RIGHT,
        KB_C,
        KB_Q
    };

    Console();
    std::int32_t WhereX();
    std::int32_t WhereY();
    void Cls();
    void GotoXY(std::int32_t x, std::int32_t y);
    void Write(const std::wstring &s);

    KeyEvent ReadKeyboard();
    void SetCursor(bool enable);
private:

#ifdef USE_WINDOWS_OS
    HANDLE mHandle;
#endif

    int fd;


};

#endif // CONSOLE_H
