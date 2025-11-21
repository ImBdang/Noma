#include "A7600C1.h"

/**
 * @brief   Power on A7600C1
 */
void A7600_PowerOn(void) {
    SIM_PWKEY_HIGH();
    delay_ms(500);
    
    SIM_PWKEY_LOW();
    delay_ms(500);
    
    SIM_PWKEY_HIGH();
}

void A7600_PowerOff(void)
{
    GPIO_SetBits(GPIOA, GPIO_Pin_0);
    delay_ms(50);

    GPIO_ResetBits(GPIOA, GPIO_Pin_0);
    delay_ms(1800);   

    GPIO_SetBits(GPIOA, GPIO_Pin_0);
    delay_ms(4000);
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
    memset(response, 0, sizeof(response)); 
    uint16_t idx = 0;
    uint32_t timeout = 5000; 
    uint32_t start_time = get_systick_ms();

    while(USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == SET) {
        USART_ReceiveData(USART1);
    }

    usart_sendstring(USART1, "AT\r\n");
    usart_sendstring(USART2, ">> AT\r\n");

    while ((get_systick_ms() - start_time) < timeout) {
        // if (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == SET) {
        //     uint8_t c = (uint8_t)USART_ReceiveData(USART1);
        //     USART_SendData(USART2, c);
        //     while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);
            
        //     if (idx < sizeof(response)-1) {
        //         response[idx++] = c;
        //         response[idx] = '\0';
        //     }
        //     if (strstr(response, "OK") != NULL) {
        //         return 1;
        //     }
        // }
    }
    
    usart_sendstring(USART2, "\r\n<< TIMEOUT\r\n");
    return 0;
}


/**
 * @brief   Send AT Command
 * 
 * @param   command: command that you need to send
 */
void AT_SendCmd(char* command) {
    usart_sendstring(USART1, command);
    usart_sendstring(USART1, "\r\n");
    usart_sendstring(USART2, ">> ");
    usart_sendstring(USART2, command);
    usart_sendstring(USART2, "\r\n");
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
        usart_sendstring(USART2, "\nAT is OK\n");
        return 1; 
    }
    usart_sendstring(USART2, "AT is not response\n");
    return 0; 
}

