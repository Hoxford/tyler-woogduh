//*****************************************************************************
//
// usb_dev_serial.c - Main routines for the USB CDC serial example.
//
// Copyright (c) 2011-2014 Texas Instruments Incorporated.  All rights reserved.
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
// This is part of revision 2.1.0.12573 of the EK-LM4F232 Firmware Package.
//
//*****************************************************************************

#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
//#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_gpio.h"
#include "inc/hw_uart.h"
#include "inc/hw_sysctl.h"
#include "inc/hw_nvic.h"
#include "driverlib/debug.h"
#include "driverlib/fpu.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/interrupt.h"
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"
#include "driverlib/timer.h"
#include "driverlib/uart.h"
#include "driverlib/usb.h"
#include "driverlib/rom.h"
//#include "grlib/grlib.h"
#include "usblib/usblib.h"
#include "usblib/usbcdc.h"
#include "usblib/usbbuffer.c"
#include "driverlib/uart.h"
#include "usblib/usb-ids.h"
#include "usblib/device/usbdevice.h"
#include "usblib/device/usbdcdc.h"
#include "utils/ustdlib.h"
//#include "drivers/cfal96x64x16.h"
#include "usb_serial_structs.h"
#include "inc/tm4c1233h6pm.h"
#include "inc/hw_nvic.h"
#include "inc/hw_sysctl.h"
#include "board.h"
#include "ineedmd_adc.h"
#include "ineedmd_led.h"
#include "ineedmd_watchdog.h"

//*****************************************************************************
//
//! \addtogroup example_list
//! <h1>USB Serial Device (usb_dev_serial)</h1>
//!
//! This example application turns the evaluation kit into a virtual serial
//! port when connected to the USB host system.  The application supports the
//! USB Communication Device Class, Abstract Control Model to redirect UART0
//! traffic to and from the USB host system.
//!
//! Assuming you installed TivaWare in the default directory, a
//! driver information (INF) file for use with Windows XP, Windows Vista and
//! Windows7 can be found in C:/ti/TivaWare_C_Series-x.x/windows_drivers.
//! For Windows 2000, the required INF file is in
//! C:/ti/TivaWare_C_Series-x.x/windows_drivers/win2K.
//
//*****************************************************************************

//*****************************************************************************
//
// Note:
//
// This example is intended to run on Tiva C Series evaluation kit hardware
// where the UARTs are wired solely for TX and RX, and do not have GPIOs
// connected to act as handshake signals.  As a result, this example mimics
// the case where communication is always possible.  It reports DSR, DCD
// and CTS as high to ensure that the USB host recognizes that data can be
// sent and merely ignores the host's requested DTR and RTS states.  "TODO"
// comments in the code indicate where code would be required to add support
// for real handshakes.
//
//*****************************************************************************

//*****************************************************************************
//
// Configuration and tuning parameters.
//
//*****************************************************************************

//*****************************************************************************
//
// The system tick rate expressed both as ticks per second and a millisecond
// period.
//
//*****************************************************************************
#define SYSTICKS_PER_SECOND 100
#define SYSTICK_PERIOD_MS (1000 / SYSTICKS_PER_SECOND)

//*****************************************************************************
//
// Variables tracking transmit and receive counts.
//
//*****************************************************************************
volatile uint32_t g_ui32UARTTxCount = 0;
volatile uint32_t g_ui32UARTRxCount = 0;
#ifdef DEBUG
uint32_t g_ui32UARTRxErrors = 0;
#endif

//*****************************************************************************
//
// The base address, peripheral ID and interrupt ID of the UART that is to
// be redirected.
//
//*****************************************************************************

//*****************************************************************************
//
// Defines required to redirect UART0 via USB.
//
//*****************************************************************************
#define USB_UART_BASE           UART1_BASE//UART5_BASE
#define USB_UART_PERIPH         SYSCTL_PERIPH_UART1//SYSCTL_PERIPH_UART5
#define USB_UART_INT            INT_UART1//INT_UART5

//*****************************************************************************
//
// Default line coding settings for the redirected UART.
//
//*****************************************************************************
#define DEFAULT_BIT_RATE        115200
#define DEFAULT_UART_CONFIG     (UART_CONFIG_WLEN_8 | UART_CONFIG_PAR_NONE | \
                                 UART_CONFIG_STOP_ONE)

//*****************************************************************************
//
// GPIO peripherals and pins muxed with the redirected UART.  These will depend
// upon the IC in use and the UART selected in USB_UART_BASE.  Be careful that
// these settings all agree with the hardware you are using.
//
//*****************************************************************************

//*****************************************************************************
//
// Defines required to redirect UART1 via USB.
//
//*****************************************************************************
#define TX_GPIO_BASE            GPIO_PORTC_BASE//GPIO_PORTE_BASE
#define TX_GPIO_PERIPH          SYSCTL_PERIPH_GPIOC//SYSCTL_PERIPH_GPIOE
#define TX_GPIO_PIN             GPIO_PIN_5

