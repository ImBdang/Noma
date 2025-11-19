#include "hardware_map.h"

/**
 * @brief Init GPIO_9, PORT_B, 25Mhz speed, mode out, NOPULL
 * 
 * @retval None
 */
void led_green_init(void){
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.GPIO_Pin = LED_4G_PIN;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_25MHz;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(LED_4G_PORT, &GPIO_InitStruct);
}

/**
 * @brief Blink green led on the custom module (GPIO_9 PORT_B)
 * 
 * @note Remember init systick first
 * 
 * @retval None
 */
void led_green_blink(uint32_t ms){
    GPIO_SetBits(LED_4G_PORT, LED_4G_PIN);
    delay_ms(ms);
    GPIO_ResetBits(LED_4G_PORT, LED_4G_PIN);
    delay_ms(ms);
}

void led_blue_init(void){
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.GPIO_Pin = LED_STT_PIN;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_25MHz;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(LED_STT_PORT, &GPIO_InitStruct);
}


void hw_led_write(uint8_t index, uint8_t state){
    uint16_t pin;
    GPIO_TypeDef* port;
    
    if(index == LED_4G_INDEX) {
        pin = LED_4G_PIN;
        port = LED_4G_PORT;
    } else if(index == LED_STT_INDEX) {
        pin = LED_STT_PIN;
        port = LED_STT_PORT;
    } else {
        return; 
    }
    
    if (state == 1){
        GPIO_SetBits(port, pin);
    } else {
        GPIO_ResetBits(port, pin);
    }
}

/**
 * @brief   Init hardware 
 * 
 * @note    Init usart1, 2, led green, systick, A7600 GPIO
 */
void hardware_init(void){
    usart1_init();
    usart2_init();
    systick_init(1000);
    led_green_init();
    A7600_GPIO_Init();
}
