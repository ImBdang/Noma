#ifndef __EVENT_S__
#define __EVENT_S__

#include "stdint.h"
#include "stdbool.h"
#include "stddef.h"

#define MODEM_EVENT_QUEUE_SIZE 16

/* ==================================== TYPEDEF ENUM =========================================== */
typedef enum {
    EVT_NONE = 0,

    EVT_AT_OK,
    EVT_AT_ERROR,
    EVT_AT_TIMEOUT,

    EVT_CPIN_READY,
    EVT_CPIN_NOT_READY,

    EVT_CREG_HOME,
    EVT_CREG_ROAM,
    EVT_CREG_SEARCHING,
    EVT_CREG_DENIED,

    EVT_CGATT_ATTACHED,
    EVT_CGATT_DETACHED,

    EVT_CGACT_ACTIVE,
    EVT_CGACT_INACTIVE,

} modem_event_t;


/* ==================================== RING QUEUE EVENT ======================================= */
typedef struct {
    modem_event_t buf[MODEM_EVENT_QUEUE_SIZE];
    uint8_t head;
    uint8_t tail;
} modem_event_queue_t;

/* ==================================== API DECLARATION ======================================== */
bool push_event(modem_event_t evt);
bool pop_event(modem_event_t* evt);
bool event_queue_is_empty(void);
bool event_queue_is_full(void);
void event_queue_init(modem_event_queue_t* q);

#endif /* __EVENT_S__ */