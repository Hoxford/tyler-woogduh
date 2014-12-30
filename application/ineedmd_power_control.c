/*
 * ineedmd_power_control.c
 *
 *  Created on: Jul 30, 2014
 *      Author: BrianS
 */
#ifndef __INEEDMD_POWER_CONTROL_C__
#define __INEEDMD_POWER_CONTROL_C__
/******************************************************************************
* includes ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>

#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "inc/hw_gpio.h"
#include "inc/hw_sysctl.h"
#include "driverlib/adc.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/i2c.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/ssi.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "driverlib/usb.h"
#include "driverlib/timer.h"
#include "driverlib/interrupt.h"

#include "utils_inc/error_codes.h"
#include "utils_inc/osal.h"
#include "utils_inc/proj_debug.h"

#include "board.h"

#include "drivers_inc/ineedmd_adc.h"
#include "drivers_inc/ineedmd_bluetooth_radio.h"
#include "drivers_inc/ineedmd_USB.h"
#include "inc/tm4c1233h6pm.h"
#include "app_inc/ineedmd_power_control.h"
#include "app_inc/ineedmd_UI.h"

/******************************************************************************
* defines /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/

#define EKG_IS_ACTIVE()                  true
#define EKG_IS_NOT_ACTIVE()              false
#define RADIO_IS_ACTIVE()                true
#define SYSTEM_IN_SERVICE()              true
#define UNIT_TEMPERATURE_IN_TOLERANCE()  true

#define debug_printf vDEBUG

#define POWER_CONTROL_SHUTDOWN_TIMEOUT   (60000 * 10)

/******************************************************************************
* variables ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/
/******************************************************************************
* external variables //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/

/******************************************************************************
* enums ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/

ePower_Control_state eTask_state = PWR_CTRL_TASK_NONE;

typedef enum PWR_CNTRL_MESSAGE_ID
{
  PC_MESSAGE_NONE = 0,
  PC_MESSAGE_FORCE_SHUTDOWN,
  PC_MESSAGE_FORCE_LOW_POWER,
  PC_MESSAGE_FORCE_FULL_POWER,
  PC_MESSAGE_LIMIT
}PWR_CNTRL_MESSAGE_ID;

/******************************************************************************
* structures //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/
typedef struct tPowerControl_Activity_State
{
  bool     bIs_System_Service_Running;
  bool     bIs_EKG_Running;
  bool     bIs_Temp_In_Tolerance;
  bool     bIs_Radio_Ready;
  bool     bIs_Radio_On;
  bool     bIs_Radio_Connected;
  bool     bIs_USB_Data_Connected;
  uint32_t uiCurrent_Supply_Voltage;
}tPowerControl_Activity_State;

static tPowerControl_Activity_State tPC_Activity_State =
{
  false, //  bIs_System_Service_Running
  false, //  bIs_EKG_Running =
  false, //  bIs_Temp_In_Tolerance =
  false, //  bIs_Radio_Ready =
  false, //  bIs_Radio_On;
  false, //  bIs_Radio_Connected =
  false, //  bIs_USB_Data_Connected =
  0,     //  uiCurrent_Supply_Voltage =
};

typedef struct tPC_Message
{
  PWR_CNTRL_MESSAGE_ID eMessage;
}tPC_Message;

extern Mailbox_Handle tPC_mailbox;
extern Clock_Handle tPower_Control_shutdown_timer;

/******************************************************************************
* external functions //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/

/******************************************************************************
* private function declarations ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/

void vPC_Radio_connection_callback      (bool bRadio_Carrier_Connection, bool bRadio_Protocol_Connection);
void vPC_Radio_change_settings_callback (bool bDid_Setting_Change);
void vPC_Radio_sent_frame_callback      (uint32_t uiCount);
void vPC_Radio_rcv_frame_callback       (uint8_t * pBuff, uint32_t uiRcvd_len);
void vPC_Radio_setup_callback           (bool bRadio_Ready, bool bRadio_On);

void vPC_USB_conn_callback(bool bPhys_conn, bool bData_conn);

void vPower_Control_shtdwn_timeout_int_service(void);
ERROR_CODE ePower_Control_Stop_shutdown_clock(void);
ERROR_CODE ePower_Control_Start_shutdown_clock(void);

//extern uint32_t ineedmd_voltage(void);

//ERROR_CODE ineedmd_init_temperature_adc(void);

//ERROR_CODE ineedmd_init_voltage_adc(void);

//ERROR_CODE ineedmd_measure_temperature_adc(void);

//ERROR_CODE ineedmd_measure_voltage_adc(void);

//ERROR_CODE ineedmd_initial_condition_temperature_adc(void);

//ERROR_CODE ineedmd_initial_condition_voltage_adc(void);

//void ineedmd_average_temperature_adc(void);

//void ineedmd_average_voltage_adc(void);

//extern void vIineedmd_system_measurement();

/******************************************************************************
* private functions ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/
/******************************************************************************
* name:
* description:
* param description: type - value: value description (in order from left to right)
*                    bool - true: do action when set to true
* return value description: type - value: value description
******************************************************************************/
//int example_PrivateFunction(param_1, param_2)
//{
//}

