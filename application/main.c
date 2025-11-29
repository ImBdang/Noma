#include "stdbool.h"
#include "modem_service.h"
#include "clock_api.h"

/*============================================== MAIN ================================================= */
int main(void) {
  bool f = true;
  clock_init_64MHz_HSI();
  systick_init(1000);
  modem_init_service();
  bool tmp = true;
  while(1){
    modem_service_process();
    delay_ms(500);
    if (tmp && service_is_ready()){
      if (f){
        // read_sms("21");
        f = false;
      }
      tmp = !send_sms("Bdang", "0358868830");
    }
  }
}

