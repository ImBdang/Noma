#include "gpio.h"

void init_gpio(GPIO_TypeDef* GPIOx, GPIO_InitTypeDef* GPIO_InitStruct) {
    if (GPIOx == GPIOA)  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    else if (GPIOx == GPIOB)  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
    else if (GPIOx == GPIOC)  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
    else if (GPIOx == GPIOD)  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
    else if (GPIOx == GPIOE)  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
    else if (GPIOx == GPIOF)  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);
    else if (GPIOx == GPIOG)  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);
    else if (GPIOx == GPIOH)  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOH, ENABLE);
    else if (GPIOx == GPIOI)  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOI, ENABLE);
    else return; 

    GPIO_Init(GPIOx, GPIO_InitStruct);
}

void GPIO_SetAF_Mask(GPIO_TypeDef* GPIOx, uint16_t pin_mask, uint8_t af){
    for (uint8_t pin_source = 0; pin_source < 16; pin_source++) {
        if (pin_mask & (1 << pin_source)) {
            GPIO_PinAFConfig(GPIOx, pin_source, af);
        }
    }
}