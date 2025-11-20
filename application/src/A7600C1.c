#include "A7600C1.h"

/**
 * @brief   Power on A7600C1
 */
void A7600_PowerOn(void) {
    usart_sendstring(USART2, "Power ON sequence start\r\n");
    
    // Đảm bảo PWKEY HIGH ban đầu
    SIM_PWKEY_HIGH();
    delay_ms(500);
    
    // Kéo LOW 500ms
    SIM_PWKEY_LOW();
    usart_sendstring(USART2, "PWKEY LOW\r\n");
    delay_ms(500);
    
    // Thả về HIGH
    SIM_PWKEY_HIGH();
    usart_sendstring(USART2, "PWKEY HIGH\r\n");
    delay_ms(500);
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
 * @brief   Check if A7600C1 is OK
 * 
 * @retval  1 is OK
 * @retval  0 is NOT OK
 */

uint8_t A7600_CheckOK(void) {
    char response[256];
    memset(response, 0, sizeof(response)); // Xóa buffer
    uint16_t idx = 0;
    uint32_t timeout = 5000; // Tăng lên 5s
    uint32_t start_time = get_systick_ms();

    // Xóa UART buffer
    while(USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == SET) {
        USART_ReceiveData(USART1);
    }

    // Gửi AT
    usart_sendstring(USART1, "AT\r\n");
    usart_sendstring(USART2, ">>> AT\r\n");

    while ((get_systick_ms() - start_time) < timeout) {
        if (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == SET) {
            uint8_t c = (uint8_t)USART_ReceiveData(USART1);
            
            // Echo ra USART2
            USART_SendData(USART2, c);
            while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);
            
            if (idx < sizeof(response)-1) {
                response[idx++] = c;
                response[idx] = '\0';
            }
            
            // Check cho "OK"
            if (strstr(response, "OK") != NULL) {
                usart_sendstring(USART2, "\r\n<<< OK FOUND!\r\n");
                return 1;
            }
        }
    }
    
    usart_sendstring(USART2, "\r\n<<< TIMEOUT\r\n");
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
    delay_ms(15000);
    if(A7600_CheckOK()) {
        usart_sendstring(USART2, "AT is OK\n");
        return 1; 
    }
    usart_sendstring(USART2, "AT is not response\n");
    return 0; 
}

void A7600_GPIO_Init(void) {
    GPIO_InitTypeDef gpio_c = {
        .GPIO_Pin = SIM_PWKEY_Pin,
        .GPIO_Mode = GPIO_Mode_OUT,
        .GPIO_OType = GPIO_OType_PP,
        .GPIO_Speed = GPIO_Speed_50MHz,
        .GPIO_PuPd = GPIO_PuPd_NOPULL 
    };
    init_gpio(SIM_PWKEY_GPIO, &gpio_c);

    gpio_c.GPIO_Pin = SIM_STATUS_Pin;
    gpio_c.GPIO_Mode = GPIO_Mode_IN;
    gpio_c.GPIO_PuPd = GPIO_PuPd_DOWN;
    init_gpio(SIM_STATUS_GPIO, &gpio_c);
}