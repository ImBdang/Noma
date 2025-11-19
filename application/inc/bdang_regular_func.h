#ifndef BDANG_REGULAR_FUNC
#define BDANG_REGULAR_FUNC

#include "stdint.h"
#include "stm32f4xx.h"
#include "core_cm4.h"
#include "system_stm32f4xx.h"
#include "stm32f4xx_it.h"

extern volatile uint32_t msTicks;

void delay_ms(uint32_t ms);
uint32_t get_systick_ms(void);
void bdang_init_systick(uint32_t tick_hz);

#endif