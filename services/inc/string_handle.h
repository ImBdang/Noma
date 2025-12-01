#ifndef __STRING_HANDLE_S__
#define __STRING_HANDLE_S__

#include "string.h"
#include <stdio.h>
#include "stdint.h"

/* ====================================== API DECLARATION ===================================== */
void primary_dispatch(const char* str, char* result);
void new_sms_dispatch(const char* str, char* id_out);
void http_action_dispatch(const char* str, uint8_t* method, char status[4], uint32_t* len) ;
void httpaction_dispatch(const char* str, uint32_t* size_out);
uint32_t httpread_dispatch(const char* str);
uint32_t fast_atoi(const char *s);
#endif /* __STRING_HANDLE_S__ */