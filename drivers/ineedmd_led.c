/******************************************************************************
*
* ineedmd_led.c - led driver source code
* Copyright (c) notice
*
******************************************************************************/
#ifndef __INEEDMD_LED_H__
#define __INEEDMD_LED_H__

/******************************************************************************
* includes ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/
#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"

#include "driverlib/gpio.h"
#include "utils_inc/error_codes.h"
#include "utils_inc/proj_debug.h"
#include "drivers_inc/ineedmd_led.h"
#include "board.h"
#include "app_inc/ineedmd_power_modes.h"

#include <ti/drivers/I2C.h>
#include <ti/drivers/i2c/I2CTiva.h>

/******************************************************************************
* defines /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/
#define INEEDMD_I2C_SPEED_100KHZ        I2C_100kHz
#define INEEDMD_I2C_SPEED_400KHZ        I2C_400kHz

#define INEEDMD_I2C_ADDRESS_LED_DRIVER  0x1b

#define INEEDMD_LED_OFF                  0x00
#define INEEDMD_COMMS_RED_ENABLE         0x01
#define INEEDMD_COMMS_BLUE_ENABLE        0x02
#define INEEDMD_COMMS_GREEN_ENABLE       0x04
#define INEEDMD_HEART_RED_ENABLE         0x20
#define INEEDMD_HEART_BLUE_ENABLE        0x40
#define INEEDMD_HEART_GREEN_ENABLE       0x80

#define INEEDMD_COMMS_RED_ILEVEL         0x00
#define INEEDMD_COMMS_BLUE_ILEVEL        0x01
#define INEEDMD_COMMS_GREEN_ILEVEL       0x02

#define INEEDMD_HEART_RED_ILEVEL         0x05
#define INEEDMD_HEART_BLUE_ILEVEL        0x06
#define INEEDMD_HEART_GREEN_ILEVEL       0x07

#define INEEDMD_COMMS_RED_PWM            0x08
#define INEEDMD_COMMS_BLUE_PWM           0x09
#define INEEDMD_COMMS_GREEN_PWM          0x0a

#define INEEDMD_HEART_RED_PWM            0x0d
#define INEEDMD_HEART_BLUE_PWM           0x0e
#define INEEDMD_HEART_GREEN_PWM          0x0f

#define INEEDMD_LED_OUTPUT_CONTROL      0x10
#define INEEDMD_LED_STATUS_REGISTER     0x11

#define INEEDMD_LED_TOGGLE_ALL          0x00
#define INEEDMD_LED_PROGRAM_SINGLE      0x20
#define INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE 0x40
#define INEEDMD_LED_PROGRAM_ALL_ILEVEL  0x60
#define INEEDMD_LED_PROGRAM_ALL_PWM     0x80


/******************************************************************************
* variables ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/
bool bAre_LEDs_on = true;  //led on control variabl, start true to ensure they are turned off when LED_OFF is called
/******************************************************************************
* external variables
******************************************************************************/

/******************************************************************************
* external variables //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/

/******************************************************************************
* enums ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/

/******************************************************************************
* structures //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/
I2C_Handle tI2C_handle = NULL;
I2C_Params tI2C_params;

/******************************************************************************
* external functions //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/

/******************************************************************************
* private function declarations ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/

/******************************************************************************
* private functions ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/

/******************************************************************************
* public functions ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/
/******************************************************************************
* name: Filename_or_abreviation_funciton
* description:
* param description: type - value: value description (in order from left to right)
*                    bool - true: do action when set to true
* return value description: type - value: value description
******************************************************************************/
ERROR_CODE eIneedmd_LED_driver_setup(void)
{
  ERROR_CODE eEC = ER_FAIL;

  I2C_Params_init(&tI2C_params);

  tI2C_params.transferMode  = I2C_MODE_BLOCKING;
  tI2C_params.bitRate  = INEEDMD_I2C_SPEED_400KHZ;

  tI2C_handle = I2C_open(EK_TM4C123GXL_I2C_LED, &tI2C_params);
  if (tI2C_handle != NULL)
  {
    eEC = ER_OK;
  }
  else
  {
    eEC = ER_FAIL;
  }

  return eEC;
}

