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