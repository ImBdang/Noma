#include "led_state.h"

void led_blink_default(void){
    GPIO_ToggleBits(LED_4G_PORT, LED_4G_PIN);
    delay_ms(DEFAULT_TIME);
}

void led_user_mode(void){
    GPIO_ToggleBits(LED_4G_PORT, LED_4G_PIN);
    delay_ms(USER_MODE_TIME);
}

void led_blink_important(void){
    GPIO_ToggleBits(LED_4G_PORT, LED_4G_PIN);
    delay_ms(IMPORTANT_MODE_TIME);
}

void led_notify_user_mode(void){
    GPIO_ResetBits(LED_4G_PORT, LED_4G_PIN);
    delay_ms(1500);

    GPIO_SetBits(LED_4G_PORT, LED_4G_PIN);
    delay_ms(300);
    GPIO_ResetBits(LED_4G_PORT, LED_4G_PIN);
    delay_ms(300);
    GPIO_SetBits(LED_4G_PORT, LED_4G_PIN);
    delay_ms(300);
    GPIO_ResetBits(LED_4G_PORT, LED_4G_PIN);
    delay_ms(1000);

    GPIO_SetBits(LED_4G_PORT, LED_4G_PIN);
    delay_ms(300);
    GPIO_ResetBits(LED_4G_PORT, LED_4G_PIN);
    delay_ms(300);
    GPIO_SetBits(LED_4G_PORT, LED_4G_PIN);
    delay_ms(300);
    GPIO_ResetBits(LED_4G_PORT, LED_4G_PIN);
    delay_ms(300);
    GPIO_SetBits(LED_4G_PORT, LED_4G_PIN);
    delay_ms(300);
    GPIO_ResetBits(LED_4G_PORT, LED_4G_PIN);
    delay_ms(300);

    GPIO_ResetBits(LED_4G_PORT, LED_4G_PIN);
    delay_ms(1500);
}