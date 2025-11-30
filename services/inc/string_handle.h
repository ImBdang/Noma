#ifndef __STRING_HANDLE_S__
#define __STRING_HANDLE_S__

#include "string.h"
#include <stdio.h>
#include "stdint.h"

/* ====================================== API DECLARATION ===================================== */
void primary_dispatch(const char* str, char* result);
void new_sms_dispatch(const char* str, char* id_out);
void http_get_dispatch(const char* str, char status[4], uint32_t* len) ;
uint32_t fast_atoi(const char *s);
#endif /* __STRING_HANDLE_S__ */