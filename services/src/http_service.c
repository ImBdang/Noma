#include "http_service.h"

static uint8_t term_step = 0;
static uint8_t step_config = 0;
static uint8_t step_http_init = 0;
static uint8_t step_set_url = 0;
static uint8_t step_get = 0;
static uint8_t step_read = 0;

extern uint32_t http_read_size;
extern modem_event_queue_t event_queue; 
bool http_init_flag = false;


bool http_term(void);
static bool http_term_entry(void);
static bool http_term_wait(void);

static bool http_init_entry(void);
static void http_init_wait(void);


static bool http_set_url_entry(const char* url);
static bool http_set_url_wait(void);

static bool http_config_entry(void);
static void http_config_wait(void);

static bool http_reactive_ate0(void);

static bool http_get_entry(void);
static bool http_get_wait(void);

static bool http_read_entry(uint32_t* offset, uint32_t size);
static bool http_read_wait(uint32_t* offset, uint32_t size);

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
            step_http_init++;
            break;

        case EVT_ERROR:
            DEBUG_PRINT("HTTP INIT ERR\r\n");
            break;
        case EVT_TIMEOUT:
            DEBUG_PRINT("HTTP INIT FAIL, RETRY\r\n");
            step_http_init--;
            break;
    }
}


bool http_term_entry(void){
    modem_at_cmd_t cmd = {
        .cmd = "AT+HTTPTERM",
        .expect = "",
        .timeout_ms = 12000,
        .start_tick = get_systick_ms(),
        .cb = dummy_callback
        };
    return modem_send_at_cmd(cmd);
}
bool http_term_wait(void){
    modem_event_t evt;
    if (!pop_event(&event_queue, &evt))
        return false;

    switch(evt){
        case EVT_OK:
            DEBUG_PRINT("HTTP TERM DONE\r\n");
            return true;

        case EVT_ERROR:
            DEBUG_PRINT("HTTP TERM ERR\r\n");
            return true;
        case EVT_TIMEOUT:
            return false;
    }
    return true;
}
bool http_term(void) {
    bool tmp = false;
    switch (term_step)
    {
    case 0:
        tmp = http_term_entry();
        if (tmp){
            term_step++;
        }
        return false;

    case 1:
        http_term_wait();
        return true;
    }
}


static bool http_config_entry(void){
    return true;
    switch (step_config)
    {
        // case 0:
        // {
        //     modem_at_cmd_t cmd = {
        //         //.cmd = "AT+HTTPPARA=\"URL\",\"https://raw.githubusercontent.com/ImBdang/raw/main/main.bin\"",
        //         .cmd = "AT+HTTPPARA=\"URL\",\"http://opinion.people.com.cn/GB/n1/2018/0815/c1003-30228758.html\"",
        //         .expect = "",
        //         .timeout_ms = 5000,
        //         .start_tick = get_systick_ms(),
        //         .cb = dummy_callback
        //     };
        //     return modem_send_at_cmd(cmd);
        // }

        default:
            return false;
    }
}
static void http_config_wait(void){
    step_http_init++;
    // modem_event_t evt;
    // static uint8_t timeout_count = 0;
    // static uint8_t max_timeout = 3;
    // if (!pop_event(&event_queue, &evt))
    //     return;

    // switch (evt)
    // {
    //     case EVT_OK:
    //         timeout_count = 0; 
    //         step_config++;
    //         if (step_config >= 1) {
    //             current_http_state = REACTIVE_ATE0;
    //         } else {
    //             current_http_state = HTTP_CONFIG_ENTRY;  
    //         }
    //         break;

    //     case EVT_TIMEOUT:
    //         timeout_count++;
    //         if (timeout_count >= max_timeout){
    //             timeout_count = 0;
    //         }
    //         break;
        

    //     case EVT_ERROR:
    //         timeout_count = 0;
    //         break;


    //     default:
    //         break;
    // }
}

bool http_set_url_entry(const char* url){
    modem_at_cmd_t cmd = {
        .expect = "",
        .timeout_ms = 5000,
        .start_tick = get_systick_ms(),
        .cb = dummy_callback
    };
    snprintf(cmd.cmd, sizeof(cmd.cmd),"AT+HTTPPARA=\"URL\",\"%s\"", url);
    return modem_send_at_cmd(cmd);   
}

