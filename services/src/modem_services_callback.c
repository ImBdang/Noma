#include "modem_services_callback.h"

extern modem_event_queue_t event_queue;

void modem_sync_at_callback(respon_status_t resp_status, const char* line, uint32_t len){
    switch (resp_status){
    case OK_RESP:
        DEBUG_PRINT("AT is OK\r\n");
        push_event(&event_queue, EVT_OK);
        break;

    case ERROR_RESP:
        DEBUG_PRINT("AT FAILED\r\n");
        push_event(&event_queue, EVT_ERROR);
        break;

    case TIMEOUT_RESP:
        DEBUG_PRINT("AT TIMEOUT\r\n");
        push_event(&event_queue, EVT_TIMEOUT);
        break;
    }
}            

void modem_config_ate0_callback(respon_status_t resp_status ,const char* line, uint32_t len) {
    switch (resp_status){
    case OK_RESP:
        DEBUG_PRINT("ATE0 DONE\r\n");
        push_event(&event_queue, EVT_OK);
        break;

    case ERROR_RESP:
        DEBUG_PRINT("ATE0 FAILED\r\n");
        push_event(&event_queue, EVT_ERROR);
        break;

    case TIMEOUT_RESP:
        DEBUG_PRINT("ATE0 TIMEOUT\r\n");
        push_event(&event_queue, EVT_TIMEOUT);
        break;
    }
};



void modem_check_sim_cpin_callback(respon_status_t status, const char* line, uint32_t len){
    
    static sim_state_t sim_state = SIM_STATE_NOT_RDY;
    char value[16];

    switch (status)
    {
    case PRIMARY:
        primary_dispatch(line, value);

        if (strstr(value, "READY"))
            sim_state = SIM_STATE_READY;

        else if (strstr(value, "SIM PIN"))
            sim_state = SIM_STATE_PIN;

        else if (strstr(value, "SIM PUK"))
            sim_state = SIM_STATE_PUK;

        else
            sim_state = SIM_STATE_NOT_RDY;

        break;

    case OK_RESP:
        switch (sim_state)
        {
        case SIM_STATE_READY:
            push_event(&event_queue, EVT_CPIN_READY);
            break;

        case SIM_STATE_PIN:
            push_event(&event_queue, EVT_CPIN_PIN);
            break;

        case SIM_STATE_PUK:
            push_event(&event_queue, EVT_CPIN_PUK);
            break;

        default:
            push_event(&event_queue, EVT_ERROR);
            break;
        }

        sim_state = SIM_STATE_NOT_RDY;
        break;

    case ERROR_RESP:
        push_event(&event_queue, EVT_ERROR);
        sim_state = SIM_STATE_NOT_RDY;    
        break;

    case TIMEOUT_RESP:
        push_event(&event_queue, EVT_TIMEOUT);
        sim_state = SIM_STATE_NOT_RDY;     
        break;
    }
}


void modem_check_sim_cpin_callback(respon_status_t s, const char* line, uint32_t len) {

};

void modem_check_reg_callback(respon_status_t s, const char* line, uint32_t len) {};

void modem_attach_psd_cgatt_callback(respon_status_t s, const char* line, uint32_t len) {};

void modem_activate_pdp_cgact_callback(respon_status_t s, const char* line, uint32_t len) {};