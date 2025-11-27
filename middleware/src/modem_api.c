#include "modem_api.h"

/* ====================================== DECLARATIONS ======================================= */
lwrb_t usart_rb;
static uint8_t usart_rx_raw[256];

char line_buff[256];
static uint16_t line_len = 0;

static modem_at_cmd_t executing_cmd;
static bool is_busy = false;

bool modem_send_at_cmd(modem_at_cmd_t cmd);
static void handle_response_line(const char *line);
static void handle_urc_line(const char *urc);
static void line_parse(void);
/* ============================================================================================ */


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
    usart_sendstring(SIM_USART_PORT, "\r\n");
    DEBUG_PRINT(">>");
    DEBUG_PRINT(executing_cmd.cmd);
    DEBUG_PRINT("\r\n");
    return true;
}

void line_parse(void){
    uint8_t c;
    while (lwrb_read(&usart_rb, &c, 1)){
        if (line_len < sizeof(line_buff) - 1) {
            if (c != 0xFF)
                line_buff[line_len++] = c;
        }
        if (c == '\n') {
            line_buff[line_len] = '\0';     
            if (is_busy){
                DEBUG_PRINT("<<");
                DEBUG_PRINT(line_buff);
                DEBUG_PRINT("\r");
                handle_response_line(line_buff);
            }
            else
                handle_urc_line(line_buff);
            line_len = 0;              
        }
    }
}

/**
 * @brief   Handle response line
 * 
 * @note    Active when is_busy = true
 */
void handle_response_line(const char *line)
{
    /*<! If the line is empty */
    size_t len = strlen(line);
    if (len == 0 || line[0] == '\r' || line[0] == '\n')
        return;

    /*<! Primary response */
    if (executing_cmd.expect[0] != '\0' &&
        strstr(line, executing_cmd.expect) != NULL)
    {
        if (executing_cmd.cb)
            executing_cmd.cb(PRIMARY, line, strlen(line));
        return;
    }

    /*<! OK response */
    if (strstr(line, "OK") != NULL)
    {
        is_busy = false;
        if (executing_cmd.cb)
            executing_cmd.cb(OK_RESP ,line, 2);
        return;
    }

    /*<! ERROR response*/    
    if (strstr(line, "ERROR") != NULL)
    {
        is_busy = false;
        if (executing_cmd.cb)
            executing_cmd.cb(ERROR_RESP, line, 5);

        return;
    }

    /*<! Intermediate response */
    if (executing_cmd.cb) {
        executing_cmd.cb(INTERMEDIATE, line, strlen(line));
    }
}

/**
 * @brief   Handle URC response
 * 
 * @note    Active when is_busy = false
 */
void handle_urc_line(const char *urc){

}

void modem_process(void){
    line_parse();
    if (is_busy) {
        uint32_t now = get_systick_ms();
        if (now - executing_cmd.start_tick >= executing_cmd.timeout_ms) {
            is_busy = false;
            if (executing_cmd.cb){
                executing_cmd.cb(TIMEOUT_RESP ,"TIMEOUT", 7);
                is_busy = false;
            }
        }
    }
}


/**
 * @brief   Init green led on the board
 * 
 * @note    Mode out, speed 25MHz, no PuPd, PushPull
 */
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

/**
 * @brief   Init GPIO control PWKEY, GPIO status SIM
 * 
 * @note    GPIO that line to the PWKEY to control on/off
 *          Mode out, 50MHz, no PuPd
 * 
 * @note    GPIO that read the status of the module SIM
 *          Mode int, Pull down mode
 */
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


/**
 * @brief   Init the modem as like ring buffer hold the data, ...
 */
void modem_init_md(void){
    modem_init_hw();            
    lwrb_init(&usart_rb, usart_rx_raw, sizeof(usart_rx_raw));
}