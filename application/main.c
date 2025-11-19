#include "hardware_map.h"
#include "systick.h"
#include "stm32f4xx_usart.h"
#include "usart.h"

int main(void) {
    systick_init(1000);
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

    while (1) {
        usart_sendstring(USART2, "Bdang\n");
    }
}