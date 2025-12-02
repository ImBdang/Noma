#include "modem_urc_api.h"

/* ====================================== DECLARATIONS ======================================= */
extern urc_event_queue_t urc_event_queue;
extern bool is_ota;
extern uint32_t current_offset;
extern uint32_t chunk_actual;

uint32_t http_read_size = 0;
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
        urc_t evt = {0};
        evt.type  = URC_EVT_HTTP_READ;

        evt.info.http_read.data_len = httpread_dispatch(urc);
        uint32_t actual = evt.info.http_read.data_len;

        if (is_ota && actual > 0) {

            current_firmware_size -= actual;     
            current_offset += actual;           

            httpread_ptr       = temp_buf;
            httpread_incoming  = true;
            httpread_remaining = actual;
            http_read_size     = actual;
        }

        urc_push_event(&urc_event_queue, &evt);
        return;
    }


    if (strcmp(urc, "RING") == 0) {
        urc_t evt = {0};
        evt.type = URC_EVT_CALL_RING;
        urc_push_event(&urc_event_queue, &evt);
        return;
    }
}
