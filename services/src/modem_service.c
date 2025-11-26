#include "modem_service.h"

/* ====================================== DECLARATIONS ======================================= */
modem_service_state_t modem_state = MODEM_STATE_IDLE;
static modem_state_flag_t flag_state;

/*<! STATE HANDLE FUNCTIONS DECLARATIONS */                             
static void modem_state_idle_process(void);                                             /*<! IDLE */
static void modem_state_power_on_process(void);                                         /*<! POWER ON */
static void modem_state_power_off_process(void);                                        /*<! POWER OFF */
static void modem_state_sync_at_process(void);                                          /*<! SYNC AT */
static void modem_state_config_process(void);                                           /*<! CONFIG */
static void modem_state_check_sim_process(void);                                        /*<! CHECK SIM */
static void modem_state_check_reg_process(void);                                        /*<! CHECK REG */
static void modem_state_attach_psd_process(void);                                       /*<! ATTACH PSD */
static void modem_state_activate_pdp_process(void);                                     /*<! ACTIVATE PDP */
static void modem_state_ready_process(void);                                            /*<! READY */
static void modem_state_error_process(void);                                            /*<! ERROR */

/*<! POWER CONTROL FUNCTIONS */                             
static bool modem_power(modem_power_t state);                                           /*<! CONTROL POWER */
static bool modem_power_on(void);                                                       /*<! POWER ON */
static bool modem_power_off(void);                                                      /*<! POWER OFF */
static bool modem_power_reset(void);                                                    /*<! POWER RESET */
static void check_at(gsm_callback callback); // <---                                    

/*<! CALLBACK FOR AT SYNC */
static void at_callback(respon_status_t s, const char* line, uint32_t len);             /*<! AT_CALLBACK */

/*<! CALLBACK FOR CONFIG STATE */
static void ate0_callback(respon_status_t s, const char* line, uint32_t len);           /*<! ATE0 CALLBACK */

/*<! CALLBACK FOR CHECK SIM STATE*/
static void cpin_callback(respon_status_t s, const char* line, uint32_t len);           /*<! CPIN CALLBACK */

static void cereg_callback(respon_status_t s, const char* line, uint32_t len);          /*<! CEREG CALLBACK */
static void cgatt_callback(respon_status_t s, const char* line, uint32_t len);          /*<! CGATT CALLBACK */
static void cgact_callback(respon_status_t s, const char* line, uint32_t len);          /*<! CGACT CALLBACK */
/* ============================================================================================= */



/**
 * @brief   Power off modem
 */
bool modem_power_off(void){
    static uint8_t step = 0;
    static uint32_t t0 = 0;

    switch (step)
    {
        case 0:
            SIM_PWKEY_LOW();
            t0 = get_systick_ms();
            step = 1;
            DEBUG_PRINT("Step 0, PWKEY LOW 2.5s\r\n");
            return false;

        case 1:
            if (get_systick_ms() - t0 < 2500)
                return false;
            SIM_PWKEY_HIGH();
            t0 = get_systick_ms();
            step = 2;
            DEBUG_PRINT("Step 1, PWKEY HIGH about 8.9s\r\n");
            return false;

        case 2:
            if (!SIM_STATUS_READ()){
                DEBUG_PRINT("STATUS is LOW, MODEM is now OFFLINE\r\n");
                step = 0;
                return true;
            }
            if (get_systick_ms() - t0 > 8900) {
                step = 0;                      /*<! TIMEOUT reset step */
                DEBUG_PRINT("TIMEOUT, reset to step 0\r\n");
                return false;
            }
            DEBUG_PRINT("Waiting STATUS\r\n");
            return false;
        
        default:
            break;
    }
    return false;
}


/**
 * @brief   Power on modem
 * 
 * @note    
 */
bool modem_power_on(void)
{
    static uint8_t step = 0;
    static uint32_t t0 = 0;

    switch (step)
    {
        case 0:
            SIM_PWKEY_LOW();
            t0 = get_systick_ms();
            step = 1;
            DEBUG_PRINT("Step 0, PWKEY LOW 50ms\r\n");
            return false;

        case 1:
            if (get_systick_ms() - t0 < 50)
                return false;
            SIM_PWKEY_HIGH();
            t0 = get_systick_ms();
            step = 2;
            DEBUG_PRINT("Step 1, PWKEY HIGH about 8.5s\r\n");
            return false;

        case 2:
            if (SIM_STATUS_READ()){
                DEBUG_PRINT("STATUS is HIGH, MODEM is now ONLINE\r\n");
                step = 0;
                return true;
            }
            if (get_systick_ms() - t0 > 8500) {
                step = 0;                      /*<! TIMEOUT reset step */
                DEBUG_PRINT("TIMEOUT, reset to step 0\r\n");
                return false;
            }
            DEBUG_PRINT("Waiting STATUS\r\n");
            return false;
        
        default:
            break;
    }
    return false;
}



/**
 * @brief   Power reset modem
 */
bool modem_power_reset(void){

}


/**
 * @brief   Set power for modem
 * 
 * @param   state: Power mode that you want to set ON/OFF/RESET
 * 
 * @retval  True if success
 * @retval  False if failed
 */
bool modem_power(modem_power_t state){
    switch (state)
    {
        case MODEM_PWR_OFF:
            return modem_power_off();

        case MODEM_PWR_ON:
            return modem_power_on();

        default:
            return false;
    }
}