#define RX_GPIO_BASE            GPIO_PORTC_BASE//GPIO_PORTE_BASE
#define RX_GPIO_PERIPH          SYSCTL_PERIPH_GPIOC//SYSCTL_PERIPH_GPIOE
#define RX_GPIO_PIN             GPIO_PIN_4

//*****************************************************************************
//
// Flag indicating whether or not we are currently sending a Break condition.
//
//*****************************************************************************
static bool g_bSendingBreak = false;

//*****************************************************************************
//
// Global system tick counter
//
//*****************************************************************************
volatile uint32_t g_ui32SysTickCount = 0;

//*****************************************************************************
//
// Graphics context used to show text on the color STN display.
//
//*****************************************************************************
/*
tContext g_sContext;
#define TEXT_FONT               g_psFontFixed6x8
#define TEXT_HEIGHT             (GrFontHeightGet(TEXT_FONT))
#define BUFFER_METER_HEIGHT     TEXT_HEIGHT
#define BUFFER_METER_WIDTH      56
*/
//*****************************************************************************
//
// Flags used to pass commands from interrupt context to the main loop.
//
//*****************************************************************************
#define COMMAND_PACKET_RECEIVED 0x00000001
#define COMMAND_STATUS_UPDATE   0x00000002

volatile uint32_t g_ui32Flags = 0;
char *g_pcStatus;

//*****************************************************************************
//
// Global flag indicating that a USB configuration has been set.
//
//*****************************************************************************
static volatile bool g_bUSBConfigured = false;

//*****************************************************************************
//
// Internal function prototypes.
//
//*****************************************************************************
static void USBUARTPrimeTransmit(uint32_t ui32Base);
static void CheckForSerialStateChange(const tUSBDCDCDevice *psDevice,
                                      int32_t i32Errors);
static void SetControlLineState(uint16_t ui16State);
static bool SetLineCoding(tLineCoding *psLineCoding);
static void GetLineCoding(tLineCoding *psLineCoding);
static void SendBreak(bool bSend);

//*****************************************************************************
//
// The error routine that is called if the driver library encounters an error.
//
//*****************************************************************************
#ifdef DEBUG
void
__error__(char *pcFilename, uint32_t ui32Line)
{
    while(1)
    {
    }
}
#endif

//*****************************************************************************
//
// This function is called whenever serial data is received from the UART.
// It is passed the accumulated error flags from each character received in
// this interrupt and determines from them whether or not an interrupt
// notification to the host is required.
//
// If a notification is required and the control interrupt endpoint is idle,
// we send the notification immediately.  If the endpoint is not idle, we
// accumulate the errors in a global variable which will be checked on
// completion of the previous notification and used to send a second one
// if necessary.
//
//*****************************************************************************
static void
CheckForSerialStateChange(const tUSBDCDCDevice *psDevice, int32_t i32Errors)
{
    uint16_t ui16SerialState;

    //
    // Clear our USB serial state.  Since we are faking the handshakes, always
    // set the TXCARRIER (DSR) and RXCARRIER (DCD) bits.
    //
    ui16SerialState = USB_CDC_SERIAL_STATE_TXCARRIER |
                    USB_CDC_SERIAL_STATE_RXCARRIER;
//
    //
    // Are any error bits set?
    //
    if(i32Errors)
    {
        //
        // At least one error is being notified so translate from our hardware
        // error bits into the correct state markers for the USB notification.
        //
        if(i32Errors & UART_DR_OE)
        {
            ui16SerialState |= USB_CDC_SERIAL_STATE_OVERRUN;
        }

        if(i32Errors & UART_DR_PE)
        {
            ui16SerialState |= USB_CDC_SERIAL_STATE_PARITY;
        }

        if(i32Errors & UART_DR_FE)
        {
            ui16SerialState |= USB_CDC_SERIAL_STATE_FRAMING;
        }

        if(i32Errors & UART_DR_BE)
        {
            ui16SerialState |= USB_CDC_SERIAL_STATE_BREAK;
        }

        // Call the CDC driver to notify the state change.
        USBDCDCSerialStateChange((void *)psDevice, ui16SerialState);
    }
}

int inBCSP = 0;

