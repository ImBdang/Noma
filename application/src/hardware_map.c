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

void A7600_GPIO_Init(void) {
    GPIO_InitTypeDef gpio_c = {
        .GPIO_Pin = SIM_PWKEY_Pin,
        .GPIO_Mode = GPIO_Mode_OUT,
        .GPIO_OType = GPIO_OType_PP,
        .GPIO_Speed = GPIO_Speed_50MHz,
        .GPIO_PuPd = GPIO_PuPd_NOPULL 
    };
    init_gpio(SIM_PWKEY_GPIO, &gpio_c);

    gpio_c.GPIO_Pin = SIM_STATUS_Pin;
    gpio_c.GPIO_Mode = GPIO_Mode_IN;
    gpio_c.GPIO_PuPd = GPIO_PuPd_DOWN;
    init_gpio(SIM_STATUS_GPIO, &gpio_c);
}


void bdang_init_clock(void){
  RCC_HSICmd(ENABLE);
  while(RCC_GetFlagStatus(RCC_FLAG_HSIRDY) == RESET);
  RCC_PLLConfig(
    RCC_PLLSource_HSI,
    8,                            // M: 16/8 = 2MHz
    192,                          // N: 2×192 = 384MHz 
    6,                            // P: 384/6 = 64MHz 
    8                             // Q: 384/8 = 48MHz 
  );
  RCC_PLLCmd(ENABLE);
  while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET){}; 
  FLASH_SetLatency(FLASH_ACR_LATENCY_2WS);
  RCC_HCLKConfig(RCC_SYSCLK_Div1);
  RCC_PCLK1Config(RCC_HCLK_Div2);
  RCC_PCLK2Config(RCC_HCLK_Div1);
  RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
  SystemCoreClockUpdate();
}



/**
 * @brief   Init hardware 
 * 
 * @note    Init usart1, 2, led green, systick, A7600 GPIO
 */
void hardware_init(void){
    systick_init(1000);
    usart1_init();
    usart2_init();
    usart1_interrupt_init();
    led_green_init();
    A7600_GPIO_Init();
}
