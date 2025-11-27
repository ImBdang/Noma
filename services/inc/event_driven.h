#ifndef __EVENT_S__
#define __EVENT_S__

#include "stdint.h"
#include "stdbool.h"
#include "stddef.h"

#define MODEM_EVENT_QUEUE_SIZE 16
#define SMS_EVENT_QUEUE_SIZE 16

/* ==================================== TYPEDEF ENUM =========================================== */
typedef enum {
    EVT_NONE = 0,

    EVT_OK,
    EVT_ERROR,
    EVT_TIMEOUT,

    EVT_CPIN_READY,
    EVT_CPIN_PIN,
    EVT_CPIN_PUK,

    EVT_CREG_NOT_REGISTERED,        // 0,0
    EVT_CREG_REGISTERED_HOME,       // 0,1
    EVT_CREG_SEARCHING,             // 0,2
    EVT_CREG_REG_DENIED,            // 0,3
    EVT_CREG_UNKNOWN,               // 0,4
    EVT_CREG_REGISTERED_ROAMING,    // 0,5
    EVT_CREG_REGISTERED_SMS,        // 0,6
    EVT_CREG_REGISTERED_SMS_ROAM,   // 0,7

    EVT_CGATT_ATTACHED,
    EVT_CGATT_DETACHED,

    EVT_CGACT_ACTIVE,
    EVT_CGACT_INACTIVE,

} modem_event_t;

typedef enum {
    SMS_EVT_NONE = 0,

    SMS_EVT_OK,
    SMS_EVT_ERROR,
    SMS_EVT_TIMEOUT,

    SMS_EVT_PROMPT,        
    SMS_EVT_CMGS_ACCEPT,   
    SMS_EVT_CMGS_FAIL,     

    SMS_EVT_SENT,
    SMS_EVT_FAILED

} sms_evt_t;



/* ==================================== RING QUEUE EVENT ======================================= */
typedef struct {
    modem_event_t buf[MODEM_EVENT_QUEUE_SIZE];
    uint8_t head;
    uint8_t tail;
} modem_event_queue_t;

typedef struct {
    sms_evt_t buf[SMS_EVENT_QUEUE_SIZE];
    uint8_t head;
    uint8_t tail;
} sms_event_queue_t;

/* ==================================== API DECLARATION ======================================== */
bool push_event(modem_event_queue_t* q, modem_event_t evt);
bool pop_event(modem_event_queue_t* q, modem_event_t* evt);
bool event_queue_is_empty(modem_event_queue_t* q);
bool event_queue_is_full(modem_event_queue_t* q);
void event_queue_clear(modem_event_queue_t* q);
void event_queue_init(modem_event_queue_t* q);

bool sms_push_event(sms_event_queue_t* q, sms_evt_t evt);
bool sms_pop_event(sms_event_queue_t* q, sms_evt_t* evt);
void sms_event_queue_init(sms_event_queue_t* q);
bool sms_event_queue_is_empty(sms_event_queue_t* q);
bool sms_event_queue_is_full(sms_event_queue_t* q);
void sms_event_queue_clear(sms_event_queue_t* q);


#endif /* __EVENT_S__ */