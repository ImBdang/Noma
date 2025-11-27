#include "modem_service_fsm.h"

/* ====================================== DECLARATIONS ======================================= */
modem_service_state_t modem_state = MODEM_STATE_IDLE;
modem_event_queue_t event_queue;      

static modem_config_state_t step_config = CONFIG_ECHO;
static modem_state_flag_t flag_state;

static void clear_flag_state(void);

/*<! STATE HANDLE FUNCTIONS DECLARATIONS */                             
/* IDLE */
static void modem_state_idle_process(void);

/* SYNC AT */
static bool modem_state_sync_at_entry(void);
static void modem_state_wait_sync_at(void);

/* CONFIG */
static bool modem_state_config_entry(void);
static void modem_state_wait_config(void);

/* CHECK SIM */
static bool modem_state_check_sim_entry(void);
static void modem_state_wait_check_sim(void);

/* CHECK REG */
static bool modem_state_check_reg_entry(void);
static void modem_state_wait_check_reg(void);

/* ATTACH PSD */
static bool modem_state_attach_psd_entry(void);
static void modem_state_wait_attach_psd(void);

/* ACTIVATE PDP */
static bool modem_state_activate_pdp_entry(void);
static void modem_state_wait_activate_pdp(void);

/* READY / ERROR */
static void modem_state_ready(void);
static void modem_state_error(void);


/*<! POWER CONTROL FUNCTIONS */                             
static bool modem_power(modem_power_t state);                                           /*<! CONTROL POWER */
static bool modem_power_on(void);                                                       /*<! POWER ON */
static bool modem_power_off(void);                                                      /*<! POWER OFF */
static bool modem_power_reset(void);                                                    /*<! POWER RESET */
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
    modem_power_off();
    delay_ms(50);
    modem_power_on();
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
    /*<! STATE: ERROR */
    if (flag_state.error_happen){
        modem_state = MODEM_STATE_ERROR;
        return;
    }

    /*<! STATE: POWER_ON */
    if (!flag_state.power_on) {
        modem_state = MODEM_STATE_POWER_ON;
        return;
    }

    /*<! STATE: SYNC_AT */
    if (!flag_state.at_ready) {
        modem_state = MODEM_STATE_SYNC_AT_ENTRY;
        return;
    }

    /*<! STATE: CONFIG */
    if (!flag_state.config_ready) {
        modem_state = MODEM_STATE_CONFIG_ENTRY;
        return;
    }

    /*<! STATE: CHECK_SIM */
    if (!flag_state.sim_ready) {
        modem_state = MODEM_STATE_CHECK_SIM_ENTRY;
        return;
    }

    /*<! STATE: CHECK_REG */
    if (!flag_state.reg_ready) {
        modem_state = MODEM_STATE_CHECK_REG_ENTRY;
        return;
    }

    /*<! STATE: ATTACH_PSD */
    // if (!flag_state.psd_attached) {
    //     modem_state = MODEM_STATE_ATTACH_PSD_ENTRY;
    //     return;
    // }

    /*<! STATE: ACTIVATE_PDP */
    // if (!flag_state.pdp_active) {
    //     modem_state = MODEM_STATE_ACTIVATE_PDP_ENTRY;
    //     return;
    // }

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
        modem_state = MODEM_STATE_IDLE;
        flag_state.power_on = true;
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


/* ====================================== SYNC AT ======================================= */
/**
 * @brief   ENTRY send AT command
 */
static bool modem_state_sync_at_entry(void){
    modem_at_cmd_t cmd = {
        .cmd = "AT",
        .expect = "",
        .timeout_ms = 10000,
        .start_tick = get_systick_ms(),
        .cb = modem_sync_at_callback
    };
    return modem_send_at_cmd(cmd);  
}

/**
 * @brief   Wait for SYNC AT ENTRY
 */
static void modem_state_wait_sync_at(void){
    modem_event_t evt;
    static uint8_t timeout_count = 0;
    static uint8_t max_timeout = 3;
    if (!pop_event(&event_queue, &evt))
        return;

    switch (evt)
    {
        case EVT_OK:
            timeout_count = 0; 
            flag_state.at_ready = true;
            modem_state = MODEM_STATE_IDLE;
            break;


        case EVT_TIMEOUT:
            flag_state.at_ready = false;
            timeout_count++;
            if (timeout_count >= max_timeout){
                flag_state.error_happen = true;
                timeout_count = 0;
            }
            modem_state = MODEM_STATE_IDLE;
            break;
        

        case EVT_ERROR:
            timeout_count = 0;
            flag_state.error_happen = true;
            modem_state = MODEM_STATE_IDLE;
            break;


        default:
            modem_state = MODEM_STATE_IDLE;
            break;
    }
}


