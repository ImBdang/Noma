#ifndef __MODEM_URC_S__
#define __MODEM_URC_S__

#include "modem_service_fsm.h"
#include "modem_service_sms.h"
#include "modem_api.h"
#include "usart_api.h"
#include "event_driven.h"
#include "stdbool.h"

extern uint32_t next_offset;
extern uint32_t firmware_totals;

/* =============================== API DECLARATIONS =============================== */
void modem_service_urc_process(void);
#endif /* __MODEM_URC_S__ */    