void vPC_Radio_connection_callback(bool bRadio_Carrier_Connection, bool bRadio_Protocol_Connection)
{
  tPC_Activity_State.bIs_Radio_Connected = bRadio_Protocol_Connection;
  return;
}

void vPC_Radio_change_settings_callback (bool bDid_Setting_Change)
{
  return;
}

void vPC_Radio_sent_frame_callback(uint32_t uiCount)
{
  return;
}

void vPC_Radio_rcv_frame_callback(uint8_t * pBuff, uint32_t uiRcvd_len)
{
  return;
}

void vPC_Radio_setup_callback(bool bRadio_Ready, bool bRadio_On)
{
  tPC_Activity_State.bIs_Radio_Ready = bRadio_Ready;
  tPC_Activity_State.bIs_Radio_On = bRadio_On;
  return;
}

void vPC_USB_conn_callback(bool bPhys_conn, bool bData_conn)
{
  tPC_Activity_State.bIs_USB_Data_Connected = bData_conn;
  return;
}

void vPower_Control_shtdwn_timeout_int_service(void)
{
  tPC_Message    tPC_Msg;

  //perform last moment check to ensure the system can be shut down
  //
  if((tPC_Activity_State.bIs_EKG_Running            == false) &\
     (tPC_Activity_State.bIs_Radio_Connected        == false) &\
     (tPC_Activity_State.bIs_System_Service_Running == false) &\
     (tPC_Activity_State.bIs_USB_Data_Connected     == false))
  {
    tPC_Msg.eMessage = PC_MESSAGE_FORCE_SHUTDOWN;
    Mailbox_post(tPC_mailbox, &tPC_Msg, BIOS_WAIT_FOREVER);
  }

  return;
}

ERROR_CODE ePower_Control_Stop_shutdown_clock(void)
{
  ERROR_CODE eEC = ER_FAIL;
  bool bIs_Clock_Running = false;

  if(tPower_Control_shutdown_timer != NULL)
  {
    eEC = ER_OK;
    bIs_Clock_Running = Clock_isActive(tPower_Control_shutdown_timer);
    if(bIs_Clock_Running == true)
    {
      Clock_stop(tPower_Control_shutdown_timer);
      vDEBUG("Shut down clock stopped");
    }
  }

  return eEC;
}

ERROR_CODE ePower_Control_Start_shutdown_clock(void)
{
  ERROR_CODE eEC = ER_FAIL;
  bool bIs_Clock_Running = false;

  if(tPower_Control_shutdown_timer != NULL)
  {
    eEC = ER_OK;
    bIs_Clock_Running = Clock_isActive(tPower_Control_shutdown_timer);
    if(bIs_Clock_Running == false)
    {
      Clock_setTimeout(tPower_Control_shutdown_timer, (POWER_CONTROL_SHUTDOWN_TIMEOUT));
      Clock_start(tPower_Control_shutdown_timer);
      vDEBUG("Shut down clock started");
    }
  }

  return eEC;
}

