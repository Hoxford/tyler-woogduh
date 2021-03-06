//*****************************************************************************
//
// Startup code for use with TI's Code Composer Studio.
//
// Copyright (c) 2011-2013 Texas Instruments Incorporated.  All rights reserved.
// Software License Agreement
// 
// Software License Agreement
//
// Texas Instruments (TI) is supplying this software for use solely and
// exclusively on TI's microcontroller products. The software is owned by
// TI and/or its suppliers, and is protected under applicable copyright
// laws. You may not combine this software with "viral" open-source
// software in order to form a larger program.
//
// THIS SOFTWARE IS PROVIDED "AS IS" AND WITH ALL FAULTS.
// NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT
// NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. TI SHALL NOT, UNDER ANY
// CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR CONSEQUENTIAL
// DAMAGES, FOR ANY REASON WHATSOEVER.
//
//*****************************************************************************

#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_gpio.h"
#include "driverlib/gpio.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/timer.h"
#include "driverlib/uart.h"
#include "driverlib/udma.h"
#include <inc/tm4c1233h6pm.h>
#include <inc/hw_memmap.h>
#include "utils_inc/error_codes.h"
#include "utils_inc/proj_debug.h"
#include "board.h"

//*****************************************************************************
//
// Forward declaration of the default fault handlers.
//
//*****************************************************************************
void ResetISR(void);
static void NmiSR(void);
static void FaultISR(void);
static void IntDefaultHandler (void);
static void vSysTickIntHandler(void);
static void vUART1_Rx_and_Tx  (void);
//static void Reset_me(void);
static void Watchdog_timer    (void);
static void Timer0AIntHandler (void);
#ifndef USE_USB_LIB
  static void USB0Int           (void);
#endif
static void uDMAErrorHandler  (void);

//*****************************************************************************
//
// External declarations for the interrupt handlers used by the application.
//
//*****************************************************************************
#ifdef USE_USB_LIB
  extern void USB0DeviceIntHandler(void);
#endif
//*****************************************************************************
//
// External declaration for the reset handler that is to be called when the
// processor is started
//
//*****************************************************************************
extern void _c_int00(void);

//*****************************************************************************
//
// Linker variable that marks the top of the stack.
//
//*****************************************************************************
extern uint32_t __STACK_TOP;

//*****************************************************************************
//
// External declarations for the interrupt handlers used by the application.
//
//*****************************************************************************
// To be added by user
volatile int iUart_DMA_ints_serviced = 0;
volatile int iUart_ints_serviced = 0;

