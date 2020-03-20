/*
 *  uart.c
 *  Created on: 11/3/2019
 *  Author: Saaket Timsina
 */

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "inc/tm4c1294ncpdt.h"
#include "hal/uart/uart.h"

enum HalMaxUartCount_{
    maxUartCount = 8
};

enum HalMaxUartPortCount_{
    maxUartPortCount = 8
};

enum HalMaxUartDirectionCount_{
    maxUartDirectionCount = 2
};

enum HalUartCore_
{
    uartCore0 = 0,
    uartCore1 = 1,
    uartCore2 = 2,
    uartCore3 = 3,
    uartCore4 = 4,
    uartCore5 = 5,
    uartCore6 = 6,
    uartCore7 = 7
};

enum HalUartPortCore_
{
    uartPortA = 0,
    uartPortB = 1,
    uartPortC = 2,
    uartPortD = 3,
    uartPortJ = 4,
    uartPortK = 5,
    uartPortP = 6,
    uartPortQ = 7
};

enum HalUartDirection_{
    uartRx = 0,
    uartTx = 1,
    uartRxTx = 2
};

enum HalUartBaudRateDivisor_
{
    uartIbrd = 0,
    uartFbrd = 1
};

// function prototypes
static void evaluateHalUartCoreInfo_(enum HalUartCoreAtPort uartCoreAtPort, char * ptr);
static uint32_t checkAndSelectWordLength_(enum HalUartWordLength uartWordlength);
static uint32_t checkAndSelectStopBit_(enum HalUartStopBit stopBit);
static uint32_t checkAndSelectParity_(enum HalUartParity parity);
static uint32_t checkAndSelectControlValueForRxOrTx_(int uartDirection);
static uint32_t * checkAndSelectBaudRate_(enum HalUartBaudRate uartBaudRate, uint32_t * ibrdAndFbrd);

/** All Structure declarations **/

// For Port Related to UART

struct HalPortCoreStateClockModuleValueHolder_{
    uint32_t activatePortModule;
    uint32_t isPortModulePresent;
};
/** Structures for UART & related port Map starts here **/
struct HalPortCoreStateRegisterValueHolder_{
    uint32_t portEnableRegister[maxUartDirectionCount];
    uint32_t portDirectionRegister[maxUartDirectionCount];
    uint32_t portAlternateFunctionRegister[maxUartDirectionCount];
    uint32_t portControlRegister[maxUartDirectionCount];
};

struct HalPortCoreStatePointerValueHolder_{
    struct HalPortCoreStateRegisterValueHolder_ halPortCoreStateRegisterValueHolder[maxUartPortCount];
};
/** Structures for UART & related port Map ends here **/

struct HalPortCoreStatePointerHolder_{
    volatile uint32_t * ptrPortEnableRegister;
    volatile uint32_t * ptrPortDirectionRegister;
    volatile uint32_t * ptrPortAlternateFunctionRegister;
    volatile uint32_t * ptrPortControlRegister;
};

/** Predefined Constant values  **/

static const struct HalPortCoreStateClockModuleValueHolder_ halPortCoreStateClockModuleValueHolder[] =
{
 //PORT A
 {
  .activatePortModule = SYSCTL_RCGCGPIO_R0,
  .isPortModulePresent = SYSCTL_PRGPIO_R0
 },
 //PORT B
 {
  .activatePortModule = SYSCTL_RCGCGPIO_R1,
  .isPortModulePresent = SYSCTL_PRGPIO_R1
 },
 //PORT C
 {
  .activatePortModule = SYSCTL_RCGCGPIO_R2,
  .isPortModulePresent = SYSCTL_PRGPIO_R2
 },
 //PORT D
 {
  .activatePortModule = SYSCTL_RCGCGPIO_R3,
  .isPortModulePresent = SYSCTL_PRGPIO_R3

 },
 //PORT J
 {
  .activatePortModule = SYSCTL_RCGCGPIO_R8,
  .isPortModulePresent = SYSCTL_PRGPIO_R8

 },
 //PORT K
 {
  .activatePortModule = SYSCTL_RCGCGPIO_R9,
  .isPortModulePresent = SYSCTL_PRGPIO_R9
 },
 //PORT P
 {
  .activatePortModule = SYSCTL_RCGCGPIO_R14,
  .isPortModulePresent = SYSCTL_PRGPIO_R14
 },
 //PORT Q
 {
  .activatePortModule = SYSCTL_RCGCGPIO_R0,
  .isPortModulePresent = SYSCTL_PRGPIO_R0
 }

};