/**
 * @brief   Handle which state should do fisrt
 * 
 * @note    This function use if statements to handle and compare state flag
 * 
 * @note    For exemple if on_flag isnt on, the state idle will switch 
 *          current state to POWER_ON_STATE
 */
static void modem_state_idle_process(void)
{
    /*<! STATE: POWER_ON */
    if (!flag_state.power_on) {
        modem_state = MODEM_STATE_POWER_ON;
        return;
    }

    /*<! STATE: SYNC_AT */
    if (!flag_state.at_ready) {
        modem_state = MODEM_STATE_SYNC_AT;
        return;
    }

    /*<! STATE: CONFIG */
    if (!flag_state.config_ready) {
        modem_state = MODEM_STATE_CONFIG;
        return;
    }

    /*<! STATE: CHECK_SIM */
    if (!flag_state.sim_ready) {
        modem_state = MODEM_STATE_CHECK_SIM;
        return;
    }

    /*<! STATE: CHECK_REG */
    if (!flag_state.reg_ready) {
        modem_state = MODEM_STATE_CHECK_REG;
        return;
    }

    /*<! STATE: ATTACH_PSD */
    if (!flag_state.psd_attached) {
        modem_state = MODEM_STATE_ATTACH_PSD;
        return;
    }

    /*<! STATE: ACTIVATE_PDP */
    if (!flag_state.pdp_active) {
        modem_state = MODEM_STATE_ACTIVATE_PDP;
        return;
    }

    /*<! STATE: READY */
    modem_state = MODEM_STATE_READY;
}


/**
 * @brief   Power on function
 */
static void modem_state_power_on_process(void){
    bool tmp;
    tmp = modem_power(MODEM_PWR_ON);
    if (tmp){
        modem_state = MODEM_STATE_SYNC_AT;
    }
}

/**
 * @brief  Power off function
 */
static void modem_state_power_off_process(void){
    bool tmp;
    tmp = modem_power(MODEM_PWR_OFF);
    if (tmp){
        modem_state = MODEM_STATE_IDLE;
    }
}

/**
 * @brief  Try send AT command
 */
static void modem_state_sync_at_process(void){
    modem_at_cmd_t cmd = {
        .cmd = "AT",
        .expect = "",
        .timeout_ms = 10000,
        .start_tick = get_systick_ms(),
        .cb = at_callback
    };
    modem_send_at_cmd(cmd);   
}

/**
 * @brief   Callback of sent AT command
 */
static void at_callback(respon_status_t resp_status ,const char* str, uint32_t len){
    switch (resp_status){
    case OK_RESP:
        DEBUG_PRINT("AT is OK\r\n");
        flag_state.at_ready = true;
        break;

    case ERROR_RESP:
        DEBUG_PRINT("AT is NOT OK\r\n");
        break;

    case TIMEOUT_RESP:
        DEBUG_PRINT("AT is OUT of TIME\r\n");
        break;
    }
}

/**
 * @brief   Config SIM module function
 */
static void modem_state_config_process(void){
    
}
/**
 * @brief   Callback of config module SIM
 */
// static void 

/*<! STATE: CHECK_SIM */
static void modem_state_check_sim_process(void)
{
    /* TODO: implement CHECK_SIM state */
}

/*<! STATE: CHECK_REG */
static void modem_state_check_reg_process(void)
{
    /* TODO: implement CHECK_REG state */
}

/*<! STATE: ATTACH_PSD */
static void modem_state_attach_psd_process(void)
{
    /* TODO: implement ATTACH_PSD state */
}

/*<! STATE: ACTIVATE_PDP */
static void modem_state_activate_pdp_process(void)
{
    /* TODO: implement ACTIVATE_PDP state */
}

/*<! STATE: READY */
static void modem_state_ready_process(void)
{
    /* TODO: implement READY state */
}

/*<! STATE: ERROR */
static void modem_state_error_process(void)
{
    /* TODO: implement ERROR state */
}


/**
 * @brief   STATE switch case 
 */
void modem_service_process(void){
    bool tmp;
    switch (modem_state)
    {
    case MODEM_STATE_IDLE:
        modem_state_idle_process();
        break;

    case MODEM_STATE_POWER_ON:
        modem_state_power_on_process();
        break;

    case MODEM_STATE_POWER_OFF:
        modem_state_power_off_process();
        break;

    case MODEM_STATE_SYNC_AT:
        modem_state_sync_at_process();
        break;

    case MODEM_STATE_CONFIG:
        modem_state_config_process();
        break;

    case MODEM_STATE_CHECK_SIM:
        modem_state_check_sim_process();
        break;

    case MODEM_STATE_CHECK_REG:
        modem_state_check_reg_process();
        break;

    case MODEM_STATE_ATTACH_PSD:
        modem_state_attach_psd_process();
        break;

    case MODEM_STATE_ACTIVATE_PDP:
        modem_state_activate_pdp_process();
        break;

    case MODEM_STATE_READY:
        modem_state_ready_process();
        break;

    case MODEM_STATE_ERROR:
        modem_state_error_process();
        break;

    default:
        break;
    }
}

/**
 * @brief   Call init modem in middleware
 */
void modem_init_service(void){
    modem_init_md();
}