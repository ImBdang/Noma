#include "string_handle.h"

uint32_t fast_atoi(const char *s) {
    uint32_t v = 0;
    while (*s >= '0' && *s <= '9') {
        v = v * 10 + (*s - '0');
        s++;
    }
    return v;
}

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



void http_get_dispatch(const char* str, char status[4], uint32_t* len) {
    const char *p1 = strchr(str, ',');      

    const char *p2 = strchr(p1 + 1, ','); 

    uint32_t st = fast_atoi(p1 + 1);
    status[0] = (st / 100) % 10 + '0';
    status[1] = (st / 10) % 10 + '0';
    status[2] = st % 10 + '0';
    status[3] = '\0';

    *len = fast_atoi(p2 + 1);
}