//*****************************************************************************
//
// Read as many characters from the UART FIFO as we can and move them into
// the CDC transmit buffer.
//
// \return Returns UART error flags read during data reception.
//
//*****************************************************************************
static int32_t
ReadUARTData(void)
{
    int32_t i32Char, i32Errors;
    uint8_t ui8Char;
    uint32_t ui32Space;

    //
    // Clear our error indicator.
    //
    i32Errors = 0;

    //
    // How much space do we have in the buffer?
    //
    ui32Space = USBBufferSpaceAvailable((tUSBBuffer *)&g_sTxBuffer);
    //while(!USBBufferSpaceAvailable((tUSBBuffer *)&g_sTxBuffer))



    //
    // Read data from the UART FIFO until there is none left or we run
    // out of space in our receive buffer.
    //
    while(ui32Space && UARTCharsAvail(USB_UART_BASE))
    {
        //
        // Read a character from the UART FIFO into the ring buffer if no
        // errors are reported.
        //
        i32Char = UARTCharGetNonBlocking(USB_UART_BASE);

        //
        // If the character did not contain any error notifications,
        // copy it to the output buffer.
        //
        if(i32Char < 0xFF)///*(i32Char != USB_CDC_SERIAL_STATE_OVERRUN)*/ & (i32Char != USB_CDC_SERIAL_STATE_PARITY) & (i32Char != USB_CDC_SERIAL_STATE_FRAMING) & (i32Char != USB_CDC_SERIAL_STATE_BREAK) & (USB_CDC_SERIAL_STATE_RING_SIGNAL)) //(!(i32Char & ~0xFF)))
        {
            ui8Char = (uint8_t)(i32Char & 0xFF);
            if((ui8Char == 0xC0) & !inBCSP)
            {
            	UARTConfigSetExpClk(USB_UART_BASE, 16000000, 115200, ( UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_EVEN));
            	UARTParityModeSet(USB_UART_BASE, UART_CONFIG_PAR_EVEN);

            	//SendLineCodingChange(void* psDevice);
            	inBCSP = 1;
            }
            USBBufferWrite((tUSBBuffer *)&g_sTxBuffer,
                           (uint8_t *)&ui8Char, 1);
            //
            // Decrement the number of bytes we know the buffer can accept.
            //
            ui32Space--;
        }
        else
        	//error received
        {
        	ui8Char = (uint8_t)(i32Char & 0xFF);
        	if((ui8Char == 0xC0) & !inBCSP)
        	{
        	     UARTConfigSetExpClk(USB_UART_BASE, 16000000, 115200, ( UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_EVEN));
        	     UARTParityModeSet(USB_UART_BASE, UART_CONFIG_PAR_EVEN);

        	    //SendLineCodingChange(void* psDevice);
        	     inBCSP = 1;
        	}
        	USBBufferWrite((tUSBBuffer *)&g_sTxBuffer,
        	                           (uint8_t *)&ui8Char, 1);
#ifdef DEBUG
            //
            // Increment our receive error counter.
            //
            g_ui32UARTRxErrors++;
#endif
            //
            // Update our error accumulator.
            //
            i32Errors |= i32Char;
        }

        //
        // Update our count of bytes received via the UART.
        //
        g_ui32UARTRxCount++;
    }

    //
    // Pass back the accumulated error indicators.
    //
    return(i32Errors);
}

//*****************************************************************************
//
// Take as many bytes from the transmit buffer as we have space for and move
// them into the USB UART's transmit FIFO.
//
//*****************************************************************************
static void
USBUARTPrimeTransmit(uint32_t ui32Base)
{
    uint32_t ui32Read;
    uint8_t ui8Char;

    //
    // If we are currently sending a break condition, don't receive any
    // more data. We will resume transmission once the break is turned off.
    //
    if(g_bSendingBreak)
    {
        return;
    }

    //
    // If there is space in the UART FIFO, try to read some characters
    // from the receive buffer to fill it again.
    //
    while(UARTSpaceAvail(ui32Base))
    {
        //
        // Get a character from the buffer.
        //
        ui32Read = USBBufferRead((tUSBBuffer *)&g_sRxBuffer, &ui8Char, 1);

        //
        // Did we get a character?
        //
        if(ui32Read)
        {
            //
            // Place the character in the UART transmit FIFO.
            //
            UARTCharPutNonBlocking(ui32Base, ui8Char);
            //UARTCharPutNonBlocking(UART5_BASE, ui8Char);
            //
            // Update our count of bytes transmitted via the UART.
            //
            g_ui32UARTTxCount++;
        }
        else
        {
            //
            // We ran out of characters so exit the function.
            //
            return;
        }
    }
}

//*****************************************************************************
//
// Interrupt handler for the system tick counter.
//
//*****************************************************************************

void
SysTickIntHandler(void)
{
    //
    // Update our system time.
    //
    g_ui32SysTickCount++;
}

