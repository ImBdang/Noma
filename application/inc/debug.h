#ifndef __DEBUG__
#define __DEBUG__

#include "stdint.h"
#include "usart_api.h"

#define DEBUG_PRINT(s)              usart_debug(s)

void usart_debug(const char* s);


#endif /* __DEBUG__ */