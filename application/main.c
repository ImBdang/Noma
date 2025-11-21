#include "hardware_map.h"
#include "usart.h"
#include "misc.h"
#include "datastructure.h"
#include "A7600C1.h"
#include "clock.h"

Ring_buffer usart1_rx_ring;

uint8_t buffer_rx_usart1[255] = {0};
module_state_t current_state = POWER_ON;

Queue_line queue_response[64];
uint8_t queue_idx = 0;

int main(void) {
  bdang_init_clock();                                         

  init_ring_buffer(&usart1_rx_ring, buffer_rx_usart1, 255);
  hardware_init(); 
  delay_ms(2000);
  

  while(1){

    line_parse(&usart1_rx_ring, queue_response, &queue_idx);

    switch (current_state)
    {
    case POWER_ON:
      A7600_PowerOn();
      current_state = WAIT_APP_RDY;
      break;

    case WAIT_APP_RDY:
      uint8_t tmp = line_search(queue_response, queue_idx, "*ATREADY: 1");
      if (tmp){
        current_state = WAIT_SIM_READY;
      }
      break;

    case WAIT_SIM_READY:
      GPIO_SetBits(LED_4G_PORT, LED_4G_PIN);
      break;

    default:
      break;
    }
  }
}

