#include "event_driven.h"

/* ====================================== DECLARATIONS ======================================= */
extern modem_event_queue_t event_queue;
/* =========================================================================================== */

bool push_event(modem_event_t evt){
    uint8_t next = (event_queue.tail + 1) % MODEM_EVENT_QUEUE_SIZE;
    if (next == event_queue.head)
        return false; // full
    event_queue.buf[event_queue.tail] = evt;
    event_queue.tail = next;
    return true;
}

bool pop_event(modem_event_t* evt){
    if (event_queue.head == event_queue.tail)
        return false; // empty
    *evt = event_queue.buf[event_queue.head];
    event_queue.head = (event_queue.head + 1) % MODEM_EVENT_QUEUE_SIZE;
    return true;
}

void event_queue_init(modem_event_queue_t* q){
    q->head = 0;
    q->tail = 0;
}
