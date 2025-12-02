#include "http_callback.h"

extern modem_event_queue_t event_queue;
extern uint32_t firmware_size;

// void httpread_callback(respon_status_t  resp, const char* line, uint16_t len)
// {
//     if (resp == OK_RESP) {

//         uint32_t chunk_size = last_httpread_size;   // modem báo +HTTPREAD: <size>
        
//         // Xử lý dữ liệu chunk trong temp_buf
//         flash_write_chunk(APP_START_ADDR + next_offset, temp_buf, chunk_size);

//         next_offset += chunk_size;

//         if (next_offset < firmware_total) {
//             uint32_t remain = firmware_total - next_offset;
//             uint32_t next_chunk = (remain > 512) ? 512 : remain;
//             http_read(next_offset, next_chunk);
//         }
//         else {
//             DEBUG_PRINT("DOWNLOAD DONE\r\n");
//         }
//     }
// }



void httpget_callback(respon_status_t resp_status, const char* line, uint32_t len){
    switch (resp_status){

    case PRIMARY:
        DEBUG_PRINT("PRIMARY\r\n");
        uint8_t method;
        char status[4];
        http_action_dispatch(line, &method, status, &firmware_size);
        push_event(&event_queue, EVT_PRIMARY);
        break;

    case OK_RESP:
        DEBUG_PRINT("OK EVENT\r\n");
        push_event(&event_queue, EVT_OK);
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

void httpread_callback(respon_status_t resp_status, const char* line, uint32_t len){
    switch (resp_status){

    case PRIMARY:
        DEBUG_PRINT("PRIMARY EVENT\r\n");
        push_event(&event_queue, EVT_PRIMARY);
        break;

    case OK_RESP:
        DEBUG_PRINT("OK EVENT\r\n");
        push_event(&event_queue, EVT_OK);
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