static struct HalPortCoreStatePointerValueHolder_ halPortCoreStatePointerValueHolder[] =
{
 // UART0
 {.halPortCoreStateRegisterValueHolder[uartPortA].portEnableRegister[uartTx]= 0x02, // enable PA1
  .halPortCoreStateRegisterValueHolder[uartPortA].portEnableRegister[uartRx]= 0x01, // enable PA0

  .halPortCoreStateRegisterValueHolder[uartPortA].portDirectionRegister[uartTx]= 0x02, //set PA1 to output
  .halPortCoreStateRegisterValueHolder[uartPortA].portDirectionRegister[uartRx]= ~0x01, // set PA0 to input

  .halPortCoreStateRegisterValueHolder[uartPortA].portAlternateFunctionRegister[uartTx]= 0x02, // Enable AF of PA1
  .halPortCoreStateRegisterValueHolder[uartPortA].portAlternateFunctionRegister[uartRx]= 0x01, // Enable AF of PA0

  .halPortCoreStateRegisterValueHolder[uartPortA].portControlRegister[uartTx]= 0x00000010, // assign pin MUX value PA1(1), U0Tx
  .halPortCoreStateRegisterValueHolder[uartPortA].portControlRegister[uartRx]= 0x00000001, // assign pin MUX value PA0(1), U0Rx
 },
 // UART1
 {
  .halPortCoreStateRegisterValueHolder[uartPortB].portEnableRegister[uartTx]= 0x02, // enable PB1
  .halPortCoreStateRegisterValueHolder[uartPortB].portEnableRegister[uartRx]= 0x01, // enable PB0
  .halPortCoreStateRegisterValueHolder[uartPortQ].portEnableRegister[uartRx]= 0x10, // enable PQ4

  .halPortCoreStateRegisterValueHolder[uartPortB].portDirectionRegister[uartTx]= 0x02, //set PB1 to output
  .halPortCoreStateRegisterValueHolder[uartPortB].portDirectionRegister[uartRx]= ~0x01, // set PB0 to input
  .halPortCoreStateRegisterValueHolder[uartPortQ].portDirectionRegister[uartRx]= ~0x10, // set PQ4 to input

  .halPortCoreStateRegisterValueHolder[uartPortB].portAlternateFunctionRegister[uartTx]= 0x02, // Enable AF of PB1
  .halPortCoreStateRegisterValueHolder[uartPortB].portAlternateFunctionRegister[uartRx]= 0x01, // Enable AF of PB0
  .halPortCoreStateRegisterValueHolder[uartPortQ].portAlternateFunctionRegister[uartRx]= 0x10, // Enable AF of PQ4

  .halPortCoreStateRegisterValueHolder[uartPortB].portControlRegister[uartTx]= 0x00000010, // assign pin MUX value PB1(1), U1Tx
  .halPortCoreStateRegisterValueHolder[uartPortB].portControlRegister[uartRx]= 0x00000001, // assign pin MUX value PB0(1), U1Rx
  .halPortCoreStateRegisterValueHolder[uartPortQ].portControlRegister[uartRx]= 0x00010000 // assign pin MUX value PQ4(1), U1Rx
 },
 // UART2
 {
  .halPortCoreStateRegisterValueHolder[uartPortA].portEnableRegister[uartTx]= 0x80, // enable PA7
  .halPortCoreStateRegisterValueHolder[uartPortD].portEnableRegister[uartTx]= 0x20, // enable PD5
  .halPortCoreStateRegisterValueHolder[uartPortA].portEnableRegister[uartRx]= 0x40, // enable PA6
  .halPortCoreStateRegisterValueHolder[uartPortD].portEnableRegister[uartRx]= 0x10, // enable PD4

  .halPortCoreStateRegisterValueHolder[uartPortA].portDirectionRegister[uartTx]= 0x80, //set PA7 to output
  .halPortCoreStateRegisterValueHolder[uartPortD].portDirectionRegister[uartTx]= 0x20, //set PD5 to output
  .halPortCoreStateRegisterValueHolder[uartPortA].portDirectionRegister[uartRx]= ~0x40, // set PA6 to input
  .halPortCoreStateRegisterValueHolder[uartPortD].portDirectionRegister[uartRx]= ~0x10, // set PD5 to input

  .halPortCoreStateRegisterValueHolder[uartPortA].portAlternateFunctionRegister[uartTx]= 0x80, // Enable AF of PA7
  .halPortCoreStateRegisterValueHolder[uartPortD].portAlternateFunctionRegister[uartTx]= 0x20, // Enable AF of PD5
  .halPortCoreStateRegisterValueHolder[uartPortA].portAlternateFunctionRegister[uartRx]= 0x40, // Enable AF of PA6
  .halPortCoreStateRegisterValueHolder[uartPortD].portAlternateFunctionRegister[uartRx]= 0x10, // Enable AF of PD4

  .halPortCoreStateRegisterValueHolder[uartPortA].portControlRegister[uartTx]= 0x10000000, // assign pin MUX value PA7(1), U2Tx
  .halPortCoreStateRegisterValueHolder[uartPortD].portControlRegister[uartTx]= 0x00100000, // assign pin MUX value PD5(1), U2Tx
  .halPortCoreStateRegisterValueHolder[uartPortA].portControlRegister[uartRx]= 0x01000000, // assign pin MUX value PA6(1), U2Rx
  .halPortCoreStateRegisterValueHolder[uartPortD].portControlRegister[uartRx]= 0x00010000 // assign pin MUX value PD4(1), U2Rx
 },
 // UART3
 {
  .halPortCoreStateRegisterValueHolder[uartPortA].portEnableRegister[uartTx] = 0x20, // enable PA5
  .halPortCoreStateRegisterValueHolder[uartPortJ].portEnableRegister[uartTx] = 0x02, // enable PJ1
  .halPortCoreStateRegisterValueHolder[uartPortA].portEnableRegister[uartRx] = 0x10, // enable PA4
  .halPortCoreStateRegisterValueHolder[uartPortJ].portEnableRegister[uartRx] = 0x01, // enable PJ0

  .halPortCoreStateRegisterValueHolder[uartPortA].portDirectionRegister[uartTx] = 0x20, //set PA5 to output
  .halPortCoreStateRegisterValueHolder[uartPortJ].portDirectionRegister[uartTx] = 0x02, //set PJ1 to output
  .halPortCoreStateRegisterValueHolder[uartPortA].portDirectionRegister[uartRx] = ~0x10, // set PA4 to input
  .halPortCoreStateRegisterValueHolder[uartPortJ].portDirectionRegister[uartRx] = ~0x01, // set PJ0 to input

  .halPortCoreStateRegisterValueHolder[uartPortA].portAlternateFunctionRegister[uartTx] = 0x20, // Enable AF of PA5
  .halPortCoreStateRegisterValueHolder[uartPortJ].portAlternateFunctionRegister[uartTx] = 0x02, // Enable AF of PJ1
  .halPortCoreStateRegisterValueHolder[uartPortA].portAlternateFunctionRegister[uartRx] = 0x10, // Enable AF of PA4
  .halPortCoreStateRegisterValueHolder[uartPortJ].portAlternateFunctionRegister[uartRx] = 0x01, // Enable AF of PJ0

  .halPortCoreStateRegisterValueHolder[uartPortA].portControlRegister[uartTx] = 0x00100000, // assign pin MUX value PA5(1),U3Tx
  .halPortCoreStateRegisterValueHolder[uartPortJ].portControlRegister[uartTx] = 0x00000010, // assign pin MUX value PJ1(1),U3Tx
  .halPortCoreStateRegisterValueHolder[uartPortA].portControlRegister[uartRx] = 0x00010000, // assign pin MUX value PA4(1),U3Rx
  .halPortCoreStateRegisterValueHolder[uartPortJ].portControlRegister[uartRx] = 0x00000001 // assign pin MUX value PJ0(1),U3Rx
 },
 // UART4
 {
  .halPortCoreStateRegisterValueHolder[uartPortK].portEnableRegister[uartTx] = 0x02, // enable PK1
  .halPortCoreStateRegisterValueHolder[uartPortA].portEnableRegister[uartTx] = 0x08, // enable PA3
  .halPortCoreStateRegisterValueHolder[uartPortK].portEnableRegister[uartRx] = 0x01, // enable PK0
  .halPortCoreStateRegisterValueHolder[uartPortA].portEnableRegister[uartRx] = 0x04, // enable PA2

  .halPortCoreStateRegisterValueHolder[uartPortK].portDirectionRegister[uartTx] = 0x02, //set PK1 to output
  .halPortCoreStateRegisterValueHolder[uartPortA].portDirectionRegister[uartTx] = 0x08, //set PA3 to output
  .halPortCoreStateRegisterValueHolder[uartPortK].portDirectionRegister[uartRx] = ~0x01, // set PK0 to input
  .halPortCoreStateRegisterValueHolder[uartPortA].portDirectionRegister[uartRx] = ~0x04, // set PA2 to input

  .halPortCoreStateRegisterValueHolder[uartPortK].portAlternateFunctionRegister[uartTx] = 0x02, // Enable AF of PK1
  .halPortCoreStateRegisterValueHolder[uartPortA].portAlternateFunctionRegister[uartTx] = 0x08, // Enable AF of PA3
  .halPortCoreStateRegisterValueHolder[uartPortK].portAlternateFunctionRegister[uartRx] = 0x01, // Enable AF of PK0
  .halPortCoreStateRegisterValueHolder[uartPortA].portAlternateFunctionRegister[uartRx] = 0x04, // Enable AF of PA2

  .halPortCoreStateRegisterValueHolder[uartPortK].portControlRegister[uartTx] = 0x00000010, // assign pin MUX value PK1(1),U4Tx
  .halPortCoreStateRegisterValueHolder[uartPortA].portControlRegister[uartTx] = 0x00001000, // assign pin MUX value PA3(1),U4Tx
  .halPortCoreStateRegisterValueHolder[uartPortK].portControlRegister[uartRx] = 0x00000001, // assign pin MUX value PK0(1),U4Rx
  .halPortCoreStateRegisterValueHolder[uartPortK].portControlRegister[uartRx] = 0x00000100 // assign pin MUX value PA2(1),U4Rx
 },
 // UART5
 {
  .halPortCoreStateRegisterValueHolder[uartPortC].portEnableRegister[uartTx] = 0x80, // enable PC7
  .halPortCoreStateRegisterValueHolder[uartPortC].portEnableRegister[uartRx] = 0x40, // enable PC6

  .halPortCoreStateRegisterValueHolder[uartPortC].portDirectionRegister[uartTx] = 0x80, //set PC7 to output
  .halPortCoreStateRegisterValueHolder[uartPortC].portDirectionRegister[uartRx] = ~0x40, // set PC6 to input

  .halPortCoreStateRegisterValueHolder[uartPortC].portAlternateFunctionRegister[uartTx] = 0x80, // Enable AF of PC7
  .halPortCoreStateRegisterValueHolder[uartPortC].portAlternateFunctionRegister[uartRx] = 0x40, // Enable AF of PC6

  .halPortCoreStateRegisterValueHolder[uartPortC].portControlRegister[uartTx] = 0x10000000, // assign pin MUX value PC7(1), U5Tx
  .halPortCoreStateRegisterValueHolder[uartPortC].portControlRegister[uartRx] = 0x01000000, // assign pin MUX value PC6(1), U5Rx
 },
 // UART6
 {
  .halPortCoreStateRegisterValueHolder[uartPortP].portEnableRegister[uartTx] = 0x02, // enable PP1
  .halPortCoreStateRegisterValueHolder[uartPortP].portEnableRegister[uartRx] = 0x01, // enable PP0

  .halPortCoreStateRegisterValueHolder[uartPortP].portDirectionRegister[uartTx] = 0x02, //set PP1 to output
  .halPortCoreStateRegisterValueHolder[uartPortP].portDirectionRegister[uartRx] = ~0x01, // set PP0 to input

  .halPortCoreStateRegisterValueHolder[uartPortP].portAlternateFunctionRegister[uartTx] = 0x02, // Enable AF of PP1
  .halPortCoreStateRegisterValueHolder[uartPortP].portAlternateFunctionRegister[uartRx] = 0x01, // Enable AF of PP0

  .halPortCoreStateRegisterValueHolder[uartPortP].portControlRegister[uartTx] = 0x00000010, // assign pin MUX value PP1(1), U6Tx
  .halPortCoreStateRegisterValueHolder[uartPortP].portControlRegister[uartRx] = 0x00000001, // assign pin MUX value PP0(1), U6Rx
 },
 // UART7
 {
  .halPortCoreStateRegisterValueHolder[uartPortC].portEnableRegister[uartTx] = 0x20, // enable PC5
  .halPortCoreStateRegisterValueHolder[uartPortC].portEnableRegister[uartRx] = 0x10, // enable PC4

  .halPortCoreStateRegisterValueHolder[uartPortC].portDirectionRegister[uartTx] = 0x20, //set PC5 to output
  .halPortCoreStateRegisterValueHolder[uartPortC].portDirectionRegister[uartRx] = ~0x10, // set PC4 to input

  .halPortCoreStateRegisterValueHolder[uartPortC].portAlternateFunctionRegister[uartTx] = 0x20, // Enable AF of PC5
  .halPortCoreStateRegisterValueHolder[uartPortC].portAlternateFunctionRegister[uartRx] = 0x10, // Enable AF of PC4

  .halPortCoreStateRegisterValueHolder[uartPortC].portControlRegister[uartTx] = 0x00100000, // assign pin MUX value PC5(1), U0Tx
  .halPortCoreStateRegisterValueHolder[uartPortC].portControlRegister[uartRx] = 0x00010000, // assign pin MUX value PC4(1), U0Rx
 }
};

