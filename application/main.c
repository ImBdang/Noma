/**
 * @brief Blink led in GPIO_9 PORT_B
 * 
 * @note This code was created for my custom module, so that why its name led green
 */
#include "hardware.h"

volatile uint32_t msTicks = 0;

int main(void) {

  led_green_init();
  while (1) {
    led_green_blink(1000);
  }
}