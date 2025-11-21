#include "hardware_map.h"
#include "usart.h"
#include "misc.h"
#include "datastructure.h"
#include "A7600C1.h"
#include "clock.h"
#include "string_handle.h"

/* TYPEDEF STRUCT STATE --------------------------------------------------------------------------------- */
typedef struct {
  uint8_t retry_count;
  uint32_t timeout_ms;
  uint32_t start_time;
} state_context_t;

/* DEFINE ----------------------------------------------------------------------------------------------- */
#define TIMEOUT_WAIT_APP_RDY  7000
#define TIMEOUT_WAIT_SIM_RDY  7000
#define TIMEOUT_WAIT_AT_OK    7000
#define TIMEOUT_WAIT_CONFIG   7000
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
char excmd[16];
char excmd_prefix[8];


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

        uint32_t start = get_systick_ms();
        while(SIM_STATUS_Pin != 1) {
            if (get_systick_ms() - start > 8000) { 
                current_state = ERROR_STATE;
                break;
            }
        }
        usart_sendstring(USART2, "Power ON, switching to WAIT_APP_RDY_STATE\r\n");

        flag_send = 1;
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

          flag_send = 1;
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
          AT_SendCmd("AT+CPIN?\r\n");
          flag_send = 0;
        }
        led_green_blink(500);
        if (line_search(queue_response, queue_idx, "+CPIN: READY")) {
          usart_sendstring(USART2, "SIM is READY, switching to WAIT_AT_OK_STATE\r\n");
          clear_ring_buffer(&usart1_rx_ring);
          line_clear_queue(&queue_idx);

          flag_send = 1;
          state_context.start_time = get_systick_ms();
          state_context.timeout_ms = TIMEOUT_WAIT_AT_OK;
          state_context.retry_count = 0;

          current_state = WAIT_AT_OK_STATE;
        } 

        else if ((get_systick_ms() - state_context.start_time) > TIMEOUT_WAIT_SIM_RDY) {
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


      /* WAIT AT OK STATE ------------------------------------------------------------------ */
      case WAIT_AT_OK_STATE:
        if (flag_send == 1){
          AT_SendCmd("AT\r\n");
          flag_send = 0;
        }
        led_green_blink(500);

        if (line_search(queue_response, queue_idx, "OK")) {
          usart_sendstring(USART2, "AT is OK, switching to READY_STATE\r\n");
          clear_ring_buffer(&usart1_rx_ring);
          line_clear_queue(&queue_idx);

          flag_send = 1;
          state_context.start_time = get_systick_ms();
          state_context.timeout_ms = 0;
          state_context.retry_count = 0;

          current_state = CONFIG_SIM_STATE;
        } 

        else if ((get_systick_ms() - state_context.start_time) > TIMEOUT_WAIT_AT_OK) {
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


        /* CONFIG SIM STATE ------------------------------------------------------------------ */
        case CONFIG_SIM_STATE:
          if (flag_send == 1){
            AT_SendCmd("ATE0\r\n");
            flag_send = 0;
          }
          led_green_blink(500);

          if (line_search(queue_response, queue_idx, "OK")) {
            usart_sendstring(USART2, "Config is done, switching to NETWORK_REGISTER_STATE\r\n");
            clear_ring_buffer(&usart1_rx_ring);
            line_clear_queue(&queue_idx);

            flag_send = 1;
            state_context.start_time = get_systick_ms();
            state_context.timeout_ms = 0;
            state_context.retry_count = 0;

            current_state = NETWORK_REG_STATE;
          } 

          else if ((get_systick_ms() - state_context.start_time) > TIMEOUT_WAIT_CONFIG) {
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
        
        
        /* NETWORK REGISTER STATE ---------------------------------------------------------------- */
        case NETWORK_REG_STATE:
          if (flag_send == 1){
            AT_SendCmd("AT+CREG?\r\n");
            flag_send = 0;
          }
          led_green_blink(200);

          if (line_search(queue_response, queue_idx, "+CREG: 0,1") || line_search(queue_response, queue_idx, "+CREG: 0,5")) {
            usart_sendstring(USART2, "Netowrk register success, switching to READY_STATE\r\n");
            clear_ring_buffer(&usart1_rx_ring);
            line_clear_queue(&queue_idx);

            flag_send = 1;
            state_context.start_time = get_systick_ms();
            state_context.timeout_ms = 0;
            state_context.retry_count = 0;

            current_state = READY_STATE;
          } 

          else if ((get_systick_ms() - state_context.start_time) > TIMEOUT_WAIT_CONFIG) {
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
        if (flag_send == 1){
          AT_SendCmd("AT+CMGF=1\r\n");
          delay_ms(500);  // đợi module trả OK

          // 2️⃣ Gửi số điện thoại người nhận
          AT_SendCmd("AT+CMGS=\"+84837645067\"\r\n");
          delay_ms(500);  // module sẽ chờ tin nhắn

          // 3️⃣ Gửi nội dung tin nhắn
          AT_SendCmd("hello world");  

          // 4️⃣ Kết thúc bằng Ctrl+Z (ASCII 26)
          USART_SendData(USART1, 26);  

          // 5️⃣ Tùy chọn: đợi module trả OK hoặc +CMGS:<msg_ref>
          delay_ms(3000);
          flag_send = 0;
        }
        break;

      
      /* SEND EXTENDED COMMAND STATE ------------------------------------------------------------- */
      // case SEND_EXCMD_STATE:
      //   if (flag_send == 1){
      //     AT_SendCmd(excmd);
      //     flag_send = 0;
      //     split_excmd(excmd, excmd_prefix, sizeof(excmd_prefix));
      //   }
      //   led_green_blink(200);

      //   if (line_search(queue_response, queue_idx, "OK")) {
      //     usart_sendstring(USART2, "Config is done, switching to READY_STATE\r\n");
      //     clear_ring_buffer(&usart1_rx_ring);
      //     line_clear_queue(&queue_idx);

      //     flag_send = 1;
      //     state_context.start_time = get_systick_ms();
      //     state_context.timeout_ms = 0;
      //     state_context.retry_count = 0;

      //     current_state = READY_STATE;
      //   } 

      //   else if ((get_systick_ms() - state_context.start_time) > TIMEOUT_WAIT_CONFIG) {
      //     flag_send = 1;
      //     state_context.retry_count++;
      //     if (state_context.retry_count < MAX_RETRY) {
      //       usart_sendstring(USART2, "Out of timeout, retry\r\n");

      //       state_context.start_time = get_systick_ms();            //reset start time for new retry of state
      //     }
      //     else {
      //       usart_sendstring(USART2, "Out of retry state, switching to ERROR STATE\r\n");
      //       current_state = ERROR_STATE;
      //     }
      //   }


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

