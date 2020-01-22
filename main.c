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

/**
 * Constant specifying the length of the message, i.e.
 * number of bytes to be received (Rx) or send (Tx).
 *
 * It is preferable to use enums instead of
 * #define messageLength_
 *
 * The underscore (_) is a naming convention.
 * It tells us that the type is not public, i.e.
 * it will not be used outside of this file.
 */
enum MessageLength_ {
    messageLength_ = 6
};

/**
 * Static variables will not be seen outside
 * of their compilation unit.
 */
// extra character for the '\0' termination of the string
static char messageBuffer_[messageLength_];
static char const greetingMessage[messageLength_ + 1] = "Echo:\n";


int main(void) {

    // UART driver initialization
    halUartDriverInit();

    // UART2 core initialization paramters
    enum HalUartBaudRate uartBaudRate = uartBaudRate115200;
    enum HalUartWordLength uartWordlength = uartEightBits;
    enum HalUartStopBit uartStopBit = uartOneStopBit;
    enum HalUartParity uartParity = uartNoParity;

    // we may use a simple easy name for a variable here,
    // as it is used only locally
    HalUartCoreStateHandle handle = halUartCoreInit(uart2RxPD4TxPD5,
        uartBaudRate, uartWordlength, uartStopBit, uartParity);

    // greeting message
    halUartTx(&greetingMessage[0], messageLength_, handle);

    // echo functionality
    while(1)
    {
        halUartRx(&messageBuffer_[0], messageLength_, handle);
        halUartTx(&messageBuffer_[0], messageLength_, handle);
    }

}