ERROR_CODE eIneedmd_LED_pattern(LED_MODE led_pattern)
{
#define DEBUG_ineedmd_led_pattern
#ifdef DEBUG_ineedmd_led_pattern
  #define  vDEBUG_INMD_LED_PAT  vDEBUG
#else
  #define vDEBUG_INMD_LED_PAT(a)
#endif

  uint8_t write_byte;
  uint8_t read_byte;
  I2C_Transaction i2cTransaction;
  uint8_t i2c_send_buffer[2];

  i2cTransaction.writeBuf = i2c_send_buffer;
  i2cTransaction.writeCount = 2;
  i2cTransaction.readBuf = NULL;
  i2cTransaction.readCount = 0;

  switch (led_pattern)
  {
    case POWER_ON_BATT_GOOD:

      i2c_send_buffer[0] = (INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_HEART_RED_PWM);
      i2c_send_buffer[1] = INEEDMD_LED_OFF;
      I2C_transfer (tI2C_handle, &i2cTransaction);

      i2c_send_buffer[0] = (INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_HEART_GREEN_PWM);
      i2c_send_buffer[1] = INEEDMD_LED_OFF;
      I2C_transfer (tI2C_handle, &i2cTransaction);

      i2c_send_buffer[0] = (INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_HEART_BLUE_PWM);
      i2c_send_buffer[1] = 20;
      I2C_transfer (tI2C_handle, &i2cTransaction);

      i2c_send_buffer[0] = (INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_HEART_RED_ILEVEL);
      i2c_send_buffer[1] = INEEDMD_LED_OFF;
      I2C_transfer (tI2C_handle, &i2cTransaction);

      i2c_send_buffer[0] = (INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_HEART_GREEN_ILEVEL);
      i2c_send_buffer[1] = INEEDMD_LED_OFF;
      I2C_transfer (tI2C_handle, &i2cTransaction);

      i2c_send_buffer[0] = (INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_HEART_BLUE_ILEVEL);
      i2c_send_buffer[1] = 0x1F;
      I2C_transfer (tI2C_handle, &i2cTransaction);

      //switch off all the LEDS not needed
      i2c_send_buffer[0] = (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL);
      i2c_send_buffer[1] = (read_byte & ~(INEEDMD_HEART_RED_ENABLE | INEEDMD_HEART_GREEN_ENABLE )) | INEEDMD_HEART_BLUE_ENABLE;
      I2C_transfer (tI2C_handle, &i2cTransaction);

      break;
    default:
      break;
  }
//        case LED_OFF:
//          if(bAre_LEDs_on == true)
//          {
//            write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED_OFF));
//            bAre_LEDs_on = false;
//          }else{/*do nothing*/}
//          break;
//        case POWER_ON_BATT_LOW_BLOCKING:
//          //power led 1/1 flashing rwd
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED2_RED_PWM, 0x20);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED2_RED_ILEVEL , 0x05);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED2_GREEN_ILEVEL , 0x00);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED2_BLUE_ILEVEL , 0x00);
//
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_RED_PWM, 0x20);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_RED_ILEVEL , 0x05);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_GREEN_ILEVEL , 0x00);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_BLUE_ILEVEL , 0x00);
//
//          write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED1_RED_ENABLE | INEEDMD_LED2_RED_ENABLE));
//
//          iHW_delay(LED_1_SEC_DELAY);
//
//          write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED_OFF));
//
//          break;
//        case POWER_ON_BATT_LOW:
//          //power led green
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED2_RED_PWM, 0x20);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED2_RED_ILEVEL , 0x05);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED2_GREEN_ILEVEL , 0x00);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED2_BLUE_ILEVEL , 0x00);
//
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_RED_PWM, 0x20);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_RED_ILEVEL , 0x05);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_GREEN_ILEVEL , 0x00);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_BLUE_ILEVEL , 0x00);
//          write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED_OFF));
//
//          break;
//        case POWER_ON_BATT_LOW_ON:
//          //power led green
//          write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED1_RED_ENABLE | INEEDMD_LED2_RED_ENABLE));
//
//          break;
//        case POWER_ON_BATT_LOW_OFF:
//          //power led green
//          write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED_OFF));
//
//          break;
//        case POWER_ON_BATT_GOOD_BLOCKING:
//          //power led orange
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED2_RED_PWM, 0x00);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED2_GREEN_PWM, 0x22);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED2_BLUE_PWM, 0x00);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED2_RED_ILEVEL , 0x00);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED2_GREEN_ILEVEL , 0x22);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED2_BLUE_ILEVEL , 0x00);
//
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_RED_PWM, 0x00);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_GREEN_PWM, 0x22);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_BLUE_PWM, 0x00);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_RED_ILEVEL , 0x00);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_GREEN_ILEVEL , 0x22);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_BLUE_ILEVEL , 0x00);
//
//          write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED2_GREEN_ENABLE | INEEDMD_LED2_RED_ENABLE | INEEDMD_LED1_GREEN_ENABLE | INEEDMD_LED1_RED_ENABLE));
//
//          iHW_delay(LED_1_SEC_DELAY);
//
//          write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED_OFF));
//
//          break;
//        case POWER_ON_BATT_GOOD:
//          //power led orange
//          write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED_OFF));
//
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED2_RED_PWM, 0x00);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED2_GREEN_PWM, 0x22);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED2_BLUE_PWM, 0x00);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED2_RED_ILEVEL , 0x00);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED2_GREEN_ILEVEL , 0x22);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED2_BLUE_ILEVEL , 0x00);
//
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_RED_PWM, 0x00);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_GREEN_PWM, 0x22);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_BLUE_PWM, 0x00);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_RED_ILEVEL , 0x00);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_GREEN_ILEVEL , 0x22);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_BLUE_ILEVEL , 0x00);
//
//
//          break;
//        case POWER_ON_BATT_GOOD_ON:
//          //power led orange
//          write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED2_GREEN_ENABLE | INEEDMD_LED2_RED_ENABLE | INEEDMD_LED1_GREEN_ENABLE | INEEDMD_LED1_RED_ENABLE));
//
//          break;
//        case POWER_ON_BATT_GOOD_OFF:
//          //power led orange
//          write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED_OFF));
//
//          break;
//        case BATT_CHARGING_BLOCKING:
//          //power led orange 1 on 1 off
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED2_RED_PWM, 23);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED2_GREEN_PWM, 22);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED2_BLUE_PWM, 0x00);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED2_RED_ILEVEL , 0x1f);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED2_GREEN_ILEVEL , 0x10);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED2_BLUE_ILEVEL , 0x00);
//
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_RED_PWM, 23);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_GREEN_PWM, 22);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_BLUE_PWM, 0x00);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_RED_ILEVEL , 0x1f);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_GREEN_ILEVEL , 0x10);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_BLUE_ILEVEL , 0x00);
//          write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED1_GREEN_ENABLE | INEEDMD_LED1_RED_ENABLE | INEEDMD_LED2_GREEN_ENABLE | INEEDMD_LED2_RED_ENABLE));
//
//          iHW_delay(LED_1_SEC_DELAY);
//
//          write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED_OFF));
//          break;
//        case BATT_CHARGING:
//          //power led orange
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED2_RED_PWM, 23);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED2_GREEN_PWM, 22);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED2_BLUE_PWM, 0x00);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED2_RED_ILEVEL , 0x1f);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED2_GREEN_ILEVEL , 0x10);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED2_BLUE_ILEVEL , 0x00);
//
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_RED_PWM, 23);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_GREEN_PWM, 22);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_BLUE_PWM, 0x00);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_RED_ILEVEL , 0x1f);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_GREEN_ILEVEL , 0x10);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_BLUE_ILEVEL , 0x00);
//          write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED_OFF));
//          break;
//        case BATT_CHARGING_ON:
//          //power led orange 1 on 1 off
//          //power led orange
//          write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED1_GREEN_ENABLE | INEEDMD_LED1_RED_ENABLE | INEEDMD_LED2_GREEN_ENABLE | INEEDMD_LED2_RED_ENABLE));
//          break;
//        case BATT_CHARGING_OFF:
//          //power led orange 1 on 1 off
//          //power led orange
//          write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED_OFF));
//          break;
//        case BATT_CHARGING_LOW_BLOCKING:
//          //power led red 1 on 1 off
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED2_RED_PWM, 23);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED2_GREEN_PWM, 22);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED2_BLUE_PWM, 0x00);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED2_RED_ILEVEL , 0x1f);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED2_GREEN_ILEVEL , 0x10);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED2_BLUE_ILEVEL , 0x00);
//
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_RED_PWM, 23);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_GREEN_PWM, 22);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_BLUE_PWM, 0x00);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_RED_ILEVEL , 0x1f);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_GREEN_ILEVEL , 0x10);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_BLUE_ILEVEL , 0x00);
//          write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED1_RED_ENABLE | INEEDMD_LED2_RED_ENABLE));
//
//          iHW_delay(LED_1_SEC_DELAY);
//
//          write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED_OFF));
//          break;
//        case BATT_CHARGING_LOW:
//          //power led red 1 on 1 off
//          write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED_OFF));
//
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED2_RED_PWM, 23);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED2_GREEN_PWM, 22);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED2_BLUE_PWM, 0x00);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED2_RED_ILEVEL , 0x1f);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED2_GREEN_ILEVEL , 0x10);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED2_BLUE_ILEVEL , 0x00);
//
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_RED_PWM, 23);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_GREEN_PWM, 22);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_BLUE_PWM, 0x00);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_RED_ILEVEL , 0x1f);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_GREEN_ILEVEL , 0x10);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_BLUE_ILEVEL , 0x00);
//
//          break;
//        case BATT_CHARGING_LOW_ON:
//          //power led red 1 on 1 off
//          write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED1_RED_ENABLE | INEEDMD_LED2_RED_ENABLE | INEEDMD_LED1_GREEN_ENABLE | INEEDMD_LED2_GREEN_ENABLE));
//          break;
//        case BATT_CHARGING_LOW_OFF:
//          //power led red 1 on 1 off
//          write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED_OFF));
//          break;
//        case LEAD_LOOSE_BLOCKING:
//          //EKG led orange on off 2 seconds
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED2_RED_PWM, 23);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED2_GREEN_PWM, 22);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED2_BLUE_PWM, 0x00);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED2_RED_ILEVEL , 0x1f);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED2_GREEN_ILEVEL , 0x10);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED2_BLUE_ILEVEL , 0x00);
//
//          write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED2_GREEN_ENABLE | INEEDMD_LED2_RED_ENABLE));
//
//          iHW_delay(LED_2_SEC_DELAY); //2 second
//
//          write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED_OFF));
//
//          break;
//        case LEAD_LOOSE:
//          //EKG led orange on off 2 seconds
//          write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED_OFF));
//
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED2_RED_PWM, 23);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED2_GREEN_PWM, 22);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED2_BLUE_PWM, 0x00);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED2_RED_ILEVEL , 0x1f);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED2_GREEN_ILEVEL , 0x10);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED2_BLUE_ILEVEL , 0x00);
//
//          break;
//        case LEAD_LOOSE_ON:
//          //EKG led orange on off 2 seconds
//          write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED2_GREEN_ENABLE | INEEDMD_LED2_RED_ENABLE));
//
//          break;
//        case LEAD_LOOSE_OFF:
//          //EKG led orange on off 2 seconds
//
//          write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED_OFF));
//          break;
//        case LEAD_GOOD_UPLOADING_BLOCKING:
//          //EKG led green, on off 2 seconds
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED2_GREEN_PWM, 0x20);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED2_RED_ILEVEL , 0x00);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED2_GREEN_ILEVEL , 0x05);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED2_BLUE_ILEVEL , 0x00);
//
//          write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED2_GREEN_ENABLE));
//
//          iHW_delay(LED_2_SEC_DELAY); //2 second
//
//          write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED_OFF));
//          break;
//
//        case LEAD_GOOD_UPLOADING:
//          //EKG led green, on off 2 seconds
//          write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED_OFF));
//
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED2_GREEN_PWM, 0x20);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED2_RED_ILEVEL , 0x00);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED2_GREEN_ILEVEL , 0x05);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED2_BLUE_ILEVEL , 0x00);
//          break;
//        case LEAD_GOOD_UPLOADING_ON:
//          //EKG led green, on off 2 seconds
//          write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED2_GREEN_ENABLE));
//
//          break;
//        case LEAD_GOOD_UPLOADING_OFF:
//          //EKG led green, on off 2 seconds
//          write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED_OFF));
//          break;
//
//        case DIG_FLATLINE_BLOCKING:
//          //EKG led red on/off 1/2 second
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED2_RED_PWM, 0x20);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED2_RED_ILEVEL , 0x10);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED2_GREEN_ILEVEL , 0x00);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED2_BLUE_ILEVEL , 0x00);
//
//          write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED2_RED_ENABLE));
//          iHW_delay(LED_0_5_SEC_DELAY); //1/2 second
//          write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED_OFF));
//          break;
//        case DIG_FLATLINE:
//          //EKG led red on/off 1/2 second
//          write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED_OFF));
//
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED2_RED_PWM, 0x20);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED2_RED_ILEVEL , 0x10);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED2_GREEN_ILEVEL , 0x00);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED2_BLUE_ILEVEL , 0x00);
//
//
//          break;
//        case DIG_FLATLINE_ON:
//          //EKG led red on/off 1/2 second
//          write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED2_RED_ENABLE));
//
//          break;
//        case DIG_FLATLINE_OFF:
//          //EKG led red on/off 1/2 second
//
//          write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED_OFF));
//          break;
//
//        case BT_CONNECTED_BLOCKING:
//          //COM led blue steady for 5 sec
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_BLUE_PWM, 0x20);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_RED_ILEVEL , 0x00);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_GREEN_ILEVEL , 0x00);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_BLUE_ILEVEL , 0x005);
//          write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED1_BLUE_ENABLE));
//          iHW_delay(LED_5_SEC_DELAY); //5 sec
//          write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED_OFF));
//          break;
//        case BT_CONNECTED:
//          //COM led blue steady for 5 sec
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_BLUE_PWM, 0x20);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_RED_ILEVEL , 0x00);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_GREEN_ILEVEL , 0x00);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_BLUE_ILEVEL , 0x005);
//
//          write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED_OFF));
//          break;
//        case BT_CONNECTED_ON:
//          //COM led blue steady for 5 sec
//
//          write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED1_BLUE_ENABLE));
//          break;
//        case BT_CONNECTED_OFF:
//          //COM led blue steady for 5 sec
//
//          write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED_OFF));
//          break;
//        case BT_ATTEMPTING_BLOCKING:
//          //COM led blue 1 on 1 off
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_BLUE_PWM, 0x20);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_RED_ILEVEL , 0x00);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_GREEN_ILEVEL , 0x00);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_BLUE_ILEVEL , 0x005);
//          write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED1_BLUE_ENABLE));
//          iHW_delay(LED_1_SEC_DELAY); //5 sec
//          write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED_OFF));
//          break;
//        case BT_ATTEMPTING:
//          //COM led blue 1 on 1 off
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_BLUE_PWM, 0x20);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_RED_ILEVEL , 0x00);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_GREEN_ILEVEL , 0x00);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_BLUE_ILEVEL , 0x005);
//          write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED_OFF));
//          break;
//        case BT_ATTEMPTING_ON:
//          //COM led blue 1 on 1 off
//          write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED1_BLUE_ENABLE));
//
//          break;
//        case BT_ATTEMPTING_OFF:
//          //COM led blue 1 on 1 off
//          write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED_OFF));
//
//          break;
//
//        case BT_FAILED_BLOCKING:
//          //COM Orange
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_RED_PWM, 23);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_GREEN_PWM, 22);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_BLUE_PWM, 0x00);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_RED_ILEVEL , 0x1f);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_GREEN_ILEVEL , 0x10);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_BLUE_ILEVEL , 0x00);
//
//          write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED1_GREEN_ENABLE | INEEDMD_LED1_RED_ENABLE));
//
//          iHW_delay(LED_5_SEC_DELAY); //1 second
//
//          write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED_OFF));
//
//          break;
//        case BT_FAILED:
//          //COM Orange
//          write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED_OFF));
//
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_RED_PWM, 23);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_GREEN_PWM, 22);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_BLUE_PWM, 0x00);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_RED_ILEVEL , 0x1f);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_GREEN_ILEVEL , 0x10);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_BLUE_ILEVEL , 0x00);
//          break;
//        case BT_FAILED_ON:
//          //COM Orange
//
//          write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED1_GREEN_ENABLE | INEEDMD_LED1_RED_ENABLE));
//
//          break;
//        case BT_FAILED_OFF:
//          //COM Orange
//          write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED_OFF));
//
//          break;
//
//        case USB_CONNECTED_BLOCKING:
//          //COM BLUE, steady 5 sec
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_BLUE_PWM, 0x20);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_RED_ILEVEL , 0x00);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_GREEN_ILEVEL , 0x00);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_BLUE_ILEVEL , 0x005);
//          write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED1_BLUE_ENABLE));
//
//          iHW_delay(LED_5_SEC_DELAY); //5 seconds
//
//          write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED_OFF));
//          break;
//
//        case USB_CONNECTED:
//          //COM BLUE, steady 5 sec
//          write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED_OFF));
//
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_BLUE_PWM, 0x20);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_RED_ILEVEL , 0x00);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_GREEN_ILEVEL , 0x00);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_BLUE_ILEVEL , 0x005);
//          break;
//
//        case USB_CONNECTED_ON:
//          //COM BLUE, steady 5 sec
//          write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED1_BLUE_ENABLE));
//          break;
//
//        case USB_CONNECTED_OFF:
//          //COM BLUE, steady 5 sec
//          write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED_OFF));
//          break;
//
//        case USB_FAILED_BLOCKING:
//          //COM orange steady
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_RED_PWM, 23);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_GREEN_PWM, 22);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_BLUE_PWM, 0x00);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_RED_ILEVEL , 0x1f);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_GREEN_ILEVEL , 0x10);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_BLUE_ILEVEL , 0x00);
//
//          write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED1_GREEN_ENABLE | INEEDMD_LED1_RED_ENABLE));
//
//          iHW_delay(LED_5_SEC_DELAY); //5 second
//          write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED_OFF));
//
//          break;
//        case USB_FAILED:
//          //COM orange steady
//          write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED_OFF));
//
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_RED_PWM, 23);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_GREEN_PWM, 22);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_BLUE_PWM, 0x00);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_RED_ILEVEL , 0x1f);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_GREEN_ILEVEL , 0x10);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_BLUE_ILEVEL , 0x00);
//
//
//          break;
//        case USB_FAILED_ON:
//          //COM orange steady
//
//          write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED1_GREEN_ENABLE | INEEDMD_LED1_RED_ENABLE));
//
//          break;
//        case USB_FAILED_OFF:
//          //COM orange steady
//
//          write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED_OFF));
//
//          break;
//        case DATA_TRANSFER_BLOCKING:
//          //COM purple 1 on one off
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_RED_PWM, 0x10);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_BLUE_PWM, 0x20);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_RED_ILEVEL , 0x05);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_GREEN_ILEVEL , 0x00);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_BLUE_ILEVEL , 0x05);
//
//          write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED1_RED_ENABLE | INEEDMD_LED1_BLUE_ENABLE));
//          iHW_delay(LED_1_SEC_DELAY); //5 second
//          write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED_OFF));
//          iHW_delay(LED_1_SEC_DELAY); //5 second
//          write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED1_RED_ENABLE | INEEDMD_LED1_BLUE_ENABLE));
//          iHW_delay(LED_1_SEC_DELAY); //5 second
//          write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED_OFF));
//          iHW_delay(LED_1_SEC_DELAY); //5 second
//          write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED1_RED_ENABLE | INEEDMD_LED1_BLUE_ENABLE));
//          iHW_delay(LED_1_SEC_DELAY); //5 second
//          write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED_OFF));
//
//
//          break;
//
//        case DATA_TRANSFER:
//          //COM purple 1 on one off
//          write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED_OFF));
//
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_RED_PWM, 0x10);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_BLUE_PWM, 0x20);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_RED_ILEVEL , 0x05);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_GREEN_ILEVEL , 0x00);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_BLUE_ILEVEL , 0x05);
//
//          break;
//        case DATA_TRANSFER_ON:
//          //COM purple 1 on one off
//          write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED1_RED_ENABLE | INEEDMD_LED1_BLUE_ENABLE));
//          break;
//        case DATA_TRANSFER_OFF:
//          //COM purple 1 on one off
//          write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED_OFF));
//          break;
//
//        case TRANSFER_DONE_BLOCKING:
//          //COM purple steady
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_RED_PWM, 0x10);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_BLUE_PWM, 0x20);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_RED_ILEVEL , 0x05);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_GREEN_ILEVEL , 0x00);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_BLUE_ILEVEL , 0x05);
//
//          write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED1_BLUE_ENABLE | INEEDMD_LED1_RED_ENABLE));
//
//          iHW_delay(LED_5_SEC_DELAY); //1 second
//
//          write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED_OFF));
//
//          break;
//        case TRANSFER_DONE:
//          //COM purple steady
//          write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED_OFF));
//
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_RED_PWM, 0x10);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_BLUE_PWM, 0x20);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_RED_ILEVEL , 0x05);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_GREEN_ILEVEL , 0x00);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_BLUE_ILEVEL , 0x05);
//
//          break;
//        case TRANSFER_DONE_ON:
//          //COM purple steady
//
//          write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED1_BLUE_ENABLE | INEEDMD_LED1_RED_ENABLE));
//
//          break;
//        case TRANSFER_DONE_OFF:
//          //COM purple steady
//
//          write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED_OFF));
//
//          break;
//
//        case STORAGE_WARNING_BLOCKING:
//          //COM LED orange
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_RED_PWM, 23);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_GREEN_PWM, 22);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_BLUE_PWM, 0x00);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_RED_ILEVEL , 0x1f);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_GREEN_ILEVEL , 0x10);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_BLUE_ILEVEL , 0x00);
//
//          write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED1_GREEN_ENABLE | INEEDMD_LED1_RED_ENABLE));
//
//          iHW_delay(LED_5_SEC_DELAY); //5 seconds
//          iHW_delay(LED_5_SEC_DELAY); //5 seconds
//
//          write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED_OFF));
//          break;
//
//        case STORAGE_WARNING:
//          //COM LED orange
//          write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED_OFF));
//
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_RED_PWM, 23);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_GREEN_PWM, 22);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_BLUE_PWM, 0x00);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_RED_ILEVEL , 0x1f);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_GREEN_ILEVEL , 0x10);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_BLUE_ILEVEL , 0x00);
//
//          break;
//        case STORAGE_WARNING_ON:
//          //COM LED orange
//
//          write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED1_GREEN_ENABLE | INEEDMD_LED1_RED_ENABLE));
//          break;
//        case STORAGE_WARNING_OFF:
//          //COM LED orange
//          write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED_OFF));
//          break;
//        case ERASING_BLOCKING:
//          //COM LED orange 2 on 2 off
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_RED_PWM, 23);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_GREEN_PWM, 22);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_BLUE_PWM, 0x00);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_RED_ILEVEL , 0x1f);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_GREEN_ILEVEL , 0x10);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_BLUE_ILEVEL , 0x00);
//
//          write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED1_GREEN_ENABLE | INEEDMD_LED1_RED_ENABLE));
//          iHW_delay(LED_2_SEC_DELAY); //2 second
//          write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED_OFF));
//          iHW_delay(LED_2_SEC_DELAY); //2 second
//          write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED1_GREEN_ENABLE | INEEDMD_LED1_RED_ENABLE));
//          iHW_delay(LED_2_SEC_DELAY); //2 second
//          write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED_OFF));
//
//          break;
//        case ERASING:
//          //COM LED orange 2 on 2 off
//          write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED_OFF));
//
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_RED_PWM, 23);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_GREEN_PWM, 22);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_BLUE_PWM, 0x00);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_RED_ILEVEL , 0x1f);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_GREEN_ILEVEL , 0x10);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_BLUE_ILEVEL , 0x00);
//
//          break;
//        case ERASING_ON:
//          //COM LED orange 2 on 2 off
//
//          write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED1_GREEN_ENABLE | INEEDMD_LED1_RED_ENABLE));
//
//          break;
//        case ERASING_OFF:
//          //COM LED orange 2 on 2 off
//          write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED_OFF));
//
//
//          break;
//        case ERASE_DONE_BLOCKING:
//          //COM led green steady
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_GREEN_PWM, 0x20);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_RED_ILEVEL , 0x00);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_GREEN_ILEVEL , 0x05);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_BLUE_ILEVEL , 0x00);
//          write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED1_GREEN_ENABLE));
//
//          iHW_delay(LED_2_SEC_DELAY); //1 second
//
//          write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED_OFF));
//
//        break;
//        case ERASE_DONE:
//          //COM led green steady
//          write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED_OFF));
//
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_GREEN_PWM, 0x20);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_RED_ILEVEL , 0x00);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_GREEN_ILEVEL , 0x05);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_BLUE_ILEVEL , 0x00);
//          write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED_OFF));
//        break;
//        case ERASE_DONE_ON:
//          //COM led green steady
//          write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED1_GREEN_ENABLE));
//        break;
//        case ERASE_DONE_OFF:
//          write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED_OFF));
//        break;
//        case DFU_MODE_BLOCKING:
//          //COM led pink
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_RED_PWM, 0x20);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_GREEN_PWM, 0x20);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_BLUE_PWM, 0x20);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_RED_ILEVEL , 0xFF);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_GREEN_ILEVEL , 0x60);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_BLUE_ILEVEL , 0xBB);
//          write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED1_RED_ENABLE | INEEDMD_LED1_BLUE_ENABLE | INEEDMD_LED1_GREEN_ENABLE));
//
//          iHW_delay(LED_1_SEC_DELAY); //1 second
//
//          write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED_OFF));
//
//        break;
//        case DFU_MODE:
//          //COM led pink
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_RED_PWM, 0x20);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_GREEN_PWM, 0x20);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_BLUE_PWM, 0x20);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_RED_ILEVEL , 0xFF);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_GREEN_ILEVEL , 0x60);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_BLUE_ILEVEL , 0xBB);
//          write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED_OFF));
//           break;
//        case DFU_MODE_ON:
//          //COM led pink
//           write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED1_RED_ENABLE | INEEDMD_LED1_BLUE_ENABLE | INEEDMD_LED1_GREEN_ENABLE));
//           break;
//        case DFU_MODE_OFF:
//          //COM led pink
//           write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED_OFF));
//           break;
//        case MV_CAL_BLOCKING:
//          //COM led purple, flashing ~1Hz
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_RED_PWM, 0x10);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_BLUE_PWM, 0x20);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_RED_ILEVEL , 0x05);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_GREEN_ILEVEL , 0x00);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_BLUE_ILEVEL , 0x05);
//          write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED1_RED_ENABLE | INEEDMD_LED1_BLUE_ENABLE));
//
//          iHW_delay(LED_0_5_SEC_DELAY); //0.5 second
//
//          write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED_OFF));
//          break;
//        case MV_CAL:
//          //COM led purple, flashing ~1Hz
//          write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED_OFF));
//
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_RED_PWM, 0x10);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_BLUE_PWM, 0x20);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_RED_ILEVEL , 0x05);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_GREEN_ILEVEL , 0x00);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_BLUE_ILEVEL , 0x05);
//          break;
//        case MV_CAL_ON:
//          //COM led purple, flashing ~1Hz
//          write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED1_RED_ENABLE | INEEDMD_LED1_BLUE_ENABLE));
//          break;
//        case MV_CAL_OFF:
//          //COM led purple, flashing ~1Hz
//          write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED_OFF));
//          break;
//
//        case TRI_WVFRM_BLOCKING:
//          //COM led orange, flashing ~1Hz
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_RED_PWM, 23);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_GREEN_PWM, 22);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_BLUE_PWM, 0x00);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_RED_ILEVEL , 0x1f);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_GREEN_ILEVEL , 0x10);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_BLUE_ILEVEL , 0x00);
//          write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED1_GREEN_ENABLE | INEEDMD_LED1_RED_ENABLE));
//
//          iHW_delay(LED_2_SEC_DELAY); //2 second
//
//          write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED_OFF));
//          break;
//        case TRI_WVFRM:
//          //COM led orange, flashing ~1Hz
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_RED_PWM, 23);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_GREEN_PWM, 22);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_BLUE_PWM, 0x00);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_RED_ILEVEL , 0x1f);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_GREEN_ILEVEL , 0x10);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_BLUE_ILEVEL , 0x00);
//          write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED_OFF));
//          break;
//        case TRI_WVFRM_ON:
//          //COM led orange, flashing ~1Hz
//          write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED1_GREEN_ENABLE | INEEDMD_LED1_RED_ENABLE));
//          break;
//        case TRI_WVFRM_OFF:
//          //COM led orange, flashing ~1Hz
//          write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED_OFF));
//          break;
//        case REBOOT_BLOCKING:
//          //ALL led's RED
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_RED_PWM, 0x20);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_RED_ILEVEL , 0x05);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_GREEN_ILEVEL , 0x00);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_BLUE_ILEVEL , 0x00);
//
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED2_RED_PWM, 0x20);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED2_RED_ILEVEL , 0x05);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED2_GREEN_ILEVEL , 0x00);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED2_BLUE_ILEVEL , 0x00);
//
//          write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED2_RED_ENABLE | INEEDMD_LED1_RED_ENABLE));
//          //write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED1_RED_ENABLE));
//
//          iHW_delay(LED_1_SEC_DELAY); //1 second
//
//          write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED_OFF));
//
//          break;
//        case REBOOT:
//          //ALL led's RED
//          write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED_OFF));
//
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_RED_PWM, 0x20);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_RED_ILEVEL , 0x05);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_GREEN_ILEVEL , 0x00);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_BLUE_ILEVEL , 0x00);
//
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED2_RED_PWM, 0x20);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED2_RED_ILEVEL , 0x05);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED2_GREEN_ILEVEL , 0x00);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED2_BLUE_ILEVEL , 0x00);
//
//          break;
//        case REBOOT_ON:
//          //ALL led's RED
//          write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED2_RED_ENABLE | INEEDMD_LED1_RED_ENABLE));
//          break;
//        case REBOOT_OFF:
//          //ALL led's RED
//          write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED_OFF));
//
//          break;
//        case HIBERNATE_GOOD_BLOCKING:
//          //TODO:
//          //Power LED, checks battery state
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_GREEN_PWM, 0x20);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_RED_ILEVEL , 0x00);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_GREEN_ILEVEL , 0x05);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_BLUE_ILEVEL , 0x00);
//
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED2_GREEN_PWM, 0x20);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED2_RED_ILEVEL , 0x00);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED2_GREEN_ILEVEL , 0x05);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED2_BLUE_ILEVEL , 0x00);
//
//          write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED2_GREEN_ENABLE | INEEDMD_LED1_GREEN_ENABLE));
//
//          iHW_delay(LED_0_5_SEC_DELAY); //1 second
//
//          write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED_OFF));
//          break;
//        case HIBERNATE_GOOD:
//          //TODO:
//          //Power LED, checks battery state
//          write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED_OFF));
//
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_GREEN_PWM, 0x20);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_RED_ILEVEL , 0x00);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_GREEN_ILEVEL , 0x05);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_BLUE_ILEVEL , 0x00);
//
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED2_GREEN_PWM, 0x20);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED2_RED_ILEVEL , 0x00);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED2_GREEN_ILEVEL , 0x05);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED2_BLUE_ILEVEL , 0x00);
//          break;
//        case HIBERNATE_GOOD_ON:
//          //TODO:
//          //Power LED, checks battery state
//          write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED2_GREEN_ENABLE | INEEDMD_LED1_GREEN_ENABLE));
//          break;
//        case HIBERNATE_GOOD_OFF:
//          //TODO:
//          //Power LED, checks battery state
//          write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED_OFF));
//          break;
//        case HIBERNATE_MEDIUM_BLOCKING:
//          //TODO:
//          //Power LED, checks battery state
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_RED_PWM, 0x20);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_GREEN_PWM, 0x20);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_RED_ILEVEL , 0x05);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_GREEN_ILEVEL , 0x05);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_BLUE_ILEVEL , 0x00);
//
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED2_RED_PWM, 0x20);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_GREEN_PWM, 0x20);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED2_RED_ILEVEL , 0x05);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED2_GREEN_ILEVEL , 0x05);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED2_BLUE_ILEVEL , 0x00);
//
//          write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED2_GREEN_ENABLE | INEEDMD_LED1_GREEN_ENABLE | INEEDMD_LED2_RED_ENABLE | INEEDMD_LED1_RED_ENABLE));
//
//          iHW_delay(LED_0_5_SEC_DELAY); //1 second
//
//          write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED_OFF));
//          break;
//        case HIBERNATE_MEDIUM:
//          //TODO:
//          //Power LED, checks battery state
//          write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED_OFF));
//
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_RED_PWM, 0x20);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_GREEN_PWM, 0x20);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_RED_ILEVEL , 0x05);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_GREEN_ILEVEL , 0x05);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_BLUE_ILEVEL , 0x00);
//
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED2_RED_PWM, 0x20);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_GREEN_PWM, 0x20);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED2_RED_ILEVEL , 0x05);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED2_GREEN_ILEVEL , 0x05);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED2_BLUE_ILEVEL , 0x00);
//
//          break;
//        case HIBERNATE_MEDIUM_ON:
//          //TODO:
//          //Power LED, checks battery state
//          write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED2_GREEN_ENABLE | INEEDMD_LED1_GREEN_ENABLE | INEEDMD_LED2_RED_ENABLE | INEEDMD_LED1_RED_ENABLE));
//          break;
//        case HIBERNATE_MEDIUM_OFF:
//          //TODO:
//          //Power LED, checks battery state
//          write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED_OFF));
//          break;
//        case HIBERNATE_LOW_BLOCKING:
//          //TODO:
//          //Power LED, checks battery state
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_RED_PWM, 0x20);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_RED_ILEVEL , 0x05);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_GREEN_ILEVEL , 0x00);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_BLUE_ILEVEL , 0x00);
//
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED2_RED_PWM, 0x20);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED2_RED_ILEVEL , 0x05);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED2_GREEN_ILEVEL , 0x00);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED2_BLUE_ILEVEL , 0x00);
//
//          write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED2_RED_ENABLE | INEEDMD_LED1_RED_ENABLE));
//          //write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED1_RED_ENABLE));
//
//          iHW_delay(LED_0_5_SEC_DELAY); //1 second
//
//          write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED_OFF));
//          break;
//        case HIBERNATE_LOW:
//          //TODO:
//          //Power LED, checks battery state
//          write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED_OFF));
//
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_RED_PWM, 0x20);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_RED_ILEVEL , 0x05);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_GREEN_ILEVEL , 0x00);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_BLUE_ILEVEL , 0x00);
//
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED2_RED_PWM, 0x20);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED2_RED_ILEVEL , 0x05);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED2_GREEN_ILEVEL , 0x00);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED2_BLUE_ILEVEL , 0x00);
//
//          iHW_delay(LED_0_5_SEC_DELAY); //1 second
//
//          break;
//        case HIBERNATE_LOW_ON:
//          //TODO:
//          //Power LED, checks battery state
//          write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED2_RED_ENABLE | INEEDMD_LED1_RED_ENABLE));
//          break;
//        case HIBERNATE_LOW_OFF:
//          //TODO:
//          //Power LED, checks battery state
//          write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED_OFF));
//          break;
//
//        case LEADS_ON_BLOCKING:
//         //Power LED red - N/A
//         //Orange
//         //power led orange
//         write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED2_RED_PWM, 23);
//         write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED2_GREEN_PWM, 22);
//         write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED2_BLUE_PWM, 0x00);
//         write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED2_RED_ILEVEL , 0x1f);
//         write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED2_GREEN_ILEVEL , 0x10);
//         write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED2_BLUE_ILEVEL , 0x00);
//
//         write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_RED_PWM, 23);
//         write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_GREEN_PWM, 22);
//         write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_BLUE_PWM, 0x00);
//         write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_RED_ILEVEL , 0x1f);
//         write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_GREEN_ILEVEL , 0x10);
//         write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_BLUE_ILEVEL , 0x00);
//
//         write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED2_GREEN_ENABLE | INEEDMD_LED2_RED_ENABLE | INEEDMD_LED1_GREEN_ENABLE | INEEDMD_LED1_RED_ENABLE));
//
//         iHW_delay(LED_1_SEC_DELAY); //1 second
//
//         write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED_OFF));
//
//         break;
//        case LEADS_ON:
//         //Power LED red - N/A
//         //Orange
//         //power led orange
//         write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED2_RED_PWM, 23);
//         write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED2_GREEN_PWM, 22);
//         write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED2_BLUE_PWM, 0x00);
//         write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED2_RED_ILEVEL , 0x1f);
//         write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED2_GREEN_ILEVEL , 0x10);
//         write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED2_BLUE_ILEVEL , 0x00);
//
//         write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_RED_PWM, 23);
//         write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_GREEN_PWM, 22);
//         write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_BLUE_PWM, 0x00);
//         write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_RED_ILEVEL , 0x1f);
//         write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_GREEN_ILEVEL , 0x10);
//         write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_BLUE_ILEVEL , 0x00);
//
//         write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED_OFF));
//         break;
//        case LEADS_ON_ON:
//         //Power LED red - N/A
//         //Orange
//         //power led orange
//         write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED2_GREEN_ENABLE | INEEDMD_LED2_RED_ENABLE | INEEDMD_LED1_GREEN_ENABLE | INEEDMD_LED1_RED_ENABLE));
//         break;
//
//        case LEADS_ON_OFF:
//         write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED_OFF));
//         break;
//
//        case MEMORY_TEST_BLOCKING:
//          //Power LED red - N/A
//          //Red
//          //ALL led's RED
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_RED_PWM, 0x20);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_RED_ILEVEL , 0x05);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_GREEN_ILEVEL , 0x00);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_BLUE_ILEVEL , 0x00);
//
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED2_RED_PWM, 0x20);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED2_RED_ILEVEL , 0x05);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED2_GREEN_ILEVEL , 0x00);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED2_BLUE_ILEVEL , 0x00);
//
//          write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED2_RED_ENABLE | INEEDMD_LED1_RED_ENABLE));
//          //write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED1_RED_ENABLE));
//
//          iHW_delay(LED_1_SEC_DELAY); //1 second
//
//          write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED_OFF));
//
//          break;
//        case MEMORY_TEST:
//          //Power LED red - N/A
//          //Red
//          //ALL led's RED
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_RED_PWM, 0x20);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_RED_ILEVEL , 0x05);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_GREEN_ILEVEL , 0x00);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_BLUE_ILEVEL , 0x00);
//
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED2_RED_PWM, 0x20);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED2_RED_ILEVEL , 0x05);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED2_GREEN_ILEVEL , 0x00);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED2_BLUE_ILEVEL , 0x00);
//
//          write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED_OFF));
//
//          break;
//        case MEMORY_TEST_ON:
//          write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED2_RED_ENABLE | INEEDMD_LED1_RED_ENABLE));
//          //write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED1_RED_ENABLE));
//          break;
//        case MEMORY_TEST_OFF:
//          //Power LED red - N/A
//          //Red
//          //ALL led's RED
//          write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED_OFF));
//          break;
//
//        case COM_BUS_TEST_BLOCKING:
//          //Power LED red - N/A
//          //Blue
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_BLUE_PWM, 0x20);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_RED_ILEVEL , 0x00);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_GREEN_ILEVEL , 0x00);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_BLUE_ILEVEL , 0x005);
//
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED2_BLUE_PWM, 0x20);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED2_RED_ILEVEL , 0x00);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED2_GREEN_ILEVEL , 0x00);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED2_BLUE_ILEVEL , 0x005);
//
//          write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED1_BLUE_ENABLE | INEEDMD_LED2_BLUE_ENABLE));
//
//
//          iHW_delay(LED_1_SEC_DELAY); //1 second
//
//          write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED_OFF));
//
//          break;
//        case COM_BUS_TEST:
//          //Power LED red - N/A
//          //Blue
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_BLUE_PWM, 0x20);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_RED_ILEVEL , 0x00);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_GREEN_ILEVEL , 0x00);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_BLUE_ILEVEL , 0x005);
//
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED2_BLUE_PWM, 0x20);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED2_RED_ILEVEL , 0x00);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED2_GREEN_ILEVEL , 0x00);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED2_BLUE_ILEVEL , 0x005);
//
//          write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED_OFF));
//
//          break;
//        case COM_BUS_TEST_ON:
//          //Power LED red - N/A
//          //Blue
//          write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED1_BLUE_ENABLE | INEEDMD_LED2_BLUE_ENABLE));
//          break;
//
//        case COM_BUS_TEST_OFF:
//          //Power LED red - N/A
//          //Blue
//          write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED_OFF));
//
//          break;
//
//        case CPU_CLOCK_TEST_BLOCKING:
//          //Power LED red - N/A
//          //Purple
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_RED_PWM, 0x10);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_BLUE_PWM, 0x20);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_RED_ILEVEL , 0x05);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_GREEN_ILEVEL , 0x00);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_BLUE_ILEVEL , 0x05);
//
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED2_RED_PWM, 0x10);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED2_BLUE_PWM, 0x20);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED2_RED_ILEVEL , 0x05);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED2_GREEN_ILEVEL , 0x00);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED2_BLUE_ILEVEL , 0x05);
//
//          write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED1_RED_ENABLE | INEEDMD_LED1_BLUE_ENABLE | INEEDMD_LED2_RED_ENABLE | INEEDMD_LED2_BLUE_ENABLE));
////
////
//          iHW_delay(LED_1_SEC_DELAY);
//
//          write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED_OFF));
//
//          break;
//        case CPU_CLOCK_TEST:
//          //Power LED red - N/A
//          //Purple
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_RED_PWM, 0x10);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_BLUE_PWM, 0x20);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_RED_ILEVEL , 0x05);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_GREEN_ILEVEL , 0x00);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_BLUE_ILEVEL , 0x05);
//
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED2_RED_PWM, 0x10);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED2_BLUE_PWM, 0x20);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED2_RED_ILEVEL , 0x05);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED2_GREEN_ILEVEL , 0x00);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED2_BLUE_ILEVEL , 0x05);
//          write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED_OFF));
//          break;
//        case CPU_CLOCK_TEST_ON:
//          //Power LED red - N/A
//          //Purple
//          write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED1_RED_ENABLE | INEEDMD_LED1_BLUE_ENABLE | INEEDMD_LED2_RED_ENABLE | INEEDMD_LED2_BLUE_ENABLE));
//          break;
//        case CPU_CLOCK_TEST_OFF:
//          //Power LED red - N/A
//          //Purple
//          write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED_OFF));
//
//          break;
//        case FLASH_TEST_BLOCKING:
//          //Power LED red - N/A
//          //Pink
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_RED_PWM, 0x20);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_GREEN_PWM, 0x20);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_BLUE_PWM, 0x20);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_RED_ILEVEL , 0xFF);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_GREEN_ILEVEL , 0x60);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_BLUE_ILEVEL , 0xBB);
//
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED2_RED_PWM, 0x20);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED2_GREEN_PWM, 0x20);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED2_BLUE_PWM, 0x20);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED2_RED_ILEVEL , 0xFF);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED2_GREEN_ILEVEL , 0x60);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED2_BLUE_ILEVEL , 0xBB);
//
//          write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED1_RED_ENABLE | INEEDMD_LED1_BLUE_ENABLE | INEEDMD_LED1_GREEN_ENABLE | INEEDMD_LED2_RED_ENABLE | INEEDMD_LED2_BLUE_ENABLE | INEEDMD_LED2_GREEN_ENABLE));
//          // iHW_delay(LED_1_SEC_DELAY);
//          write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED_OFF));
//
//          break;
//        case FLASH_TEST:
//          //Power LED red - N/A
//          //Pink
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_RED_PWM, 0x20);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_GREEN_PWM, 0x20);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_BLUE_PWM, 0x20);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_RED_ILEVEL , 0xFF);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_GREEN_ILEVEL , 0x60);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_BLUE_ILEVEL , 0xBB);
//
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED2_RED_PWM, 0x20);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED2_GREEN_PWM, 0x20);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED2_BLUE_PWM, 0x20);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED2_RED_ILEVEL , 0xFF);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED2_GREEN_ILEVEL , 0x60);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED2_BLUE_ILEVEL , 0xBB);
//          break;
//        case FLASH_TEST_ON:
//          //Power LED red - N/A
//          //Pink
//          write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED1_RED_ENABLE | INEEDMD_LED1_BLUE_ENABLE | INEEDMD_LED1_GREEN_ENABLE | INEEDMD_LED2_RED_ENABLE | INEEDMD_LED2_BLUE_ENABLE | INEEDMD_LED2_GREEN_ENABLE));
//
//          break;
//        case FLASH_TEST_OFF:
//          //Power LED red - N/A
//          //Pink
//          write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED_OFF));
//
//          break;
//        case TEST_PASS_BLOCKING:
//          //Power LED red - N/A
//          //Green
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_GREEN_PWM, 0x20);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_RED_ILEVEL , 0x00);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_GREEN_ILEVEL , 0x05);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_BLUE_ILEVEL , 0x00);
//
//          write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED1_GREEN_ENABLE | INEEDMD_LED2_GREEN_ENABLE));
//
//          iHW_delay(LED_1_SEC_DELAY); //1 second
//
//          write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED_OFF));
//
//          break;
//        case TEST_PASS:
//          //Power LED red - N/A
//          //Green
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_GREEN_PWM, 0x20);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_RED_ILEVEL , 0x00);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_GREEN_ILEVEL , 0x05);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_BLUE_ILEVEL , 0x00);
//          write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED_OFF));
//
//          break;
//        case TEST_PASS_ON:
//          //Power LED red - N/A
//          //Green
//          if(bAre_LEDs_on == false)
//          {
//            write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED1_GREEN_ENABLE | INEEDMD_LED2_GREEN_ENABLE));
//            bAre_LEDs_on = true;
//          }else{/*do nothing*/}
//
//          break;
//        case TEST_PASS_OFF:
//          //Power LED red - N/A
//          //Green
//          if(bAre_LEDs_on == true)
//          {
//            write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED_OFF));
//            bAre_LEDs_on = false;
//          }else{/*do nothing*/}
//          break;
//
//        case ACTUAL_DFU_BLOCKING:
//          //sets LED's purple, stays purple
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_RED_PWM, 0x20);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_GREEN_PWM, 0x20);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_BLUE_PWM, 0x20);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_RED_ILEVEL , 0xFF);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_GREEN_ILEVEL , 0x60);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_BLUE_ILEVEL , 0xBB);
//          write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED1_RED_ENABLE | INEEDMD_LED1_GREEN_ENABLE | INEEDMD_LED1_BLUE_ENABLE));
////
////
//          iHW_delay(LED_1_SEC_DELAY);
//
//          write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED_OFF));
////
////
//          break;
//        case ACTUAL_DFU:
//          //sets LED's purple, stays purple
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_RED_PWM, 0x20);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_GREEN_PWM, 0x20);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_BLUE_PWM, 0x20);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_RED_ILEVEL , 0xFF);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_GREEN_ILEVEL , 0x60);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_BLUE_ILEVEL , 0xBB);
//          write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED_OFF));
//          break;
//        case ACTUAL_DFU_ON:
//          //sets LED's purple, stays purple
//          write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED1_RED_ENABLE | INEEDMD_LED1_GREEN_ENABLE | INEEDMD_LED1_BLUE_ENABLE));
//          break;
//        case ACTUAL_DFU_OFF:
//          //sets LED's purple, stays purple
//          write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED_OFF));
//          break;
//
//        case POWER_UP_GOOD_BLOCKING:
//          //power led green
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED2_GREEN_PWM, 0x20);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED2_RED_ILEVEL , 0x00);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED2_GREEN_ILEVEL , 0x05);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED2_BLUE_ILEVEL , 0x00);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_GREEN_PWM, 0x20);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_RED_ILEVEL , 0x00);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_GREEN_ILEVEL , 0x05);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_BLUE_ILEVEL , 0x00);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED_OFF));
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED_OFF));
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED1_GREEN_ENABLE | INEEDMD_LED2_GREEN_ENABLE));
//
//          iHW_delay(LED_0_5_SEC_DELAY);
//
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED_OFF));
//
//          iHW_delay(LED_0_5_SEC_DELAY);
//
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED1_GREEN_ENABLE | INEEDMD_LED2_GREEN_ENABLE));
//
//          iHW_delay(LED_0_5_SEC_DELAY);
//
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED_OFF));
//
//          iHW_delay(LED_0_5_SEC_DELAY);
//
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED1_GREEN_ENABLE | INEEDMD_LED2_GREEN_ENABLE));
//
//          iHW_delay(LED_0_5_SEC_DELAY);
//
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED_OFF));
//
//          break;
//
//        case POWER_UP_GOOD:
//          //power led green on
//          write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED_OFF));
//
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED2_GREEN_PWM, 0x20);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED2_RED_ILEVEL , 0x00);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED2_GREEN_ILEVEL , 0x05);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED2_BLUE_ILEVEL , 0x00);
//
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_GREEN_PWM, 0x20);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_RED_ILEVEL , 0x00);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_GREEN_ILEVEL , 0x05);
//          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_BLUE_ILEVEL , 0x00);
//
//          break;
//        case POWER_UP_GOOD_ON:
//          //power led green on
//            write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED1_GREEN_ENABLE | INEEDMD_LED2_GREEN_ENABLE));
//          break;
//
//        case POWER_UP_GOOD_OFF:
//          //power led green off
//            write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED_OFF));
//          break;
//
//        default:
////          for(i = 0; i<8; i++ ){
////              write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED_OFF));
////          }
//          vDEBUG_INMD_LED_PAT("LED Driver SYS HALT, unknown LED pattern");
//          while(1){};
//          break;
//    }
#undef vDEBUG_INMD_LED_PAT
}


