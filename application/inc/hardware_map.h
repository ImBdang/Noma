/* STM32f401CCU6 */

#ifndef __HARDWARE_H__
#define __HARDWARE_H__

/* Include header ----------------------------------------------------- */
#include <stdint.h>
#include <stddef.h>
#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_usart.h"
#include "core_cm4.h"
#include "system_stm32f4xx.h"
#include "stm32f4xx_it.h"

#include "systick.h"
#include "usart_init.h"
#include "gpio.h"

typedef enum {
    POWER_ON,
    WAIT_APP_RDY,
    WAIT_SIM_READY,
    WAIT_NETWORK,
    WAIT_PDP,
    READY
} module_state_t;

#define LED_ON 1
#define LED_OFF 0

#define HW_MAX_CHANNEL_SUPPORT      4
#define HW_MAX_BUTTON      			(4 +1) // +1 for sensor pin
#define HW_MAX_LED					2

/* Define Led indicate status (Pair, reset default) */
#define LED_GREEN_NETWORK_PIN GPIO_Pin_9
#define LED_GREEN_NETWORK_PORT GPIOB
#define LED_BLUE_STATEMACHINE_PIN GPIO_Pin_4
#define LED_BLUE_STATEMACHINE_PORT GPIOB

#define LED_4G_PIN LED_GREEN_NETWORK_PIN
#define LED_4G_PORT LED_GREEN_NETWORK_PORT
#define LED_STT_PIN LED_BLUE_STATEMACHINE_PIN
#define LED_STT_PORT LED_BLUE_STATEMACHINE_PORT

#define LED_4G_INDEX 0
#define LED_STT_INDEX 1

/* Define Button on board */
#define BTN_1_PIN GPIO_Pin_0
#define BTN_1_PORT GPIOA
#define BTN_2_PIN GPIO_Pin_1
#define BTN_2_PORT GPIOA
#define BTN_3_PIN GPIO_Pin_0
#define BTN_3_PORT GPIOB
#define BTN_4_PIN GPIO_Pin_1
#define BTN_4_PORT GPIOB
/* Define SENSOR INPUT PIN */
#define SENSOR_PIN GPIO_Pin_4
#define SENSOR_PORT GPIOA

/* Define Switch Pin */
#define OS_SWITCH_1_GPIO_PORT GPIOB
#define OS_SWITCH_1_GPIO_PIN GPIO_Pin_5
#define SWITCH_1_READ_PIN() \
    GPIO_ReadInputDataBit(OS_SWITCH_1_GPIO_PORT, OS_SWITCH_1_GPIO_PIN)

#define OS_SWITCH_2_GPIO_PORT GPIOB
#define OS_SWITCH_2_GPIO_PIN GPIO_Pin_7
#define SWITCH_2_READ_PIN() \
    GPIO_ReadInputDataBit(OS_SWITCH_2_GPIO_PORT, OS_SWITCH_2_GPIO_PIN)

#define OS_SWITCH_3_GPIO_PORT GPIOB
#define OS_SWITCH_3_GPIO_PIN GPIO_Pin_6
#define SWITCH_3_READ_PIN() \
    GPIO_ReadInputDataBit(OS_SWITCH_3_GPIO_PORT, OS_SWITCH_3_GPIO_PIN)

#define OS_SWITCH_4_GPIO_PORT GPIOB
#define OS_SWITCH_4_GPIO_PIN GPIO_Pin_8
#define SWITCH_4_READ_PIN() \
    GPIO_ReadInputDataBit(OS_SWITCH_4_GPIO_PORT, OS_SWITCH_4_GPIO_PIN)

/* VBAT */
#define OS_GSM_VBAT_PORT GPIOB
#define OS_GSM_VBAT_PIN GPIO_Pin_3

/* Define PC UART pin */
#define OS_PC_UART_TX_GPIO_PIN GPIO_Pin_2
#define OS_PC_UART_RX_GPIO_PIN GPIO_Pin_3
#define OS_PC_UART_GPIO_PORT GPIOA
#define OS_PC_UART_INSTANCE USART2

/* Define GSM Uart Pin */
#define SIM_USART_Tx_Pin GPIO_Pin_9
#define SIM_USART_Rx_Pin GPIO_Pin_10
#define SIM_USART_GPIO GPIOA
#define SIM_USART_PORT USART1

#define SIM_STATUS_Pin GPIO_Pin_12
#define SIM_STATUS_GPIO GPIOA

#define SIM_PWKEY_Pin GPIO_Pin_8
#define SIM_PWKEY_GPIO GPIOA

#define SIM_PWKEY_LOW() GPIO_ResetBits(SIM_PWKEY_GPIO, SIM_PWKEY_Pin)
#define SIM_PWKEY_HIGH() GPIO_SetBits(SIM_PWKEY_GPIO, SIM_PWKEY_Pin)

/* system */
void hardware_init(void);

/* button */
uint32_t button_read(uint32_t pin);
void button_config(uint32_t pin);

/* gpio */
void hw_led_config(uint8_t pin);
void hw_led_write(uint8_t pin, uint8_t state);
void switch_init(void);
uint8_t switch_read(uint8_t index);

/* Bdang code ---------------------------------------------------*/
void led_green_init(void);
void led_blue_init(void);
void led_green_blink(uint32_t ms);
void A7600_GPIO_Init(void);
void bdang_init_clock(void);

#endif
