#ifndef __HTTP_S__
#define __HTTP_S__

#include "stdint.h"
#include "stdbool.h"
#include "modem_typdef.h"
#include "modem_service_callback_fsm.h"


/* ================================= FSM =========================================== */
typedef enum {
    HTTP_CHECK_ONLINE,
    HTTP_WAIT_CHECK_ONLINE,

    HTTP_INIT_ENTRY,
    HTTP_INIT_WAIT,

    HTTP_CONFIG_ENTRY,
    HTTP_CONFIG_WAIT,

    REACTIVE_ATE0,

    HTTP_GET_ENTRY,

    HTTP_STATE_READY
} http_state_t;

/* ================================= API DECLARATIONS ============================== */
extern bool http_init_flag;

void http_read(uint32_t data_len);
void http_get(void);
bool http_init(void);

#endif /* __HTTP_M__ */