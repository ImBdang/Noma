#include "modem_service_sms.h"

/* ====================================== DECLARATIONS ======================================= */
extern sms_event_queue_t sms_event_queue;

static sms_message_t sms_cur;
static sms_state_t sms_state = SMS_STATE_INIT;
static uint8_t sms_retry_count = 0;
static uint8_t sms_max_retry = 3;

static void sms_state_entry_init(const char* message, const char* phone);

static bool sms_state_entry_set_format(void);
static void sms_state_entry_wait_set_format(void);

static bool sms_state_entry_set_encoding(void);
static void sms_state_entry_wait_set_encoding(void);

static bool sms_state_entry_set_number(void);
static void sms_state_entry_wait_set_number(void);

static bool sms_state_entry_send_text(void);
static void sms_state_wait_result(void);

static void sms_state_entry_done(void);
static void sms_state_entry_error(void);

static void sms_cmgf_callback(respon_status_t status, const char *str,  uint32_t len);
static void sms_cscs_callback(respon_status_t status, const char *str, uint32_t len);
static void sms_cmgs_callback(respon_status_t status, const char *str, uint32_t len);
/* =========================================================================================== */

static void sms_state_entry_init(const char* message, const char* phone){
    strncpy(sms_cur.phone, phone, sizeof(sms_cur.phone) - 1);
    strncpy(sms_cur.message, message, sizeof(sms_cur.message) - 1);
    sms_cur.is_send = false;
    sms_retry_count = 0;
    sms_state = SMS_STATE_SET_FORMAT;
}


static bool sms_state_entry_set_format(void){
    modem_at_cmd_t cmd = {
        .cmd = "AT+CMGF=1",             /*<! Modify this later <===================== */
        .expect = "",
        .timeout_ms = 10000,
        .start_tick = get_systick_ms(),
        .cb = sms_cmgf_callback
    };
    return modem_send_at_cmd(cmd);  
}
static void sms_state_entry_wait_set_format(void){
    sms_evt_t evt;
    static uint8_t timeout_count = 0;
    static uint8_t max_timeout = 3;
    if (!sms_pop_event(&sms_event_queue, &evt))
        return;

    switch (evt)
    {
        case SMS_EVT_OK:
            timeout_count = 0; 
            sms_state = SMS_STATE_SET_ENCODING;
            break;

        case SMS_EVT_TIMEOUT:
            timeout_count++;
            if (timeout_count >= max_timeout){
                sms_state = SMS_STATE_ERROR;
                timeout_count = 0;
            }
            break;
        

        case SMS_EVT_ERROR:
            timeout_count = 0;
            sms_state = SMS_STATE_ERROR;
            break;


        default:
            sms_state = SMS_STATE_ERROR;
            break;
    }
}



/**/
static bool sms_state_entry_set_encoding(void){
    modem_at_cmd_t cmd = {
        .cmd = "AT+CSCS=\"GSM\"",             /*<! Modify this later <===================== */
        .expect = "",
        .timeout_ms = 10000,
        .start_tick = get_systick_ms(),
        .cb = sms_cscs_callback
    };
    return modem_send_at_cmd(cmd);  
}
static void sms_state_entry_wait_set_encoding(void){
    sms_evt_t evt;
    static uint8_t timeout_count = 0;
    static uint8_t max_timeout = 3;
    if (!sms_pop_event(&sms_event_queue, &evt))
        return;

    switch (evt)
    {
        case SMS_EVT_OK:
            timeout_count = 0; 
            sms_state = SMS_STATE_SET_NUMBER;
            break;

        case SMS_EVT_TIMEOUT:
            timeout_count++;
            if (timeout_count >= max_timeout){
                sms_state = SMS_STATE_ERROR;
                timeout_count = 0;
            }
            break;
        
        case SMS_EVT_ERROR:
            timeout_count = 0;
            sms_state = SMS_STATE_ERROR;
            break;

        default:
            sms_state = SMS_STATE_ERROR;
            break;
    }
}


