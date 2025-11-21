#include "hardware_map.h"
#include "usart.h"
#include "misc.h"
#include "datastructure.h"
#include "A7600C1.h"
#include "clock.h"

/* TYPEDEF STRUCT STATE --------------------------------------------------------------------------------- */
typedef struct {
  uint8_t retry_count;
  uint32_t timeout_ms;
  uint32_t start_time;
} state_context_t;

/* DEFINE ----------------------------------------------------------------------------------------------- */
#define TIMEOUT_WAIT_APP_RDY  7000
#define TIMEOUT_WAIT_SIM_RDY  7000
#define MAX_RETRY             3
#define MAX_MASTER_RETRY      5



/* GLOBAL VARIABLES ------------------------------------------------------------------------------------- */
state_context_t state_context = {0};
uint8_t master_retry_count = 0;
Ring_buffer usart1_rx_ring;
uint8_t buffer_rx_usart1[255] = {0};
module_state_t current_state = POWER_ON_STATE;
Queue_line queue_response[64];
uint8_t queue_idx = 0;


/*======================================================= MAIN ========================================================= */
int main(void) {

  bdang_init_clock();                                         
  init_ring_buffer(&usart1_rx_ring, buffer_rx_usart1, 255);
  hardware_init(); 
  delay_ms(2000);

  uint8_t tmp;
  uint8_t flag_send = 1;

  while(1){
    line_parse(&usart1_rx_ring, queue_response, &queue_idx);

    switch (current_state)
    {
      /* POWER ON STATE ------------------------------------------------------------------------ */
      case POWER_ON_STATE:
        A7600_PowerOn();
        usart_sendstring(USART2, "Power ON, switching to WAIT_APP_RDY_STATE\r\n");

        state_context.timeout_ms = TIMEOUT_WAIT_APP_RDY;
        state_context.start_time = get_systick_ms();
        state_context.retry_count = 0;

        current_state = WAIT_APP_RDY_STATE;
        break;


      /* WAIT APP READY STATE ------------------------------------------------------------------ */
      case WAIT_APP_RDY_STATE:
        if (line_search(queue_response, queue_idx, "*ATREADY: 1")) {
          usart_sendstring(USART2, "APP is READY, switching to WAIT_SIM_READY_STATE\r\n");
          clear_ring_buffer(&usart1_rx_ring);
          line_clear_queue(&queue_idx);

          state_context.start_time = get_systick_ms();
          state_context.timeout_ms = TIMEOUT_WAIT_SIM_RDY;
          state_context.retry_count = 0;

          current_state = WAIT_SIM_READY_STATE;
        } 

        else if ((get_systick_ms() - state_context.start_time) > TIMEOUT_WAIT_APP_RDY) {
          state_context.retry_count++;
          if (state_context.retry_count < MAX_RETRY) {
            usart_sendstring(USART2, "Out of timeout, retry\r\n");

            state_context.start_time = get_systick_ms();            //reset start time for new retry of state
          }
          else {
            usart_sendstring(USART2, "Out of retry state, switching to ERROR STATE\r\n");
            current_state = ERROR_STATE;
          }
        }
        break;

        /* WAIT SIM READY STATE ------------------------------------------------------------------ */
        case WAIT_SIM_READY_STATE:
          if (flag_send == 1){
            AT_SendCmd("AT\r\n");
            flag_send = 0;
          }
          led_green_blink(500);

          if (line_search(queue_response, queue_idx, "OK")) {
            usart_sendstring(USART2, "SIM is READY, switching to READY_STATE\r\n");
            clear_ring_buffer(&usart1_rx_ring);
            line_clear_queue(&queue_idx);

            flag_send = 1;
            state_context.start_time = get_systick_ms();
            state_context.timeout_ms = 0;
            state_context.retry_count = 0;

            current_state = READY_STATE;
          } 

          else if ((get_systick_ms() - state_context.start_time) > TIMEOUT_WAIT_SIM_RDY) {
            flag_send = 1;
            state_context.retry_count++;
            if (state_context.retry_count < MAX_RETRY) {
              usart_sendstring(USART2, "Out of timeout, retry\r\n");

              state_context.start_time = get_systick_ms();            //reset start time for new retry of state
            }
            else {
              usart_sendstring(USART2, "Out of retry state, switching to ERROR STATE\r\n");
              current_state = ERROR_STATE;
            }
          }
          break;

      /* READY STATE ----------------------------------------------------------------------------- */
      case READY_STATE:
        GPIO_SetBits(LED_4G_PORT, LED_4G_PIN);
        break;

      /* ERROR STATE ----------------------------------------------------------------------------- */
      case ERROR_STATE:
        usart_sendstring(USART2, "ERROR STATE happened, trying to reboot. Back to POWER_ON_STATE\r\n");
        master_retry_count++;
        if (master_retry_count >= MAX_MASTER_RETRY){
          usart_sendstring(USART2, "OUT OF RETRY BOOT, PLEASE CONNTACT TO HUNONIC\r\n");
        }
        else{
          A7600_PowerOff();
          current_state = POWER_ON_STATE;
        }
        break;
      default:
        current_state = ERROR_STATE;
        break;
    }
  }
}

