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
#include "modem_urc_api.h"

/* LWRB --------------------------------------------------------------------------- */
extern  lwrb_t usart_rb;
extern  char line_buff[256];


/* TYPDEF STRCUT ------------------------------------------------------------------ */
typedef enum {
    OK_RESP,
    ERROR_RESP,
    TIMEOUT_RESP,
    PRIMARY,
    INTERMEDIATE
} respon_status_t;


/*<! Callback typedef */  
typedef void (*gsm_callback)(respon_status_t resp_status, const char* str, uint32_t len);     

typedef struct {
    char cmd[64];                                                   /*<! AT command */
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