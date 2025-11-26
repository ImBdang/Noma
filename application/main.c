#include "modem_service.h"
#include "clock_api.h"

/*======================================================= MAIN ========================================================= */
int main(void) {

  bdang_init_clock();                                         

  hardware_init(); 
  delay_ms(2000);
  

  while(1){
    
  }
}

