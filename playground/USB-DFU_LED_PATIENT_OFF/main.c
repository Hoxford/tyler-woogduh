
#ifndef PART_TM4C1233H6PM
 #define PART_TM4C1233H6PM
#endif

#ifndef TARGET_IS_TM4C1233H6PM
 #define TARGET_IS_TM4C1233H6PM
#endif

// Set us to debug mode
#define DEBUG_CODE
//

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include <driverlib/rom.h>

//Pull the processor
#include <inc/tm4c1233h6pm.h>


//processor pin config files
#include <driverlib/i2c.h>
#include "driverlib/rom_map.h"

#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "inc/hw_gpio.h"
#include "inc/hw_nvic.h"
#include "driverlib/rom.h"
#include "driverlib/sysctl.h"
#include "driverlib/rom.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom_map.h"
#include "driverlib/gpio.h"
#include "driverlib/uart.h"
#include "driverlib/timer.h"
#include "driverlib/interrupt.h"
#include "driverlib/adc.h"

#include "utils_inc/proj_debug.h"

#include "board.h"

#include "ineedmd_led.h"
#include "ineedmd_adc.h"
#include "ineedmd_bluetooth_radio.h"
#include "ineedmd_power_modes.h"


//static volatile bool g_bIntFlag = false;


void led_test(void)
{

    //ints to use in switch stmt

    ineedmd_led_pattern(POWER_ON_BATGOOD);
    ineedmd_led_pattern(POWER_ON_BATLOW);
    ineedmd_led_pattern(BAT_CHARGING);
    ineedmd_led_pattern(BT_CONNECTED);
    ineedmd_led_pattern(BT_ATTEMPTING);
    ineedmd_led_pattern(BT_FAILED);
    ineedmd_led_pattern(USB_CONNECTED);
    ineedmd_led_pattern(USB_FAILED);
    ineedmd_led_pattern(DATA_TRANSFER);
    ineedmd_led_pattern(TRANSFER_DONE);
    ineedmd_led_pattern(STORAGE_WARNING);
    ineedmd_led_pattern(LEAD_LOOSE);
    ineedmd_led_pattern(ACQUIRE_UPLOAD_DATA);
    ineedmd_led_pattern(PATIENT_ALERT);
    ineedmd_led_pattern(LED_OFF);

}




//void bluetooth_setup(void)
//{
//    uint32_t i;
//  char  *send_string;
//  //char *recieve_string;
//
//
//
//  ineedmd_radio_power(true);
//
//  ineedmd_radio_reset();
//
//  //
//    // Configure the UART for 115,200, 8-N-1 operation.
//    // This function uses SysCtlClockGet() to get the system clock
//    // frequency.  This could be also be a variable or hard coded value
//    // instead of a function call.
//    //
//    UARTConfigSetExpClk( UART1_BASE, MAP_SysCtlClockGet(), 115200, ( UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE ));
//
//    UARTEnable(UART1_BASE);
//
//
//    //tell the radio we are using BT SSP pairing
//    send_string = "SET BT SSP 1 1\n";
//    for (i = 0; i<strlen(send_string); i++)
//  {
//    UARTCharPut(UART1_BASE, send_string[i]);
//    while(UARTBusy(UART1_BASE)){}
//  }
//  //tells the radio we are using SPP protocol
//  send_string = "SET PROFILE SPP\n";
//    for (i = 0; i<strlen(send_string); i++)
//  {
//    UARTCharPut(UART1_BASE, send_string[i]);
//    while(UARTBusy(UART1_BASE)){}
//  }
//
//  // sets the battery mode for the radio,  configures the - low bat warning voltage - the low voltage lock out - the charge release voltage - that this signal is radio GPIO 01
//  send_string = "SET CONTOL BATTERY 3300 3100 3400 01\n";
//    for (i = 0; i<strlen(send_string); i++)
//  {
//    UARTCharPut(UART1_BASE, send_string[i]);
//    while(UARTBusy(UART1_BASE)){}
//  }
//
//  //commits these changes to the radio
//  send_string = "SET\n";
//    for (i = 0; i<strlen(send_string); i++)
//  {
//    UARTCharPut(UART1_BASE, send_string[i]);
//    while(UARTBusy(UART1_BASE)){}
//  }
//
//}

/*
 * this routeen puts us in really deep sleep and waits till the short is removed - ist power up
 */

/*
 * check_battery()
 * this routeen checks for an update and then forces the update.
 */
int
check_battery(void){

  uint32_t  adc_reading;

  //check the state of the low battery pin and if it is low the we park the bus...
  BatMeasureADCEnable();
  ADCProcessorTrigger(ADC0_BASE, 3);
  while(!ADCIntStatus(ADC0_BASE, 3, false))
  {
  }
  ADCIntClear(ADC0_BASE, 3);
  ADCSequenceDataGet(ADC0_BASE, 3, &adc_reading);

  BatMeasureADCDisable();

  return adc_reading;

}


void switch_on_adc_for_lead_detection(void)
{
  //switch on the SPI port
  EKGSPIEnable();

  GPIOPinWrite(GPIO_PORTA_BASE, INEEDMD_PORTA_ADC_nCS_PIN, INEEDMD_PORTA_ADC_nCS_PIN);

  //when done set the CS high
  ineedmd_adc_Start_Low();
  //power on ADC, disable continuous conversions
  ineedmd_adc_Power_On();
  //turn off continuous conversion for register read/writes
  ineedmd_adc_Stop_Continuous_Conv();

  //id = INEEDMD_ADC_Get_ID();
  ineedmd_adc_Enable_Lead_Detect();
  //increase comparator threshold for lead off detect
  uint32_t regVal = ineedmd_adc_Register_Read(LOFF);
  ineedmd_adc_Register_Write(LOFF, (regVal | ILEAD_OFF0 | ILEAD_OFF1));
  //check that we can read the device if not reset

  //start conversions
  ineedmd_adc_Start_Internal_Reference();
  ineedmd_adc_Start_High();
}


