#ifndef __MODEM_S__
#define __MODEM_S__


#include "modem_api.h"
#include "stdbool.h"
#include "systick_api.h"
#include "debug.h"
#include "modem_service_fsm.h"


/* ==================================== API DECLARATION ========================================== */
void send_sms(const char* str);
bool service_is_ready(void);
void modem_service_process(void);
void modem_init_service(void);

#endif /* __MODEM_S__ */