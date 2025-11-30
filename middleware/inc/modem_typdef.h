#ifndef __MODEM_TYPES_H__
#define __MODEM_TYPES_H__

#include "stdint.h"

typedef enum {
    OK_RESP,
    ERROR_RESP,
    TIMEOUT_RESP,
    PRIMARY,
    INTERMEDIATE
} respon_status_t;

typedef void (*gsm_callback)(respon_status_t status, const char* line, uint32_t len);

#endif
