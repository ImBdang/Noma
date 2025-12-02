#ifndef __MODEM_URC_M__
#define __MODEM_URC_M__

#include "stdbool.h"
#include "stdint.h"

#include "event_driven.h"
#include "systick_api.h"
#include "string_handle.h"
#include "debug.h"
#include "flash_mcu.h"
#include "http_service.h"
#include "modem_service_sms.h"

extern uint32_t http_read_size; 

/* ============================== API DECLARATIONS ============================== */
void modem_urc_make_event(const char *urc);

#endif /* __MODEM_URC_M__ */