#include "Console.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifdef USE_UNIX_OS
#define _XOPEN_SOURCE 700
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <locale.h> // to set Unicode locale
#endif


Console::Console(bool native)
    : mUseNativeKbEvents(native)
{
#ifdef USE_WINDOWS_OS
    mHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    _setmode(_fileno(stdout), _O_U16TEXT);
#else
    setlocale(LC_ALL, "");

    struct termios term;

    tcgetattr( STDIN_FILENO, &term );
    term.c_lflag &= ~( ICANON | ECHO );
    tcsetattr( STDIN_FILENO, TCSANOW, &term );
#endif
}

void Console::Write(const std::wstring &s)
{
#ifdef USE_WINDOWS_OS
    WriteConsoleW(mHandle, s.c_str(), static_cast<DWORD>(s.size()), NULL, NULL);
#else
    // On linux, UTF8 is native, so std::cout terminal understand this encoding
    std::wstring_convert<std::codecvt_utf8<wchar_t>,wchar_t> convert; // converts between UTF-8 and UCS-4 (given sizeof(wchar_t)==4)
    std::cout << convert.to_bytes(s);
#endif
}


void Console::SetCursor(bool enable)
{
#ifdef USE_WINDOWS_OS
   CONSOLE_CURSOR_INFO info;
   info.dwSize = 100;
   info.bVisible = FALSE;
   SetConsoleCursorInfo(mHandle, &info);
#else
    if (enable)
    {
        printf("\e[?25h");
    }
    else
    {
        printf("\e[?25l");
    }
#endif
}

Console::KeyEvent Console::ReadKeyboard()
{
    Console::KeyEvent event = KeyEvent::KB_NONE;

    if (mUseNativeKbEvents)
    {
#ifdef USE_WINDOWS_OS
        event = ReadWin32ConsoleEvents();
#else
#warning "Implement OS native console events"
#endif
    }
    else
    {
        int ch = getchar() & 0xFF;

        if (ch == 27)
        {
            ch = getchar() & 0xFF;
            if (ch == '[')
            {
                ch = getchar() & 0xFF;
                if (ch == 'D')
                {
                    event = KeyEvent::KB_LEFT;
                }
                else if (ch == 'C')
                {
                    event = KeyEvent::KB_RIGHT;
                }
                else
                {
                    event = KeyEvent::KB_NONE;
                }
            }
        }
        else
        {
            if (ch == 'c')
            {
                event = KeyEvent::KB_C;
            }
            else if (ch == 'q')
            {
                event = KeyEvent::KB_Q;
            }
            else if (ch == ' ')
            {
                event = KeyEvent::KB_SPACE;
            }
            else
            {
                event = KeyEvent::KB_NONE;
            }
        }
    }

    return event;
}

std::int32_t Console::WhereX()
{

#ifdef USE_WINDOWS_OS
    CONSOLE_SCREEN_BUFFER_INFO csbi;
	COORD  result = { 0, 0 };
    if (!GetConsoleScreenBufferInfo(mHandle, &csbi))
    return -1;
    return result.X;
#else
    return 0;
#endif
}

std::int32_t Console::WhereY()
{
#ifdef USE_WINDOWS_OS
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    COORD  result = { 0, 0 };
    if (!GetConsoleScreenBufferInfo(mHandle, &csbi
         ))
    return -1;
    return result.Y;
#else
    return 0;
#endif
}


void Console::Cls()
{
#ifdef USE_WINDOWS_OS
   COORD coordScreen = { 0, 0 };    // home for the cursor
   DWORD cCharsWritten;
   CONSOLE_SCREEN_BUFFER_INFO csbi;
   DWORD dwConSize;

// Get the number of character cells in the current buffer.

   if( !GetConsoleScreenBufferInfo( mHandle, &csbi ))
   {
      return;
   }

   dwConSize = csbi.dwSize.X * csbi.dwSize.Y;

   // Fill the entire screen with blanks.

   if( !FillConsoleOutputCharacter( mHandle,        // Handle to console screen buffer
                                    (TCHAR) ' ',     // Character to write to the buffer
                                    dwConSize,       // Number of cells to write
                                    coordScreen,     // Coordinates of first cell
                                    &cCharsWritten ))// Receive number of characters written
   {
      return;
   }

   // Get the current text attribute.

   if( !GetConsoleScreenBufferInfo( mHandle, &csbi ))
   {
      return;
   }

   // Set the buffer's attributes accordingly.

   if( !FillConsoleOutputAttribute( mHandle,         // Handle to console screen buffer
                                    csbi.wAttributes, // Character attributes to use
                                    dwConSize,        // Number of cells to set attribute
                                    coordScreen,      // Coordinates of first cell
                                    &cCharsWritten )) // Receive number of characters written
   {
      return;
   }

   // Put the cursor at its home coordinates.

   SetConsoleCursorPosition( mHandle, coordScreen );

#else
    printf("\033[H\033[J"); // printf("\033[2J"); // Clear screen ??

#endif
}

void Console::GotoXY(std::int32_t x, std::int32_t y)
{
#ifdef USE_WINDOWS_OS
    COORD coord;
    coord.X = x;
    coord.Y = y;
    SetConsoleCursorPosition(mHandle, coord);
#else
    printf("%c[%d;%df",0x1B, y+1, x);
#endif
}

#ifdef USE_WINDOWS_OS
Console::KeyEvent Console::ReadWin32ConsoleEvents()
{
    Console::KeyEvent event = KeyEvent::KB_NONE;
    HANDLE hInput = GetStdHandle(STD_INPUT_HANDLE);
    DWORD NumInputs = 0;
    DWORD InputsRead = 0;
    INPUT_RECORD irInput;

    GetNumberOfConsoleInputEvents(hInput, &NumInputs);

    ReadConsoleInput(hInput, &irInput, 1, &InputsRead);

    if (irInput.Event.KeyEvent.bKeyDown)
    {
        switch(irInput.Event.KeyEvent.wVirtualKeyCode)
        {
        case VK_LEFT:
                event = KeyEvent::KB_LEFT;
            break;

        case VK_RIGHT:
                event = KeyEvent::KB_RIGHT;
            break;

        case VK_SPACE:
                event = KeyEvent::KB_SPACE;
            break;
        case 'C':
                event = KB_C;
            break;
        case 'Q':
                event = KB_Q;
            break;
        default:
            break;
        }

        // FlushConsoleInputBuffer(hInput); // in case of problems of multiple events, enable this...
    }

    return event;
}
#endif // USE_WINDOWS_OS

