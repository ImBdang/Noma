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

void new_sms_dispatch(const char* str, char* id_out)
{
    const char* p = strrchr(str, ',');
    if (!p) {
        id_out[0] = '\0';
        return;
    }

    p++; 
    int i = 0;
    while (*p >= '0' && *p <= '9' && i < 3) {
        id_out[i++] = *p++;
    }
    id_out[i] = '\0';  
}