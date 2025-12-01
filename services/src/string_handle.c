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



void http_action_dispatch(const char* str,
                          uint8_t* method,
                          char status_code[4],
                          uint32_t* len)
{
    const char* p = strstr(str, "+HTTPACTION:");
    if (!p) return;

    p += strlen("+HTTPACTION:");

    while (*p == ' ') p++;

    // Parse method
    *method = (uint8_t)fast_atoi(p);

    // Parse status
    const char* p1 = strchr(p, ',');
    if (!p1) return;

    uint32_t st = fast_atoi(p1 + 1);

    // Convert 
    status_code[0] = (st / 100) % 10 + '0';
    status_code[1] = (st / 10)  % 10 + '0';
    status_code[2] = st % 10        + '0';
    status_code[3] = '\0';

    // Parse length
    const char* p2 = strchr(p1 + 1, ',');
    if (!p2) return;

    *len = fast_atoi(p2 + 1);
}


uint32_t httpread_dispatch(const char* str){
    const char* p = strstr(str, "+HTTPREAD:");
    if (!p) return 0;

    p += strlen("+HTTPREAD:");

    while (*p == ' ' || *p == '\t') p++;

    return fast_atoi(p);
}

void httpaction_dispatch(const char* str, uint32_t* size_out){
    if (!str || !size_out) return;

    uint16_t method = 0;
    uint16_t status = 0;
    uint32_t length = 0;

    if (sscanf(str, "+HTTPACTION: %d,%d,%d", &method, &status, &length) == 3) {
        *size_out = (uint32_t)length;
    } else {
        *size_out = 0;   
    }
}