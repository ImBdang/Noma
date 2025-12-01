#include "http_callback.h"

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