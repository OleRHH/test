/*
 * main.c
 *
 *  Created on: Oct 13, 2019
 *  Author: Saaket Timsina
 */

#include <stdint.h>
#include <stdio.h>
#include "hal/uart/uart.h"
#include "inc/tm4c1294ncpdt.h"

enum MessageLength_ {
    messageLength_ = 6
};

static char const greetingMessage[messageLength_ + 1] = "Echo:\n";
static char buffer[messageLength_];

int main(void) {

    // UART driver initialization
    halUartDriverInit();

    // UART2 core initialization
    enum HalUartBaudRate uartBaudRate = uartBaudRate115200;
    enum HalUartWordLength uartWordlength = uartEightBits;
    enum HalUartStopBit uartStopBit = uartOneStopBit;
    enum HalUartParity uartParity = uartNoParity;
    HalUartCoreStateHandle uartCoreStateHandleReceiveTransmit = halUartCoreInit(uart2RxPD4TxPD5, uartBaudRate, uartWordlength, uartStopBit, uartParity);

    // greeting message
    halUartTx(&greetingMessage[0], messageLength_, uartCoreStateHandleReceiveTransmit);

    // echo functionality
    while (1)
    {
        halUartRx(&buffer[0], messageLength_, uartCoreStateHandleReceiveTransmit);
        halUartTx(&buffer[0], messageLength_, uartCoreStateHandleReceiveTransmit);
    }

}


