#include "debug.h"

void usart_debug(const char* s){
    usart_sendstring(USART2, s);
}