static  struct HalPortCoreStatePointerHolder_ halPortCoreStatePointerHolder[] =
{
 // PORT A
 {.ptrPortEnableRegister = &GPIO_PORTA_AHB_DEN_R,
  .ptrPortDirectionRegister = &GPIO_PORTA_AHB_DIR_R,
  .ptrPortAlternateFunctionRegister = &GPIO_PORTA_AHB_AFSEL_R,
  .ptrPortControlRegister = &GPIO_PORTA_AHB_PCTL_R
 },
 // PORT B
 {
  .ptrPortEnableRegister = &GPIO_PORTB_AHB_DEN_R,
  .ptrPortDirectionRegister = &GPIO_PORTB_AHB_DIR_R,
  .ptrPortAlternateFunctionRegister = &GPIO_PORTB_AHB_AFSEL_R,
  .ptrPortControlRegister = &GPIO_PORTB_AHB_PCTL_R
 },
 // PORT C
 {.ptrPortEnableRegister = &GPIO_PORTC_AHB_DEN_R,
  .ptrPortDirectionRegister = &GPIO_PORTC_AHB_DIR_R,
  .ptrPortAlternateFunctionRegister = &GPIO_PORTC_AHB_AFSEL_R,
  .ptrPortControlRegister = &GPIO_PORTC_AHB_PCTL_R
 },
 // PORT D
 {
  .ptrPortEnableRegister = &GPIO_PORTD_AHB_DEN_R,
  .ptrPortDirectionRegister = &GPIO_PORTD_AHB_DIR_R,
  .ptrPortAlternateFunctionRegister = &GPIO_PORTD_AHB_AFSEL_R,
  .ptrPortControlRegister = &GPIO_PORTD_AHB_PCTL_R
 },
 // PORT J
 {
  .ptrPortEnableRegister = &GPIO_PORTJ_AHB_DEN_R,
  .ptrPortDirectionRegister = &GPIO_PORTJ_AHB_DIR_R,
  .ptrPortAlternateFunctionRegister = &GPIO_PORTJ_AHB_AFSEL_R,
  .ptrPortControlRegister = &GPIO_PORTJ_AHB_PCTL_R
 },
 // PORT K
 {
  .ptrPortEnableRegister = &GPIO_PORTK_DEN_R,
  .ptrPortDirectionRegister = &GPIO_PORTK_DIR_R,
  .ptrPortAlternateFunctionRegister = &GPIO_PORTK_AFSEL_R,
  .ptrPortControlRegister = &GPIO_PORTK_PCTL_R
 },
 // PORT P
 {
  .ptrPortEnableRegister = &GPIO_PORTP_DEN_R,
  .ptrPortDirectionRegister = &GPIO_PORTP_DIR_R,
  .ptrPortAlternateFunctionRegister = &GPIO_PORTP_AFSEL_R,
  .ptrPortControlRegister = &GPIO_PORTP_PCTL_R
 },
 // PORT Q
 {
  .ptrPortEnableRegister = &GPIO_PORTQ_DEN_R,
  .ptrPortDirectionRegister = &GPIO_PORTQ_DIR_R,
  .ptrPortAlternateFunctionRegister = &GPIO_PORTQ_AFSEL_R,
  .ptrPortControlRegister = &GPIO_PORTQ_PCTL_R
 }
};



