#include "modem_services.h"

void send_sms(const char* str){
    
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

