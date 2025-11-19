#include "A7600C1.h"

/**
 * @brief   Power on A7600C1
 */
void A7600_PowerOn(void) {
    SIM_PWKEY_HIGH();
    delay_ms(100);
    
    SIM_PWKEY_LOW();
    delay_ms(1500);
    
    SIM_PWKEY_HIGH();
    delay_ms(5000);
}


/**
 * @brief   Receive 1 byte 
 * 
 * @retval  1 if true
 * @retval  0 if false
 */
uint8_t UART_ReceiveByte(uint8_t* data) {
    if(USART_GetFlagStatus(SIM_USART_PORT, USART_FLAG_RXNE) == SET) {
        *data = (uint8_t)USART_ReceiveData(SIM_USART_PORT);
        return 1;
    }
    return 0; 
}

/**
 * @brief   Check f A7600C1 is OK
 * 
 * @retval  1 is OK
 * @retval  0 is NOT OK
 */
uint8_t A7600_CheckOK(void) {
    char response[32];
    uint8_t index = 0;
    uint8_t data;
    usart_sendstring(SIM_USART_PORT ,"AT\r\n");
    
    uint32_t start = get_systick_ms();
    while((get_systick_ms() - start) < 2000) {
        if(UART_ReceiveByte(&data)) {
            response[index++] = data;
            response[index] = '\0';
            
            if(strstr(response, "OK")) {
                return 1; 
            }
            if(index >= 31) break;
        }
    }
    return 0; 
}

/**
 * @brief   Init and check A7600C1
 * 
 * @retval  1 is true
 * @retval  0 is false
 */
uint8_t A7600_SimpleInit(void) {
    A7600_PowerOn();
    if(A7600_CheckOK()) {
        usart_sendstring(USART2, "AT is OK\n");
        return 1; 
    }
    delay_ms(500);
    usart_sendstring(USART2, "AT is not response\n");
    return 0; 
}

void A7600_GPIO_Init(void) {
    GPIO_InitTypeDef gpio_c = {
        .GPIO_Pin = SIM_PWKEY_Pin,
        .GPIO_Mode = GPIO_Mode_OUT,
        .GPIO_OType = GPIO_OType_PP,
        .GPIO_Speed = GPIO_Speed_2MHz,
        .GPIO_PuPd = GPIO_PuPd_NOPULL 
    };

    init_gpio(SIM_PWKEY_GPIO, &gpio_c);
}