/* ====================================== CONFIG ======================================= */
static bool modem_state_config_entry(void) {
    modem_at_cmd_t cmd ;
    switch (step_config)
    {
        case CONFIG_ECHO:
        {
            modem_at_cmd_t cmd = {
                .cmd = "ATE0",
                .expect = "",
                .timeout_ms = 5000,
                .start_tick = get_systick_ms(),
                .cb = modem_config_ate0_callback
            };
            return modem_send_at_cmd(cmd);
        }

        case CONFIG_CFUN1_CODE:
        {
            modem_at_cmd_t cmd = {
                .cmd = "AT+CFUN=1",
                .expect = "",
                .timeout_ms = 5000,
                .start_tick = get_systick_ms(),
                .cb = modem_config_cfun1_callback
            };
            return modem_send_at_cmd(cmd);
        }

        default:
            return false;
    }
};

static void modem_state_wait_config(void) {
    modem_event_t evt;
    static uint8_t timeout_count = 0;
    static uint8_t max_timeout = 3;
    if (!pop_event(&event_queue, &evt))
        return;

    switch (evt)
    {
        case EVT_OK:
            timeout_count = 0; 
            step_config++;
            if (step_config >= CONFIG_DONE){
                flag_state.config_ready = true;
            }
            modem_state = MODEM_STATE_IDLE;
            break;


        case EVT_TIMEOUT:
            flag_state.config_ready = false;
            timeout_count++;
            if (timeout_count >= max_timeout){
                flag_state.error_happen = true;
                timeout_count = 0;
            }
            modem_state = MODEM_STATE_IDLE;
            break;
        

        case EVT_ERROR:
            timeout_count = 0;
            flag_state.error_happen = true;
            modem_state = MODEM_STATE_IDLE;
            break;


        default:
            modem_state = MODEM_STATE_IDLE;
            break;
    }
};

/* ====================================== CHECK SIM ======================================= */
static bool modem_state_check_sim_entry(void) {
    modem_at_cmd_t cmd = {
        .cmd = "AT+CPIN?",
        .expect = "+CPIN:",
        .timeout_ms = 10000,
        .start_tick = get_systick_ms(),
        .cb = modem_check_sim_cpin_callback
    };
    return modem_send_at_cmd(cmd);  
};
static void modem_state_wait_check_sim(void) {
    modem_event_t evt;
    static uint8_t timeout_count = 0;
    static uint8_t max_timeout = 3;
    if (!pop_event(&event_queue, &evt))
        return;

    switch (evt)
    {
        case EVT_CPIN_READY:
            timeout_count = 0; 
            flag_state.sim_ready = true;
            modem_state = MODEM_STATE_IDLE;
            break;

        case EVT_CPIN_PIN:
            timeout_count = 0; 
            flag_state.sim_ready = false;
            modem_state = MODEM_STATE_IDLE;
            DEBUG_PRINT("SIM LOCKED, ENTER PIN TO UNLOCK\r\n");
            break;

        case EVT_CPIN_PUK:
            timeout_count = 0; 
            flag_state.sim_ready = false;
            modem_state = MODEM_STATE_IDLE;
            DEBUG_PRINT("SIM PUK, HAVE NO IDEA WITH THIS STATE\r\n");
            break;

        case EVT_TIMEOUT:
            flag_state.sim_ready = false;
            timeout_count++;
            if (timeout_count >= max_timeout){
                flag_state.error_happen = true;
                timeout_count = 0;
            }
            modem_state = MODEM_STATE_IDLE;
            break;
        

        case EVT_ERROR:
            timeout_count = 0;
            flag_state.error_happen = true;
            modem_state = MODEM_STATE_IDLE;
            break;


        default:
            modem_state = MODEM_STATE_IDLE;
            break;
    }
};


/* ====================================== CHECK REG ======================================= */
static bool modem_state_check_reg_entry(void) {
    modem_at_cmd_t cmd = {
        .cmd = "AT+CREG?",
        .expect = "+CREG:",
        .timeout_ms = 10000,
        .start_tick = get_systick_ms(),
        .cb = modem_check_reg_callback
    };
    return modem_send_at_cmd(cmd);
};

