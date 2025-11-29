#include "modem_service_callback_sms.h"

extern sms_event_queue_t sms_event_queue;
extern sms_state_t sms_state;

void sms_cmgf_callback(respon_status_t resp_status, const char *str,  uint32_t len){
    switch (resp_status){
    case OK_RESP:
        DEBUG_PRINT("CMGF SUCCESS\r\n");
        DEBUG_PRINT(str);
        sms_push_event(&sms_event_queue, SMS_EVT_OK);
        break;

    case ERROR_RESP:
        DEBUG_PRINT("CMGF FAIL\r\n");
        DEBUG_PRINT(str);
                sms_push_event(&sms_event_queue, SMS_EVT_ERROR);
        break;

    case TIMEOUT_RESP:
        DEBUG_PRINT("SEND TIMEOUT\r\n");
        sms_push_event(&sms_event_queue, SMS_EVT_TIMEOUT);
        break;
    }
}

void sms_cscs_callback(respon_status_t resp_status, const char *str, uint32_t len){
    switch (resp_status){
    case OK_RESP:
        DEBUG_PRINT("ENCODING SUCCESS\r\n");
        DEBUG_PRINT(str);
        sms_push_event(&sms_event_queue, SMS_EVT_OK);
        break;

    case ERROR_RESP:
        DEBUG_PRINT("ENCODING FAIL\r\n");
        DEBUG_PRINT(str);
        sms_push_event(&sms_event_queue, SMS_EVT_ERROR);
        break;

    case TIMEOUT_RESP:
        DEBUG_PRINT("ENCODING TIMEOUT\r\n");
        sms_push_event(&sms_event_queue, SMS_EVT_TIMEOUT);
        break;
    }
}

void sms_cmgs_callback(respon_status_t resp_status, const char *str, uint32_t len){

    switch (resp_status)
    {
        case INTERMEDIATE:
            // PROMPT '>'
            if (str[0] == '>' ) {
                DEBUG_PRINT("CMGS PROMPT >\r\n");
                sms_push_event(&sms_event_queue, SMS_EVT_PROMPT);
            } 
            break;

        case OK_RESP:
            DEBUG_PRINT("SMS SENT SUCCESS\r\n");
            DEBUG_PRINT(str);
            sms_push_event(&sms_event_queue, SMS_EVT_SENT);
            break;

        case ERROR_RESP:
            DEBUG_PRINT("SMS SENT FAIL\r\n");
            DEBUG_PRINT(str);
            DEBUG_PRINT("\r\n");
            sms_push_event(&sms_event_queue, SMS_EVT_FAILED);
            break;

        case TIMEOUT_RESP:
            DEBUG_PRINT("SMS SENT TIMEOUT\r\n");
            sms_push_event(&sms_event_queue, SMS_EVT_TIMEOUT);
            break;

        default:
            break;
    }
}


void sms_wait_result_callback(respon_status_t resp_status, const char *str, uint32_t len){
    switch (resp_status)
    {
        case INTERMEDIATE:
            if (strncmp(str, "+CMGS:", 6) == 0) {
                DEBUG_PRINT("CMGS REF RECEIVED: ");
                DEBUG_PRINT(str);
                DEBUG_PRINT("\r\n");
            }
            break;

        case OK_RESP:
            DEBUG_PRINT("SMS SENT SUCCESS\r\n");
            DEBUG_PRINT(str);
            sms_push_event(&sms_event_queue, SMS_EVT_SENT);
            break;

        case ERROR_RESP:
            DEBUG_PRINT("SMS SENT FAIL\r\n");
            DEBUG_PRINT(str);
            DEBUG_PRINT("\r\n");
            sms_push_event(&sms_event_queue, SMS_EVT_FAILED);
            break;

        case TIMEOUT_RESP:
            DEBUG_PRINT("SMS SEND TIMEOUT\r\n");
            sms_push_event(&sms_event_queue, SMS_EVT_TIMEOUT);
            break;
        default:
            break;
    }
}

void sms_read_sms_callback(respon_status_t resp_status, const char *str, uint32_t len){
    static char sms_body[255];
    switch (resp_status)
    {
        case PRIMARY:
            DEBUG_PRINT(str);
            sms_push_event(&sms_event_queue, SMS_EVT_READ);
            break;

        case INTERMEDIATE:  
            strncpy(sms_body, str, sizeof(sms_body)-1);
            sms_body[sizeof(sms_body)-1] = '\0';

            DEBUG_PRINT("SMS BODY: ");
            DEBUG_PRINT(sms_body);
            DEBUG_PRINT("\r\n");
            break;

        case OK_RESP:
            DEBUG_PRINT("READ SUCCESS\r\n");
            DEBUG_PRINT(str);
            sms_push_event(&sms_event_queue, SMS_EVT_READ_DONE);
            break;

        case ERROR_RESP:
            DEBUG_PRINT("READ FAIL\r\n");
            DEBUG_PRINT(str);
            DEBUG_PRINT("\r\n");
            sms_push_event(&sms_event_queue, SMS_EVT_READ_ERR);
            break;

        case TIMEOUT_RESP:
            DEBUG_PRINT("TIMEOUT\r\n");
            sms_push_event(&sms_event_queue, SMS_EVT_TIMEOUT);
            break;
        default:
            break;
    }
}
