#ifndef __SYSTICK__
#define __SYSTICK__

/* Include header ---------------------------------------------- */
#include "stdint.h"
#include "stm32f4xx.h"
#include "core_cm4.h"
#include "system_stm32f4xx.h"
#include "stm32f4xx_it.h"


/* API declaratations ------------------------------------------ */
void init_systick(uint32_t ms);
uint32_t get_systick_ms(void);
void delay_ms(uint32_t ms);
uint32_t get_systick_ms(void);

#endif /* __SYSTICK__ */