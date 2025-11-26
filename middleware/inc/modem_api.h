#ifndef __MODEM_M__
#define __MODEM_M__

/* INCLUDE HEADER ----------------------------------------------------------------- */
#include "stdbool.h"
#include "stdint.h"
#include "modem_hw.h"
#include "lwrb.h"
#include "usart_api.h"
#include "systick_api.h"

/* LWRB --------------------------------------------------------------------------- */
extern  lwrb_t usart_rb;
extern  char line_buff[256];

/* TYPDEF STRCUT ------------------------------------------------------------------ */
typedef void (*gsm_callback)(const char* resp, uint32_t len);       /*<! Callback typedef */  

typedef struct {
    char cmd[64];                                                   /*<! AT command */
    char expect[32];                                                /*<! AT expect response */
    uint32_t timeout_ms;                                            /*<! Timeout for command */
    uint32_t start_tick;                                            /*<! Started time when send */
    gsm_callback cb;                                                /*<! Callback handle the command */
} modem_at_cmd_t;

typedef enum {
    MODEM_OK,
    MODEM_ERROR,
    MODEM_TIMEOUT,
} modem_status_t;

typedef struct {
    bool got_expect;
    bool got_final;
    bool got_error;
    uint8_t line_count;
} modem_cmd_state_t;


/* API DECLARATIONS --------------------------------------------------------------- */
bool modem_send_at_cmd(modem_at_cmd_t cmd);
void handle_response_line(const char *line);
void handle_urc_line(const char *urc);
void line_parse(void);
void clear_old_data(void);

void modem_init_hw(void);
void modem_init_md(void);
void modem_process(void);


#endif /* __MODEM_M__ */