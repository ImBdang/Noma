#include "string_handle.h"

void primary_dispatch(const char* str, char* result)
{
    if (str == NULL || result == NULL) {
        return;
    }

    const char *p = strchr(str, ':');
    if (!p) {
        result[0] = '\0';
        return;
    }

    p++; 
    while (*p == ' ' || *p == '\t') {
        p++;
    }

    while (*p != '\0' && *p != '\r' && *p != '\n') {
        *result++ = *p++;
    }
    *result = '\0';
}

uint8_t new_sms_dispatch(const char* str)
{
    const char* p = strrchr(str, ',');   
    if (!p) return 0;
    p++;
    uint8_t value = 0;
    
    while (*p >= '0' && *p <= '9') {
        value = value * 10 + (*p - '0');
        p++;
    }

    return value;
}
