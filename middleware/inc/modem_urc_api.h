#ifndef __MODEM_URC_M__
#define __MODEM_URC_M__

#include "stdbool.h"
#include "stdint.h"

#include "event_driven.h"
#include "systick_api.h"
#include "debug.h"

 

/* ============================== API DECLARATIONS ============================== */
void modem_urc_process(const char *urc);

#endif /* __MODEM_URC_M__ */