// For UART

struct HalUartCoreStateClockModuleValueHolder_{
    uint32_t activateUartModule;
    uint32_t isUartModulePresent;
};

struct HalUartCoreStatePointerHolder_{
    volatile uint32_t * ptrUartControlRegister;
    volatile uint32_t * ptrUartIbrdRegister;
    volatile uint32_t * ptrUartFbrdRegister;
    volatile uint32_t * ptrUartLineControlRegister;
    volatile uint32_t * ptrUartFlagRegister;
    volatile uint32_t * ptrUartDataRegister;
};

static const struct HalUartCoreStateClockModuleValueHolder_ halUartCoreStateClockModuleValueHolder[] =
{
 // UART0
 { .activateUartModule = SYSCTL_RCGCUART_R0,
   .isUartModulePresent = SYSCTL_PPUART_P0
 },
 // UART1
 { .activateUartModule = SYSCTL_RCGCUART_R1,
   .isUartModulePresent = SYSCTL_PPUART_P1
 },
 // UART2
 { .activateUartModule = SYSCTL_RCGCUART_R2,
   .isUartModulePresent = SYSCTL_PPUART_P2
 },
 // UART3
 { .activateUartModule = SYSCTL_RCGCUART_R3,
   .isUartModulePresent = SYSCTL_PPUART_P3
 },
 // UART4
 { .activateUartModule = SYSCTL_RCGCUART_R4,
   .isUartModulePresent = SYSCTL_PPUART_P4
 },
 // UART5
 { .activateUartModule = SYSCTL_RCGCUART_R5,
   .isUartModulePresent = SYSCTL_PPUART_P5
 },
 // UART6
 { .activateUartModule = SYSCTL_RCGCUART_R6,
   .isUartModulePresent = SYSCTL_PPUART_P6
 },
 // UART7
 { .activateUartModule = SYSCTL_RCGCUART_R7,
   .isUartModulePresent = SYSCTL_PPUART_P7
 }

};