//*****************************************************************************
//
// The vector table.  Note that the proper constructs must be placed on this to
// ensure that it ends up at physical address 0x0000.0000 or at the start of
// the program if located at a start address other than 0.
//
//*****************************************************************************
#pragma DATA_SECTION(g_pfnVectors, ".intvecs")
void (* const g_pfnVectors[])(void) =
{
    (void (*)(void))((uint32_t)&__STACK_TOP),
                                            // The initial stack pointer
    ResetISR,                               // The reset handler
    NmiSR,                                  // The NMI handler
    FaultISR,                               // The hard fault handler
    IntDefaultHandler,                      // The MPU fault handler
    IntDefaultHandler,                      // The bus fault handler
    IntDefaultHandler,                      // The usage fault handler
    0,                                      // Reserved
    0,                                      // Reserved
    0,                                      // Reserved
    0,                                      // Reserved
    IntDefaultHandler,                      // SVCall handler
    IntDefaultHandler,                      // Debug monitor handler
    0,                                      // Reserved
    IntDefaultHandler,                      // The PendSV handler
    vSysTickIntHandler,                      // The SysTick handler
    IntDefaultHandler,                      // GPIO Port A
    IntDefaultHandler,                      // GPIO Port B
    IntDefaultHandler,                      // GPIO Port C
    IntDefaultHandler,                      // GPIO Port D
    IntDefaultHandler,                      // GPIO Port E
    IntDefaultHandler,                      // UART0 Rx and Tx
    vUART1_Rx_and_Tx,                       // UART1 Rx and Tx
    IntDefaultHandler,                      // SSI0 Rx and Tx
    IntDefaultHandler,                      // I2C0 Master and Slave
    IntDefaultHandler,                      // PWM Fault
    IntDefaultHandler,                      // PWM Generator 0
    IntDefaultHandler,                      // PWM Generator 1
    IntDefaultHandler,                      // PWM Generator 2
    IntDefaultHandler,                      // Quadrature Encoder 0
    IntDefaultHandler,                      // ADC Sequence 0
    IntDefaultHandler,                      // ADC Sequence 1
    IntDefaultHandler,                      // ADC Sequence 2
    IntDefaultHandler,                      // ADC Sequence 3
    Watchdog_timer,                         // Watchdog timer
    Timer0AIntHandler,                      // Timer 0 subtimer A
    IntDefaultHandler,                      // Timer 0 subtimer B
    IntDefaultHandler,                      // Timer 1 subtimer A
    IntDefaultHandler,                      // Timer 1 subtimer B
    IntDefaultHandler,                      // Timer 2 subtimer A
    IntDefaultHandler,                      // Timer 2 subtimer B
    IntDefaultHandler,                      // Analog Comparator 0
    IntDefaultHandler,                      // Analog Comparator 1
    IntDefaultHandler,                      // Analog Comparator 2
    IntDefaultHandler,                      // System Control (PLL, OSC, BO)
    IntDefaultHandler,                      // FLASH Control
    IntDefaultHandler,                      // GPIO Port F
    IntDefaultHandler,                      // GPIO Port G
    IntDefaultHandler,                      // GPIO Port H
    IntDefaultHandler,                      // UART2 Rx and Tx
    IntDefaultHandler,                      // SSI1 Rx and Tx
    IntDefaultHandler,                      // Timer 3 subtimer A
    IntDefaultHandler,                      // Timer 3 subtimer B
    IntDefaultHandler,                      // I2C1 Master and Slave
    IntDefaultHandler,                      // Quadrature Encoder 1
    IntDefaultHandler,                      // CAN0
    IntDefaultHandler,                      // CAN1
    0,                                      // Reserved
    0,                                      // Reserved
    IntDefaultHandler,                      // Hibernate
#ifdef USE_USBLIB
    USB0DeviceIntHandler,                   // USB0
#else
    USB0Int,                                // USB0
#endif
    IntDefaultHandler,                      // PWM Generator 3
    IntDefaultHandler,                      // uDMA Software Transfer
    uDMAErrorHandler,                       // uDMA Error
    IntDefaultHandler,                      // ADC1 Sequence 0
    IntDefaultHandler,                      // ADC1 Sequence 1
    IntDefaultHandler,                      // ADC1 Sequence 2
    IntDefaultHandler,                      // ADC1 Sequence 3
    0,                                      // Reserved
    0,                                      // Reserved
    IntDefaultHandler,                      // GPIO Port J
    IntDefaultHandler,                      // GPIO Port K
    IntDefaultHandler,                      // GPIO Port L
    IntDefaultHandler,                      // SSI2 Rx and Tx
    IntDefaultHandler,                      // SSI3 Rx and Tx
    IntDefaultHandler,                      // UART3 Rx and Tx
    IntDefaultHandler,                      // UART4 Rx and Tx
    IntDefaultHandler,                      // UART5 Rx and Tx
    IntDefaultHandler,                      // UART6 Rx and Tx
    IntDefaultHandler,                      // UART7 Rx and Tx
    0,                                      // Reserved
    0,                                      // Reserved
    0,                                      // Reserved
    0,                                      // Reserved
    IntDefaultHandler,                      // I2C2 Master and Slave
    IntDefaultHandler,                      // I2C3 Master and Slave
    IntDefaultHandler,                      // Timer 4 subtimer A
    IntDefaultHandler,                      // Timer 4 subtimer B
    0,                                      // Reserved
    0,                                      // Reserved
    0,                                      // Reserved
    0,                                      // Reserved
    0,                                      // Reserved
    0,                                      // Reserved
    0,                                      // Reserved
    0,                                      // Reserved
    0,                                      // Reserved
    0,                                      // Reserved
    0,                                      // Reserved
    0,                                      // Reserved
    0,                                      // Reserved
    0,                                      // Reserved
    0,                                      // Reserved
    0,                                      // Reserved
    0,                                      // Reserved
    0,                                      // Reserved
    0,                                      // Reserved
    0,                                      // Reserved
    IntDefaultHandler,                      // Timer 5 subtimer A
    IntDefaultHandler,                      // Timer 5 subtimer B
    IntDefaultHandler,                      // Wide Timer 0 subtimer A
    IntDefaultHandler,                      // Wide Timer 0 subtimer B
    IntDefaultHandler,                      // Wide Timer 1 subtimer A
    IntDefaultHandler,                      // Wide Timer 1 subtimer B
    IntDefaultHandler,                      // Wide Timer 2 subtimer A
    IntDefaultHandler,                      // Wide Timer 2 subtimer B
    IntDefaultHandler,                      // Wide Timer 3 subtimer A
    IntDefaultHandler,                      // Wide Timer 3 subtimer B
    IntDefaultHandler,                      // Wide Timer 4 subtimer A
    IntDefaultHandler,                      // Wide Timer 4 subtimer B
    IntDefaultHandler,                      // Wide Timer 5 subtimer A
    IntDefaultHandler,                      // Wide Timer 5 subtimer B
    IntDefaultHandler,                      // FPU
    0,                                      // Reserved
    0,                                      // Reserved
    IntDefaultHandler,                      // I2C4 Master and Slave
    IntDefaultHandler,                      // I2C5 Master and Slave
    IntDefaultHandler,                      // GPIO Port M
    IntDefaultHandler,                      // GPIO Port N
    IntDefaultHandler,                      // Quadrature Encoder 2
    0,                                      // Reserved
    0,                                      // Reserved
    IntDefaultHandler,                      // GPIO Port P (Summary or P0)
    IntDefaultHandler,                      // GPIO Port P1
    IntDefaultHandler,                      // GPIO Port P2
    IntDefaultHandler,                      // GPIO Port P3
    IntDefaultHandler,                      // GPIO Port P4
    IntDefaultHandler,                      // GPIO Port P5
    IntDefaultHandler,                      // GPIO Port P6
    IntDefaultHandler,                      // GPIO Port P7
    IntDefaultHandler,                      // GPIO Port Q (Summary or Q0)
    IntDefaultHandler,                      // GPIO Port Q1
    IntDefaultHandler,                      // GPIO Port Q2
    IntDefaultHandler,                      // GPIO Port Q3
    IntDefaultHandler,                      // GPIO Port Q4
    IntDefaultHandler,                      // GPIO Port Q5
    IntDefaultHandler,                      // GPIO Port Q6
    IntDefaultHandler,                      // GPIO Port Q7
    IntDefaultHandler,                      // GPIO Port R
    IntDefaultHandler,                      // GPIO Port S
    IntDefaultHandler,                      // PWM 1 Generator 0
    IntDefaultHandler,                      // PWM 1 Generator 1
    IntDefaultHandler,                      // PWM 1 Generator 2
    IntDefaultHandler,                      // PWM 1 Generator 3
    IntDefaultHandler                       // PWM 1 Fault
};

