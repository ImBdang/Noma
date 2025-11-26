#ifndef __LEDSTATE__
#define __LEDSTATE__

#include "stm32f4xx_gpio.h"
#include "hardware_map.h"

#define DEFAULT_TIME            500
#define USER_MODE_TIME          1000
#define IMPORTANT_MODE_TIME     200

/* API --------------------------------------------------------------------------- */
void led_blink_default(void);
void led_user_mode(void);
void led_blink_important(void);
void led_notify_user_mode(void);

#endif /* __LEDSTATE__ */