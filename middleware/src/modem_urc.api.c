#include "modem_urc_api.h"

/* ====================================== DECLARATIONS ======================================= */
extern urc_event_queue_t urc_event_queue;
/* =========================================================================================== */
void modem_urc_make_event(const char *urc){
    if (strncmp(urc, "+CMTI:", 6) == 0) {
        urc_t evt = {0};
        evt.type = URC_EVT_SMS_NEW;
        uint8_t idx = new_sms_dispatch(urc);
        evt.info.sms_new.index = idx;
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