//*****************************************************************************
// name: led_test
// description: runs the gamut of LED patters
// param description: none
// return value description: none
//*****************************************************************************
#ifdef DEBUG
void led_test(void)
{
    //ints to use in switch stmt
//    ineedmd_led_pattern(POWER_ON_BATT_LOW);
//    ineedmd_led_pattern(POWER_ON_BATT_GOOD);
//    ineedmd_led_pattern(BATT_CHARGING);
//    ineedmd_led_pattern(BATT_CHARGING_LOW);
//    ineedmd_led_pattern(LEAD_LOOSE);
//    ineedmd_led_pattern(LEAD_GOOD_UPLOADING);
//    ineedmd_led_pattern(DIG_FLATLINE);
//    ineedmd_led_pattern(BT_CONNECTED);
//    ineedmd_led_pattern(BT_ATTEMPTING);
//    ineedmd_led_pattern(BT_FAILED);
//    ineedmd_led_pattern(USB_CONNECTED);
//    ineedmd_led_pattern(USB_FAILED);
//    ineedmd_led_pattern(DATA_TRANSFER);
//    ineedmd_led_pattern(TRANSFER_DONE);
//    ineedmd_led_pattern(STORAGE_WARNING);
//    ineedmd_led_pattern(ERASING);
//    ineedmd_led_pattern(ERASE_DONE);
//    ineedmd_led_pattern(DFU_MODE);
//    ineedmd_led_pattern(MV_CAL);
//    ineedmd_led_pattern(TRI_WVFRM);
//    ineedmd_led_pattern(REBOOT);
//    ineedmd_led_pattern(HIBERNATE_GOOD);
//    ineedmd_led_pattern(HIBERNATE_MEDIUM);
//    ineedmd_led_pattern(HIBERNATE_LOW);
//    ineedmd_led_pattern(LEADS_ON);
//    ineedmd_led_pattern(MEMORY_TEST);
//    ineedmd_led_pattern(COM_BUS_TEST);
//    ineedmd_led_pattern(CPU_CLOCK_TEST);
//    ineedmd_led_pattern(FLASH_TEST);
//    ineedmd_led_pattern(TEST_PASS);
//    ineedmd_led_pattern(LED_OFF);

}
#endif //DEBUG

#endif //__INEEDMD_LED_H__
