#include <stdio.h>
#include <emscripten.h> // note we added the emscripten header

#include "BasicClient.h"

#ifdef __cplusplus
extern "C" {
#endif

int EMSCRIPTEN_KEEPALIVE fib(int n){
    if(n == 0 || n == 1)
        return 1;
    else
        return fib(n - 1) + fib(n - 2);
}

#ifdef __cplusplus
}
#endif

Card card(1, Card::CLUBS);
Deck deck;

BasicClient client;

int main(){
    printf("Hello world!\n");
    int res = fib(5);
    printf("fib(5) = %d\n", res);
    return 0;
}