static const struct HalUartCoreStatePointerHolder_ halUartCoreStatePointerHolder[] =
{
 // UART0
 { .ptrUartControlRegister = &UART0_CTL_R,
   .ptrUartIbrdRegister = &UART0_IBRD_R,
   .ptrUartFbrdRegister = &UART0_FBRD_R,
   .ptrUartLineControlRegister = &UART0_LCRH_R,
   .ptrUartFlagRegister= &UART0_FR_R,
   .ptrUartDataRegister = &UART0_DR_R
 },
 // UART1
 { .ptrUartControlRegister = &UART1_CTL_R,
   .ptrUartIbrdRegister = &UART1_IBRD_R,
   .ptrUartFbrdRegister = &UART1_FBRD_R,
   .ptrUartLineControlRegister = &UART1_LCRH_R,
   .ptrUartFlagRegister= &UART1_FR_R,
   .ptrUartDataRegister = &UART1_DR_R
 },
 // UART2
 { .ptrUartControlRegister = &UART2_CTL_R,
   .ptrUartIbrdRegister = &UART2_IBRD_R,
   .ptrUartFbrdRegister = &UART2_FBRD_R,
   .ptrUartLineControlRegister = &UART2_LCRH_R,
   .ptrUartFlagRegister= &UART2_FR_R,
   .ptrUartDataRegister = &UART2_DR_R
 },
 // UART3
 { .ptrUartControlRegister = &UART3_CTL_R,
   .ptrUartIbrdRegister = &UART3_IBRD_R,
   .ptrUartFbrdRegister = &UART3_FBRD_R,
   .ptrUartLineControlRegister = &UART3_LCRH_R,
   .ptrUartFlagRegister= &UART3_FR_R,
   .ptrUartDataRegister = &UART3_DR_R
 },
 // UART4
 { .ptrUartControlRegister = &UART4_CTL_R,
   .ptrUartIbrdRegister = &UART4_IBRD_R,
   .ptrUartFbrdRegister = &UART4_FBRD_R,
   .ptrUartLineControlRegister = &UART4_LCRH_R,
   .ptrUartFlagRegister= &UART4_FR_R,
   .ptrUartDataRegister = &UART4_DR_R
 },
 // UART5
 { .ptrUartControlRegister = &UART5_CTL_R,
   .ptrUartIbrdRegister = &UART5_IBRD_R,
   .ptrUartFbrdRegister = &UART5_FBRD_R,
   .ptrUartLineControlRegister = &UART5_LCRH_R,
   .ptrUartFlagRegister= &UART5_FR_R,
   .ptrUartDataRegister = &UART5_DR_R
 },
 // UART6
 { .ptrUartControlRegister = &UART6_CTL_R,
   .ptrUartIbrdRegister = &UART6_IBRD_R,
   .ptrUartFbrdRegister = &UART6_FBRD_R,
   .ptrUartLineControlRegister = &UART6_LCRH_R,
   .ptrUartFlagRegister= &UART6_FR_R,
   .ptrUartDataRegister = &UART6_DR_R
 },
 // UART7
 { .ptrUartControlRegister = &UART7_CTL_R,
   .ptrUartIbrdRegister = &UART7_IBRD_R,
   .ptrUartFbrdRegister = &UART7_FBRD_R,
   .ptrUartLineControlRegister = &UART7_LCRH_R,
   .ptrUartFlagRegister= &UART7_FR_R,
   .ptrUartDataRegister = &UART7_DR_R
 }

};

struct HalUartCoreState{
    enum HalUartCore_ halUartCore;
    bool uartCoreInitilized;
};

struct HalUartCoreState uartCores[maxUartCount];


/** All Functions are below here **/

/** Helper Functions **/

