#include "bdang_regular_func.h"

void bdang_init_systick(uint32_t tick_hz){
    SystemCoreClockUpdate();
    if (SysTick_Config(SystemCoreClock / tick_hz) != 0) {
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