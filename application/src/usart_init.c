#include "usart_init.h"

void usart1_init(void){
    usart_c usart_config = {
        .USARTx = USART1,
        .TX_Port = GPIOA,
        .TX_Pin = GPIO_Pin_9,
        .TX_PinSource = GPIO_PinSource9,
        .TX_AF = GPIO_AF_USART1,
        .RX_Port = GPIOA,
        .RX_Pin = GPIO_Pin_10,
        .RX_PinSource = GPIO_PinSource10,
        .RX_AF = GPIO_AF_USART1,
        .baudrate = 115200
    };
    usart_init(&usart_config);
}

void usart2_init(void){
    usart_c usart_config = {
        .USARTx = USART2,
        .TX_Port = GPIOA,
        .TX_Pin = GPIO_Pin_2,
        .TX_PinSource = GPIO_PinSource2,
        .TX_AF = GPIO_AF_USART2,
        .RX_Port = GPIOA,
        .RX_Pin = GPIO_Pin_3,
        .RX_PinSource = GPIO_PinSource3,
        .RX_AF = GPIO_AF_USART2,
        .baudrate = 115200
    };
    usart_init(&usart_config);
}