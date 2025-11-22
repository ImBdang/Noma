#include "hardware_map.h"
#include "usart.h"
#include "misc.h"
#include "datastructure.h"
#include "A7600C1.h"
#include "clock.h"
#include "string_handle.h"
#include "config_parameters.h"

/* TYPEDEF STRUCT STATE --------------------------------------------------------------------------------- */
typedef struct {
  uint8_t retry_count;
  uint32_t timeout_ms;
  uint32_t start_time;
} state_context_t;



/* GLOBAL VARIABLES ------------------------------------------------------------------------------------- */
uint8_t master_retry_count = 0;
Ring_buffer usart1_rx_ring;
uint8_t buffer_rx_usart1[255] = {0};
Queue_line queue_response[64];
uint8_t queue_idx = 0;
uint8_t searched_idx = 0;
char excmd[32];
char excmd_prefix[16];
state_context_t state_context = {0};
module_state_t current_state = POWER_ON_STATE;


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
        usart_sendstring(USART2, "Wating PWKEY STATUS HIGH\r\n");
        uint32_t start = get_systick_ms();
        while(SIM_STATUS_Pin != 1) {
            if (get_systick_ms() - start > 15000) { 
                current_state = ERROR_STATE;
                break;
            }
        }
        usart_sendstring(USART2, "Power ON, switching to WAIT_APP_RDY_STATE\r\n");

        flag_send = 1;
        state_context.timeout_ms = TIMEOUT_WAIT_AT_OK;
        state_context.start_time = get_systick_ms();
        state_context.retry_count = 0;

        current_state = WAIT_AT_OK_STATE;
        break;


      /* WAIT AT OK STATE ------------------------------------------------------------------ */
      case WAIT_AT_OK_STATE:
        if (flag_send == 1){
          AT_SendCmd("AT\r\n");
          flag_send = 0;
        }
        led_green_blink(500);

        if (line_search(queue_response, queue_idx, &searched_idx, "OK")) {
          usart_sendstring(USART2, "AT is OK, switching to READY_STATE\r\n");
          clear_ring_buffer(&usart1_rx_ring);
          line_clear_queue(&queue_idx);

          flag_send = 1;
          state_context.start_time = get_systick_ms();
          state_context.timeout_ms = TIMEOUT_WAIT_SIM_RDY;
          state_context.retry_count = 0;

          current_state = WAIT_SIM_READY_STATE;
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



      /* WAIT APP READY STATE ------------------------------------------------------------------ */
      // case WAIT_APP_RDY_STATE:
      //   if (line_search(queue_response, queue_idx, &searched_idx,"*ATREADY: 1")) {
      //     usart_sendstring(USART2, "APP is READY, switching to WAIT_SIM_READY_STATE\r\n");
      //     clear_ring_buffer(&usart1_rx_ring);
      //     line_clear_queue(&queue_idx);

      //     flag_send = 1;
      //     state_context.start_time = get_systick_ms();
      //     state_context.timeout_ms = TIMEOUT_WAIT_SIM_RDY;
      //     state_context.retry_count = 0;

      //     current_state = WAIT_SIM_READY_STATE;
      //   } 

      //   else if ((get_systick_ms() - state_context.start_time) > TIMEOUT_WAIT_APP_RDY) {
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
      //   break;


      /* WAIT SIM READY STATE ------------------------------------------------------------------ */
      case WAIT_SIM_READY_STATE:
        if (flag_send == 1){
          AT_SendCmd("AT+CPIN?\r\n");
          flag_send = 0;
        }
        led_green_blink(500);
        if (line_search(queue_response, queue_idx,  &searched_idx,"+CPIN: READY")) {
          usart_sendstring(USART2, "SIM is READY, switching to CONFIG_SIM_STATE\r\n");
          clear_ring_buffer(&usart1_rx_ring);
          line_clear_queue(&queue_idx);

          flag_send = 1;
          state_context.start_time = get_systick_ms();
          state_context.timeout_ms = TIMEOUT_WAIT_CONFIG;
          state_context.retry_count = 0;

          current_state = CONFIG_SIM_STATE;
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




        /* CONFIG SIM STATE ------------------------------------------------------------------ */
        case CONFIG_SIM_STATE:
          if (flag_send == 1){
            AT_SendCmd("ATE0\r\n");
            flag_send = 0;
          }
          led_green_blink(500);

          if (line_search(queue_response, queue_idx, &searched_idx, "OK")) {
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
          led_green_blink(500);

          if (line_search(queue_response, queue_idx,  &searched_idx,"+CREG: 0,1") || line_search(queue_response, queue_idx,  &searched_idx,"+CREG: 0,5")) {
            usart_sendstring(USART2, "Netowrk register success, switching to READY_STATE\r\n");
            usart_sendstring(USART2, queue_response[searched_idx].line_cmd);
            usart_sendstring(USART2, "\r\n");
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
          // AT_SendCmd("AT+CMGF=1\r\n");
          // delay_ms(200);
          // AT_SendCmd("AT+CMGS=\"+84837645067\"\r\n");
          // delay_ms(200);
          // AT_SendCmd("Hello");
          // delay_ms(50);
          // USART_SendData(USART1, 0x1A);
          // delay_ms(500);

          strcpy(excmd, "AT+CMGF=1\r\n");
          flag_send = 1;
          current_state = SEND_EXCMD_STATE;
        }
        break;

      
      /* SEND EXTENDED COMMAND STATE ------------------------------------------------------------- */
      case SEND_EXCMD_STATE:
        if (flag_send == 1){
          split_excmd(excmd, excmd_prefix, sizeof(excmd_prefix));
          if (strcmp(excmd_prefix, "+CMGF") == 0){
            flag_send = 1;
            state_context.start_time = get_systick_ms();
            state_context.timeout_ms = TIMEOUT_SMS_SEND;
            state_context.retry_count = 0;

            current_state = SMS_STATE;
            clear_ring_buffer(&usart1_rx_ring);
            line_clear_queue(&queue_idx);
            break;
          }
          AT_SendCmd(excmd);
          flag_send = 0;
        }
        led_green_blink(200);

        if (line_search(queue_response, queue_idx,  &searched_idx, excmd_prefix)) {
          usart_sendstring(USART2, queue_response->line_cmd);

          clear_ring_buffer(&usart1_rx_ring);
          line_clear_queue(&queue_idx);

          flag_send = 1;
          state_context.start_time = get_systick_ms();
          state_context.timeout_ms = 0;
          state_context.retry_count = 0;

          current_state = READY_STATE;
        } 

        else if ((get_systick_ms() - state_context.start_time) > TIMEOUT_EXCMD_SEND) {
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


      /* SMS STATE ------------------------------------------------------------------------------ */
      case SMS_STATE:
        split_excmd(excmd, excmd_prefix, sizeof(excmd_prefix));
        if (flag_send == 1){
          AT_SendCmd(excmd);
          flag_send = 0;
        }
        led_green_blink(200);
        /* STEP 1: Wait for "OK" response after AT+CMGF=1 --------------------------------------- */
        if (line_search(queue_response, queue_idx,  &searched_idx, "OK")) {
          usart_sendstring(USART2, "AT+CMGF is OK, setting phone number\r\n");
          clear_ring_buffer(&usart1_rx_ring);
          line_clear_queue(&queue_idx);

          flag_send = 1;
          state_context.start_time = get_systick_ms();
          state_context.timeout_ms = 0;
          state_context.retry_count = 0;
          
          /* If success switch command to AT+CMGS to set the phone number ---------------------- */
          strcpy(excmd, "AT+CMGS=\"");
          strcat(excmd, target_phone_number);
          strcat(excmd, "\"\r\n");
        } 
        /* STEP 2: Wait for ">" response after set phone number to type message ---------------- */
        else if (line_search(queue_response, queue_idx,  &searched_idx,">") && strcmp(excmd_prefix, "+CMGS") == 0) {
          usart_sendstring(USART2, "Phone number is setted, writing message\r\n");
          usart_sendstring(USART2, queue_response[searched_idx].line_cmd);
          usart_sendstring(USART2, "\r\n");
          clear_ring_buffer(&usart1_rx_ring);
          line_clear_queue(&queue_idx);

          flag_send = 1;  /* Set here because after this step STATE will back to READY STATE --- */
          state_context.start_time = get_systick_ms();
          state_context.timeout_ms = 0;
          state_context.retry_count = 0;

          /* Final step, send message then CTRL+Z to send -------------------------------------- */
          AT_SendCmd(message);
          delay_ms(50); 
          USART_SendData(USART1, 0x1A); 

          current_state = DEBUG_STATE;
        } 
        else if ((get_systick_ms() - state_context.start_time) > TIMEOUT_SMS_SEND) {
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

      case DEBUG_STATE:
        flag_send = 0;
        break;

      /* ERROR STATE ----------------------------------------------------------------------------- */
      case ERROR_STATE:
        master_retry_count++;
        if (master_retry_count >= MAX_MASTER_RETRY){
          usart_sendstring(USART2, "OUT OF RETRY BOOT, PLEASE CONNTACT TO HUNONIC\r\n");
        }
        else{
          usart_sendstring(USART2, "ERROR STATE happened, trying to reboot. Back to POWER_ON_STATE\r\n");
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

