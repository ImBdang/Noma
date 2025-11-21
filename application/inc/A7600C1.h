#include "hardware_map.h"
#include "gpio.h"
#include "systick.h"
#include <string.h>

typedef enum {
    SIM_STATE_NOT_INSERTED,
    SIM_STATE_READY,

    NET_STATE_NOT_REGISTERED,
    NET_STATE_REGISTERED_HOME,
    NET_STATE_REGISTERED_ROAMING,

    DATA_STATE_PDP_NOT_ACTIVE,
    DATA_STATE_PDP_ACTIVE
} sim_state_t;



void A7600_PowerOn(void);
void A7600_PowerOff(void);
uint8_t UART_ReceiveByte(uint8_t* data);
uint8_t A7600_SimpleInit(void);
void AT_SendCmd(char* command);