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

    while (USART_GetFlagStatus(USART1, USART_FLAG_RXNE)) {
        (void)USART_ReceiveData(USART1);
    }
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

    while (USART_GetFlagStatus(USART2, USART_FLAG_RXNE)) {
        (void)USART_ReceiveData(USART2);
    }
}

void usart1_interrupt_init(void){
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);

    NVIC_InitTypeDef nvic_config = {
        .NVIC_IRQChannel = USART1_IRQn,
        .NVIC_IRQChannelPreemptionPriority = 0,
        .NVIC_IRQChannelSubPriority = 0,
        .NVIC_IRQChannelCmd = ENABLE
    };
    NVIC_Init(&nvic_config);
}

void usart2_interrupt_init(void){
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);

    NVIC_InitTypeDef nvic_config = {
        .NVIC_IRQChannel = USART2_IRQn,
        .NVIC_IRQChannelPreemptionPriority = 0,
        .NVIC_IRQChannelSubPriority = 0,
        .NVIC_IRQChannelCmd = ENABLE
    };
    NVIC_Init(&nvic_config);
}
