#include "http_service.h"
#include "stdbool.h"
#include "modem_service.h"
#include "clock_api.h"
#include "modem_service_fsm.h"


/*============================================== MAIN ================================================= */
int main(void) {
  clock_init_64MHz_HSI();
  systick_init(1000);
  modem_init_service();
  bool flag = true;
  while(1){
    modem_service_process();
    delay_ms(100);
    if (modem_is_ready()){
      if (flag){
        http_get();
        flag = false;
      }
    }
  }
}

