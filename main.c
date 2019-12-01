/*
 * main.c
 *
 *  Created on: Oct 13, 2019
 *      Author: saaket
 */

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "hal/uart/uart.h"
#include "inc/tm4c1294ncpdt.h"

int main(void){
    char data[] = { 'B', 'A', '_', 'S', 'A', 'A', 'K', 'E', 'T', '_', 'T', 'I','M', 'S', 'I', 'N', 'A', '\0' };
    int dataLength = sizeof(data) / sizeof(char);

    //char buffer[5];
   // int bufferLength = sizeof(buffer) / sizeof(char);

 /*   // UART Receive

    // Required values settings for UART2_Tx at Port D5
    enum HalUartCoreAtPort_ uartCoreAtPort = uart2RxPortD;
    enum HalUartCoreInUseStatus_ uartCoreInUseStatus = uartCoreNotInUse;
    enum HalPortInUseStatus_ portInUseStatus = uartPortNotInUse;
    enum HalUartBaudRate_ uartBaudRate = uartBaudRate115200;
    enum HalUartWordLength_ uartWordlength = uartEightBits;
    enum HalUartStopBit_ uartStopBit = uartOneStopBit;
    enum HalUartParity_ uartParity = uartNoParity;

    HalUartCoreStateHandle uartCoreStateHandle = halUartInit(
            uartCoreAtPort, uartCoreInUseStatus, portInUseStatus, uartBaudRate, uartWordlength, uartStopBit, uartParity);

    while(1){
        printf("Content of Data Buffer\n");
        fflush(stdout);
       // uartReceive(&buffer[0], bufferLength, uartCoreStateHandle);
    }
*/

    // UART Transmit

    // Required values settings for UART2_Tx at Port D5
    enum HalUartCoreAtPort_ uartCoreAtPort = uart2TxPortD;
    enum HalUartCoreInUseStatus_ uartCoreInUseStatus = uartCoreNotInUse;
    enum HalPortInUseStatus_ portInUseStatus = uartPortNotInUse;
    enum HalUartBaudRate_ uartBaudRate = uartBaudRate115200;
    enum HalUartWordLength_ uartWordlength = uartEightBits;
    enum HalUartStopBit_ uartStopBit = uartOneStopBit;
    enum HalUartParity_ uartParity = uartNoParity;



    // UART initialization
    HalUartCoreStateHandle uartCoreStateHandle = halUartInit(
            uartCoreAtPort, uartCoreInUseStatus, portInUseStatus, uartBaudRate, uartWordlength, uartStopBit, uartParity);

    while (1)
        {
          uartTransmit(&data[0], dataLength,uartCoreStateHandle);
        }

}