//*****************************************************************************
//
// Interrupt handler for the UART which we are redirecting via USB.
//
//*****************************************************************************
void
USBUARTIntHandler(void)
{
    uint32_t ui32Ints;
    int32_t i32Errors;

    //
    // Get and clear the current interrupt source(s)
    //
    ui32Ints = UARTIntStatus(USB_UART_BASE, true);
    UARTIntClear(USB_UART_BASE, ui32Ints);
    //UARTCharPut(UART5_BASE, 'I');
    //
    // Are we being interrupted because the TX FIFO has space available?
    //
    if(ui32Ints & UART_INT_TX)
    {
        //
        // Move as many bytes as we can into the transmit FIFO.
        //
        USBUARTPrimeTransmit(USB_UART_BASE);
        //UARTCharPut(USB_UART_BASE, 'R');
        //UARTCharPut(USB_UART_BASE, 'X');
        //
        // If the output buffer is empty, turn off the transmit interrupt.
        //
        if(!USBBufferDataAvailable(&g_sRxBuffer))
        {
            UARTIntDisable(USB_UART_BASE, UART_INT_TX);
        }
    }

    //
    // Handle receive interrupts.
    //
    if(ui32Ints & (UART_INT_RX | UART_INT_RT))
    {
        //
        // Read the UART's characters into the buffer.
        //
        i32Errors = ReadUARTData();
        //UARTCharPut(USB_UART_BASE, 'R');
        //UARTCharPut(USB_UART_BASE, 'X');
        //
        // Check to see if we need to notify the host of any errors we just
        // detected.
        //
        CheckForSerialStateChange(&g_sCDCDevice, i32Errors);
    }
}

//*****************************************************************************
//
// Set the state of the RS232 RTS and DTR signals.
//
//*****************************************************************************
static void
SetControlLineState(uint16_t ui16State)
{
    //
    // TODO: If configured with GPIOs controlling the handshake lines,
    // set them appropriately depending upon the flags passed in the wValue
    // field of the request structure passed.
    //
}

//*****************************************************************************
//
// Set the communication parameters to use on the UART.
//
//*****************************************************************************
static bool
SetLineCoding(tLineCoding *psLineCoding)
{
    uint32_t ui32Config;
    bool bRetcode;

    //
    // Assume everything is OK until we detect any problem.
    //
    bRetcode = true;

    //
    // Word length.  For invalid values, the default is to set 8 bits per
    // character and return an error.
    //
    switch(psLineCoding->ui8Databits)
    {
        case 5:
        {
            ui32Config = UART_CONFIG_WLEN_5;
            break;
        }

        case 6:
        {
            ui32Config = UART_CONFIG_WLEN_6;
            break;
        }

        case 7:
        {
            ui32Config = UART_CONFIG_WLEN_7;
            break;
        }

        case 8:
        {
            ui32Config = UART_CONFIG_WLEN_8;
            break;
        }

        default:
        {
            ui32Config = UART_CONFIG_WLEN_8;
            bRetcode = false;
            break;
        }
    }

    //
    // Parity.  For any invalid values, we set no parity and return an error.
    //
    switch(psLineCoding->ui8Parity)
    {
        case USB_CDC_PARITY_NONE:
        {
            ui32Config |= UART_CONFIG_PAR_NONE;
            break;
        }

        case USB_CDC_PARITY_ODD:
        {
            ui32Config |= UART_CONFIG_PAR_ODD;
            break;
        }

        case USB_CDC_PARITY_EVEN:
        {
            ui32Config |= UART_CONFIG_PAR_EVEN;
            break;
        }

        case USB_CDC_PARITY_MARK:
        {
            ui32Config |= UART_CONFIG_PAR_ONE;
            break;
        }

        case USB_CDC_PARITY_SPACE:
        {
            ui32Config |= UART_CONFIG_PAR_ZERO;
            break;
        }

        default:
        {
            ui32Config |= UART_CONFIG_PAR_EVEN; //TODO:
            bRetcode = false;
            break;
        }
    }

    //
    // Stop bits.  Our hardware only supports 1 or 2 stop bits whereas CDC
    // allows the host to select 1.5 stop bits.  If passed 1.5 (or any other
    // invalid or unsupported value of ui8Stop, we set up for 1 stop bit but
    // return an error in case the caller needs to Stall or otherwise report
    // this back to the host.
    //
    switch(psLineCoding->ui8Stop)
    {
        //
        // One stop bit requested.
        //
        case USB_CDC_STOP_BITS_1:
        {
            ui32Config |= UART_CONFIG_STOP_ONE;
            break;
        }

        //
        // Two stop bits requested.
        //
        case USB_CDC_STOP_BITS_2:
        {
            ui32Config |= UART_CONFIG_STOP_TWO;
            break;
        }

        //
        // Other cases are either invalid values of ui8Stop or values that we
        // cannot support so set 1 stop bit but return an error.
        //
        default:
        {
            ui32Config = UART_CONFIG_STOP_ONE;
            bRetcode |= false;
            break;
        }
    }

    //
    // Set the UART mode appropriately.
    //
    if(inBCSP)
    {
    	UARTConfigSetExpClk( USB_UART_BASE, 16000000, 115200, ( UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_EVEN));
    	UARTParityModeSet(USB_UART_BASE, UART_CONFIG_PAR_EVEN);
    }
    else
    {
    	UARTConfigSetExpClk( USB_UART_BASE, 16000000, 115200, ( UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));
    }
    //UARTConfigSetExpClk(USB_UART_BASE, SysCtlClockGet(),
                                //DEFAULT_BIT_RATE, DEFAULT_UART_CONFIG);
    //
    // Let the caller know if we had a problem or not.
    //
    return(bRetcode);
}

