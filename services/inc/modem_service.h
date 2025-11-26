#ifndef __MODEM_S__
#define __MODEM_S__

#include "modem_api.h"
#include "stdbool.h"


/* ==================================== TYPEDEF STRUCT ========================================= */

typedef enum {
    MODEM_PWR_OFF,
    MODEM_PWR_ON,
    MODEM_RESET,
} modem_power_t;

/* ==================================== GLOBAL VARIABLES ======================================== */
modem_power_t modem_power_state;        
modem_status_t modem_status_state;


/* ==================================== API DECLARATION ========================================== */
modem_status_t modem_power(modem_power_t state);

bool modem_power_on(void);
bool modem_power_off(void);
bool modem_power_reset(void);

void init_modem(void);

#endif /* __MODEM_S__ */