//*****************************************************************************
//
// This is the code that gets called when the processor first starts execution
// following a reset event.  Only the absolutely necessary set is performed,
// after which the application supplied entry() routine is called.  Any fancy
// actions (such as making decisions based on the reset cause register, and
// resetting the bits in that register) are left solely in the hands of the
// application.
//
//*****************************************************************************
void
ResetISR(void)
{
    //
    // Jump to the CCS C initialization routine.  This will enable the
    // floating-point unit as well, so that does not need to be done here.
    //
    __asm("    .global _c_int00\n"
          "    b.w     _c_int00");
}

//*****************************************************************************
//
// This is the code that gets called when the processor receives a NMI.  This
// simply enters an infinite loop, preserving the system state for examination
// by a debugger.
//
//*****************************************************************************
static void
NmiSR(void)
{
    //
    // Enter an infinite loop.
    //
    while(1)
    {
    }
}

//*****************************************************************************
//
// This is the code that gets called when the processor receives a fault
// interrupt.  This simply enters an infinite loop, preserving the system state
// for examination by a debugger.
//
//*****************************************************************************
static void
FaultISR(void)
{
    vDEBUG("FaultISR SYS HALT");
    //
    // Enter an infinite loop.
    //
    while(1)
    {
    }
}

//*****************************************************************************
//
// This is the code that gets called when the processor receives an unexpected
// interrupt.  This simply enters an infinite loop, preserving the system state
// for examination by a debugger.
//
//*****************************************************************************
static void
IntDefaultHandler(void)
{
    vDEBUG("IntDefaultHandler SYS HALT");
    //
    // Go into an infinite loop.
    //
    while(1)
    {
    }
}