/******************************************************************************
* public functions ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/

/******************************************************************************
* name: uiPower_Control_Get_Supply_Voltage
*
* description: returns a unisgned int vale of the current system supply voltage
*
* param description: none
*
* return value description: int - 0: system voltage not read yet
*                               - -1: system voltage read failure
*                               - integer: current voltage value in millivolts
******************************************************************************/
int uiPower_Control_Get_Supply_Voltage(void)
{
  int iSupply_Voltage = 0;
  ePower_Control_state eState;

  //check the current supply voltage value
  if(tPC_Activity_State.uiCurrent_Supply_Voltage == 0)
  {
    //supply voltage is not a valid value
    //
    //get and check the current power control task state
    eState = ePower_Control_Get_Task_State();
    if(eState == PWR_CTRL_TASK_NONE)
    {
      //The power control task has not started yet and the supply voltage has not been read yet
      //set the return value to not read yet
      iSupply_Voltage = 0;
    }
    else
    {
      //The power control task has started but there was an error reading the supply voltage
      //set the return value to error
      iSupply_Voltage = -1;
    }

  }
  else
  {
    //supply voltage is a valid value
    //
    //Set the return value to the current supply voltage value
    iSupply_Voltage = (int)tPC_Activity_State.uiCurrent_Supply_Voltage;
  }

  return iSupply_Voltage;
}

/******************************************************************************
* name: ePower_Control_Get_Task_State
* description: returns the current state of the power control task
* param description: type - value: value description (in order from left to right)
*                    bool - true: do action when set to true
* return value description: type - value: value description
******************************************************************************/
ePower_Control_state ePower_Control_Get_Task_State(void)
{
  return eTask_state;
}

ERROR_CODE ePower_Control_Request_params_init(tPower_control_request * tParams)
{
  ERROR_CODE eEC = ER_FAIL;

  return eEC;
}

ERROR_CODE ePower_Control_Request(tPower_control_request * tRequest)
{
  ERROR_CODE eEC = ER_FAIL;
  tPC_Message    tPC_Msg;

  switch(tRequest->eRequest)
  {
    case PC_REQUEST_FORCE_SHUTDOWN:
    {
      tPC_Msg.eMessage = PC_MESSAGE_FORCE_SHUTDOWN;
      Mailbox_post(tPC_mailbox, &tPC_Msg, BIOS_WAIT_FOREVER);
      break;
    }
    default:
      vDEBUG_ASSERT("ePower_Control_Request, invalid request",0);
      break;
  }
  return eEC;
}

