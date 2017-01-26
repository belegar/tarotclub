#include "Console.h"

Console::Console()
{
    mHandle = GetStdHandle(STD_OUTPUT_HANDLE);

    _setmode(_fileno(stdout), _O_U16TEXT);
}

void Console::Write(const std::wstring &s)
{
    WriteConsoleW(mHandle, s.c_str(), s.size(), NULL, NULL);
}


void Console::HideCursor()
{
   CONSOLE_CURSOR_INFO info;
   info.dwSize = 100;
   info.bVisible = FALSE;
   SetConsoleCursorInfo(mHandle, &info);
}

Console::KeyEvent Console::ReadKeyboard()
{
    HANDLE hInput = GetStdHandle(STD_INPUT_HANDLE);
    DWORD NumInputs = 0;
    DWORD InputsRead = 0;
    INPUT_RECORD irInput;

    Console::KeyEvent event = NO_KEY;

    GetNumberOfConsoleInputEvents(hInput, &NumInputs);

    ReadConsoleInput(hInput, &irInput, 1, &InputsRead);

    switch(irInput.Event.KeyEvent.wVirtualKeyCode)
    {
    case VK_LEFT:
            event = KEY_LEFT;
        break;

    case VK_RIGHT:
            event = KEY_RIGHT;
        break;

    case VK_SPACE:
            event = KEY_SPACE;
        break;
    case VK_F1:
            event = KEY_F1;
        break;
    case VK_F2:
            event = KEY_F2;
        break;
    default:
        break;
    }

    return event;
}

std::int32_t Console::WhereX()
{

#ifdef USE_WINDOWS_OS
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    COORD  result;
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
    COORD  result;
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
    printf("%c[%d;%df",0x1B,y,x);
#endif
}

/*
printf("\033[XA"); // Move up X lines;
printf("\033[XB"); // Move down X lines;
printf("\033[XC"); // Move right X column;
printf("\033[XD"); // Move left X column;
*/