//*****************************************************************************
//
// This is the code that gets called for the system tick
//
//*****************************************************************************
static void vSysTickIntHandler(void)
{
  vSystick_int_service();
  return;
}

//*****************************************************************************
//
// This is the code that gets called when the USART connected to the Blue tooth
// module is proced.
//
//*****************************************************************************
static void
vUART1_Rx_and_Tx(void)
{
  ERROR_CODE eEC = ER_OK;
  uint32_t ui32Status;
  uint32_t ui32DMA_int_status = 0;
//  bool     bIsUART_TX_DMA_Enabled = false;

  eEC = eUsing_radio_uart_dma();
  if(eEC == ER_TRUE)
  {
    ui32DMA_int_status = MAP_uDMAIntStatus();
    if(ui32DMA_int_status > 0)
    {
      vRadio_interface_DMA_int_service(ui32DMA_int_status);
      iUart_DMA_ints_serviced++;
    }
    MAP_uDMAIntClear(ui32DMA_int_status);

    ui32Status = MAP_UARTTxIntModeGet(INEEDMD_RADIO_UART);

    if(ui32Status > 0)
    {
      vRadio_UARTTx_int_service(ui32Status);
    }
  }else{/*nothing*/}

  // Get the interrrupt status.
//  while((ui32Status = MAP_UARTIntStatus(INEEDMD_RADIO_UART, false)) == 0){i++;};
  ui32Status = MAP_UARTIntStatus(INEEDMD_RADIO_UART, false);
  MAP_UARTIntClear(INEEDMD_RADIO_UART, ui32Status);
//  ui32Status = ui32Status & (UART_INT_RX | UART_INT_RT | UART_INT_CTS);
////  while(ui32Status == 0)
////  {
////    ui32Status = MAP_UARTIntStatus(INEEDMD_RADIO_UART, true);
////    ui32Status = ui32Status & (UART_INT_RX | UART_INT_RT | UART_INT_CTS);
////    i++;
////  };
//
  if(ui32Status > 0)
  {

    iUart_ints_serviced++;
    // Clear the asserted interrupts.

    MAP_UARTIntClear(INEEDMD_RADIO_UART, ui32Status);

    if((ui32Status & UART_INT_RX) == UART_INT_RX)
    {
      vRadio_interface_int_service(UART_INT_RX);
    }

    if((ui32Status & UART_INT_RT) == UART_INT_RT)
    {
      vRadio_interface_int_service_timeout(ui32Status);
    }

    if((ui32Status & UART_INT_CTS) == UART_INT_CTS)
    {
      vRadio_interface_int_service(UART_INT_CTS);
    }

    if((ui32Status & UART_INT_DMATX) == UART_INT_DMATX)
    {
      vRadio_interface_int_service(UART_INT_DMATX);
    }
  }

  return;
}

/******************************************************************************
*
* watchdog timer interrupt
*
******************************************************************************/
static void Watchdog_timer    (void)
{
  MAP_WatchdogIntClear(WATCHDOG0_BASE);
  vDEBUG("Watchdog_timer, watchdog barked! SYS HALT");
  //
  // Go into an infinite loop.
  //
  while(1)
  {
  }
}

//*****************************************************************************
//
// This is the code that gets called when the reset pin is shorted set
// It will call the asm reset code which completly restarts the system.
//
//*****************************************************************************
//static void
//Reset_me(void)
//{
//  ResetISR();
//}

static void
Timer0AIntHandler(void)
{
    //
    // Clear the timer interrupt flag.
    //
    TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
}

//*****************************************************************************
//
// USB interrupt handler
//
//*****************************************************************************
static void USB0Int(void)
{
  uint32_t ui32Status;

  //
  // Get the interrrupt status.
  //
  ui32Status = ROM_USBIntStatusControl(INEEDMD_USB);

  vUSBServiceInt(ui32Status);

  MAP_USBIntStatusControl(INEEDMD_USB);
  MAP_USBIntStatusEndpoint(INEEDMD_USB);

  return;
}

//*****************************************************************************
//
// DMA error handler
//
//*****************************************************************************
static void uDMAErrorHandler(void)
{
#ifdef DEBUG
  // Go into an infinite loop.
  vDEBUG("uDMAErrorHandler SYS HALT");
  while(1){};
#else
  //reset the system
  ResetISR();

  //Should never get here but loop for ever just in case.
  while(1){};
#endif
}
