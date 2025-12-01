#include "modem_urc_api.h"

/* ====================================== DECLARATIONS ======================================= */
extern urc_event_queue_t urc_event_queue;
/* =========================================================================================== */
void modem_urc_make_event(const char *urc){
    
    if (strncmp(urc, "+CMTI:", 6) == 0) {
        urc_t evt = {0};
        char idx_msg[4];
        evt.type = URC_EVT_SMS_NEW;
        new_sms_dispatch(urc, idx_msg);
        strcpy(evt.info.sms_new.index, idx_msg);
        read_sms(idx_msg);
        urc_push_event(&urc_event_queue, &evt);
        return;
    }

    if (strncmp(urc, "+CREG:", 6) == 0) {
        urc_t evt = {0};
        evt.type = URC_EVT_NET_REG_STATUS;
        urc_push_event(&urc_event_queue, &evt);
        return;
    }

    if (strncmp(urc, "+HTTPACTION:", 12) == 0) {
        char status[4];
        uint32_t len;
        uint8_t method;
        urc_t evt = {0};
        http_action_dispatch(urc, &method, status, &len);
        if (method == 0){
            evt.type = URC_EVT_HTTP_GET;
            evt.info.http_get.data_len = len;
            strcpy(evt.info.http_get.status_code, status);
            evt.info.http_get.method = method;
        }
        urc_push_event(&urc_event_queue, &evt);
        DEBUG_PRINT(urc);
        return;
    }

    if (strncmp(urc, "+HTTPREAD:", 10) == 0) {
        uint32_t size = httpread_dispatch(urc);
        if (size > 0){
            httpread_ptr       = temp_buf;
            httpread_incoming  = true;
            httpread_remaining = size;
        }
        return;
    }


    if (strcmp(urc, "RING") == 0) {
        urc_t evt = {0};
        evt.type = URC_EVT_CALL_RING;
        urc_push_event(&urc_event_queue, &evt);
        return;
    }
}
