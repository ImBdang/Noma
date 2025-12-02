#ifndef __HTTP_CALLBACK_S__
#define __HTTP_CALLBACK_S__

#include "stdint.h"
#include "modem_typdef.h"
#include "event_driven.h"
#include "string_handle.h"
#include "debug.h"

// typedef enum {
//     OK_RESP,
//     ERROR_RESP,
//     TIMEOUT_RESP,
//     PRIMARY,
//     INTERMEDIATE
// } http_status_code_t;

// typedef void (*gsm_callback)(http_status_code_t status_code, const char* str, uint32_t len);     


void httpget_callback(respon_status_t resp, const char* line, uint32_t len);
void httpread_callback(respon_status_t resp, const char* line, uint32_t len);

#endif /* __HTTP_CALLBACK_M__ */