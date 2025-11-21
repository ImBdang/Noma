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

/**
 * @brief   Poweroff the module A7600C1
 */
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

