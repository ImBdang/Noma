#ifndef __MODEM_FSM_S__
#define __MODEM_FSM_S__


#include "modem_api.h"
#include "stdbool.h"
#include "systick_api.h"
#include "debug.h"
#include "event_driven.h"
#include "modem_service_callback.h"

/* ==================================== TYPEDEF STRUCT ========================================= */

typedef enum {
    MODEM_PWR_OFF,
    MODEM_PWR_ON,
    MODEM_RESET,
} modem_power_t;

typedef struct {
    bool power_on;          /*<! POWER_ON state flag */
    bool at_ready;          /*<! SYNC_AT state flag */
    bool config_ready;      /*<! CONFIG state flag */
    bool sim_ready;         /*<! CHECK_SIM state flag */
    bool reg_ready;         /*<! CHECK_REG state flag */
    bool psd_attached;      /*<! ATTACH_PSD state flag */
    bool pdp_active;        /*<! ACTIVATE_PDP state flag */
    bool error_happen;      /*<! ERROR HAPPENED */
} modem_state_flag_t;

typedef enum {
    MODEM_STATE_IDLE = 0,       

    MODEM_STATE_POWER_ON,   

    MODEM_STATE_POWER_OFF,

    MODEM_STATE_SYNC_AT_ENTRY,
    MODEM_STATE_WAIT_SYNC_AT,

    MODEM_STATE_CONFIG_ENTRY,
    MODEM_STATE_WAIT_CONFIG,

    MODEM_STATE_CHECK_SIM_ENTRY,  
    MODEM_STATE_WAIT_CHECK_SIM,

    MODEM_STATE_CHECK_REG_ENTRY,  
    MODEM_STATE_WAIT_CHECK_REG,

    MODEM_STATE_ATTACH_PSD_ENTRY,    
    MODEM_STATE_WAIT_ATTACH_PSD, 
    
    MODEM_STATE_ACTIVATE_PDP_ENTRY,    
    MODEM_STATE_WAIT_ACTIVATE_PDP,

    MODEM_STATE_READY,          
    MODEM_STATE_ERROR           
} modem_service_state_t;

typedef enum {
    CONFIG_ECHO,
    CONFIG_CFUN1_CODE,
    CONFIG_DONE
} modem_config_state_t;


/* ==================================== API DECLARATION ========================================== */
modem_service_state_t get_current_services_state(void);
void modem_service_fsm_process(void);

#endif /* __MODEM_FSM_S__ */