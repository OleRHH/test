/*****************************************************************************
 *  uart.h
 *  Created on: 11/3/2019
 *  Author: Saaket Timsina.
 *
 *  THIS API ALLOWS AN USER TO CONFIGURE AN UART TO IT'S CORRESPONDING
 *  PORTS TO PERFORM DATA TRANSMISSION  OR RECEIPETION OR BOTH.
 *  BLOCKING API
 *****************************************************************************/

/**
 * This defines, including the very last line of the code
 * are used to prevent multiple inclusion of the header
 * from different files.
 */
#ifndef HAL_UART_INCLUDE_HAL_UART_UART_H_
#define HAL_UART_INCLUDE_HAL_UART_UART_H_

/**
 * <b>UART and it's Corresponding Ports<b>.
 * This allows user to choose an UART.
 * <br>eg:
 * <br><b>uart0RxPA0TxNone :</b> UART 0 , only as a Receiver at Port A pin 0
 * <br><b>uart0RxNoneTxPA1 :</b> UART 0 , only as a Transmitter at Port A pin 1
 * <br><b>uart0RxPA0TxPA1 :</b> UART 0 , as a Receiver at Port A pin 0 and
 *  as a Transmitter at Port A pin 1
 */
enum HalUartCoreAtPort
{
    //UART0
    uart0RxPA0TxNone = 0,
    uart0RxNoneTxPA1 = 1,
    uart0RxPA0TxPA1 = 2,

    //UART1
    uart1RxPB0TxNone = 3,
    uart1RxPQ4TxNone = 4,
    uart1RxNoneTxPB1 = 5,
    uart1RxPB0TxPB1 = 6,
    uart1RxPQ4TxPB1 = 7,

    //UART2
    uart2RxPA6TxNone = 8,
    uart2RxPD4TxNone = 9,
    uart2RxNoneTxPA7 = 10,
    uart2RxNoneTxPD5 = 11,
    uart2RxPA6TxPA7 = 12,
    uart2RxPA6TxPD5 = 13,
    uart2RxPD4TxPA7 = 14,
    uart2RxPD4TxPD5 = 15,

    //UART3
    uart3RxPA4TxNone = 16,
    uart3RxPJ0TxNone = 17,
    uart3RxNoneTxPA5 = 18,
    uart3RxNoneTxPJ1 = 19,
    uart3RxPA4TxPA5 = 20,
    uart3RxPA4TxPJ1 = 21,
    uart3RxPJ0TxPA5 = 22,
    uart3RxPJ0TxPJ1 = 23,

    //UART4
    uart4RxPK0TxNone = 24,
    uart4RxPA2TxNone = 25,
    uart4RxNoneTxPK1 = 26,
    uart4RxNoneTxPA3 = 27,
    uart4RxPK0TxPK1 = 28,
    uart4RxPK0TxPA3 = 29,
    uart4RxPA2TxPK1 = 30,
    uart4RxPA2TxPA3 = 31,

    //UART5
    uart5RxPC6TxNone = 32,
    uart5RxNoneTxPC7 = 33,
    uart5RxPC6TxPC7 = 34,

    //UART6
    uart6RxPP0TxNone = 35,
    uart6RxNoneTxPP1 = 36,
    uart6RxPP0TxPP1 = 37,

    //UART7
    uart7RxPC4TxNone = 38,
    uart7RxNoneTxPC5 = 39,
    uart7RxPC4TxPC5 = 40

};

/**
 * @brief <b>UART Baud rate</b>.
 * Baud rate: The bit rate, which will be used to Transmit data or Receive data or both via UART
 */
enum HalUartBaudRate
{
    uartBaudRate9600 = 0,
    uartBaudRate115200 = 1
};

/**
 * @brief <b>UART World length</b>.
 * Word length: The number of data bits to be transmitted or received in a frame
 */
enum HalUartWordLength
{
    uartFiveBits = 0,
    uartSixBits = 1,
    uartSevenBits = 2,
    uartEightBits = 3
};

/**
 * @brief <b>UART Stop bit</b>.
 * Stop bit: The number of stop bit to be transmitted at the end of a frame
 */
enum HalUartStopBit
{
    uartOneStopBit = 0,
    uartTwoStopBit = 1
};

/**
 * @brief <b>UART Parity</b>.
 * Parity: The Parity generation and checking to be performed during transmission and reception
 */
enum HalUartParity
{
    uartNoParity = 0,
    uartOddParity = 1,
    uartEvenParity = 2
};

/**
 * Core: a particular UART HW-module
 */

/**
 * @brief <b>UART core state</b>.
 * The struct contains data which is used by the driver to properly service the UART core.
 *
 * It is a forward declaration. It informs the compiler that such struct exists, but
 * does not allow it to instantiate it or modify it contents,
 * as the specification of the fields in the struct is placed elsewhere (in uart.c).
 *
 * The user of this header is able to construct to pointer to the struct,
 * but is unable to modify it contents.
 */
struct HalUartCoreState;

// alias for the constant pointer to the struct
typedef struct HalUartCoreState * const HalUartCoreStateHandle;

/**
 * @brief <b>UART driver initialization</b>.
 * HAL UART driver init: this function should be called only once before doing anything.
 */
void halUartDriverInit(void);

/**
 * @brief <b>UART core initialization</b>.
 * HAL UART core state handle:
 * <br>This function allows User to configure an UART.
 * <br> \warning This function blocks the reconfiguration of the same UART
 * <br> \note Select proper settings and configure only once.
 */
HalUartCoreStateHandle halUartCoreInit(enum HalUartCoreAtPort uartCoreAtPort,
                                   enum HalUartBaudRate uartBaudRate,
                                   enum HalUartWordLength uartWordLength,
                                   enum HalUartStopBit uartStopBit,
                                   enum HalUartParity uartParity );

/**
 * @brief <b>UART Transmission</b>.
 * Transmit txDataLength number of UART frames stored in txData in the blocking mode.
 */
void halUartTx(char const * const txData, unsigned int const txDataLength, HalUartCoreStateHandle uartCoreStateHandle);

/**
 * @brief <b>UART Reception</b>.
 * Receive rxDataLength number of UART frames into rxData in the blocking mode.
 * The function does not return until the requested number of bytes are received.
 * It waits for data in a busy wait loop.
 */
void halUartRx(char * const rxData, unsigned int const rxDataLength, HalUartCoreStateHandle uartCoreStateHandle);

#endif /* HAL_UART_INCLUDE_HAL_UART_UART_H_ */