//*****************************************************************************
//
// Get the communication parameters in use on the UART.
//
//*****************************************************************************
static void
GetLineCoding(tLineCoding *psLineCoding)
{
    uint32_t ui32Config;
    uint32_t ui32Rate;

    //
    // Get the current line coding set in the UART.
    //
    UARTConfigGetExpClk(USB_UART_BASE, SysCtlClockGet(), &ui32Rate,
                            &ui32Config);
    psLineCoding->ui32Rate = ui32Rate;

    //
    // Translate the configuration word length field into the format expected
    // by the host.
    //
    switch(ui32Config & UART_CONFIG_WLEN_MASK)
    {
        case UART_CONFIG_WLEN_8:
        {
            psLineCoding->ui8Databits = 8;
            break;
        }

        case UART_CONFIG_WLEN_7:
        {
            psLineCoding->ui8Databits = 7;
            break;
        }

        case UART_CONFIG_WLEN_6:
        {
            psLineCoding->ui8Databits = 6;
            break;
        }

        case UART_CONFIG_WLEN_5:
        {
            psLineCoding->ui8Databits = 5;
            break;
        }
    }

    //
    // Translate the configuration parity field into the format expected
    // by the host.
    //
    switch(ui32Config & UART_CONFIG_PAR_MASK)
    {
        case UART_CONFIG_PAR_NONE:
        {
            psLineCoding->ui8Parity = USB_CDC_PARITY_NONE;
            break;
        }

        case UART_CONFIG_PAR_ODD:
        {
            psLineCoding->ui8Parity = USB_CDC_PARITY_ODD;
            break;
        }

        case UART_CONFIG_PAR_EVEN:
        {
            psLineCoding->ui8Parity = USB_CDC_PARITY_EVEN;
            break;
        }

        case UART_CONFIG_PAR_ONE:
        {
            psLineCoding->ui8Parity = USB_CDC_PARITY_MARK;
            break;
        }

        case UART_CONFIG_PAR_ZERO:
        {
            psLineCoding->ui8Parity = USB_CDC_PARITY_SPACE;
            break;
        }
    }

    //
    // Translate the configuration stop bits field into the format expected
    // by the host.
    //
    switch(ui32Config & UART_CONFIG_STOP_MASK)
    {
        case UART_CONFIG_STOP_ONE:
        {
            psLineCoding->ui8Stop = USB_CDC_STOP_BITS_1;
            break;
        }

        case UART_CONFIG_STOP_TWO:
        {
            psLineCoding->ui8Stop = USB_CDC_STOP_BITS_2;
            break;
        }
    }
}

//*****************************************************************************
//
// This function sets or clears a break condition on the redirected UART RX
// line.  A break is started when the function is called with \e bSend set to
// \b true and persists until the function is called again with \e bSend set
// to \b false.
//
//*****************************************************************************
static void
SendBreak(bool bSend)
{
    //
    // Are we being asked to start or stop the break condition?
    //
    if(!bSend)
    {
        //
        // Remove the break condition on the line.
        //
        UARTBreakCtl(USB_UART_BASE, false);
        g_bSendingBreak = false;
    }
    else
    {
        //
        // Start sending a break condition on the line.
        //
        UARTBreakCtl(USB_UART_BASE, true);
        g_bSendingBreak = true;
    }
}


