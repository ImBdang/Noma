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