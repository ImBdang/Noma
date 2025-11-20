#include "hardware_map.h"
#include "gpio.h"
#include "systick.h"
#include <string.h>


void A7600_PowerOn(void);
uint8_t UART_ReceiveByte(uint8_t* data);
uint8_t A7600_CheckOK(void);
uint8_t A7600_SimpleInit(void);
void A7600_GPIO_Init(void);
void AT_SendCmd(char* command);