static void evaluateHalUartCoreInfo_(enum HalUartCoreAtPort uartCoreAtPort, char * ptr) {
    /** return value contains
     *  all value as char array[]
     *  UART number (int value)/ Rx or Tx or Both (integer value)/ Rx at Port (integer value) / Tx at Port (integer value)/
     *  Tx or Rx index value = 'x' denotes that the port for respective direction is not in use
     *  **/
    switch (uartCoreAtPort) {
    case uart0RxPA0TxNone: strcpy(ptr, "000x"); break;
    case uart0RxNoneTxPA1: strcpy(ptr, "010x"); break;
    case uart0RxPA0TxPA1: strcpy(ptr, "0200"); break;

    case uart1RxPB0TxNone: strcpy(ptr, "101x"); break;
    case uart1RxPQ4TxNone: strcpy(ptr, "107x"); break;
    case uart1RxNoneTxPB1: strcpy(ptr, "11x1"); break;
    case uart1RxPB0TxPB1: strcpy(ptr, "1211"); break;
    case uart1RxPQ4TxPB1: strcpy(ptr, "1271"); break;

    case uart2RxPA6TxNone: strcpy(ptr, "200x"); break;
    case uart2RxPD4TxNone: strcpy(ptr, "203x"); break;
    case uart2RxNoneTxPA7: strcpy(ptr, "21x0"); break;
    case uart2RxNoneTxPD5: strcpy(ptr, "21x3"); break;
    case uart2RxPA6TxPA7: strcpy(ptr, "2200"); break;
    case uart2RxPA6TxPD5: strcpy(ptr, "2203"); break;
    case uart2RxPD4TxPA7: strcpy(ptr, "2230"); break;
    case uart2RxPD4TxPD5: strcpy(ptr, "2233"); break;

    case uart3RxPA4TxNone: strcpy(ptr, "300x"); break;
    case uart3RxPJ0TxNone: strcpy(ptr, "304x"); break;
    case uart3RxNoneTxPA5: strcpy(ptr, "31x0"); break;
    case uart3RxNoneTxPJ1: strcpy(ptr, "31x4"); break;
    case uart3RxPA4TxPA5: strcpy(ptr, "3200"); break;
    case uart3RxPA4TxPJ1: strcpy(ptr, "3204"); break;
    case uart3RxPJ0TxPA5: strcpy(ptr, "3240"); break;
    case uart3RxPJ0TxPJ1: strcpy(ptr, "3244"); break;

    case uart4RxPK0TxNone: strcpy(ptr, "405x"); break;
    case uart4RxPA2TxNone: strcpy(ptr, "400x"); break;
    case uart4RxNoneTxPK1: strcpy(ptr, "41x5"); break;
    case uart4RxNoneTxPA3: strcpy(ptr, "41x0"); break;
    case uart4RxPK0TxPK1: strcpy(ptr, "4255"); break;
    case uart4RxPK0TxPA3: strcpy(ptr, "4250"); break;
    case uart4RxPA2TxPK1: strcpy(ptr, "4205"); break;
    case uart4RxPA2TxPA3: strcpy(ptr, "4200"); break;

    case uart5RxPC6TxNone: strcpy(ptr, "502x"); break;
    case uart5RxNoneTxPC7: strcpy(ptr, "51x2"); break;
    case uart5RxPC6TxPC7: strcpy(ptr, "5222"); break;

    case uart6RxPP0TxNone: strcpy(ptr, "606x"); break;
    case uart6RxNoneTxPP1: strcpy(ptr, "61x6"); break;
    case uart6RxPP0TxPP1: strcpy(ptr, "6266"); break;

    case uart7RxPC4TxNone: strcpy(ptr, "702x"); break;
    case uart7RxNoneTxPC5: strcpy(ptr, "71x2"); break;
    case uart7RxPC4TxPC5: strcpy(ptr, "7222"); break;

    }
}

static uint32_t checkAndSelectWordLength_(enum HalUartWordLength uartWordlength){
    uint32_t defaultWl = UART_LCRH_WLEN_5; // default 5 bits
    if (uartWordlength == uartSixBits){
        return UART_LCRH_WLEN_6;
    }
    else if (uartWordlength == uartSevenBits){
        return UART_LCRH_WLEN_7;
    }
    else if (uartWordlength == uartEightBits){
        return UART_LCRH_WLEN_8;
    }

    return defaultWl;
}

static uint32_t checkAndSelectStopBit_(enum HalUartStopBit stopBit){
    uint32_t oneStopBit  = 0x00000000; // one Stop Bit
    if(stopBit == uartTwoStopBit){
        return UART_LCRH_STP2;
    }
    return oneStopBit;
}

static uint32_t checkAndSelectParity_(enum HalUartParity parity){
    uint32_t parityNotEnabled = 0x00000000; // no parity enabled
    if(parity == uartOddParity){
        return UART_LCRH_PEN;// parity enable and odd parity have same hex value
    }
    else if(parity == uartEvenParity){
        return UART_LCRH_EPS;
    }
    return parityNotEnabled;
}

static uint32_t checkAndSelectControlValueForRxOrTx_(int uartDirection){
    uint32_t receiver = UART_CTL_RXE; // default as Receiver
    if(uartDirection == uartTx){
        return UART_CTL_TXE;
    }
    else if(uartDirection == uartRxTx){
        return UART_CTL_RXE | UART_CTL_TXE;
    }
    return receiver;
}

static uint32_t * checkAndSelectBaudRate_(enum HalUartBaudRate uartBaudRate, uint32_t * ibrdAndFbrd){
    // 16 MHZ
    // BRD = CPU clock / 16 / baudRate
    // BRDI = Integer part &  BRDF = roundUp[(BRD - BRDI)*64]

    // default baud rate 9600
    ibrdAndFbrd[uartIbrd] = 104;
    ibrdAndFbrd[uartFbrd] = 11;

    if(uartBaudRate == uartBaudRate115200){
        ibrdAndFbrd[uartIbrd] = 8;
        ibrdAndFbrd[uartFbrd] = 44;
        return ibrdAndFbrd;
    }
    return ibrdAndFbrd;
}

/** ************* Functions that are visible to user *********************************** **/

void halUartDriverInit(void){
    int i = 0;
    for (i = 0; i < maxUartCount; i++){
        uartCores[i].uartCoreInitilized = false;
    }
}

