#ifndef __EVENT_S__
#define __EVENT_S__

#include "stdint.h"
#include "stdbool.h"
#include "stddef.h"

#define MODEM_EVENT_QUEUE_SIZE 16
#define SMS_EVENT_QUEUE_SIZE 16
#define URC_EVENT_QUEUE_SIZE 16

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

typedef enum {
    URC_EVT_NONE = 0,

    /* SMS URC */
    URC_EVT_SMS_NEW,          // +CMTI: có tin nhắn mới
    URC_EVT_SMS_RECEIVED,     // +CMT: modem tự gửi nội dung SMS đến MCU (nếu CNMI cho phép)

    /* CALL URC */
    URC_EVT_CALL_RING,        // RING: có cuộc gọi đến
    URC_EVT_CALL_CLIP,        // +CLIP: số gọi đến
    URC_EVT_CALL_DISCONNECTED,// NO CARRIER / BUSY / NO ANSWER
    URC_EVT_CALL_CONNECTED,   // CONNECT (rất hiếm khi modem URC cái này)

    /* NETWORK URC */
    URC_EVT_NET_REG_STATUS,   // +CREG, +CEREG, +CGREG
    URC_EVT_NET_DETACHED,     // rớt mạng (status 0,2,3)

    /* SIGNAL URC */
    URC_EVT_SIGNAL_UPDATE,     // +CSQ (nếu modem tự đẩy)
    
    /* DATA / PDP URC */
    URC_EVT_PDP_ACTIVE,        // PDP ACT
    URC_EVT_PDP_DEACT,         // PDP DEACT
    URC_EVT_DATA_RECV,         // QIURC: "recv"
    URC_EVT_DATA_ACCEPT,       // QIURC: "send ok"/others

    /* SIM CARD URC */
    URC_EVT_SIM_READY,         // +CPIN: READY
    URC_EVT_SIM_NOT_INSERTED,  // +CPIN: NOT INSERTED
    URC_EVT_SIM_PIN_REQUIRED,  // +CPIN: SIM PIN

    /* POWER / SYSTEM URC */
    URC_EVT_PWR_DOWN,          // POWER DOWN
    URC_EVT_PWR_UP,            // RDY hoặc PB DONE
    URC_EVT_RESET,             // RESET

} urc_event_t;


typedef struct {
    urc_event_t type;

    union {

        /* +CMTI (SMS new index) */
        struct {
            uint8_t index;
        } sms_new;

        /* +CMT (SMS received with content) */
        struct {
            char sender[32];     
            char content[256];   
        } sms_recv;

        /* +CLIP / RING */
        struct {
            char number[32];
        } call;

        /* +CREG / +CEREG / +CGREG */
        struct {
            int stat;     
            int lac;      
            int cell_id;  
        } net;

        /* +CSQ */
        struct {
            int rssi;
            int ber;
        } signal;

        /* QIURC: "recv",... */
        struct {
            int pdp_id;
            int data_len;
        } data;

        /* +CPIN: READY / NOT INSERTED / SIM PIN */
        struct {
            int sim_status;
        } sim;

    } info;

} urc_t;


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

typedef struct {
    urc_t buf[URC_EVENT_QUEUE_SIZE];
    uint8_t head;
    uint8_t tail;
} urc_event_queue_t;

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


void urc_event_queue_init(urc_event_queue_t* q);
bool urc_event_queue_is_empty(urc_event_queue_t* q);
bool urc_event_queue_is_full(urc_event_queue_t* q);
void urc_event_queue_clear(urc_event_queue_t* q);
bool urc_push_event(urc_event_queue_t* q, const urc_t* evt);
bool urc_pop_event(urc_event_queue_t* q, urc_t* evt);

#endif /* __EVENT_S__ */