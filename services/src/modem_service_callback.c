#include "modem_service_callback.h"

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

void modem_config_ate0_callback(respon_status_t resp_status ,const char* str, uint32_t len) {
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

void modem_config_cfun1_callback(respon_status_t resp_status, const char* line, uint32_t len) {
    switch (resp_status){
    case OK_RESP:
        DEBUG_PRINT("CFUN=1 DONE\r\n");
        push_event(&event_queue, EVT_OK);
        break;

    case ERROR_RESP:
        DEBUG_PRINT("CFUN=1 FAILED\r\n");
        push_event(&event_queue, EVT_ERROR);
        break;

    case TIMEOUT_RESP:
        DEBUG_PRINT("CFUN=1 TIMEOUT\r\n");
        push_event(&event_queue, EVT_TIMEOUT);
        break;
    }
}

void modem_check_sim_cpin_callback(respon_status_t resp_status, const char* line, uint32_t len){

    static sim_state_t sim_state = SIM_STATE_NOT_RDY;
    char value[16];

    switch (resp_status)
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

void modem_check_reg_callback(respon_status_t resp_status, const char* line, uint32_t len) {
    static network_state_t network_state = NET_NOT_REGISTERED;
    char value[16];
    switch (resp_status)
    {
    case PRIMARY:
        primary_dispatch(line, value);

        if (strstr(value, "0,0"))
            network_state = NET_NOT_REGISTERED;

        else if (strstr(value, "0,1"))
            network_state = NET_REGISTERED_HOME;

        else if (strstr(value, "0,2"))
            network_state = NET_SEARCHING;

        else if (strstr(value, "0,3"))
            network_state = NET_REG_DENIED;

        else if (strstr(value, "0,4"))
            network_state = NET_UNKNOWN;

        else if (strstr(value, "0,5"))
            network_state = NET_REGISTERED_ROAMING;

        else if (strstr(value, "0,6"))
            network_state = NET_REGISTERED_SMS;

        else if (strstr(value, "0,7"))
            network_state = NET_REGISTERED_SMS_ROAM;

        break;

    case OK_RESP:
        switch (network_state)
        {
        case NET_NOT_REGISTERED:
            push_event(&event_queue, EVT_CREG_NOT_REGISTERED);
            break;

        case NET_REGISTERED_HOME:
            push_event(&event_queue, EVT_CREG_REGISTERED_HOME);
            break;

        case NET_SEARCHING:
            push_event(&event_queue, EVT_CREG_SEARCHING);
            break;

        case NET_REG_DENIED:
            push_event(&event_queue, EVT_CREG_REG_DENIED);
            break;

        case NET_UNKNOWN:
            push_event(&event_queue, EVT_CREG_UNKNOWN);
            break;

        case NET_REGISTERED_ROAMING:
            push_event(&event_queue, EVT_CREG_REGISTERED_ROAMING);
            break;

        case NET_REGISTERED_SMS:
            push_event(&event_queue, EVT_CREG_REGISTERED_SMS);
            break;

        case NET_REGISTERED_SMS_ROAM:
            push_event(&event_queue, EVT_CREG_REGISTERED_SMS_ROAM);
            break;

        default:
            push_event(&event_queue, EVT_ERROR);
            break;
        }
        network_state = NET_NOT_REGISTERED;
        break;

    case ERROR_RESP:
        push_event(&event_queue, EVT_ERROR);
        network_state = NET_NOT_REGISTERED;   
        break;

    case TIMEOUT_RESP:
        push_event(&event_queue, EVT_TIMEOUT);
        network_state = NET_NOT_REGISTERED;    
        break;
    }
};

void modem_attach_psd_cgatt_callback(respon_status_t resp_status, const char* line, uint32_t len) {};

void modem_activate_pdp_cgact_callback(respon_status_t resp_status, const char* line, uint32_t len) {};