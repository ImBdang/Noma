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
        DEBUG_PRINT(urc);
        urc_push_event(&urc_event_queue, &evt);
        return;
    }

    if (strncmp(urc, "+CREG:", 6) == 0) {
        urc_t evt = {0};
        evt.type = URC_EVT_NET_REG_STATUS;
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
