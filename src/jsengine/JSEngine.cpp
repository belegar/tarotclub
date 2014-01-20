
#include "JSEngine.h"



extern "C" void my_panic_handler(int code, const char *msg)
{
    /* Your panic handling.  Must not return. */
    volatile int i = 0;
    i++;
    while(1);
}

