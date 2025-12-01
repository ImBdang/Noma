#ifndef __HTTP_CALLBACK_S__
#define __HTTP_CALLBACK_S__

#include "stdint.h"
#include "modem_typdef.h"

// typedef enum {
//     OK_RESP,
//     ERROR_RESP,
//     TIMEOUT_RESP,
//     PRIMARY,
//     INTERMEDIATE
// } http_status_code_t;

// typedef void (*gsm_callback)(http_status_code_t status_code, const char* str, uint32_t len);     

void httpread_callback(respon_status_t resp, const char* line, uint16_t len);

#endif /* __HTTP_CALLBACK_M__ */