HalUartCoreStateHandle halUartCoreInit(enum HalUartCoreAtPort uartCoreAtPort,
                                       enum HalUartBaudRate uartBaudRate,
                                       enum HalUartWordLength uartWordLength,
                                       enum HalUartStopBit uartStopBit,
                                       enum HalUartParity uartParity ){
    // Value Calculation that are required to configure the given UART
    char uartCoreInfo[5];
    char * ptrCoreInfo = &uartCoreInfo[0];
    evaluateHalUartCoreInfo_(uartCoreAtPort, ptrCoreInfo);
    int uartCore = ptrCoreInfo[0] - '0';
    int uartDir  = ptrCoreInfo[1] - '0';
    int uartPort = 0;

    if(uartCores[uartCore].uartCoreInitilized == false){
        uint32_t wordLength = checkAndSelectWordLength_(uartWordLength);
        uint32_t stopBit = checkAndSelectStopBit_(uartStopBit);
        uint32_t parity = checkAndSelectParity_(uartParity);
        uint32_t controlValue = checkAndSelectControlValueForRxOrTx_(uartDir);
        uint32_t ibrdAndFbrd[2];
        uint32_t * ptrIbrdAndFbrd = checkAndSelectBaudRate_(uartBaudRate, &ibrdAndFbrd[0]);

        /************************          PORT INITIALIZATION SECTION           *************************/

        if(ptrCoreInfo[2] == 'x' && ptrCoreInfo[3] != 'x'){
            uartPort = ptrCoreInfo[3] - '0';
        }
        else{
            uartPort = ptrCoreInfo[2] - '0';
        }

        if(uartDir == uartRxTx){
            if(ptrCoreInfo[2] != ptrCoreInfo[3]){
                int uartRxPort = ptrCoreInfo[2] - '0';
                int uartTxPort = ptrCoreInfo[3] - '0';

                // Activate and Wait until Port Clock Module is activated!
                SYSCTL_RCGCGPIO_R |= (halPortCoreStateClockModuleValueHolder[uartRxPort].activatePortModule |
                        halPortCoreStateClockModuleValueHolder[uartTxPort].activatePortModule);
                while((SYSCTL_PRGPIO_R & (halPortCoreStateClockModuleValueHolder[uartRxPort].isPortModulePresent |
                        halPortCoreStateClockModuleValueHolder[uartTxPort].isPortModulePresent)) == 0);

                //Enable Port
                (*((volatile uint32_t *)(halPortCoreStatePointerHolder[uartRxPort].ptrPortEnableRegister)))=
                        halPortCoreStatePointerValueHolder[uartCore].halPortCoreStateRegisterValueHolder[uartRxPort].portEnableRegister[uartRx];
                (*((volatile uint32_t *)(halPortCoreStatePointerHolder[uartTxPort].ptrPortEnableRegister)))=
                        halPortCoreStatePointerValueHolder[uartCore].halPortCoreStateRegisterValueHolder[uartTxPort].portEnableRegister[uartTx];

                // Enable Port Direction
                (*((volatile uint32_t *)(halPortCoreStatePointerHolder[uartRxPort].ptrPortDirectionRegister)))=
                        halPortCoreStatePointerValueHolder[uartCore].halPortCoreStateRegisterValueHolder[uartRxPort].portDirectionRegister[uartRx];
                (*((volatile uint32_t *)(halPortCoreStatePointerHolder[uartTxPort].ptrPortDirectionRegister)))=
                        halPortCoreStatePointerValueHolder[uartCore].halPortCoreStateRegisterValueHolder[uartTxPort].portDirectionRegister[uartTx];

                // Enable Alternate function at Port
                (*((volatile uint32_t *)(halPortCoreStatePointerHolder[uartRxPort].ptrPortAlternateFunctionRegister)))=
                        halPortCoreStatePointerValueHolder[uartCore].halPortCoreStateRegisterValueHolder[uartRxPort].portAlternateFunctionRegister[uartRx];
                (*((volatile uint32_t *)(halPortCoreStatePointerHolder[uartTxPort].ptrPortAlternateFunctionRegister)))=
                        halPortCoreStatePointerValueHolder[uartCore].halPortCoreStateRegisterValueHolder[uartTxPort].portAlternateFunctionRegister[uartTx];

                // Assign Pin MUX Value
                (*((volatile uint32_t *)(halPortCoreStatePointerHolder[uartRxPort].ptrPortControlRegister)))=
                        halPortCoreStatePointerValueHolder[uartCore].halPortCoreStateRegisterValueHolder[uartRxPort].portControlRegister[uartRx];
                (*((volatile uint32_t *)(halPortCoreStatePointerHolder[uartTxPort].ptrPortControlRegister)))=
                        halPortCoreStatePointerValueHolder[uartCore].halPortCoreStateRegisterValueHolder[uartTxPort].portControlRegister[uartTx];
            }
            else {
                // Activate and Wait until Port Clock Module is activated!
                SYSCTL_RCGCGPIO_R |= halPortCoreStateClockModuleValueHolder[uartPort].activatePortModule;
                while((SYSCTL_PRGPIO_R & halPortCoreStateClockModuleValueHolder[uartPort].isPortModulePresent) == 0);

                (*((volatile uint32_t *)(halPortCoreStatePointerHolder[uartPort].ptrPortEnableRegister)))=
                        halPortCoreStatePointerValueHolder[uartCore].halPortCoreStateRegisterValueHolder[uartPort].portEnableRegister[uartRx] |
                        halPortCoreStatePointerValueHolder[uartCore].halPortCoreStateRegisterValueHolder[uartPort].portEnableRegister[uartTx];

                // Enable Port Direction
                (*((volatile uint32_t *)(halPortCoreStatePointerHolder[uartPort].ptrPortDirectionRegister)))=
                        halPortCoreStatePointerValueHolder[uartCore].halPortCoreStateRegisterValueHolder[uartPort].portDirectionRegister[uartRx] |
                        halPortCoreStatePointerValueHolder[uartCore].halPortCoreStateRegisterValueHolder[uartPort].portDirectionRegister[uartTx];

                // Enable Alternate function at Port
                (*((volatile uint32_t *)(halPortCoreStatePointerHolder[uartPort].ptrPortAlternateFunctionRegister)))=
                        halPortCoreStatePointerValueHolder[uartCore].halPortCoreStateRegisterValueHolder[uartPort].portAlternateFunctionRegister[uartRx] |
                        halPortCoreStatePointerValueHolder[uartCore].halPortCoreStateRegisterValueHolder[uartPort].portAlternateFunctionRegister[uartTx];

                // Assign Pin MUX Value
                (*((volatile uint32_t *)(halPortCoreStatePointerHolder[uartPort].ptrPortControlRegister)))=
                        halPortCoreStatePointerValueHolder[uartCore].halPortCoreStateRegisterValueHolder[uartPort].portControlRegister[uartRx] |
                        halPortCoreStatePointerValueHolder[uartCore].halPortCoreStateRegisterValueHolder[uartPort].portControlRegister[uartTx];
            }

        }

        else{

            // Activate and Wait until Port Clock Module is activated!
            SYSCTL_RCGCGPIO_R |= halPortCoreStateClockModuleValueHolder[uartPort].activatePortModule;
            while((SYSCTL_PRGPIO_R & halPortCoreStateClockModuleValueHolder[uartPort].isPortModulePresent) == 0);

            //Enable Port
            (*((volatile uint32_t *)(halPortCoreStatePointerHolder[uartPort].ptrPortEnableRegister)))=
                    halPortCoreStatePointerValueHolder[uartCore].halPortCoreStateRegisterValueHolder[uartPort].portEnableRegister[uartDir];

            // Enable Port Direction
            (*((volatile uint32_t *)(halPortCoreStatePointerHolder[uartPort].ptrPortDirectionRegister)))=
                    halPortCoreStatePointerValueHolder[uartCore].halPortCoreStateRegisterValueHolder[uartPort].portDirectionRegister[uartDir];

            // Enable Alternate function at Port
            (*((volatile uint32_t *)(halPortCoreStatePointerHolder[uartPort].ptrPortAlternateFunctionRegister)))=
                    halPortCoreStatePointerValueHolder[uartCore].halPortCoreStateRegisterValueHolder[uartPort].portAlternateFunctionRegister[uartDir];

            // Assign Pin MUX Value
            (*((volatile uint32_t *)(halPortCoreStatePointerHolder[uartPort].ptrPortControlRegister)))=
                    halPortCoreStatePointerValueHolder[uartCore].halPortCoreStateRegisterValueHolder[uartPort].portControlRegister[uartDir];

        }

        /************************          UART INITIALIZATION SECTION           *************************/

        // Activate and Wait until UART Clock Module is activated!
        SYSCTL_RCGCUART_R |= halUartCoreStateClockModuleValueHolder[uartCore].activateUartModule;
        while(!(SYSCTL_PRUART_R & (halUartCoreStateClockModuleValueHolder[uartCore].isUartModulePresent)));

        (*((volatile uint32_t *)(halUartCoreStatePointerHolder[uartCore].ptrUartControlRegister))) &= ~0x00000001;// disable UARTx for configuration
        (*((volatile uint32_t *)(halUartCoreStatePointerHolder[uartCore].ptrUartIbrdRegister))) = ptrIbrdAndFbrd[uartIbrd];
        (*((volatile uint32_t *)(halUartCoreStatePointerHolder[uartCore].ptrUartFbrdRegister))) = ptrIbrdAndFbrd[uartFbrd];
        // use deep FIFOs as well, flag FEN (1 << 4), default level triggering Rx interrupt is 1/2 (8 characters)
        (*((volatile uint32_t *)(halUartCoreStatePointerHolder[uartCore].ptrUartLineControlRegister))) = wordLength | stopBit | parity | (1 << 4);
        (*((volatile uint32_t *)(halUartCoreStatePointerHolder[uartCore].ptrUartControlRegister))) = UART_CTL_UARTEN | controlValue;// disable UARTx for configuration

        uartCores[uartCore].halUartCore = (enum HalUartCore_)uartCore;
        uartCores[uartCore].uartCoreInitilized = true;

        return &uartCores[uartCore];

    }
    else {
        return NULL;
    }
}

