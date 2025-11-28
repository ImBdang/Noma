#include "modem_service_urc.h"

/* ====================================== DECLARATIONS ======================================= */
extern urc_event_queue_t urc_event_queue;
/* =========================================================================================== */
void modem_service_urc_process(void){
    urc_t evt;
    if (!urc_pop_event(&urc_event_queue, &evt))
        return;
    
    switch (evt.type)
    {
    case URC_EVT_SMS_NEW:
        DEBUG_PRINT("Have new MESSAGE\r\n");
        break;

    case URC_EVT_NET_REG_STATUS:
        /* code */
        break;

    case URC_EVT_CALL_RING:
        /* code */
        break;

    default:
        break;
    }
}