//*****************************************************************************
//
// Handles CDC driver notifications related to control and setup of the device.
//
// \param pvCBData is the client-supplied callback pointer for this channel.
// \param ui32Event identifies the event we are being notified about.
// \param ui32MsgValue is an event-specific value.
// \param pvMsgData is an event-specific pointer.
//
// This function is called by the CDC driver to perform control-related
// operations on behalf of the USB host.  These functions include setting
// and querying the serial communication parameters, setting handshake line
// states and sending break conditions.
//
// \return The return value is event-specific.
//
//*****************************************************************************
uint32_t
ControlHandler(void *pvCBData, uint32_t ui32Event,
               uint32_t ui32MsgValue, void *pvMsgData)
{
    uint32_t ui32IntsOff;

    //
    // Which event are we being asked to process?
    //
    switch(ui32Event)
    {
        //
        // We are connected to a host and communication is now possible.
        //
        case USB_EVENT_CONNECTED:
            g_bUSBConfigured = true;

            //
            // Flush our buffers.
            //
            USBBufferFlush(&g_sTxBuffer);
            USBBufferFlush(&g_sRxBuffer);

            //
            // Tell the main loop to update the display.
            //
            ui32IntsOff = IntMasterDisable();
            g_pcStatus = "Connected";
            g_ui32Flags |= COMMAND_STATUS_UPDATE;
            if(!ui32IntsOff)
            {
                IntMasterEnable();
            }
            break;

        //
        // The host has disconnected.
        //
        case USB_EVENT_DISCONNECTED:
            g_bUSBConfigured = false;
            ui32IntsOff = IntMasterDisable();
            g_pcStatus = "Disconnected";
            g_ui32Flags |= COMMAND_STATUS_UPDATE;
            if(!ui32IntsOff)
            {
                IntMasterEnable();
            }
            break;

        //
        // Return the current serial communication parameters.
        //
        case USBD_CDC_EVENT_GET_LINE_CODING:
            GetLineCoding(pvMsgData);
            break;

        //
        // Set the current serial communication parameters.
        //
        case USBD_CDC_EVENT_SET_LINE_CODING:
            SetLineCoding(pvMsgData);
            break;

        //
        // Set the current serial communication parameters.
        //
        case USBD_CDC_EVENT_SET_CONTROL_LINE_STATE:
            SetControlLineState((uint16_t)ui32MsgValue);
            break;

        //
        // Send a break condition on the serial line.
        //
        case USBD_CDC_EVENT_SEND_BREAK:
            SendBreak(true);
            break;

        //
        // Clear the break condition on the serial line.
        //
        case USBD_CDC_EVENT_CLEAR_BREAK:
            SendBreak(false);
            break;

        //
        // Ignore SUSPEND and RESUME for now.
        //
        case USB_EVENT_SUSPEND:
        case USB_EVENT_RESUME:
            break;

        //
        // We don't expect to receive any other events.  Ignore any that show
        // up in a release build or hang in a debug build.
        //
        default:
#ifdef DEBUG
            while(1);
#else
            break;
#endif

    }

    return(0);
}

//*****************************************************************************
//
// Handles CDC driver notifications related to the transmit channel (data to
// the USB host).
//
// \param pvCBData is the client-supplied callback pointer for this channel.
// \param ui32Event identifies the event we are being notified about.
// \param ui32MsgValue is an event-specific value.
// \param pvMsgData is an event-specific pointer.
//
// This function is called by the CDC driver to notify us of any events
// related to operation of the transmit data channel (the IN channel carrying
// data to the USB host).
//
// \return The return value is event-specific.
//
//*****************************************************************************
uint32_t
TxHandler(void *pvCBData, uint32_t ui32Event, uint32_t ui32MsgValue,
          void *pvMsgData)
{
    //
    // Which event have we been sent?
    //
    switch(ui32Event)
    {
        case USB_EVENT_TX_COMPLETE:
            //
            // Since we are using the USBBuffer, we don't need to do anything
            // here.
            //
            break;

        //
        // We don't expect to receive any other events.  Ignore any that show
        // up in a release build or hang in a debug build.
        //
        default:
#ifdef DEBUG
            while(1);
#else
            break;
#endif

    }
    return(0);
}

//*****************************************************************************
//
// Handles CDC driver notifications related to the receive channel (data from
// the USB host).
//
// \param pvCBData is the client-supplied callback data value for this channel.
// \param ui32Event identifies the event we are being notified about.
// \param ui32MsgValue is an event-specific value.
// \param pvMsgData is an event-specific pointer.
//
// This function is called by the CDC driver to notify us of any events
// related to operation of the receive data channel (the OUT channel carrying
// data from the USB host).
//
// \return The return value is event-specific.
//
//*****************************************************************************
uint32_t
RxHandler(void *pvCBData, uint32_t ui32Event, uint32_t ui32MsgValue,
          void *pvMsgData)
{
    uint32_t ui32Count;

    //
    // Which event are we being sent?
    //
    switch(ui32Event)
    {
        //
        // A new packet has been received.
        //
        case USB_EVENT_RX_AVAILABLE:
        {
            //
            // Feed some characters into the UART TX FIFO and enable the
            // interrupt so we are told when there is more space.
            //
            USBUARTPrimeTransmit(USB_UART_BASE);
            UARTIntEnable(USB_UART_BASE, UART_INT_TX);
            break;
        }

        //
        // We are being asked how much unprocessed data we have still to
        // process. We return 0 if the UART is currently idle or 1 if it is
        // in the process of transmitting something. The actual number of
        // bytes in the UART FIFO is not important here, merely whether or
        // not everything previously sent to us has been transmitted.
        //
        case USB_EVENT_DATA_REMAINING:
        {
            //
            // Get the number of bytes in the buffer and add 1 if some data
            // still has to clear the transmitter.
            //
            ui32Count = UARTBusy(USB_UART_BASE) ? 1 : 0;
            return(ui32Count);
        }

        //
        // We are being asked to provide a buffer into which the next packet
        // can be read. We do not support this mode of receiving data so let
        // the driver know by returning 0. The CDC driver should not be sending
        // this message but this is included just for illustration and
        // completeness.
        //
        case USB_EVENT_REQUEST_BUFFER:
        {
            return(0);
        }

        //
        // We don't expect to receive any other events.  Ignore any that show
        // up in a release build or hang in a debug build.
        //
        default:
#ifdef DEBUG
            while(1);
#else
            break;
#endif
    }

    return(0);
}

