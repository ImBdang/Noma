#ifndef __MODEM_SMS_S__
#define __MODEM_SMS_S__

#include "modem_service_fsm.h"
#include "modem_api.h"
#include "usart_api.h"
#include "event_driven.h"
#include "stdbool.h"

/* =============================== TYPDEF STRUCT =============================== */
typedef struct {
    uint8_t id;
    bool is_send;
    char phone[14 + 1];
    char message[255 + 1];
} sms_message_t;

typedef struct {
    char text_mode[12];           
    char encodeing_mode[12];  
} sms_config_t;

typedef enum {
    SMS_STATE_INIT = 0,

    SMS_STATE_SET_FORMAT,
    SMS_STATE_WAIT_SET_FORMAT,

    SMS_STATE_SET_ENCODING,
    SMS_STATE_WAIT_SET_ENCODING,

    SMS_STATE_SET_NUMBER,
    SMS_STATE_WAIT_SET_NUMBER,

    SMS_STATE_SEND_TEXT,
    SMS_STATE_WAIT_RESULT,
    
    SMS_STATE_DONE,
    SMS_STATE_ERROR

} sms_state_t;

/* =============================== API DECLARATIONS =============================== */
bool send_sms(const char* str, const char* phone);

#endif /* __MODEM_SMS_S__ */