void halUartTx(char const * const txData, unsigned int const txDataLength, HalUartCoreStateHandle uartCoreStateHandle) {
    int i = 0;
    int uartCore = uartCoreStateHandle->halUartCore;
    if((uartCoreStateHandle->uartCoreInitilized) == true){
        for(i = 0; i < txDataLength; ++i){
            while((*(volatile uint32_t *)(halUartCoreStatePointerHolder[uartCore].ptrUartFlagRegister) & (0x020)) != 0);
            (*((volatile uint32_t *)(halUartCoreStatePointerHolder[uartCore].ptrUartDataRegister))) = txData[i];
        }
    }
}

// receive requested number of UART frames in the blocking mode
void halUartRx(char * const rxData, unsigned int const rxDataLength, HalUartCoreStateHandle uartCoreStateHandle) {
    int i = 0;
    int uartCore = uartCoreStateHandle->halUartCore;
    if((uartCoreStateHandle->uartCoreInitilized) == true){
        for(i = 0; i < rxDataLength; ++i){
            while((*(volatile uint32_t *)(halUartCoreStatePointerHolder[uartCore].ptrUartFlagRegister) & (0x010)) != 0);
            rxData[i] = (*((volatile uint32_t *)(halUartCoreStatePointerHolder[uartCore].ptrUartDataRegister)));
        }
    }
}