void ineedmd_radio_reset(void)
{
  //exerts the processor rest pin
  GPIOPinWrite (GPIO_PORTE_BASE, INEEDMD_RADIO_RESET_PIN,  INEEDMD_RADIO_RESET_PIN );


	//as we dont know the state of the processor or the timers we will do the delay in cpu clock cycles
	//about a 10th of a second
	//it would be nicer to have this as a proces sleep..
	SysCtlDelay( SysCtlClockGet() / 30  );

	//de-exert,sets it low, reset pin
	GPIOPinWrite (GPIO_PORTE_BASE, INEEDMD_RADIO_RESET_PIN, 0x00);
}

void check_for_reset(void)
{
  //USBPortEnable();
  iHW_delay(1000);
  uint32_t leadStat = ineedmd_adc_Check_Lead_Off();
#define LEAD_SHORT_RESET 0xA0FF
  if(leadStat == LEAD_SHORT_RESET)
  {
	  ineedmd_led_pattern(STORAGE_WARNING);
	  iHW_delay(7000);
	  //SW reset
	  HWREG(NVIC_APINT) = NVIC_APINT_VECTKEY | NVIC_APINT_SYSRESETREQ;
	  while(1)
	  {

	  }
  }
}

//*****************************************************************************
// name:
// description:
// param description:
// return value description:
//*****************************************************************************
void check_for_update(void)
{
  uint32_t uiLead_status;
  bool bUSB_plugged_in = false;

  iHW_delay(1000);

  uiLead_status = ineedmd_adc_Check_Lead_Off();
#define LEAD_SHORT 0x82FF
  if(uiLead_status == LEAD_SHORT)
  {/*
	USBPortEnable();
	iHW_delay(1000);

    bUSB_plugged_in = bIs_usb_physical_data_conn();
    if(1)
    {
    */
	  ineedmd_watchdog_feed();
    	ineedmd_led_pattern(DFU_MODE);
    	iHW_delay(1000);
#define SYSTICKS_PER_SECOND 100
      ROM_FPULazyStackingEnable();
      SysCtlClockSet(SYSCTL_SYSDIV_2_5 | SYSCTL_USE_PLL  | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ | SYSCTL_INT_OSC_DIS);
      SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
      GPIOPinTypeUSBAnalog(GPIO_PORTD_BASE, GPIO_PIN_5 | GPIO_PIN_4);
      uint32_t ui32SysClock = SysCtlClockGet();
      SysTickPeriodSet(SysCtlClockGet() / SYSTICKS_PER_SECOND);
      SysTickIntEnable();
      SysTickEnable();
      IntMasterDisable();
      SysTickIntDisable();
      SysTickDisable();
      HWREG(NVIC_DIS0) = 0xffffffff;
      HWREG(NVIC_DIS1) = 0xffffffff;
      // 1. Enable USB PLL
      // 2. Enable USB controller
      SysCtlPeripheralEnable(SYSCTL_PERIPH_USB0);
      SysCtlPeripheralReset(SYSCTL_PERIPH_USB0);
      SysCtlUSBPLLEnable();
      // 3. Enable USB D+ D- pins
      // 4. Activate USB DFU
      SysCtlDelay(ui32SysClock / 3);

      // Re-enable interrupts at NVIC level
      IntMasterEnable();

      //begin the DFU usb update procedure
      ROM_UpdateUSB(0);

      while(1);
    //}
  }
}

void UARTCDCSetup()
{
	//
	    // Enable the UART that we will be redirecting.
	    //
	    ROM_SysCtlPeripheralEnable(USB_UART_PERIPH);

	    //uart setup
	    HWREG(USB_UART_BASE + GPIO_O_LOCK) = GPIO_LOCK_KEY;
	    HWREG(USB_UART_BASE + GPIO_O_CR) = 0x1;
	        //
	        // Now modify the configuration of the pins that we unlocked.
	        //
	    GPIOPinConfigure(GPIO_PF0_U1RTS);
	    GPIOPinTypeUART(USB_UART_BASE, GPIO_PIN_0);
	        //
	        // Enable pin PF1 for UART1 U1CTS
	        //
	    GPIOPinConfigure(GPIO_PF1_U1CTS);
	    GPIOPinTypeUART(USB_UART_BASE, GPIO_PIN_1);

	    //rx tx
	    GPIOPinConfigure(GPIO_PC5_U1TX);
	    ROM_GPIOPinTypeUART(TX_GPIO_BASE, TX_GPIO_PIN);
	    GPIOPinConfigure(GPIO_PC4_U1RX);
	    ROM_GPIOPinTypeUART(RX_GPIO_BASE, RX_GPIO_PIN);

	    //
	    // Set the default UART configuration.
	    //
	    //UARTIntClear(USB_UART_BASE, UARTIntStatus(USB_UART_BASE, false));
	    UARTClockSourceSet(USB_UART_BASE, UART_CLOCK_PIOSC);
	    UARTConfigSetExpClk(USB_UART_BASE, 16000000, 115200, ( UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));
	    //UARTConfigSetExpClk(USB_UART_BASE, SysCtlClockGet(),
	    //                        DEFAULT_BIT_RATE, DEFAULT_UART_CONFIG);
	    UARTFIFOLevelSet(USB_UART_BASE, UART_FIFO_TX1_8, UART_FIFO_RX1_8);
	    UARTFIFODisable(USB_UART_BASE);
	    //
	    // Configure and enable UART interrupts.
	    //

	    UARTEnable(USB_UART_BASE);

}

