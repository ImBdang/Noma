#include "stdbool.h"
#include "modem_services.h"
#include "clock_api.h"

/*============================================== MAIN ================================================= */
int main(void) {

  clock_init_64MHz_HSI();
  systick_init(1000);
  modem_init_service();

  while(1){
    modem_process();
    modem_service_process();
    delay_ms(500);
  }
}

