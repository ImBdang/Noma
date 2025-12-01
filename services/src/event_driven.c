#include "event_driven.h"


bool push_event(modem_event_queue_t* q, modem_event_t evt){
    uint8_t next = (q->tail + 1) % MODEM_EVENT_QUEUE_SIZE;

    if (next == q->head) 
        return false; 

    q->buf[q->tail] = evt;
    q->tail = next;
    return true;
}

bool pop_event(modem_event_queue_t* q, modem_event_t* evt){
    if (q->head == q->tail)
        return false; 

    *evt = q->buf[q->head];
    q->head = (q->head + 1) % MODEM_EVENT_QUEUE_SIZE;
    return true;
}

bool event_queue_is_empty(modem_event_queue_t* q){
    return (q->head == q->tail);
}

bool event_queue_is_full(modem_event_queue_t* q){
    return ((q->tail + 1) % MODEM_EVENT_QUEUE_SIZE) == q->head;
}

void event_queue_clear(modem_event_queue_t* q){
    q->head = 0;
    q->tail = 0;
}

void event_queue_init(modem_event_queue_t* q){
    q->head = 0;
    q->tail = 0;
}

/* ====================================== *** ======================================= */

void sms_event_queue_init(sms_event_queue_t* q)
{
    q->head = 0;
    q->tail = 0;
}

bool sms_event_queue_is_empty(sms_event_queue_t* q)
{
    return (q->head == q->tail);
}

bool sms_event_queue_is_full(sms_event_queue_t* q)
{
    return ((q->tail + 1) % SMS_EVENT_QUEUE_SIZE) == q->head;
}

void sms_event_queue_clear(sms_event_queue_t* q)
{
    q->head = 0;
    q->tail = 0;
}

bool sms_push_event(sms_event_queue_t* q, sms_evt_t evt)
{
    uint8_t next = (q->tail + 1) % SMS_EVENT_QUEUE_SIZE;

    if (next == q->head)
        return false;  

    q->buf[q->tail] = evt;
    q->tail = next;
    return true;
}

bool sms_pop_event(sms_event_queue_t* q, sms_evt_t* evt)
{
    if (q->head == q->tail)
        return false;  

    *evt = q->buf[q->head];
    q->head = (q->head + 1) % SMS_EVENT_QUEUE_SIZE;
    return true;
}

/* ====================================== *** ======================================= */

void urc_event_queue_init(urc_event_queue_t* q)
{
    q->head = 0;
    q->tail = 0;
}

bool urc_event_queue_is_empty(urc_event_queue_t* q)
{
    return (q->head == q->tail);
}

bool urc_event_queue_is_full(urc_event_queue_t* q)
{
    return ((q->tail + 1) % URC_EVENT_QUEUE_SIZE) == q->head;
}

void urc_event_queue_clear(urc_event_queue_t* q)
{
    q->head = 0;
    q->tail = 0;
}

bool urc_push_event(urc_event_queue_t* q, const urc_t* evt)
{
    uint8_t next = (q->tail + 1) % URC_EVENT_QUEUE_SIZE;

    if (next == q->head)
        return false;  // Queue is full

    q->buf[q->tail] = *evt;   // copy full struct
    q->tail = next;
    return true;
}

bool urc_pop_event(urc_event_queue_t* q, urc_t* evt)
{
    if (q->head == q->tail)
        return false;  // Queue empty

    *evt = q->buf[q->head];   // return full struct
    q->head = (q->head + 1) % URC_EVENT_QUEUE_SIZE;
    return true;
}



/* ====================================== *** ======================================= */
bool ota_event_queue_push(ota_event_queue_t* q, ota_event_t evt){
    uint8_t next = (q->tail + 1u) % URC_EVENT_OTA_SIZE;

    if (next == q->head) {
        /* queue full */
        return false;
    }

    q->buf[q->tail] = evt;
    q->tail = next;
    return true;
}

bool ota_event_queue_pop(ota_event_queue_t* q, ota_event_t* evt){
    if (q->head == q->tail) {
        /* queue empty */
        return false;
    }

    *evt = q->buf[q->head];
    q->head = (q->head + 1u) % URC_EVENT_OTA_SIZE;
    return true;
}

bool ota_event_queue_is_empty(ota_event_queue_t* q){
    return (q->head == q->tail);
}

bool ota_event_queue_is_full(ota_event_queue_t* q){
    return ((uint8_t)((q->tail + 1u) % URC_EVENT_OTA_SIZE) == q->head);
}

void ota_event_queue_clear(ota_event_queue_t* q){
    q->head = 0u;
    q->tail = 0u;
}

void ota_event_queue_init(ota_event_queue_t* q){
    q->head = 0u;
    q->tail = 0u;
}
