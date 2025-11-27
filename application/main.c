#include "stdbool.h"
#include "modem_service.h"
#include "clock_api.h"

/*============================================== MAIN ================================================= */
int main(void) {

  clock_init_64MHz_HSI();
  systick_init(1000);
  modem_init_service();
  bool tmp = true;
  while(1){
    modem_process();
    modem_service_process();
    delay_ms(500);
    if (tmp && service_is_ready()){
      tmp = !send_sms("Dung giam len co va Usagi nhe ;3", "+84869668837");
    }
  }
}