void USBSetup()
{
	ROM_FPULazyStackingEnable();
	    SysCtlClockSet(SYSCTL_SYSDIV_2_5 | SYSCTL_USE_PLL  | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ | SYSCTL_INT_OSC_DIS);
	    iHW_delay(1000);

	        ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_USB0);
	        //
	        // Enable pin PD4 for USB0 USB0DM
	        //
	        ROM_GPIOPinTypeUSBAnalog(GPIO_PORTD_BASE, GPIO_PIN_4);
	        //
	        // Enable pin PD5 for USB0 USB0DP
	        //
	        ROM_GPIOPinTypeUSBAnalog(GPIO_PORTD_BASE, GPIO_PIN_5);

	        ROM_FPULazyStackingEnable();
	        ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
	        ROM_GPIOPinTypeUSBAnalog(GPIO_PORTD_BASE, GPIO_PIN_5 | GPIO_PIN_4);
}

//TODO: this is main
//*****************************************************************************
//
// This is the main application entry function.
//
//*****************************************************************************
int
main(void)
{
	//modified board init function
	iBoard_init();
	ineedmd_watchdog_setup();
	ineedmd_watchdog_feed();
	IntMasterDisable();
	GPIOPinTypeGPIOOutput(GPIO_PORTE_BASE, INEEDMD_RADIO_ENABLE_PIN);
	GPIOPinWrite(GPIO_PORTE_BASE, INEEDMD_RADIO_ENABLE_PIN, INEEDMD_RADIO_ENABLE_PIN);
	GPIOPinTypeGPIOOutput(GPIO_PORTE_BASE, INEEDMD_RADIO_RESET_PIN);
	ineedmd_led_pattern(LED_OFF);
	ineedmd_led_pattern(POWER_ON_BATGOOD);
	iRadio_Power_Off();
	iHW_delay(1000);
	iRadio_Power_On();
	ineedmd_radio_reset();
	iADC_setup();
	/*
	//for debug - tristate radio UART to use ftdi adapter -> radio directly
	PowerInitFunction();
		  GPIOEnable();
		  GPIOPinTypeGPIOOutput(GPIO_PORTE_BASE, INEEDMD_RADIO_ENABLE_PIN);
	      GPIOPinWrite(GPIO_PORTE_BASE, INEEDMD_RADIO_ENABLE_PIN, INEEDMD_RADIO_ENABLE_PIN);
	      GPIOPinTypeGPIOOutput(GPIO_PORTE_BASE, INEEDMD_RADIO_RESET_PIN);
	      iRadio_Power_On();
	      ineedmd_radio_reset();
		  GPIOPinTypeGPIOInput(GPIO_PORTC_BASE , GPIO_PIN_5);
		  GPIOPinTypeGPIOInput(GPIO_PORTC_BASE , GPIO_PIN_4);
		  GPIOPinTypeGPIOInput(GPIO_PORTF_BASE , GPIO_PIN_0);
		  GPIOPinTypeGPIOInput(GPIO_PORTF_BASE , GPIO_PIN_1);
	while(1){}
	*/

	//setup USB0
	USBSetup();

    g_bUSBConfigured = false;

	//configure UART1
    UARTCDCSetup();

    // Initialize USB CDC
    USBBufferInit(&g_sTxBuffer);
    USBBufferInit(&g_sRxBuffer);
    USBStackModeSet(0, eUSBModeDevice, 0);
    USBDCDCInit(0, &g_sCDCDevice);

    //enable interrupts
    IntEnable(USB_UART_INT);
    UARTIntClear(USB_UART_BASE, UARTIntStatus(USB_UART_BASE, false));
    UARTIntEnable(USB_UART_BASE, (UART_INT_OE | UART_INT_BE | UART_INT_PE | UART_INT_FE | UART_INT_RT | UART_INT_TX | UART_INT_RX));
    IntMasterEnable();
    ineedmd_watchdog_pat();

    ineedmd_led_pattern(BT_CONNECTED);
    while(1)
    {
    	ineedmd_watchdog_pat();
    	check_for_reset();
    	check_for_update();
    }

}