void vPower_Control_task(UArg a0, UArg a1)
{
  ERROR_CODE    eEC = ER_FAIL;
  tPC_Message    tPC_Msg;
  tRadio_request * tRadio_req;
  tUSB_req       tUSB_request;
  ADC_REQUEST    tADC_req;
  tUI_request    tUI_req;
  int i = 0;
  uint32_t uiMsg_size = sizeof(tPC_Message);
  vDEBUG_ASSERT("vPower_Control_task, invalid mailbox msg size!",(uiMsg_size == Mailbox_getMsgSize(tPC_mailbox)));

  tRadio_req = (tRadio_request * )calloc(sizeof(tRadio_request), sizeof(char));

  eIneedmd_radio_request_params_init (tRadio_req);
  tRadio_req->eRequest                    = RADIO_REQUEST_REGISTER_CALLBACKS;
  tRadio_req->vBuff_sent_callback         = &vPC_Radio_sent_frame_callback;
  tRadio_req->vBuff_receive_callback      = &vPC_Radio_rcv_frame_callback;
  tRadio_req->vChange_setting_callback    = &vPC_Radio_change_settings_callback;
  tRadio_req->vConnection_status_callback = &vPC_Radio_connection_callback;
  tRadio_req->vSetup_state_callback       = &vPC_Radio_setup_callback;
  eIneedmd_radio_request(tRadio_req);
  free(tRadio_req);

  eUSB_request_params_init(&tUSB_request);
  tUSB_request.eRequest = USB_REQUEST_REGISTER_CONN_CALLBACK;
  tUSB_request.vUSB_connection_callback = &vPC_USB_conn_callback;
  eUSB_request(&tUSB_request);

  while(1)
  {
    if(Mailbox_pend(tPC_mailbox, &tPC_Msg, 10000) == true)
    {
      switch(tPC_Msg.eMessage)
      {
        case PC_MESSAGE_FORCE_SHUTDOWN:
          //shut off all UI elements
          //
          eIneedmd_UI_params_init(&tUI_req);
          tUI_req.uiUI_element = (INMD_UI_ELEMENT_HEART_LED | INMD_UI_ELEMENT_COMMS_LED |\
                                  INMD_UI_ELEMENT_POWER_LED | INMD_UI_ELEMENT_SOUNDER);
          tUI_req.eHeart_led_sequence = HEART_LED_UI_PERMANENT_OFF;
          tUI_req.eComms_led_sequence = COMMS_LED_UI_PERMANENT_OFF;
          tUI_req.ePower_led_sequence = POWER_LED_UI_PERMANENT_OFF;
          tUI_req.eAlert_sound = ALERT_SOUND_UI_PERMANENT_OFF;
          eIneedmd_UI_request(&tUI_req);

          //shut off radio
          //

          tRadio_req = (tRadio_request * )calloc(sizeof(tRadio_request), sizeof(char));
          eIneedmd_radio_request_params_init (tRadio_req);
          tRadio_req->eRequest = RADIO_REQUEST_PERMANENT_POWER_OFF;
          eEC = eIneedmd_radio_request(tRadio_req);
          for(i = 0; ((eEC == ER_NOT_READY) & (i < 10)); i++)
          {
            Task_sleep(1000);
            eEC = eIneedmd_radio_request(tRadio_req);
          }
          free(tRadio_req);

          //shut off ADS
          //
          eADC_Request_param_init(&tADC_req);
          tADC_req.eRequest_ID = ADC_REQUEST_ADS_PERMANENT_POWER_OFF;
          eADC_Request(&tADC_req);

          vDEBUG("vPower_Control_task System shutting down");
          //delay to allow debug shutdown msg out
          eOSAL_delay(100, NULL);

          vBSP_Power_System_Down();
          break;
        case PC_MESSAGE_FORCE_LOW_POWER:
          break;
        case PC_MESSAGE_FORCE_FULL_POWER:
          break;
        default:
          vDEBUG_ASSERT("vPower_Control_task, invalid message",0);
          break;
      }
    }
    else
    {
      //todo:check EKG activity
      tPC_Activity_State.bIs_EKG_Running = EKG_IS_NOT_ACTIVE();

      //todo: check temperature
      tPC_Activity_State.bIs_Temp_In_Tolerance = UNIT_TEMPERATURE_IN_TOLERANCE();
      if(tPC_Activity_State.bIs_Temp_In_Tolerance == false)
      {
        //todo: shut down immediatly
      }

      if(tPC_Activity_State.bIs_USB_Data_Connected == true)
      {
        ePower_Control_Stop_shutdown_clock();
      }
      else
      {
        if(tPC_Activity_State.bIs_Radio_Ready == true)
        {
          if((tPC_Activity_State.bIs_EKG_Running            == false) &\
             (tPC_Activity_State.bIs_Radio_Connected        == false) &\
             (tPC_Activity_State.bIs_System_Service_Running == false))
          {
            ePower_Control_Start_shutdown_clock();
          }
          else
          {
            ePower_Control_Stop_shutdown_clock();
          }
        }
        else
        {
          ePower_Control_Start_shutdown_clock();
        }
      }
    }
  }
}

#ifdef OLD_CODE
void shut_it_all_down()
{
//#define DEBUG_shut_it_all_down
#ifdef DEBUG_shut_it_all_down
  #define  vDEBUG_SHUT_DWN  debug_printf
#else
  #define vDEBUG_SHUT_DWN(a)
#endif
       debug_printf("going to sleep");
        // power down the radio
        if (iRadio_Power_Off()==1)
        {
          vDEBUG_SHUT_DWN("..radio asleep");
        }
        // power down the ADC
        if (EKGSPIDisable()==1)
        {
          vDEBUG_SHUT_DWN("..EKG ADC asleep");
        }
        // led's off  controller powers itself down 5ms after LEDS are off
        eIneedmd_UI_request(INMD_UI_LED, LED_SEQ_OFF, SPEAKER_SEQ_NONE, true);
        LEDI2CDisable();
        // power down ADC

//        if (BatMeasureADCDisable()==1)
//        {
//          vDEBUG_SHUT_DWN("..Battery measurement asleep");
//        }

//        USBPortDisable();
#undef vDEBUG_SHUT_DWN
}


