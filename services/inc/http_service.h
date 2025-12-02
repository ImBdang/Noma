#ifndef __HTTP_S__
#define __HTTP_S__

#include "stdio.h"
#include "stdint.h"
#include "stdbool.h"
#include "modem_typdef.h"
#include "modem_service_callback_fsm.h"
#include "http_callback.h"
#include "debug.h"


/* ================================= FSM =========================================== */

/* ================================= API DECLARATIONS ============================== */
extern bool http_init_flag;

bool http_term(void);
bool http_set_url(const char* url);
bool http_read(uint32_t* offset, uint32_t data_len);
bool http_get(void);
bool http_init(void);

#endif /* __HTTP_M__ */