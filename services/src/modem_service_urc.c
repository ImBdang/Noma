#include "modem_service_urc.h"

/* ====================================== DECLARATIONS ======================================= */
extern urc_event_queue_t urc_event_queue;

uint32_t next_offset = 0;
uint32_t firmware_total = 0;

extern bool httpread_incoming;
extern uint32_t httpread_remaining;
extern uint8_t* httpread_ptr;

static uint32_t remaining_bit = 0;
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

    case URC_EVT_HTTP_GET:
        if (strncmp(evt.info.http_get.status_code, "200", 3) == 0){
            DEBUG_PRINT("HTTP STATUS CODE 200\r\n");
            firmware_total = evt.info.http_get.data_len;
            uint32_t chunk  = (firmware_total > 512) ? 512 : firmware_total;
            http_read(next_offset ,chunk);
        }
        else{
            DEBUG_PRINT("HTTP ERROR, STATUS CODE: ");
            DEBUG_PRINT(evt.info.http_get.status_code);
            DEBUG_PRINT("\r\n");
        }

        break;
    
    case URC_EVT_HTTP_READ:
        // DEBUG_PRINT("READ NAY\r\n");
        // httpread_incoming = true;
        // httpread_remaining = evt.info.http_read.data_len;
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
