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
