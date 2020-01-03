/*
 * main.c
 *
 *  Created on: Oct 13, 2019
 *  Author: Saaket Timsina
 */

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "hal/uart/uart.h"
#include "inc/tm4c1294ncpdt.h"

int main(void){
    char data[] = { 'B', 'A', '_', 'S', 'A', 'A', 'K', 'E', 'T', '_', 'T', 'I','M', 'S', 'I', 'N', 'A', '\0' };
    unsigned int dataLength = sizeof(data) / sizeof(char);

    char buffer[5];
    int bufferLength = sizeof(buffer) / sizeof(char);

    // Required values settings for UART2
    enum HalUartBaudRate uartBaudRate = uartBaudRate115200;
    enum HalUartWordLength uartWordlength = uartEightBits;
    enum HalUartStopBit uartStopBit = uartOneStopBit;
    enum HalUartParity uartParity = uartNoParity;



    // UART initialization
    halUartDriverInit();
   // HalUartCoreStateHandle uartCoreStateHandleTransmit = halUartCoreInit(uart2RxNoneTxPD5, uartBaudRate, uartWordlength, uartStopBit, uartParity);
   // HalUartCoreStateHandle uartCoreStateHandleReceive = halUartCoreInit(uart2RxPD4TxNone, uartBaudRate, uartWordlength, uartStopBit, uartParity);
    HalUartCoreStateHandle uartCoreStateHandleReceiveTransmit = halUartCoreInit(uart2RxPD4TxPD5, uartBaudRate, uartWordlength, uartStopBit, uartParity);


    while (1)
        {
          //uartTransmit(&data[0], dataLength,uartCoreStateHandleTransmit);
          //uartReceive(&buffer[0], bufferLength, uartCoreStateHandleReceive);
          uartReceive(&buffer[0], bufferLength, uartCoreStateHandleReceiveTransmit);
          uartTransmit(&data[0], dataLength,uartCoreStateHandleReceiveTransmit);
        }

}


