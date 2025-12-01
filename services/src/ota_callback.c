#include "ota_callback.h"

void ota_get_firmware_size_callback(respon_status_t resp_status, const char* line, uint32_t len){
    switch (resp_status){
    case OK_RESP:
        DEBUG_PRINT("OTA SET SIZE OK\r\n");
        push_event(&event_queue, EVT_OK);
        break;
        
    case PRIMARY:
        DEBUG_PRINT(line);
        DEBUG_PRINT("\r\n");
        
        push_event(&event_queue, EVT_PRIMARY);
        break;

    case ERROR_RESP:
        DEBUG_PRINT("ERROR EVENT\r\n");
        push_event(&event_queue, EVT_ERROR);
        break;

    case TIMEOUT_RESP:
        DEBUG_PRINT("TIMEOUT EVENT\r\n");
        push_event(&event_queue, EVT_TIMEOUT);
        break;
    }
}