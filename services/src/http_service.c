#include "http_service.h"

static http_state_t current_http_state = HTTP_CHECK_ONLINE;
static uint8_t step_config = 0;
extern modem_event_queue_t event_queue; 
bool http_init_flag = false;

static bool http_check_init(void);
static void http_wait_check_init(void);

static bool http_init_entry(void);
static void http_init_wait(void);

static bool http_config_entry(void);
static void http_config_wait(void);

static bool http_reactive_ate0(void);

static bool http_get_entry(void);
static void http_get_wait(void);


bool http_init_entry(void){
    modem_at_cmd_t cmd = {
        .cmd = "AT+HTTPINIT",
        .expect = "",
        .timeout_ms = 12000,
        .start_tick = get_systick_ms(),
        .cb = dummy_callback
        };
    return modem_send_at_cmd(cmd);
}
void http_init_wait(void){
    modem_event_t evt;
    if (!pop_event(&event_queue, &evt))
        return;

    switch(evt){
        case EVT_OK:
            DEBUG_PRINT("HTTP INIT OK\r\n");
            current_http_state = HTTP_CONFIG_ENTRY;
            break;

        case EVT_ERROR:
            DEBUG_PRINT("HTTP INTI ERR\r\n");
            break;
        case EVT_TIMEOUT:
            DEBUG_PRINT("HTTP INIT FAIL, RETRY\r\n");
            current_http_state = HTTP_INIT_ENTRY;
            break;
    }
}

bool http_check_init(void){
    modem_at_cmd_t cmd = {
        .expect = "",
        .timeout_ms = 12000,
        .start_tick = get_systick_ms(),
        .cb = dummy_callback
    };
    strcpy(cmd.cmd, "AT+HTTPINIT=?");
    return modem_send_at_cmd(cmd);
}

void http_wait_check_init(){
    modem_event_t evt;
    if (!pop_event(&event_queue, &evt))
        return;

    switch(evt){
        case EVT_OK:
            // DEBUG_PRINT("HTTP ALREADY INIT\r\n");
            current_http_state = HTTP_INIT_ENTRY;
            break;

        default:
            current_http_state = HTTP_INIT_ENTRY;
            break;
    }
}


static bool http_config_entry(void){
    switch (step_config)
    {
        case 0:
        {
            modem_at_cmd_t cmd = {
                //.cmd = "AT+HTTPPARA=\"URL\",\"https://raw.githubusercontent.com/ImBdang/raw/main/main.bin\"",
                .cmd = "AT+HTTPPARA=\"URL\",\"http://opinion.people.com.cn/GB/n1/2018/0815/c1003-30228758.html\"",
                .expect = "",
                .timeout_ms = 5000,
                .start_tick = get_systick_ms(),
                .cb = dummy_callback
            };
            return modem_send_at_cmd(cmd);
        }

        default:
            return false;
    }
}


static void http_config_wait(void){
    modem_event_t evt;
    static uint8_t timeout_count = 0;
    static uint8_t max_timeout = 3;
    if (!pop_event(&event_queue, &evt))
        return;

    switch (evt)
    {
        case EVT_OK:
            timeout_count = 0; 
            step_config++;
            if (step_config >= 1) {
                current_http_state = REACTIVE_ATE0;
            } else {
                current_http_state = HTTP_CONFIG_ENTRY;  
            }
            break;

        case EVT_TIMEOUT:
            timeout_count++;
            if (timeout_count >= max_timeout){
                timeout_count = 0;
            }
            break;
        

        case EVT_ERROR:
            timeout_count = 0;
            break;


        default:
            break;
    }
}


bool http_reactive_ate0(void){
    modem_at_cmd_t cmd = {
        .cmd = "ATE0",
        .expect = "",
        .timeout_ms = 5000,
        .start_tick = get_systick_ms(),
        .cb = modem_config_ate0_callback
    };
    return modem_send_at_cmd(cmd);
}


bool http_init(void){
    bool tmp = false;
    switch (current_http_state)
    {
    case HTTP_CHECK_ONLINE:
        tmp = http_check_init();
        if (tmp){
            current_http_state = HTTP_WAIT_CHECK_ONLINE;
        }
        return false;

    case HTTP_WAIT_CHECK_ONLINE:
        http_wait_check_init();
        return false;

    case HTTP_INIT_ENTRY:
        tmp = http_init_entry();
        if (tmp){
            current_http_state = HTTP_INIT_WAIT;
        }
        return false;
        
    case HTTP_INIT_WAIT:
        http_init_wait();
        return false;

    case HTTP_CONFIG_ENTRY:
        tmp = http_config_entry();
        if (tmp){
            current_http_state = HTTP_CONFIG_WAIT;
        }
        return false;

    case HTTP_CONFIG_WAIT:
        http_config_wait();
        return false;
    
    case REACTIVE_ATE0:
        tmp = http_reactive_ate0();
        if (tmp)
            current_http_state = HTTP_STATE_READY;
        return false;

    case HTTP_STATE_READY:
        http_init_flag = true;
        return true;
    }
    return false;
}



void http_get(void){
    modem_at_cmd_t cmd = {
        .cmd = "AT+HTTPACTION=0",
        .expect = "",
        .timeout_ms = 5000,
        .start_tick = get_systick_ms(),
        .cb = dummy_callback
    };
    modem_send_at_cmd(cmd);
}


void http_read(uint32_t data_len){
    modem_at_cmd_t cmd = {
        .cmd = "AT+HTTPREAD=0,512",
        .expect = "",
        .timeout_ms = 5000,
        .start_tick = get_systick_ms(),
        .cb = dummy_callback
    };

    // httpread_incoming  = true;
    // httpread_remaining = 512;
    // httpread_ptr       = temp_buf;
    modem_send_at_cmd(cmd);
}