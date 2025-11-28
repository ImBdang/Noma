#ifndef __STRING_HANDLE_S__
#define __STRING_HANDLE_S__

#include "string.h"
#include "stdint.h"

/* ====================================== API DECLARATION ===================================== */
void primary_dispatch(const char* str, char* result);
uint8_t new_sms_dispatch(const char* str);

#endif /* __STRING_HANDLE_S__ */