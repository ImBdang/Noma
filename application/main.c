#include "http_service.h"
#include "stdbool.h"
#include "stdint.h"
#include "modem_service.h"
#include "clock_api.h"
#include "modem_service_fsm.h"
#include "flash_mcu.h"

extern bool is_ota;

/*============================================== MAIN ================================================= */
int main(void) {
  clock_init_64MHz_HSI();
  systick_init(1000);
  modem_init_service();
  uint8_t step = 0;
  while(1){
    modem_service_process();
    if (modem_is_ready()){
      switch (step)
      {
      case 0:
        if (http_init()){
          step++;
        }
        break;
      
      case 1:
        if (http_set_url("https://raw.githubusercontent.com/ImBdang/raw/main/main.bin")){
          step++;
        }
        break;
      
      case 2:{
        if (http_get()){
          step++;
        }

        break;
      }

      case 3: {
        if (firmware_size > 0){
          step++;
          current_firmware_size = firmware_size;
        }
        break;
      }

      case 4: {
        is_ota = true;
        if (current_firmware_size > 0){
          uint32_t chunk  = (current_firmware_size > CHUNK_SIZE) ? CHUNK_SIZE : current_firmware_size;
          if (http_read(&current_offset, chunk))
          if (chunk <=0 )
            step++;
        }
        break;
      }
      default:
        break;
      }
    }
  }
}

