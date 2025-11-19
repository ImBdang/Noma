/**
 * @brief Blink led in GPIO_9 PORT_B
 * 
 * @note This code was created for my custom module, so that why its name led green
 */
#include "hardware_map.h"
#include "systick.h"

int main(void) {
    init_systick(1000);
    led_green_init();
    while (1) {
        led_green_blink(1000);
    }
}