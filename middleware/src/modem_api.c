#include "modem_api.h"

lwrb_t usart_rb;
static uint8_t uart_rx_raw[256];

char line_buff[256];
static uint16_t line_len = 0;

static modem_cmd_state_t cmd_state;
static modem_at_cmd_t executing_cmd;
static bool is_busy = false;


/**
 * @brief   Send AT command
 * 
 * @param   cmd: Struct that description command as liek expect response, command, ...
 * 
 * @note    Callback func will handler the response, dont need to process response in this func
 *          We only need to make sure that setup the params as like start time then send through USART
 * 
 * @retval  true if send success
 * @retval  false is send not success (mostly is_busy is true, other command is still wating for response)
 */
bool modem_send_at_cmd(modem_at_cmd_t cmd){
    if (is_busy)
        return false;
    executing_cmd = cmd;
    executing_cmd.start_tick = get_systick_ms();
    is_busy = true;

    usart_sendstring(SIM_USART_PORT, executing_cmd.cmd);
    uart_send_string(SIM_USART_PORT, "\r\n");
    return true;
}

void line_parse(void){
    uint8_t c;
    while (lwrb_read(&usart_rb, &c, 1)){
        if (line_len < sizeof(line_buff) - 1) {
            line_buff[line_len++] = c;
        }
        if (c == '\n') {
            line_buff[line_len] = 0;     
            if (is_busy)
                handle_response_line(line_buff);
            else
                handle_urc_line(line_buff);
            line_len = 0;              
        }
    }
}

/**
 * @brief   Handle response line if is_busy is true
 */
void handle_response_line(const char *line)
{
    /*<! If the line is empty */
    if (line[0] == '\r' || line[0] == '\n')
        return;

    /*<! Primary response */
    if (strlen(executing_cmd.expect) > 0 &&
        strstr(line, executing_cmd.expect) != NULL)
    {
        if (executing_cmd.cb)
            executing_cmd.cb(line, strlen(line));
        return;
    }

    /*<! OK response */
    if (strstr(line, "OK") != NULL)
    {
        is_busy = false;
        if (executing_cmd.cb)
            executing_cmd.cb("OK", 2);
        return;
    }

    /*<! ERROR response*/    
    if (strstr(line, "ERROR") != NULL)
    {
        is_busy = false;
        if (executing_cmd.cb)
            executing_cmd.cb("ERROR", 5);

        return;
    }

    /*<! Intermediate response */
    if (executing_cmd.cb) {
        executing_cmd.cb(line, strlen(line));
    }
}


void handle_urc_line(const char *urc){

}

void modem_process(void){
    parse_from_ringbuffer();
    if (is_busy) {
        uint32_t now = get_systick_ms();
        if (now - executing_cmd.start_tick >= executing_cmd.timeout_ms) {
            is_busy = false;
            if (executing_cmd.cb)
                executing_cmd.cb("TIMEOUT", 7);
        }
    }
}

void clear_old_data(void){

}

void led_green_init(void){
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.GPIO_Pin = LED_4G_PIN;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_25MHz;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(LED_4G_PORT, &GPIO_InitStruct);
}

void pwkey_gpio_init(void){
    GPIO_InitTypeDef gpio_c = {
        .GPIO_Pin = SIM_PWKEY_Pin,
        .GPIO_Mode = GPIO_Mode_OUT,
        .GPIO_OType = GPIO_OType_PP,
        .GPIO_Speed = GPIO_Speed_50MHz,
        .GPIO_PuPd = GPIO_PuPd_NOPULL 
    };
    init_gpio(SIM_PWKEY_GPIO, &gpio_c);

    gpio_c.GPIO_Pin = SIM_STATUS_Pin;
    gpio_c.GPIO_Mode = GPIO_Mode_IN;
    gpio_c.GPIO_PuPd = GPIO_PuPd_DOWN;
    init_gpio(SIM_STATUS_GPIO, &gpio_c);
}



void modem_init_hw(void){

}

/**
 * @brief   Init the modem as like ring buffer hold the data, ...
 */
void modem_init_md(void){
    lwrb_init(&usart_rb, uart_rx_raw, sizeof(uart_rx_raw));

}