static void modem_state_wait_check_reg(void) {
    modem_event_t evt;
    static uint8_t timeout_count = 0;
    static const uint8_t max_timeout = 3;

    if (!pop_event(&event_queue, &evt))
        return;

    switch (evt)
    {
        case EVT_CREG_REGISTERED_HOME:
            timeout_count = 0;
            flag_state.reg_ready = true;
            modem_state = MODEM_STATE_IDLE;
            break;

        case EVT_CREG_REGISTERED_ROAMING:
            timeout_count = 0;
            flag_state.reg_ready = true;
            modem_state = MODEM_STATE_IDLE;
            break;

        case EVT_CREG_NOT_REGISTERED:
        case EVT_CREG_SEARCHING:
        case EVT_CREG_REG_DENIED:
        case EVT_CREG_UNKNOWN:
        case EVT_CREG_REGISTERED_SMS:
        case EVT_CREG_REGISTERED_SMS_ROAM:
            timeout_count = 0;
            flag_state.reg_ready = false;
            modem_state = MODEM_STATE_IDLE;
            break;

        case EVT_TIMEOUT:
            flag_state.reg_ready = false;
            timeout_count++;

            if (timeout_count >= max_timeout) {
                flag_state.error_happen = true;
                timeout_count = 0;
            }

            modem_state = MODEM_STATE_IDLE;
            break;

        case EVT_ERROR:
            timeout_count = 0;
            flag_state.error_happen = true;
            modem_state = MODEM_STATE_IDLE;
            break;

        default:
            modem_state = MODEM_STATE_IDLE;
            break;
    }
}


/* ATTACH PSD */
static bool modem_state_attach_psd_entry(void) {};
static void modem_state_wait_attach_psd(void) {};

/* ACTIVATE PDP */
static bool modem_state_activate_pdp_entry(void) {};
static void modem_state_wait_activate_pdp(void) {};

/* READY / ERROR */
static void modem_state_ready(void) {};
static void modem_state_error(void) {};


/**
 * @brief   STATE switch case 
 */
void modem_service_fsm_process(void)
{
    bool tmp = false;
    switch (modem_state)
    {
    /*===========================================
     *                IDLE
     *===========================================*/
    case MODEM_STATE_IDLE:
        modem_state_idle_process();
        break;


    /*===========================================
     *              POWER ON
     *===========================================*/
    case MODEM_STATE_POWER_ON:
        modem_state_power_on_process();
        break;


    /*===========================================
     *              POWER OFF
     *===========================================*/
    case MODEM_STATE_POWER_OFF:
        modem_state_power_off_process();
        break;


    /*===========================================
     *              SYNC AT
     *===========================================*/
    case MODEM_STATE_SYNC_AT_ENTRY:
        tmp = modem_state_sync_at_entry();
        if (tmp){
            modem_state = MODEM_STATE_WAIT_SYNC_AT;
        }
        break;

    case MODEM_STATE_WAIT_SYNC_AT:
        modem_state_wait_sync_at();
        break;


    /*===========================================
     *                CONFIG
     *===========================================*/
    case MODEM_STATE_CONFIG_ENTRY:
        tmp = modem_state_config_entry();
        if (tmp){
            modem_state = MODEM_STATE_WAIT_CONFIG;
        }
        break;

    case MODEM_STATE_WAIT_CONFIG:
        modem_state_wait_config();
        break;


    /*===========================================
     *              CHECK SIM
     *===========================================*/
    case MODEM_STATE_CHECK_SIM_ENTRY:
        tmp = modem_state_check_sim_entry();
        if (tmp){
            modem_state = MODEM_STATE_WAIT_CHECK_SIM;
        }
        break;

    case MODEM_STATE_WAIT_CHECK_SIM:
        modem_state_wait_check_sim();
        break;


    /*===========================================
     *              CHECK REG
     *===========================================*/
    case MODEM_STATE_CHECK_REG_ENTRY:
        tmp = modem_state_check_reg_entry();
        if (tmp){
            modem_state = MODEM_STATE_WAIT_CHECK_REG;
        }
        break;

    case MODEM_STATE_WAIT_CHECK_REG:
        modem_state_wait_check_reg();
        break;


    /*===========================================
     *              ATTACH PSD
     *===========================================*/
    // case MODEM_STATE_ATTACH_PSD_ENTRY:
    //     modem_state_attach_psd_entry();
    //     break;

    // case MODEM_STATE_WAIT_ATTACH_PSD:
    //     modem_state_wait_attach_psd();
    //     break;


    /*===========================================
     *             ACTIVATE PDP
     *===========================================*/
    // case MODEM_STATE_ACTIVATE_PDP_ENTRY:
    //     modem_state_activate_pdp_entry();
    //     break;

    // case MODEM_STATE_WAIT_ACTIVATE_PDP:
    //     modem_state_wait_activate_pdp();
    //     break;


    /*===========================================
     *              READY / ERROR
     *===========================================*/
    case MODEM_STATE_READY:
        modem_state_ready();
        break;

    case MODEM_STATE_ERROR:
        // modem_state_error();
        clear_flag_state();
        modem_power_reset();
        break;


    /*===========================================*/
    default:
        break;
    }
}

/*<! Clear all flag state */
static void clear_flag_state(void){
    memset(&flag_state, 0, sizeof(flag_state));
    step_config = CONFIG_ECHO;
}


modem_service_state_t get_current_services_state(void){
    return modem_state;
}