static bool sms_state_entry_set_number(void)
{
    char cmd_buf[48];

    strcpy(cmd_buf, "AT+CMGS=\"");
    strncat(cmd_buf, sms_cur.phone, sizeof(cmd_buf) - strlen(cmd_buf) - 2);
    strcat(cmd_buf, "\"");

    modem_at_cmd_t cmd;
    memset(&cmd, 0, sizeof(cmd));

    strncpy(cmd.cmd, cmd_buf, sizeof(cmd.cmd)-1);
    cmd.cmd[sizeof(cmd.cmd)-1] = '\0';

    cmd.expect[0] = '\0';
    cmd.timeout_ms = 10000;
    cmd.start_tick = get_systick_ms();
    cmd.cb = sms_cmgs_callback;   

    return modem_send_at_cmd(cmd);
}

static void sms_state_entry_wait_set_number(void)
{
    sms_evt_t evt;
    static uint8_t timeout_count = 0;
    const uint8_t max_timeout = 3;

    if (!sms_pop_event(&sms_event_queue, &evt))
        return;

    switch (evt)
    {
        case SMS_EVT_PROMPT:
            timeout_count = 0;
            sms_state = SMS_STATE_SEND_TEXT;
            break;

        case SMS_EVT_TIMEOUT:
                timeout_count++;
            if (timeout_count >= max_timeout){
                sms_state = SMS_STATE_ERROR;
                timeout_count = 0;
            }
            break;

        case SMS_EVT_ERROR:
            timeout_count = 0;
            sms_state = SMS_STATE_ERROR;
            break;

        default:
            break;
    }
}


static bool sms_state_entry_send_text(void)
{
    usart_sendstring(SIM_USART_PORT, sms_cur.message);
    USART_SendData(SIM_USART_PORT, 0x1A);
    sms_state = SMS_STATE_WAIT_RESULT;
    return true;
}

static void sms_state_wait_result(void){
    sms_evt_t evt;

    if (!sms_pop_event(&sms_event_queue, &evt))
        return;

    switch (evt)
    {
        case SMS_EVT_SENT:
            sms_cur.is_send = true;
            sms_state = SMS_STATE_DONE;
            break;

        case SMS_EVT_FAILED:   
            break;
        case SMS_EVT_TIMEOUT:
            if (sms_retry_count < sms_max_retry) {
                sms_retry_count++;
                sms_state = SMS_STATE_SET_NUMBER;
            } else {
                sms_state = SMS_STATE_ERROR;
            }
            break;

        default:
            break;
    }
}


static void sms_state_entry_done(void){}
static void sms_state_entry_error(void){}

bool send_sms(const char* message, const char* phone)
{
    bool tmp = false;
    switch (sms_state)
    {
        case SMS_STATE_INIT:
            sms_state_entry_init(message, phone);
            return false;

        case SMS_STATE_SET_FORMAT:
            tmp = sms_state_entry_set_format();
            if (tmp){
                sms_state = SMS_STATE_WAIT_SET_FORMAT;
            }
            return false;
        case SMS_STATE_WAIT_SET_FORMAT:
            sms_state_entry_wait_set_format();
            return false;

        case SMS_STATE_SET_ENCODING:
            tmp = sms_state_entry_set_encoding();
            if (tmp){
                sms_state = SMS_STATE_WAIT_SET_ENCODING;
            }
            return false;
            
        case SMS_STATE_WAIT_SET_ENCODING:
            sms_state_entry_wait_set_encoding();
            return false;

        case SMS_STATE_SET_NUMBER:
            tmp = sms_state_entry_set_number();
            if (tmp){
                sms_state = SMS_STATE_WAIT_SET_NUMBER;
            }
            return false;

        case SMS_STATE_WAIT_SET_NUMBER:
            sms_state_entry_wait_set_number();
            return false;

        case SMS_STATE_SEND_TEXT:
            sms_state_entry_send_text();
            return false;

        case SMS_STATE_WAIT_RESULT:
            sms_state_wait_result();
            return false;

        case SMS_STATE_DONE:
            return true;

        case SMS_STATE_ERROR:
            return false;
    }
    return false;
}