void sleep_for_tenths(int number_tenths_seconds)
{
//#define DEBUG_sleep_for_tenths
#ifdef DEBUG_sleep_for_tenths
  #define  vDEBUG_SLEEP_10THS  debug_printf
#else
  #define vDEBUG_SLEEP_10THS(a)
#endif
//#ifdef SLEEP_FOR_TENTHS_TO_BE_REMOVED
  uint16_t uiPrev_sys_speed = 0;
  uint16_t uiCurr_sys_speed = 0;

  //Get the current system speed
  eGet_system_speed(&uiPrev_sys_speed);
  // start timer
  //
  // Set the Timer0B load value to 10s.
  //
//  ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
//  eMaster_int_enable();
//  ROM_TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);
//  TimerLoadSet(TIMER0_BASE, TIMER_A, (50000 * number_tenths_seconds) );

  //
  // Enable processor interrupts.
  //
  eMaster_int_enable();
  //
  // Configure the Timer0 interrupt for timer timeout.
  //
//  TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
  //
  // Enable the Timer0A interrupt on the processor (NVIC).
  //
//  IntEnable(INT_TIMER0A);
  //
  // clocks down the processor to REALLY slow ( 500khz) and
  //
  // go to a slow clock
  uiCurr_sys_speed = uiPrev_sys_speed;
//todo: this causes problems with the led patterns and ineedmd protocl implementation making the system unresponsive
//    if (set_system_speed (INEEDMD_CPU_SPEED_SLOW_INTERNAL) == INEEDMD_CPU_SPEED_SLOW_INTERNAL)
//    {
//      vDEBUG_SLEEP_10THS("..CPU slow");
//      uiCurr_sys_speed = INEEDMD_CPU_SPEED_SLOW_INTERNAL;
//    }

  uiCurr_sys_speed = set_system_speed (INEEDMD_CPU_SPEED_HALF_INTERNAL_OSC);

  //
  // Enable Timer0(A)
  //
//  TimerEnable(TIMER0_BASE, TIMER_A);

  // and deep sleep.
  ROM_SysCtlDeepSleep();

//  TimerDisable(TIMER0_BASE, TIMER_A);
//  IntDisable(INT_TIMER0A);
  vDEBUG_SLEEP_10THS("waking_up");

  //restore the systemclock
  if(uiCurr_sys_speed != uiPrev_sys_speed)
  {
    set_system_speed(uiPrev_sys_speed);
  }

  eMaster_int_enable();
//#endif //#ifdef SLEEP_FOR_TENTHS_TO_BE_REMOVED
#undef vDEBUG_SLEEP_10THS
}

/* ***************************************************
 * sleeps for 10seconds on low power mode
 *
 * this is designed for the led4 sleep with a flash of
 * the battery state while in sleep
 *****************************************************/