bool http_set_url_wait(void){
    modem_event_t evt;
    static uint8_t timeout_count = 0;
    static uint8_t max_timeout = 3;
    if (!pop_event(&event_queue, &evt))
        return false;
    switch (evt)
    {
        case EVT_OK:
            step_set_url = 0;
            DEBUG_PRINT("SET OK\r\n");
            return true;

        case EVT_TIMEOUT:
            timeout_count++;
            if (timeout_count >= max_timeout){
                timeout_count = 0;
            }
            return false;
        

        case EVT_ERROR:
            timeout_count = 0;
            return false;
    }
    return false;
}

bool http_set_url(const char* url){
    bool tmp = false;
    switch (step_set_url)
    {
        case 0:{
            tmp = http_set_url_entry(url);
            if (tmp){
                step_set_url++;
            }
            return false;
        }
        case 1:{
            return http_set_url_wait();
        }
    }
    return false;
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

    switch (step_http_init)
    {
    case 0:
        if (http_term())
            step_http_init = 1;
        return false;

    case 1:
        tmp = http_init_entry();
        if (tmp){
            step_http_init = 2;
        }
        return false;
        
    case 2:
        http_init_wait();
        return false;

    case 3:
        tmp = http_config_entry();
        if (tmp){
            step_http_init = 4;
        }
        return false;

    case 4:
        http_config_wait();
        return false;
    
    case 5:
        tmp = http_reactive_ate0();
        if (tmp)
            step_http_init++;
        return false;

    case 6:
        step_http_init = 0;
        return true;
    }
    return false;
}

static bool http_get_entry(void){
    modem_at_cmd_t cmd = {
        .cmd = "AT+HTTPACTION=0",
        .expect = "+HTTPACTION:",
        .timeout_ms = 5000,
        .start_tick = get_systick_ms(),
        .cb = httpget_callback
    };
    return modem_send_at_cmd(cmd);
}

static bool http_get_wait(void){
    modem_event_t evt;
    if (!pop_event(&event_queue, &evt))
        return false;

    switch(evt){
        case EVT_PRIMARY:   
            DEBUG_PRINT("HTTPGET PRIMARY FOUND\r\n");
            return true;

        case EVT_OK:
            DEBUG_PRINT("HTTP GET OK\r\n");
            return true;

        case EVT_ERROR:
            DEBUG_PRINT("HTTP GET ERR\r\n");
            return false; 

        case EVT_TIMEOUT:
            DEBUG_PRINT("HTTP GET TIMEOUT, RETRY\r\n");
            return false;
    }
    return false;
}

bool http_get(void){
    bool tmp = false;
    switch (step_get)
    {
    case 0:
        tmp = http_get_entry();
        if (tmp){
            step_get++;
        }
        return false;
    
    case 1:
        return http_get_wait();

    default:
        break;
    }
    return false;
}


static bool http_read_entry(uint32_t* offset, uint32_t size){
    char cmd_str[32];
    sprintf(cmd_str, "AT+HTTPREAD=%lu,%lu", *offset, size);
    modem_at_cmd_t cmd = {
        .expect = "",
        .timeout_ms = 5000,
        .cb = httpread_callback
    };
    strcpy(cmd.cmd, cmd_str);
    return modem_send_at_cmd(cmd);
}

static bool http_read_wait(uint32_t* offset, uint32_t size){
    modem_event_t evt;
    if (!pop_event(&event_queue, &evt))
        return false;

    switch(evt){
        case EVT_OK:
            DEBUG_PRINT("HTTP GET OK\r\n");
            return true;    

        case EVT_ERROR:
            DEBUG_PRINT("HTTP GET ERR\r\n");
            return false; 

        case EVT_TIMEOUT:
            DEBUG_PRINT("HTTP GET TIMEOUT, RETRY\r\n");
            return false;
    }
    return false;
}

bool http_read(uint32_t* offset, uint32_t size){
    bool tmp = false;
    switch (step_read)
    {
    case 0:
        tmp = http_read_entry(offset, size);
        if (tmp){
            step_read++;
        }
        return false;
    
    case 1:
        if (http_read_wait(offset, size)) {
            step_read = 0;
            return true;
        }
        return false;
    }
    return false;
}