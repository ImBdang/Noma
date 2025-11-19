#include "systick.h"

volatile uint32_t msTicks = 0;

void systick_init(uint32_t ms){
    SystemCoreClockUpdate();
    if (SysTick_Config(SystemCoreClock / ms) != 0) {
        while(1); 
    }
}

void delay_ms(uint32_t ms) {
    uint32_t start = msTicks;
    while ((msTicks - start) < ms);
}

uint32_t get_systick_ms(void){
    return msTicks;
}