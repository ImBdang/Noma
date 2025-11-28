#include "modem_urc_api.h"

/* ====================================== DECLARATIONS ======================================= */
extern char line_buff[256];
extern urc_event_t urc_event_queue;
/* =========================================================================================== */
void modem_urc_process(const char *urc){
    if (strncmp(urc, "+CMTI:", 6) == 0) {
        urc_push_event(&urc_event_queue, URC_EVT_SMS_NEW);
        return;
    }

    if (strncmp(urc, "+CREG:", 6) == 0) {
        urc_push_event(&urc_event_queue, URC_EVT_NET_REG_STATUS);
        return;
    }

    if (strcmp(urc, "RING") == 0) {
        urc_push_event(&urc_event_queue, URC_EVT_CALL_RING);
        return;
    }
}