void
hold_until_short_removed(void){

  uint32_t uiLead_statusA;
  uint32_t uiLead_statusB;
  uint32_t battery_level;

  //check the state of the short on the ekg connector
  //check the USB for a short
  //if not go back to sleep
  //if wake up  open up the system clocks and go for it...
  //

  uiLead_statusA = ineedmd_adc_Check_Lead_Off();
  uiLead_statusB = ineedmd_adc_Check_Lead_Off();

  #define LEAD_SHORT 0x82FF
  #define LEAD_OPEN  0xAAFF

  while(ineedmd_adc_Check_Lead_Off() == LEAD_SHORT | ineedmd_adc_Get_ID() != ADS1198_ID)
  {
    shut_it_all_down();
    sleep_for_tenths(290);

    LEDI2CEnable();
    battery_level = check_battery();
    if (battery_level > 2250)
      {
        ineedmd_led_pattern(LED_CHIRP_GREEN);
      }
    else if (battery_level > 2050)
      {
        ineedmd_led_pattern(LED_CHIRP_ORANGE);
      }
    else
      {
        ineedmd_led_pattern(LED_CHIRP_RED);
      }

    switch_on_adc_for_lead_detection();
  }
}


/*
 * check_battery()
 * this routeen checks for a low battery.  If the battery is low it goes to minimum power mode and waits till the battery is good.  There is the possibility of a low power lockout if this fails
 */
void
check_for_update(void){

  USBPortEnable();
  iHW_delay(1000);

  //check if there is a USB data connection attached
  bUSB_plugged_in = bIs_usb_physical_data_conn();
  if(bUSB_plugged_in == true)
    volatile uint32_t ui32Loop;
  //check the state of the short on the ekg connector
  //checks the short on the update_pin GPIO
  //if short call the map_usbupdate() to force a USB update.
  while(ineedmd_adc_Check_Lead_Off() != LEAD_OPEN)
  {
        #define SYSTICKS_PER_SECOND 100
    ROM_FPULazyStackingEnable();
      MAP_SysCtlClockSet(SYSCTL_SYSDIV_2_5 | SYSCTL_USE_PLL  | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ | SYSCTL_INT_OSC_DIS);
      MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
      MAP_GPIOPinTypeUSBAnalog(GPIO_PORTD_BASE, GPIO_PIN_5 | GPIO_PIN_4);
    uint32_t ui32SysClock = MAP_SysCtlClockGet();
    MAP_SysTickPeriodSet(MAP_SysCtlClockGet() / SYSTICKS_PER_SECOND);
    MAP_SysTickIntEnable();
    MAP_SysTickEnable();
      MAP_IntMasterDisable();
      MAP_SysTickIntDisable();
      MAP_SysTickDisable();
      HWREG(NVIC_DIS0) = 0xffffffff;
      HWREG(NVIC_DIS1) = 0xffffffff;
      // 1. Enable USB PLL
      // 2. Enable USB controller
    //    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_USB0);
      MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_USB0);
    //    ROM_SysCtlPeripheralReset(SYSCTL_PERIPH_USB0);
      MAP_SysCtlPeripheralReset(SYSCTL_PERIPH_USB0);
      MAP_SysCtlUSBPLLEnable();
      // 3. Enable USB D+ D- pins
      // 4. Activate USB DFU
      MAP_SysCtlDelay(ui32SysClock / 3);
      MAP_IntMasterEnable(); // Re-enable interrupts at NVIC level
      ROM_UpdateUSB(0);
      // 5. Should never get here since update is in progress
      //
      // Enable the GPIO port that is used for the on-board LED.
      //
      SYSCTL_RCGC2_R = SYSCTL_RCGC2_GPIOF;
      //
      // Do a dummy read to insert a few cycles after enabling the peripheral.
      //
      ui32Loop = SYSCTL_RCGC2_R;
      //
      // Enable the GPIO pin for the LED (PF3).  Set the direction as output, and
      // enable the GPIO pin for digital function.
      //
      GPIO_PORTF_DIR_R = 0x08;
      GPIO_PORTF_DEN_R = 0x08;
      while(1);
  }
  USBPortDisable();
}


}



/*
 * main.c
 */
void main(void) {

  volatile uint32_t ui32Loop;
  PowerInitFunction();
  GPIOEnable();

  vDEBUG_init();
  //enable the radio - so that it is out of reset and the battery charger is running...

  vDEBUG("hello world!");

  LEDI2CEnable();
  ineedmd_led_pattern(LED_OFF);

  ConfigureSleep();
  ConfigureDeepSleep();
  Set_Timer0_Sleep();

  switch_on_adc_for_lead_detection();

//#if DO_SHORT_HOLD
    hold_until_short_removed(); //todo: doesn't return and hangs on the lead check when short is not present
//#endif //DO_SHORT_HOLD

//    BatMeasureADCEnable();  //todo: doesn't return, hangs on MAP_ADCSequenceDisable(BATTERY_ADC, 3);
    LEDI2CEnable();

//    iIneedMD_radio_setup();  //todo: doesn't return, causes a hard fault

    while(1)

    {
      if (check_battery() > 2040 )
        {
          led_test();
        }
      else
        {
          sleep_for_tenths(290);
          ineedmd_led_pattern(LED_CHIRP_RED);
        }

     //check_for_update();

    }
}
