#ifndef __USART__
#define __USART__

/* Include header ---------------------------------------------------------- */
#include "stdint.h"
#include "stm32f4xx_usart.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"

/* Struct ------------------------------------------------------------------ */
typedef struct {
    USART_TypeDef* USARTx;
    GPIO_TypeDef* TX_Port;
    uint16_t TX_Pin;
    uint8_t TX_PinSource;  
    uint8_t TX_AF;
    
    GPIO_TypeDef* RX_Port;
    uint16_t RX_Pin;
    uint8_t RX_PinSource;  
    uint8_t RX_AF;
    
    uint32_t baudrate;
} usart_c;


/* API declrations --------------------------------------------------------- */
void usart_init(usart_c* cfg); 
void usart_sendstring(USART_TypeDef *USARTx, const char *str);
uint8_t usart_receive(USART_TypeDef* USARTx);

#endif /* __USART__ */