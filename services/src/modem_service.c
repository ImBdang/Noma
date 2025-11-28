#include "modem_service.h"

modem_event_queue_t event_queue; 
sms_event_queue_t sms_event_queue;
urc_event_queue_t urc_event_queue;

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
    modem_process();
    modem_service_fsm_process();
    modem_service_urc_process();
}

/**
 * @brief   Call init modem in middleware
 */
void modem_init_service(void){
    event_queue_init(&event_queue);
    sms_event_queue_init(&sms_event_queue);
    urc_event_queue_init(&urc_event_queue);
    modem_init_md();
}

