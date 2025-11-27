#include "modem_service.h"

/**
 * @brief   Call send sms logic in fsm_state
 * 
 * @param   str: Message that you want to send
 */
void send_sms(const char* str){
    
}

/**
 * @brief   Check if modem state is ready
 * 
 * @retval  True if is ready
 * @retval  False if isnt ready
 */
bool service_is_ready(void){
    if (get_current_services_state() == MODEM_STATE_READY)
        return true;
    return false;
}

/**
 * @brief   Call FSM of the service
 */
void modem_service_process(void){
    modem_service_fsm_process();
}

/**
 * @brief   Call init modem in middleware
 */
void modem_init_service(void){
    modem_init_md();
}