void ineedmd_sleep(void)
{
  ERROR_CODE eEC = ER_OK;
  ERROR_CODE eEC_did_timer_expire = ER_FALSE;
  uint32_t uiCurrent_sys_clock = 0;
  uint32_t uiADC_warmup = 0;
  uint32_t i = 0;

  uint16_t uiPrevious_speed = 0;

  //get the current system speed index
  eEC = eGet_system_speed(&uiPrevious_speed);
  if(eEC == ER_NOT_SET)
  {
    uiPrevious_speed = set_system_speed (INEEDMD_CPU_SPEED_DEFAULT);
  }else{/* do nothing */}

  //shut down the LED's
  eIneedmd_UI_request(INMD_UI_LED, LED_SEQ_OFF, SPEAKER_SEQ_NONE, true);

  //disable the spi port
  EKGSPIDisable();

  //power down the ADC
  ineedmd_adc_Power_Off();

  //turn the radio off
  iRadio_Power_Off();

  // Set a timer to wake the processor out of sleep at a specified interval
  //
  //Enable the timer
  MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
//  SysCtlPeripheralSleepEnable(SYSCTL_PERIPH_TIMER0);

  //Config the timer to be a single shot
  MAP_TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);

  // Configure and enable the Timer interrupt for timer timeout
  MAP_IntEnable(INT_TIMER0A);
  MAP_TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

  // Slow the system speed to conserve power
  set_system_speed (INEEDMD_CPU_SPEED_SLOW_INTERNAL);
  uiCurrent_sys_clock = MAP_SysCtlClockGet();

  //Set the timer to timeout based on the new system speed
  MAP_TimerLoadSet(TIMER0_BASE, TIMER_A, uiCurrent_sys_clock);

  // Clear the timer set vale and enable the timer
  eEC_did_timer_expire = eBSP_Did_Timer0_Expire(true);
  MAP_TimerEnable(TIMER0_BASE, TIMER_A);


  eEC_did_timer_expire = eBSP_Did_Timer0_Expire(true);
  i = 0;
  while(eEC_did_timer_expire == ER_FALSE)
  {
    i += iHW_delay(1);
    if(i >= 1000)
    {
      break;
    }
    eEC_did_timer_expire = eBSP_Did_Timer0_Expire(true);
  }

  //set the system speed to what it was originally
  set_system_speed (uiPrevious_speed);

  MAP_IntDisable(INT_TIMER0A);
  MAP_TimerDisable(TIMER0_BASE, TIMER_A);

  switch_on_adc_for_lead_detection();

  uiADC_warmup = ineedmd_adc_Check_Lead_Off();
  uiADC_warmup &= 0x000000FF;
  i = 200;
  while(uiADC_warmup != 0x000000FF)
  {
    ineedmd_adc_Power_Off();
    switch_on_adc_for_lead_detection();
    iHW_delay(i);
    i+= 100;
    uiADC_warmup = ineedmd_adc_Check_Lead_Off();
    uiADC_warmup &= 0x000000FF;
  }

  //power the radio back on
  iRadio_Power_On();

  //re-enable master interrupts
  eMaster_int_enable();
}

/******************************************************************************
* name:
* description:
* param description:
* return value description:
******************************************************************************/
void hold_until_short_removed(void)
{
  ERROR_CODE eEC = ER_OK;
  uint32_t uiLead_check = 0;
  uint16_t uiPrevious_speed = 0;

  eEC = eGet_system_speed(&uiPrevious_speed);
  if(eEC == ER_NOT_SET)
  {
    uiPrevious_speed = INEEDMD_CPU_SPEED_FULL_INTERNAL;
    set_system_speed (uiPrevious_speed);
  }else{/*nothing*/}

  switch_on_adc_for_lead_detection();

  uiLead_check = ineedmd_adc_Check_Lead_Off();

  while(uiLead_check == LEAD_SHORT_SLEEP)
  {
      //disable the spi port
    EKGSPIDisable();
    //power down the ADC
    ineedmd_adc_Power_Off();

    //
    // Set the Timer0B load value to 10s.
    //
    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
    eMaster_int_enable();
    MAP_TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);
    MAP_TimerLoadSet(TIMER0_BASE, TIMER_A, 5000000 );

    //
    // Enable processor interrupts.
    //todo: redundant?
    eMaster_int_enable();
    //
    // Configure the Timer0 interrupt for timer timeout.
    //
    MAP_TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    //
    // Enable the Timer0A interrupt on the processor (NVIC).
    //
    MAP_IntEnable(INT_TIMER0A);
    //
    // clocks down the processor to REALLY slow ( 500khz) and
    //
    set_system_speed (INEEDMD_CPU_SPEED_SLOW_INTERNAL);
    //
    // Enable Timer0(A)
    //
    MAP_TimerEnable(TIMER0_BASE, TIMER_A);

    MAP_SysCtlSleep();

    //comming out we turn the processor all the way up
    set_system_speed (INEEDMD_CPU_SPEED_FULL_INTERNAL);

    MAP_TimerDisable(TIMER0_BASE, TIMER_A);
    MAP_IntDisable(INT_TIMER0A);
    eMaster_int_disable();

    switch_on_adc_for_lead_detection();
    iHW_delay(100);

    uiLead_check = ineedmd_adc_Check_Lead_Off();
  }

  //re-enable master interrupts
  eMaster_int_enable();

  //set the system speed to what it was originally
  set_system_speed (uiPrevious_speed);
}
#endif //#ifdef OLD_CODE

#endif //#define __INEEDMD_POWER_CONTROL_C__
