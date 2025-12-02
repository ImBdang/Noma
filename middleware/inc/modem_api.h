#ifndef __MODEM_M__
#define __MODEM_M__

/* INCLUDE HEADER ----------------------------------------------------------------- */
#include "stdbool.h"
#include "stdint.h"

#include "debug.h"
#include "modem_hw.h"
#include "lwrb.h"
#include "usart_api.h"
#include "systick_api.h"
#include "flash_mcu.h"
#include "modem_urc_api.h"
#include "modem_typdef.h"

/* LWRB --------------------------------------------------------------------------- */
extern  lwrb_t usart_rb;
extern  char line_buff[1024];

extern bool httpread_incoming;
extern uint32_t httpread_remaining;
extern uint8_t* httpread_ptr;
extern uint8_t temp_buf[1024];

/* TYPDEF STRCUT ------------------------------------------------------------------ */ 

typedef struct {
    char cmd[128];                                                   /*<! AT command */
    char expect[32];                                                /*<! AT expect response */
    uint32_t timeout_ms;                                            /*<! Timeout for command */
    uint32_t start_tick;                                            /*<! Started time when send */
    gsm_callback cb;                                                /*<! Callback handle the command */
} modem_at_cmd_t;  


/* API DECLARATIONS --------------------------------------------------------------- */
bool modem_send_at_cmd(modem_at_cmd_t cmd);
void modem_init_md(void);
void modem_process(void);


#endif /* __MODEM_M__ */