/******************************************************************************
*
* ineedmd_led.c - led driver source code
* Copyright (c) notice
*
******************************************************************************/
#ifndef __INEEDMD_LED_C__
#define __INEEDMD_LED_C__

/******************************************************************************
* includes ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include "inc/hw_memmap.h"

#include "driverlib/gpio.h"
#include "utils_inc/error_codes.h"
#include "utils_inc/osal.h"
#include "utils_inc/proj_debug.h"
#include "drivers_inc/ineedmd_led.h"
#include "board.h"
//#include "app_inc/ineedmd_power_modes.h"

#include <ti/drivers/I2C.h>
#include <ti/drivers/i2c/I2CTiva.h>

/******************************************************************************
* defines /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/
#define INEEDMD_I2C_SPEED_100KHZ        I2C_100kHz
#define INEEDMD_I2C_SPEED_400KHZ        I2C_400kHz

//#define INEEDMD_I2C_ADDRESS_LED_DRIVER  0x1b
//todo: get rid off
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
tStruct_UI_State last_UI_state;

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

//todo: check register names with header file
ERROR_CODE eIneedmd_LED_driver_enable(void)
{
  ERROR_CODE eEC = ER_FAIL;

  //todo: ineedmd pin def - PB5
  GPIO_write(EK_TM4C123GXL_LED_ENABLE, INEEDMD_LED_ENABLE_PIN_SET);

  //GPIOPinWrite(INEEDMD_GPIO_LED_EN_PORT, INEEDMD_LED_ENABLE_PIN_SET, 0xFF);
  //eEC = eIneedmd_LED_driver_setup();

  I2C_Transaction i2cTransaction;
  uint8_t i2c_send_buffer[2];

  i2cTransaction.slaveAddress = INEEDMD_I2C_ADDRESS_LED_DRIVER;
  i2cTransaction.writeBuf = i2c_send_buffer;
  i2cTransaction.writeCount = 2;
  i2cTransaction.readBuf = NULL;
  i2cTransaction.readCount = 0;

  //write CHIP_EN to ENABLE register
  i2c_send_buffer[0] = INEEDMD_LED_ENABLE_ADD;
  i2c_send_buffer[1] = INEEDMD_LED_ENABLE;
  I2C_transfer(tI2C_handle, &i2cTransaction);
  //wait 500uS
  eOSAL_delay(1, NULL);

  //set supply levels for automatic gain charge pump to work
  //write CHP_CON_7_9 | CHP_CON_1_6 (0x04) to POWER_CONFIG (0x05)
  i2c_send_buffer[0] = INEEDMD_LED_PWR_ADD;
  i2c_send_buffer[1] = INEEDMD_LED_PWR;
  I2C_transfer (tI2C_handle, &i2cTransaction);

  //enable PWM powersave, regular powersave, charge pump to auto, use internal clock
  //write PWM_POWERSAVE | POWERSAVE_EN | CP_MODE[0] | CP_MODE[1] | CLOCK_SEL to CONFIG register (0x36)
  i2c_send_buffer[0] = INEEDMD_LED_CONFIG_ADD;
  i2c_send_buffer[1] = INEEDMD_LED_CONFIG;
  I2C_transfer (tI2C_handle, &i2cTransaction);
  eEC = ER_OK;
  return eEC;
}

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
  I2C_Params * ptI2C_params;

  ptI2C_params = (I2C_Params *)malloc(sizeof(I2C_Params));

//  I2C_Params_init(&tI2C_params);
//
//  tI2C_params.transferMode  = I2C_MODE_BLOCKING;
//  tI2C_params.bitRate  = INEEDMD_I2C_SPEED_400KHZ;
//
//  tI2C_handle = I2C_open(EK_TM4C123GXL_I2C_LED, &tI2C_params);

  I2C_Params_init(ptI2C_params);

  ptI2C_params->transferMode  = I2C_MODE_BLOCKING;
  ptI2C_params->bitRate  = INEEDMD_I2C_SPEED_400KHZ;

  tI2C_handle = I2C_open(EK_TM4C123GXL_I2C_LED, ptI2C_params);

  if (tI2C_handle != NULL)
  {
    eEC = ER_OK;
  }
  else
  {
    eEC = ER_FAIL;
  }

  free(ptI2C_params);

  if(eEC == ER_OK)
  {
    eEC = eIneedmd_LED_driver_enable();
  }
  return eEC;
}

ERROR_CODE eIneedmd_LED_driver_standby(bool bPowerDown)
{
  I2C_Transaction i2cTransaction;
  uint8_t i2c_send_buffer[2];

  //todo: bring enable low? decide power states
  if(bPowerDown)
  {
    //GPIOPinWrite(INEEDMD_GPIO_LED_EN_PORT, INEEDMD_LED_ENABLE_PIN_SET, 0x00);
    GPIO_write(EK_TM4C123GXL_LED_ENABLE, INEEDMD_LED_ENABLE_PIN_CLEAR);
  }

  i2cTransaction.slaveAddress = INEEDMD_I2C_ADDRESS_LED_DRIVER;
  i2cTransaction.writeBuf = i2c_send_buffer;
  i2cTransaction.writeCount = 2;
  i2cTransaction.readBuf = NULL;
  i2cTransaction.readCount = 0;

  //write CHIP_EN to ENABLE register
  i2c_send_buffer[0] = INEEDMD_LED_ENABLE_ADD;
  i2c_send_buffer[1] = INEEDMD_LED_DISABLE;
  I2C_transfer (tI2C_handle, &i2cTransaction);
  return ER_OK;
}

ERROR_CODE eIneedmd_LED_pattern(NO_UI_ELEMENT led_pattern)
{
  #define DEBUG_ineedmd_led_pattern
  #ifdef DEBUG_ineedmd_led_pattern
    #define  vDEBUG_INMD_LED_PAT  vDEBUG
  #else
    #define vDEBUG_INMD_LED_PAT(a)
  #endif

  I2C_Transaction i2cTransaction;
  uint8_t i2c_send_buffer[2];

  i2cTransaction.slaveAddress = INEEDMD_I2C_ADDRESS_LED_DRIVER;
  i2cTransaction.writeBuf = i2c_send_buffer;
  i2cTransaction.writeCount = 2;
  i2cTransaction.readBuf = NULL;
  i2cTransaction.readCount = 0;
	
  switch(led_pattern)
  {
    case UI_ALL_OFF:
      //set all PWM registers to 0x00
      i2c_send_buffer[0] = INEEDMD_HEART_RED_PWM_ADD;
      i2c_send_buffer[1] = 0x00;
      I2C_transfer (tI2C_handle, &i2cTransaction);

      i2c_send_buffer[0] = INEEDMD_HEART_GREEN_PWM_ADD;
      i2c_send_buffer[1] = 0x00;
      I2C_transfer (tI2C_handle, &i2cTransaction);

      i2c_send_buffer[0] = INEEDMD_HEART_BLUE_PWM_ADD;
      i2c_send_buffer[1] = 0x00;
      I2C_transfer (tI2C_handle, &i2cTransaction);

      i2c_send_buffer[0] = INEEDMD_COMMS_RED_PWM_ADD;
      i2c_send_buffer[1] = 0x00;
      I2C_transfer (tI2C_handle, &i2cTransaction);

      i2c_send_buffer[0] = INEEDMD_COMMS_GREEN_PWM_ADD;
      i2c_send_buffer[1] = 0x00;
      I2C_transfer (tI2C_handle, &i2cTransaction);

      i2c_send_buffer[0] = INEEDMD_COMMS_BLUE_PWM_ADD;
      i2c_send_buffer[1] = 0x00;
      I2C_transfer (tI2C_handle, &i2cTransaction);

      i2c_send_buffer[0] = INEEDMD_POWER_RED_PWM_ADD;
      i2c_send_buffer[1] = 0x00;
      I2C_transfer (tI2C_handle, &i2cTransaction);

      i2c_send_buffer[0] = INEEDMD_POWER_GREEN_PWM_ADD;
      i2c_send_buffer[1] = 0x00;
      I2C_transfer (tI2C_handle, &i2cTransaction);

      i2c_send_buffer[0] = INEEDMD_POWER_BLUE_PWM_ADD;
      i2c_send_buffer[1] = 0x00;
      I2C_transfer (tI2C_handle, &i2cTransaction);

      last_UI_state.heart_led_on = false;
      last_UI_state.comms_led_on = false;
      last_UI_state.power_led_on = false;
      last_UI_state.sounder_on = false;
      last_UI_state.heart_led_last_state = HEART_LED_OFF;
      last_UI_state.comms_led_last_state = COMMS_LED_OFF;
      last_UI_state.power_led_last_state = POWER_LED_OFF;
      last_UI_state.sounder_last_state = ALERT_SOUND_OFF;

    //sounder off
    break;

    case HEART_LED_OFF:
      if ( last_UI_state.heart_led_on != false )
      {
        //turn off heart leds
        i2c_send_buffer[0] = INEEDMD_HEART_RED_PWM_ADD;
        i2c_send_buffer[1] = 0x00;
        I2C_transfer (tI2C_handle, &i2cTransaction);

        i2c_send_buffer[0] = INEEDMD_HEART_GREEN_PWM_ADD;
        i2c_send_buffer[1] = 0x00;
        I2C_transfer (tI2C_handle, &i2cTransaction);

        i2c_send_buffer[0] = INEEDMD_HEART_BLUE_PWM_ADD;
        i2c_send_buffer[1] = 0x00;
        I2C_transfer (tI2C_handle, &i2cTransaction);
      }
      last_UI_state.heart_led_on = false;
      last_UI_state.heart_led_last_state = HEART_LED_OFF;

      break;
    case HEART_LED_RED:
      if ( (last_UI_state.heart_led_on == false)  || (last_UI_state.heart_led_last_state !=  HEART_LED_RED) )
      {

        i2c_send_buffer[0] = INEEDMD_HEART_RED_PWM_ADD;
        i2c_send_buffer[1] = COLOR_RED_LED_RED;
        I2C_transfer (tI2C_handle, &i2cTransaction);

        i2c_send_buffer[0] = INEEDMD_HEART_GREEN_PWM_ADD;
        i2c_send_buffer[1] = COLOR_RED_LED_GREEN;
        I2C_transfer (tI2C_handle, &i2cTransaction);

        i2c_send_buffer[0] = INEEDMD_HEART_BLUE_PWM_ADD;
        i2c_send_buffer[1] = COLOR_RED_LED_BLUE;
        I2C_transfer (tI2C_handle, &i2cTransaction);
      }

      last_UI_state.heart_led_on = true;
      last_UI_state.heart_led_last_state = HEART_LED_RED;
      break;

    case HEART_LED_GREEN:
      if ( (last_UI_state.heart_led_on == false)  || (last_UI_state.heart_led_last_state !=  HEART_LED_GREEN) )
      {
        i2c_send_buffer[0] = INEEDMD_HEART_RED_PWM_ADD;
        i2c_send_buffer[1] = COLOR_GREEN_LED_RED;
        I2C_transfer (tI2C_handle, &i2cTransaction);

        i2c_send_buffer[0] = INEEDMD_HEART_GREEN_PWM_ADD;
        i2c_send_buffer[1] = COLOR_GREEN_LED_GREEN;
        I2C_transfer (tI2C_handle, &i2cTransaction);

        i2c_send_buffer[0] = INEEDMD_HEART_BLUE_PWM_ADD;
        i2c_send_buffer[1] = COLOR_GREEN_LED_BLUE;
        I2C_transfer (tI2C_handle, &i2cTransaction);
      }
      last_UI_state.heart_led_on = true;
      last_UI_state.heart_led_last_state = HEART_LED_GREEN;
      break;

    case HEART_LED_BLUE:
      if ( (last_UI_state.heart_led_on == false)  || (last_UI_state.heart_led_last_state !=  HEART_LED_BLUE) )
      {
        i2c_send_buffer[0] = INEEDMD_HEART_RED_PWM_ADD;
        i2c_send_buffer[1] = COLOR_BLUE_LED_RED;
        I2C_transfer (tI2C_handle, &i2cTransaction);

        i2c_send_buffer[0] = INEEDMD_HEART_GREEN_PWM_ADD;
        i2c_send_buffer[1] = COLOR_BLUE_LED_GREEN;
        I2C_transfer (tI2C_handle, &i2cTransaction);

        i2c_send_buffer[0] = INEEDMD_HEART_BLUE_PWM_ADD;
        i2c_send_buffer[1] = COLOR_BLUE_LED_BLUE;
        I2C_transfer (tI2C_handle, &i2cTransaction);
      }
      last_UI_state.heart_led_on = true;
      last_UI_state.heart_led_last_state = HEART_LED_BLUE;
      break;

    case HEART_LED_ORANGE:
      if ( (last_UI_state.heart_led_on == false)  || (last_UI_state.heart_led_last_state !=  HEART_LED_ORANGE) )
      {
        i2c_send_buffer[0] = INEEDMD_HEART_RED_PWM_ADD;
        i2c_send_buffer[1] = COLOR_ORANGE_LED_RED;
        I2C_transfer (tI2C_handle, &i2cTransaction);

        i2c_send_buffer[0] = INEEDMD_HEART_GREEN_PWM_ADD;
        i2c_send_buffer[1] = COLOR_ORANGE_LED_GREEN;
        I2C_transfer (tI2C_handle, &i2cTransaction);

        i2c_send_buffer[0] = INEEDMD_HEART_BLUE_PWM_ADD;
        i2c_send_buffer[1] = COLOR_ORANGE_LED_BLUE;
        I2C_transfer (tI2C_handle, &i2cTransaction);
      }
      last_UI_state.heart_led_on = true;
      last_UI_state.heart_led_last_state = HEART_LED_ORANGE;
      break;

    case HEART_LED_PURPLE:
      if ( (last_UI_state.heart_led_on == false)  || (last_UI_state.heart_led_last_state !=  HEART_LED_PURPLE) )
      {
        i2c_send_buffer[0] = INEEDMD_HEART_RED_PWM_ADD;
        i2c_send_buffer[1] = COLOR_PURPLE_LED_RED;
        I2C_transfer (tI2C_handle, &i2cTransaction);

        i2c_send_buffer[0] = INEEDMD_HEART_GREEN_PWM_ADD;
        i2c_send_buffer[1] = COLOR_PURPLE_LED_GREEN;
        I2C_transfer (tI2C_handle, &i2cTransaction);

        i2c_send_buffer[0] = INEEDMD_HEART_BLUE_PWM_ADD;
        i2c_send_buffer[1] = COLOR_PURPLE_LED_BLUE;
        I2C_transfer (tI2C_handle, &i2cTransaction);
      }
      last_UI_state.heart_led_on = true;
      last_UI_state.heart_led_last_state = HEART_LED_PURPLE;
      break;

    case HEART_LED_PINK:
      if ( (last_UI_state.heart_led_on == false)  || (last_UI_state.heart_led_last_state !=  HEART_LED_PINK) )
      {
        i2c_send_buffer[0] = INEEDMD_HEART_RED_PWM_ADD;
        i2c_send_buffer[1] = COLOR_PINK_LED_RED;
        I2C_transfer (tI2C_handle, &i2cTransaction);

        i2c_send_buffer[0] = INEEDMD_HEART_GREEN_PWM_ADD;
        i2c_send_buffer[1] = COLOR_PINK_LED_GREEN;
        I2C_transfer (tI2C_handle, &i2cTransaction);

        i2c_send_buffer[0] = INEEDMD_HEART_BLUE_PWM_ADD;
        i2c_send_buffer[1] = COLOR_PINK_LED_BLUE;
        I2C_transfer (tI2C_handle, &i2cTransaction);
      }
      last_UI_state.heart_led_on = true;
      last_UI_state.heart_led_last_state = HEART_LED_PURPLE;
      break;

    // end of the heart LED cases

    case COMMS_LED_OFF:
      if ( last_UI_state.comms_led_on != false )
      {
        i2c_send_buffer[0] = INEEDMD_COMMS_RED_PWM_ADD;
        i2c_send_buffer[1] = 0x00;
        I2C_transfer (tI2C_handle, &i2cTransaction);

        i2c_send_buffer[0] = INEEDMD_COMMS_GREEN_PWM_ADD;
        i2c_send_buffer[1] = 0x00;
        I2C_transfer (tI2C_handle, &i2cTransaction);

        i2c_send_buffer[0] = INEEDMD_COMMS_BLUE_PWM_ADD;
        i2c_send_buffer[1] = 0x00;
        I2C_transfer (tI2C_handle, &i2cTransaction);
      }
      last_UI_state.comms_led_on = false;
      last_UI_state.comms_led_last_state = COMMS_LED_OFF;

      break;
    case COMMS_LED_RED:
      if ( (last_UI_state.comms_led_on == false)  || (last_UI_state.comms_led_last_state !=  COMMS_LED_RED) )
      {
        i2c_send_buffer[0] = INEEDMD_COMMS_RED_PWM_ADD;
        i2c_send_buffer[1] = COLOR_RED_LED_RED;
        I2C_transfer (tI2C_handle, &i2cTransaction);

        i2c_send_buffer[0] = INEEDMD_COMMS_GREEN_PWM_ADD;
        i2c_send_buffer[1] = COLOR_RED_LED_GREEN;
        I2C_transfer (tI2C_handle, &i2cTransaction);

        i2c_send_buffer[0] = INEEDMD_COMMS_BLUE_PWM_ADD;
        i2c_send_buffer[1] = COLOR_RED_LED_BLUE;
        I2C_transfer (tI2C_handle, &i2cTransaction);
      }

      last_UI_state.comms_led_on = true;
      last_UI_state.comms_led_last_state = COMMS_LED_RED;
      break;

    case COMMS_LED_GREEN:
      if ( (last_UI_state.comms_led_on == false)  || (last_UI_state.comms_led_last_state !=  COMMS_LED_GREEN) )
      {
        i2c_send_buffer[0] = INEEDMD_COMMS_RED_PWM_ADD;
        i2c_send_buffer[1] = COLOR_GREEN_LED_RED;
        I2C_transfer (tI2C_handle, &i2cTransaction);

        i2c_send_buffer[0] = INEEDMD_COMMS_GREEN_PWM_ADD;
        i2c_send_buffer[1] = COLOR_GREEN_LED_GREEN;
        I2C_transfer (tI2C_handle, &i2cTransaction);

        i2c_send_buffer[0] = INEEDMD_COMMS_BLUE_PWM_ADD;
        i2c_send_buffer[1] = COLOR_GREEN_LED_BLUE;
        I2C_transfer (tI2C_handle, &i2cTransaction);
      }
      last_UI_state.comms_led_on = true;
      last_UI_state.comms_led_last_state = COMMS_LED_GREEN;
      break;

    case COMMS_LED_BLUE:
      if ( (last_UI_state.comms_led_on == false)  || (last_UI_state.comms_led_last_state !=  COMMS_LED_BLUE) )
      {
        i2c_send_buffer[0] = INEEDMD_COMMS_RED_PWM_ADD;
        i2c_send_buffer[1] = COLOR_BLUE_LED_RED;
        I2C_transfer (tI2C_handle, &i2cTransaction);

        i2c_send_buffer[0] = INEEDMD_COMMS_GREEN_PWM_ADD;
        i2c_send_buffer[1] = COLOR_BLUE_LED_GREEN;
        I2C_transfer (tI2C_handle, &i2cTransaction);

        i2c_send_buffer[0] = INEEDMD_COMMS_BLUE_PWM_ADD;
        i2c_send_buffer[1] = COLOR_BLUE_LED_BLUE;
        I2C_transfer (tI2C_handle, &i2cTransaction);
      }
      last_UI_state.comms_led_on = true;
      last_UI_state.comms_led_last_state = COMMS_LED_BLUE;
      break;

    case COMMS_LED_ORANGE:
      if ( (last_UI_state.comms_led_on == false)  || (last_UI_state.comms_led_last_state !=  COMMS_LED_ORANGE) )
      {
        i2c_send_buffer[0] = INEEDMD_COMMS_RED_PWM_ADD;
        i2c_send_buffer[1] = COLOR_ORANGE_LED_RED;
        I2C_transfer (tI2C_handle, &i2cTransaction);

        i2c_send_buffer[0] = INEEDMD_COMMS_GREEN_PWM_ADD;
        i2c_send_buffer[1] = COLOR_ORANGE_LED_GREEN;
        I2C_transfer (tI2C_handle, &i2cTransaction);

        i2c_send_buffer[0] = INEEDMD_COMMS_BLUE_PWM_ADD;
        i2c_send_buffer[1] = COLOR_ORANGE_LED_BLUE;
        I2C_transfer (tI2C_handle, &i2cTransaction);
      }
      last_UI_state.comms_led_on = true;
      last_UI_state.comms_led_last_state = COMMS_LED_ORANGE;
      break;

    case COMMS_LED_PURPLE:
      if ( (last_UI_state.comms_led_on == false)  || (last_UI_state.comms_led_last_state !=  COMMS_LED_PURPLE) )
      {
        i2c_send_buffer[0] = INEEDMD_COMMS_RED_PWM_ADD;
        i2c_send_buffer[1] = COLOR_PURPLE_LED_RED;
        I2C_transfer (tI2C_handle, &i2cTransaction);

        i2c_send_buffer[0] = INEEDMD_COMMS_GREEN_PWM_ADD;
        i2c_send_buffer[1] = COLOR_PURPLE_LED_GREEN;
        I2C_transfer (tI2C_handle, &i2cTransaction);

        i2c_send_buffer[0] = INEEDMD_COMMS_BLUE_PWM_ADD;
        i2c_send_buffer[1] = COLOR_PURPLE_LED_BLUE;
        I2C_transfer (tI2C_handle, &i2cTransaction);
      }
      last_UI_state.comms_led_on = true;
      last_UI_state.comms_led_last_state = COMMS_LED_PURPLE;
      break;

    case COMMS_LED_PINK:
      if ( (last_UI_state.comms_led_on == false)  || (last_UI_state.comms_led_last_state !=  COMMS_LED_PURPLE) )
      {
        i2c_send_buffer[0] = INEEDMD_COMMS_RED_PWM_ADD;
        i2c_send_buffer[1] = COLOR_PINK_LED_RED;
        I2C_transfer (tI2C_handle, &i2cTransaction);

        i2c_send_buffer[0] = INEEDMD_COMMS_GREEN_PWM_ADD;
        i2c_send_buffer[1] = COLOR_PINK_LED_GREEN;
        I2C_transfer (tI2C_handle, &i2cTransaction);

        i2c_send_buffer[0] = INEEDMD_COMMS_BLUE_PWM_ADD;
        i2c_send_buffer[1] = COLOR_PINK_LED_BLUE;
        I2C_transfer (tI2C_handle, &i2cTransaction);
      }
      last_UI_state.comms_led_on = true;
      last_UI_state.comms_led_last_state = COMMS_LED_PINK;
      break;

  //todo, bring these cases in when 3LED pattern is implemented
    case POWER_LED_OFF:
      if ( last_UI_state.power_led_on != false )
      {
        i2c_send_buffer[0] = INEEDMD_POWER_RED_PWM_ADD;
        i2c_send_buffer[1] = 0x00;
        I2C_transfer (tI2C_handle, &i2cTransaction);

        i2c_send_buffer[0] = INEEDMD_POWER_GREEN_PWM_ADD;
        i2c_send_buffer[1] = 0x00;
        I2C_transfer (tI2C_handle, &i2cTransaction);

        i2c_send_buffer[0] = INEEDMD_POWER_BLUE_PWM_ADD;
        i2c_send_buffer[1] = 0x00;
        I2C_transfer (tI2C_handle, &i2cTransaction);
      }
      last_UI_state.power_led_on = false;
      last_UI_state.power_led_last_state = POWER_LED_OFF;
            break;
    case POWER_LED_RED:
      if ( (last_UI_state.power_led_on == false)  || (last_UI_state.power_led_last_state !=  POWER_LED_RED) )
      {
        i2c_send_buffer[0] = INEEDMD_POWER_RED_PWM_ADD;
        i2c_send_buffer[1] = COLOR_RED_LED_RED;
        I2C_transfer (tI2C_handle, &i2cTransaction);

        i2c_send_buffer[0] = INEEDMD_POWER_GREEN_PWM_ADD;
        i2c_send_buffer[1] = COLOR_RED_LED_GREEN;
        I2C_transfer (tI2C_handle, &i2cTransaction);

        i2c_send_buffer[0] = INEEDMD_POWER_BLUE_PWM_ADD;
        i2c_send_buffer[1] = COLOR_RED_LED_BLUE;
        I2C_transfer (tI2C_handle, &i2cTransaction);
      }
      last_UI_state.power_led_on = true;
      last_UI_state.power_led_last_state = POWER_LED_RED;
      break;
    case POWER_LED_GREEN:
      if ( (last_UI_state.power_led_on == false)  || (last_UI_state.power_led_last_state !=  POWER_LED_GREEN) )
      {
        i2c_send_buffer[0] = INEEDMD_POWER_RED_PWM_ADD;
        i2c_send_buffer[1] = COLOR_GREEN_LED_RED;
        I2C_transfer (tI2C_handle, &i2cTransaction);

        i2c_send_buffer[0] = INEEDMD_POWER_GREEN_PWM_ADD;
        i2c_send_buffer[1] = COLOR_GREEN_LED_GREEN;
        I2C_transfer (tI2C_handle, &i2cTransaction);

        i2c_send_buffer[0] = INEEDMD_POWER_BLUE_PWM_ADD;
        i2c_send_buffer[1] = COLOR_GREEN_LED_BLUE;
        I2C_transfer (tI2C_handle, &i2cTransaction);
      }
      last_UI_state.power_led_on = true;
      last_UI_state.power_led_last_state = POWER_LED_GREEN;
      break;
    case POWER_LED_BLUE:
      if ( (last_UI_state.power_led_on == false)  || (last_UI_state.power_led_last_state !=  POWER_LED_BLUE) )
      {
        i2c_send_buffer[0] = INEEDMD_POWER_RED_PWM_ADD;
        i2c_send_buffer[1] = COLOR_BLUE_LED_RED;
        I2C_transfer (tI2C_handle, &i2cTransaction);

        i2c_send_buffer[0] = INEEDMD_POWER_GREEN_PWM_ADD;
        i2c_send_buffer[1] = COLOR_BLUE_LED_GREEN;
        I2C_transfer (tI2C_handle, &i2cTransaction);

        i2c_send_buffer[0] = INEEDMD_POWER_BLUE_PWM_ADD;
        i2c_send_buffer[1] = COLOR_BLUE_LED_BLUE;
        I2C_transfer (tI2C_handle, &i2cTransaction);
      }
      last_UI_state.power_led_on = true;
      last_UI_state.power_led_last_state = POWER_LED_BLUE;
      break;
    case POWER_LED_ORANGE:
      if ( (last_UI_state.power_led_on == false)  || (last_UI_state.power_led_last_state !=  POWER_LED_ORANGE) )
      {
        i2c_send_buffer[0] = INEEDMD_POWER_RED_PWM_ADD;
        i2c_send_buffer[1] = COLOR_ORANGE_LED_RED;
        I2C_transfer (tI2C_handle, &i2cTransaction);

        i2c_send_buffer[0] = INEEDMD_POWER_GREEN_PWM_ADD;
        i2c_send_buffer[1] = COLOR_ORANGE_LED_GREEN;
        I2C_transfer (tI2C_handle, &i2cTransaction);

        i2c_send_buffer[0] = INEEDMD_POWER_BLUE_PWM_ADD;
        i2c_send_buffer[1] = COLOR_ORANGE_LED_BLUE;
        I2C_transfer (tI2C_handle, &i2cTransaction);
      }
      last_UI_state.power_led_on = true;
      last_UI_state.power_led_last_state = POWER_LED_ORANGE;
      break;
    case POWER_LED_PURPLE:
      if ( (last_UI_state.power_led_on == false)  || (last_UI_state.power_led_last_state !=  POWER_LED_PURPLE) )
      {
        i2c_send_buffer[0] = INEEDMD_POWER_RED_PWM_ADD;
        i2c_send_buffer[1] = COLOR_PURPLE_LED_RED;
        I2C_transfer (tI2C_handle, &i2cTransaction);

        i2c_send_buffer[0] = INEEDMD_POWER_GREEN_PWM_ADD;
        i2c_send_buffer[1] = COLOR_PURPLE_LED_GREEN;
        I2C_transfer (tI2C_handle, &i2cTransaction);

        i2c_send_buffer[0] = INEEDMD_POWER_BLUE_PWM_ADD;
        i2c_send_buffer[1] = COLOR_PURPLE_LED_BLUE;
        I2C_transfer (tI2C_handle, &i2cTransaction);
      }
      last_UI_state.power_led_on = true;
      last_UI_state.power_led_last_state = POWER_LED_PURPLE;
      break;
    case POWER_LED_PINK:
      if ( (last_UI_state.power_led_on == false)  || (last_UI_state.power_led_last_state !=  POWER_LED_PINK) )
      {
        i2c_send_buffer[0] = INEEDMD_POWER_RED_PWM_ADD;
        i2c_send_buffer[1] = COLOR_PINK_LED_RED;
        I2C_transfer (tI2C_handle, &i2cTransaction);

        i2c_send_buffer[0] = INEEDMD_POWER_GREEN_PWM_ADD;
        i2c_send_buffer[1] = COLOR_PINK_LED_GREEN;
        I2C_transfer (tI2C_handle, &i2cTransaction);

        i2c_send_buffer[0] = INEEDMD_POWER_BLUE_PWM_ADD;
        i2c_send_buffer[1] = COLOR_PINK_LED_BLUE;
        I2C_transfer (tI2C_handle, &i2cTransaction);
      }
      last_UI_state.power_led_on = true;
      last_UI_state.power_led_last_state = POWER_LED_PINK;
      break;
    case ALERT_SOUND_ON:
      break;
    case ALERT_SOUND_OFF:
      break;
    default:
      //all leds off
      //heart led off
      i2c_send_buffer[0] = INEEDMD_HEART_RED_PWM_ADD;
      i2c_send_buffer[1] = 0x00;
      I2C_transfer (tI2C_handle, &i2cTransaction);

      i2c_send_buffer[0] = INEEDMD_HEART_GREEN_PWM_ADD;
      i2c_send_buffer[1] = 0x00;
      I2C_transfer (tI2C_handle, &i2cTransaction);

      i2c_send_buffer[0] = INEEDMD_HEART_BLUE_PWM_ADD;
      i2c_send_buffer[1] = 0x00;
      I2C_transfer (tI2C_handle, &i2cTransaction);

      //comms led off
      i2c_send_buffer[0] = INEEDMD_COMMS_RED_PWM_ADD;
      i2c_send_buffer[1] = 0x00;
      I2C_transfer (tI2C_handle, &i2cTransaction);

      i2c_send_buffer[0] = INEEDMD_COMMS_GREEN_PWM_ADD;
      i2c_send_buffer[1] = 0x00;
      I2C_transfer (tI2C_handle, &i2cTransaction);

      i2c_send_buffer[0] = INEEDMD_COMMS_BLUE_PWM_ADD;
      i2c_send_buffer[1] = 0x00;
      I2C_transfer (tI2C_handle, &i2cTransaction);

      //power led off
      i2c_send_buffer[0] = INEEDMD_POWER_RED_PWM_ADD;
      i2c_send_buffer[1] = 0x00;
      I2C_transfer (tI2C_handle, &i2cTransaction);

      i2c_send_buffer[0] = INEEDMD_POWER_GREEN_PWM_ADD;
      i2c_send_buffer[1] = 0x00;
      I2C_transfer (tI2C_handle, &i2cTransaction);

      i2c_send_buffer[0] = INEEDMD_POWER_BLUE_PWM_ADD;
      i2c_send_buffer[1] = 0x00;
      I2C_transfer (tI2C_handle, &i2cTransaction);
      //sounder off
      break;
  }
  #undef vDEBUG_INMD_LED_PAT
  return ER_OK;
}

/*
//ERROR_CODE eIneedmd_LED_pattern(LED_MODE led_pattern)
ERROR_CODE eIneedmd_LED_pattern(NO_UI_ELEMENT led_pattern)
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
  bool bIs_data = false;

  //Read which LEDS are on
  i2cTransaction.slaveAddress = INEEDMD_I2C_ADDRESS_LED_DRIVER;
  i2cTransaction.writeBuf = &write_byte;
  i2cTransaction.writeCount = 1;
  i2cTransaction.readBuf = &read_byte;
  //i2cTransaction.readBuf = NULL;
  i2cTransaction.readCount = 1;

  write_byte =(INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL);
  while(bIs_data == false)
  {
    bIs_data = I2C_transfer (tI2C_handle, &i2cTransaction);
  }

  i2cTransaction.writeBuf = i2c_send_buffer;
  i2cTransaction.writeCount = 2;
  i2cTransaction.readBuf = NULL;
  i2cTransaction.readCount = 0;

  switch (led_pattern)
  {
    case UI_ALL_OFF:
      //all leds_off
//      if ( last_UI_state.heart_led_on | last_UI_state.comms_led_on | last_UI_state.power_led_on | last_UI_state.sounder_on )
//      {
        i2c_send_buffer[0] = (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL);
        i2c_send_buffer[1] = INEEDMD_LED_OFF;
        I2C_transfer (tI2C_handle, &i2cTransaction);
        //no sounder shutdown yet
//      }
      last_UI_state.heart_led_on = false;
      last_UI_state.comms_led_on = false;
      last_UI_state.power_led_on = false;
      last_UI_state.sounder_on = false;
      last_UI_state.heart_led_last_state = HEART_LED_OFF;
      last_UI_state.comms_led_last_state = COMMS_LED_OFF;
      last_UI_state.power_led_last_state = POWER_LED_OFF;
      last_UI_state.sounder_last_state = ALERT_SOUND_OFF;

    //spinder off
    break;

    case HEART_LED_OFF:
      if ( last_UI_state.heart_led_on != false )
      {
        i2c_send_buffer[0] = (INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_HEART_RED_PWM);
        i2c_send_buffer[1] = INEEDMD_LED_OFF;
        I2C_transfer (tI2C_handle, &i2cTransaction);

        i2c_send_buffer[0] = (INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_HEART_GREEN_PWM);
        i2c_send_buffer[1] = INEEDMD_LED_OFF;
        I2C_transfer (tI2C_handle, &i2cTransaction);

        i2c_send_buffer[0] = (INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_HEART_BLUE_PWM);
        i2c_send_buffer[1] = INEEDMD_LED_OFF;
        I2C_transfer (tI2C_handle, &i2cTransaction);

        i2c_send_buffer[0] = (INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_HEART_RED_ILEVEL);
        i2c_send_buffer[1] = INEEDMD_LED_OFF;
        I2C_transfer (tI2C_handle, &i2cTransaction);

        i2c_send_buffer[0] = (INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_HEART_GREEN_ILEVEL);
        i2c_send_buffer[1] = INEEDMD_LED_OFF;
        I2C_transfer (tI2C_handle, &i2cTransaction);

        i2c_send_buffer[0] = (INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_HEART_BLUE_ILEVEL);
        i2c_send_buffer[1] = INEEDMD_LED_OFF;
        I2C_transfer (tI2C_handle, &i2cTransaction);

        //switch off all the LEDS not needed
        i2c_send_buffer[0] = (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL);
        i2c_send_buffer[1] = (read_byte & ~(INEEDMD_HEART_RED_ENABLE | INEEDMD_HEART_GREEN_ENABLE|INEEDMD_HEART_BLUE_ENABLE ));
        I2C_transfer (tI2C_handle, &i2cTransaction);
      }
      last_UI_state.heart_led_on = false;
      last_UI_state.heart_led_last_state = HEART_LED_OFF;

      break;
    case HEART_LED_RED:
      if ( (last_UI_state.heart_led_on == false)  || (last_UI_state.heart_led_last_state !=  HEART_LED_RED) )
      {
        i2c_send_buffer[0] = (INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_HEART_RED_PWM);
        i2c_send_buffer[1] = 0x16;
        I2C_transfer (tI2C_handle, &i2cTransaction);

        i2c_send_buffer[0] = (INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_HEART_GREEN_PWM);
        i2c_send_buffer[1] = INEEDMD_LED_OFF;
        I2C_transfer (tI2C_handle, &i2cTransaction);

        i2c_send_buffer[0] = (INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_HEART_BLUE_PWM);
        i2c_send_buffer[1] = INEEDMD_LED_OFF;
        I2C_transfer (tI2C_handle, &i2cTransaction);

        i2c_send_buffer[0] = (INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_HEART_RED_ILEVEL);
        i2c_send_buffer[1] = 0x1f;
        I2C_transfer (tI2C_handle, &i2cTransaction);

        i2c_send_buffer[0] = (INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_HEART_GREEN_ILEVEL);
        i2c_send_buffer[1] = INEEDMD_LED_OFF;
        I2C_transfer (tI2C_handle, &i2cTransaction);

        i2c_send_buffer[0] = (INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_HEART_BLUE_ILEVEL);
        i2c_send_buffer[1] = INEEDMD_LED_OFF;
        I2C_transfer (tI2C_handle, &i2cTransaction);

        //switch off all the LEDS not needed
        i2c_send_buffer[0] = (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL);
        i2c_send_buffer[1] = (read_byte & ~( INEEDMD_HEART_GREEN_ENABLE | INEEDMD_HEART_BLUE_ENABLE )) | INEEDMD_HEART_RED_ENABLE;
        I2C_transfer (tI2C_handle, &i2cTransaction);
      }

      last_UI_state.heart_led_on = true;
      last_UI_state.heart_led_last_state = HEART_LED_RED;
      break;

    case HEART_LED_GREEN:
      if ( (last_UI_state.heart_led_on == false)  || (last_UI_state.heart_led_last_state !=  HEART_LED_GREEN) )
      {
        i2c_send_buffer[0] = (INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_HEART_RED_PWM);
        i2c_send_buffer[1] = INEEDMD_LED_OFF;
        I2C_transfer (tI2C_handle, &i2cTransaction);

        i2c_send_buffer[0] = (INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_HEART_GREEN_PWM);
        i2c_send_buffer[1] = 0x20;;
        I2C_transfer (tI2C_handle, &i2cTransaction);

        i2c_send_buffer[0] = (INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_HEART_BLUE_PWM);
        i2c_send_buffer[1] = INEEDMD_LED_OFF;
        I2C_transfer (tI2C_handle, &i2cTransaction);

        i2c_send_buffer[0] = (INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_HEART_RED_ILEVEL);
        i2c_send_buffer[1] = INEEDMD_LED_OFF;
        I2C_transfer (tI2C_handle, &i2cTransaction);

        i2c_send_buffer[0] = (INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_HEART_GREEN_ILEVEL);
        i2c_send_buffer[1] = 0x1f;
        I2C_transfer (tI2C_handle, &i2cTransaction);

        i2c_send_buffer[0] = (INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_HEART_BLUE_ILEVEL);
        i2c_send_buffer[1] = INEEDMD_LED_OFF;
        I2C_transfer (tI2C_handle, &i2cTransaction);

        //switch off all the LEDS not needed
        i2c_send_buffer[0] = (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL);
        i2c_send_buffer[1] = (read_byte & ~(INEEDMD_HEART_RED_ENABLE | INEEDMD_HEART_BLUE_ENABLE )) | INEEDMD_HEART_GREEN_ENABLE;
        I2C_transfer (tI2C_handle, &i2cTransaction);
      }
      last_UI_state.heart_led_on = true;
      last_UI_state.heart_led_last_state = HEART_LED_GREEN;
      break;

    case HEART_LED_BLUE:
      if ( (last_UI_state.heart_led_on == false)  || (last_UI_state.heart_led_last_state !=  HEART_LED_BLUE) )
      {
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
      }
      last_UI_state.heart_led_on = true;
      last_UI_state.heart_led_last_state = HEART_LED_BLUE;
      break;

    case HEART_LED_ORANGE:
      if ( (last_UI_state.heart_led_on == false)  || (last_UI_state.heart_led_last_state !=  HEART_LED_ORANGE) )
      {
        i2c_send_buffer[0] = (INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_HEART_RED_PWM);
        i2c_send_buffer[1] = 0x20;
        I2C_transfer (tI2C_handle, &i2cTransaction);

        i2c_send_buffer[0] = (INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_HEART_GREEN_PWM);
        i2c_send_buffer[1] = 0x20;
        I2C_transfer (tI2C_handle, &i2cTransaction);

        i2c_send_buffer[0] = (INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_HEART_BLUE_PWM);
        i2c_send_buffer[1] = INEEDMD_LED_OFF;
        I2C_transfer (tI2C_handle, &i2cTransaction);

        i2c_send_buffer[0] = (INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_HEART_RED_ILEVEL);
        i2c_send_buffer[1] = 0x1F;
        I2C_transfer (tI2C_handle, &i2cTransaction);

        i2c_send_buffer[0] = (INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_HEART_GREEN_ILEVEL);
        i2c_send_buffer[1] = 0x10;
        I2C_transfer (tI2C_handle, &i2cTransaction);

        i2c_send_buffer[0] = (INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_HEART_BLUE_ILEVEL);
        i2c_send_buffer[1] = INEEDMD_LED_OFF;
        I2C_transfer (tI2C_handle, &i2cTransaction);

        //switch off all the LEDS not needed
        i2c_send_buffer[0] = (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL);
        i2c_send_buffer[1] = (read_byte & ~(INEEDMD_HEART_BLUE_ENABLE)) | INEEDMD_HEART_RED_ENABLE |INEEDMD_HEART_GREEN_ENABLE ;
        I2C_transfer (tI2C_handle, &i2cTransaction);
      }
      last_UI_state.heart_led_on = true;
      last_UI_state.heart_led_last_state = HEART_LED_ORANGE;
      break;

    case HEART_LED_PURPLE:
      if ( (last_UI_state.heart_led_on == false)  || (last_UI_state.heart_led_last_state !=  HEART_LED_PURPLE) )
      {
        i2c_send_buffer[0] = (INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_HEART_RED_PWM);
        i2c_send_buffer[1] = 0x28;
        I2C_transfer (tI2C_handle, &i2cTransaction);

        i2c_send_buffer[0] = (INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_HEART_GREEN_PWM);
        i2c_send_buffer[1] = INEEDMD_LED_OFF;
        I2C_transfer (tI2C_handle, &i2cTransaction);

        i2c_send_buffer[0] = (INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_HEART_BLUE_PWM);
        i2c_send_buffer[1] = 0x0A;
        I2C_transfer (tI2C_handle, &i2cTransaction);

        i2c_send_buffer[0] = (INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_HEART_RED_ILEVEL);
        i2c_send_buffer[1] = 0x1f;
        I2C_transfer (tI2C_handle, &i2cTransaction);

        i2c_send_buffer[0] = (INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_HEART_GREEN_ILEVEL);
        i2c_send_buffer[1] = INEEDMD_LED_OFF;
        I2C_transfer (tI2C_handle, &i2cTransaction);

        i2c_send_buffer[0] = (INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_HEART_BLUE_ILEVEL);
        i2c_send_buffer[1] = 0x1f;
        I2C_transfer (tI2C_handle, &i2cTransaction);

        //switch off all the LEDS not needed
        i2c_send_buffer[0] = (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL);
        i2c_send_buffer[1] = (read_byte & ~(INEEDMD_HEART_GREEN_ENABLE)) | INEEDMD_HEART_RED_ENABLE | INEEDMD_HEART_BLUE_ENABLE ;
        I2C_transfer (tI2C_handle, &i2cTransaction);
      }
      last_UI_state.heart_led_on = true;
      last_UI_state.heart_led_last_state = HEART_LED_PURPLE;
      break;

    case HEART_LED_PINK:
      if ( (last_UI_state.heart_led_on == false)  || (last_UI_state.heart_led_last_state !=  HEART_LED_PURPLE) )
      {
        i2c_send_buffer[0] = (INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_HEART_RED_PWM);
        i2c_send_buffer[1] = 0x20;
        I2C_transfer (tI2C_handle, &i2cTransaction);

        i2c_send_buffer[0] = (INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_HEART_GREEN_PWM);
        i2c_send_buffer[1] = 0x20;
        I2C_transfer (tI2C_handle, &i2cTransaction);

        i2c_send_buffer[0] = (INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_HEART_BLUE_PWM);
        i2c_send_buffer[1] = 0x20;
        I2C_transfer (tI2C_handle, &i2cTransaction);

        i2c_send_buffer[0] = (INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_HEART_RED_ILEVEL);
        i2c_send_buffer[1] = 0x14;
        I2C_transfer (tI2C_handle, &i2cTransaction);

        i2c_send_buffer[0] = (INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_HEART_GREEN_ILEVEL);
        i2c_send_buffer[1] = 0x08;
        I2C_transfer (tI2C_handle, &i2cTransaction);

        i2c_send_buffer[0] = (INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_HEART_BLUE_ILEVEL);
        i2c_send_buffer[1] = 0x14;
        I2C_transfer (tI2C_handle, &i2cTransaction);

        //switch off all the LEDS not needed
        i2c_send_buffer[0] = (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL);
        i2c_send_buffer[1] = (read_byte | INEEDMD_HEART_RED_ENABLE | INEEDMD_HEART_GREEN_ENABLE | INEEDMD_HEART_BLUE_ENABLE );
        I2C_transfer (tI2C_handle, &i2cTransaction);
      }
      last_UI_state.heart_led_on = true;
      last_UI_state.heart_led_last_state = HEART_LED_PURPLE;
      break;

    // end of the heart LED cases

    case COMMS_LED_OFF:
      if ( last_UI_state.comms_led_on != false )
      {
        i2c_send_buffer[0] = (INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_COMMS_RED_PWM);
        i2c_send_buffer[1] = INEEDMD_LED_OFF;
        I2C_transfer (tI2C_handle, &i2cTransaction);

        i2c_send_buffer[0] = (INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_COMMS_GREEN_PWM);
        i2c_send_buffer[1] = INEEDMD_LED_OFF;
        I2C_transfer (tI2C_handle, &i2cTransaction);

        i2c_send_buffer[0] = (INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_COMMS_BLUE_PWM);
        i2c_send_buffer[1] = INEEDMD_LED_OFF;
        I2C_transfer (tI2C_handle, &i2cTransaction);

        i2c_send_buffer[0] = (INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_COMMS_RED_ILEVEL);
        i2c_send_buffer[1] = INEEDMD_LED_OFF;
        I2C_transfer (tI2C_handle, &i2cTransaction);

        i2c_send_buffer[0] = (INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_COMMS_GREEN_ILEVEL);
        i2c_send_buffer[1] = INEEDMD_LED_OFF;
        I2C_transfer (tI2C_handle, &i2cTransaction);

        i2c_send_buffer[0] = (INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_COMMS_BLUE_ILEVEL);
        i2c_send_buffer[1] = INEEDMD_LED_OFF;
        I2C_transfer (tI2C_handle, &i2cTransaction);

        //switch off all the LEDS not needed
        i2c_send_buffer[0] = (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL);
        i2c_send_buffer[1] = (read_byte & ~(INEEDMD_COMMS_RED_ENABLE | INEEDMD_COMMS_GREEN_ENABLE|INEEDMD_COMMS_BLUE_ENABLE ));
        I2C_transfer (tI2C_handle, &i2cTransaction);
      }
      last_UI_state.comms_led_on = false;
      last_UI_state.comms_led_last_state = COMMS_LED_OFF;

      break;
    case COMMS_LED_RED:
      if ( (last_UI_state.comms_led_on == false)  || (last_UI_state.comms_led_last_state !=  COMMS_LED_RED) )
      {
        i2c_send_buffer[0] = (INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_COMMS_RED_PWM);
        i2c_send_buffer[1] = 0x16;
        I2C_transfer (tI2C_handle, &i2cTransaction);

        i2c_send_buffer[0] = (INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_COMMS_GREEN_PWM);
        i2c_send_buffer[1] = INEEDMD_LED_OFF;
        I2C_transfer (tI2C_handle, &i2cTransaction);

        i2c_send_buffer[0] = (INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_COMMS_BLUE_PWM);
        i2c_send_buffer[1] = INEEDMD_LED_OFF;
        I2C_transfer (tI2C_handle, &i2cTransaction);

        i2c_send_buffer[0] = (INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_COMMS_RED_ILEVEL);
        i2c_send_buffer[1] = 0x1f;
        I2C_transfer (tI2C_handle, &i2cTransaction);

        i2c_send_buffer[0] = (INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_COMMS_GREEN_ILEVEL);
        i2c_send_buffer[1] = INEEDMD_LED_OFF;
        I2C_transfer (tI2C_handle, &i2cTransaction);

        i2c_send_buffer[0] = (INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_COMMS_BLUE_ILEVEL);
        i2c_send_buffer[1] = INEEDMD_LED_OFF;
        I2C_transfer (tI2C_handle, &i2cTransaction);

        //switch off all the LEDS not needed
        i2c_send_buffer[0] = (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL);
        i2c_send_buffer[1] = (read_byte & ~( INEEDMD_COMMS_GREEN_ENABLE | INEEDMD_COMMS_BLUE_ENABLE )) | INEEDMD_COMMS_RED_ENABLE;
        I2C_transfer (tI2C_handle, &i2cTransaction);
      }

      last_UI_state.comms_led_on = true;
      last_UI_state.comms_led_last_state = COMMS_LED_RED;
      break;

    case COMMS_LED_GREEN:
      if ( (last_UI_state.comms_led_on == false)  || (last_UI_state.comms_led_last_state !=  COMMS_LED_GREEN) )
      {
        i2c_send_buffer[0] = (INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_COMMS_RED_PWM);
        i2c_send_buffer[1] = INEEDMD_LED_OFF;
        I2C_transfer (tI2C_handle, &i2cTransaction);

        i2c_send_buffer[0] = (INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_COMMS_GREEN_PWM);
        i2c_send_buffer[1] = 0x20;;
        I2C_transfer (tI2C_handle, &i2cTransaction);

        i2c_send_buffer[0] = (INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_COMMS_BLUE_PWM);
        i2c_send_buffer[1] = INEEDMD_LED_OFF;
        I2C_transfer (tI2C_handle, &i2cTransaction);

        i2c_send_buffer[0] = (INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_COMMS_RED_ILEVEL);
        i2c_send_buffer[1] = INEEDMD_LED_OFF;
        I2C_transfer (tI2C_handle, &i2cTransaction);

        i2c_send_buffer[0] = (INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_COMMS_GREEN_ILEVEL);
        i2c_send_buffer[1] = 0x1f;
        I2C_transfer (tI2C_handle, &i2cTransaction);

        i2c_send_buffer[0] = (INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_COMMS_BLUE_ILEVEL);
        i2c_send_buffer[1] = INEEDMD_LED_OFF;
        I2C_transfer (tI2C_handle, &i2cTransaction);

        //switch off all the LEDS not needed
        i2c_send_buffer[0] = (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL);
        i2c_send_buffer[1] = (read_byte & ~(INEEDMD_COMMS_RED_ENABLE | INEEDMD_COMMS_BLUE_ENABLE )) | INEEDMD_COMMS_GREEN_ENABLE;
        I2C_transfer (tI2C_handle, &i2cTransaction);
      }
      last_UI_state.comms_led_on = true;
      last_UI_state.comms_led_last_state = COMMS_LED_GREEN;
      break;

    case COMMS_LED_BLUE:
      if ( (last_UI_state.comms_led_on == false)  || (last_UI_state.comms_led_last_state !=  COMMS_LED_BLUE) )
      {
        i2c_send_buffer[0] = (INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_COMMS_RED_PWM);
        i2c_send_buffer[1] = INEEDMD_LED_OFF;
        I2C_transfer (tI2C_handle, &i2cTransaction);

        i2c_send_buffer[0] = (INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_COMMS_GREEN_PWM);
        i2c_send_buffer[1] = INEEDMD_LED_OFF;
        I2C_transfer (tI2C_handle, &i2cTransaction);

        i2c_send_buffer[0] = (INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_COMMS_BLUE_PWM);
        i2c_send_buffer[1] = 20;
        I2C_transfer (tI2C_handle, &i2cTransaction);

        i2c_send_buffer[0] = (INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_COMMS_RED_ILEVEL);
        i2c_send_buffer[1] = INEEDMD_LED_OFF;
        I2C_transfer (tI2C_handle, &i2cTransaction);

        i2c_send_buffer[0] = (INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_COMMS_GREEN_ILEVEL);
        i2c_send_buffer[1] = INEEDMD_LED_OFF;
        I2C_transfer (tI2C_handle, &i2cTransaction);

        i2c_send_buffer[0] = (INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_COMMS_BLUE_ILEVEL);
        i2c_send_buffer[1] = 0x1F;
        I2C_transfer (tI2C_handle, &i2cTransaction);

        //switch off all the LEDS not needed
        i2c_send_buffer[0] = (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL);
        i2c_send_buffer[1] = (read_byte & ~(INEEDMD_COMMS_RED_ENABLE | INEEDMD_COMMS_GREEN_ENABLE )) | INEEDMD_COMMS_BLUE_ENABLE;
        I2C_transfer (tI2C_handle, &i2cTransaction);
      }
      last_UI_state.comms_led_on = true;
      last_UI_state.comms_led_last_state = COMMS_LED_BLUE;
      break;

    case COMMS_LED_ORANGE:
      if ( (last_UI_state.comms_led_on == false)  || (last_UI_state.comms_led_last_state !=  COMMS_LED_ORANGE) )
      {
        i2c_send_buffer[0] = (INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_COMMS_RED_PWM);
        i2c_send_buffer[1] = 0x20;
        I2C_transfer (tI2C_handle, &i2cTransaction);

        i2c_send_buffer[0] = (INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_COMMS_GREEN_PWM);
        i2c_send_buffer[1] = 0x20;
        I2C_transfer (tI2C_handle, &i2cTransaction);

        i2c_send_buffer[0] = (INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_COMMS_BLUE_PWM);
        i2c_send_buffer[1] = INEEDMD_LED_OFF;
        I2C_transfer (tI2C_handle, &i2cTransaction);

        i2c_send_buffer[0] = (INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_COMMS_RED_ILEVEL);
        i2c_send_buffer[1] = 0x1F;
        I2C_transfer (tI2C_handle, &i2cTransaction);

        i2c_send_buffer[0] = (INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_COMMS_GREEN_ILEVEL);
        i2c_send_buffer[1] = 0x10;
        I2C_transfer (tI2C_handle, &i2cTransaction);

        i2c_send_buffer[0] = (INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_COMMS_BLUE_ILEVEL);
        i2c_send_buffer[1] = INEEDMD_LED_OFF;
        I2C_transfer (tI2C_handle, &i2cTransaction);

        //switch off all the LEDS not needed
        i2c_send_buffer[0] = (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL);
        i2c_send_buffer[1] = (read_byte & ~(INEEDMD_COMMS_BLUE_ENABLE)) | INEEDMD_COMMS_RED_ENABLE |INEEDMD_COMMS_GREEN_ENABLE ;
        I2C_transfer (tI2C_handle, &i2cTransaction);
      }
      last_UI_state.comms_led_on = true;
      last_UI_state.comms_led_last_state = COMMS_LED_ORANGE;
      break;

    case COMMS_LED_PURPLE:
      if ( (last_UI_state.comms_led_on == false)  || (last_UI_state.comms_led_last_state !=  COMMS_LED_PURPLE) )
      {
        i2c_send_buffer[0] = (INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_COMMS_RED_PWM);
        i2c_send_buffer[1] = 0x28;
        I2C_transfer (tI2C_handle, &i2cTransaction);

        i2c_send_buffer[0] = (INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_COMMS_GREEN_PWM);
        i2c_send_buffer[1] = INEEDMD_LED_OFF;
        I2C_transfer (tI2C_handle, &i2cTransaction);

        i2c_send_buffer[0] = (INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_COMMS_BLUE_PWM);
        i2c_send_buffer[1] = 0x0A;
        I2C_transfer (tI2C_handle, &i2cTransaction);

        i2c_send_buffer[0] = (INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_COMMS_RED_ILEVEL);
        i2c_send_buffer[1] = 0x1f;
        I2C_transfer (tI2C_handle, &i2cTransaction);

        i2c_send_buffer[0] = (INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_COMMS_GREEN_ILEVEL);
        i2c_send_buffer[1] = INEEDMD_LED_OFF;
        I2C_transfer (tI2C_handle, &i2cTransaction);

        i2c_send_buffer[0] = (INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_COMMS_BLUE_ILEVEL);
        i2c_send_buffer[1] = 0x1f;
        I2C_transfer (tI2C_handle, &i2cTransaction);

        //switch off all the LEDS not needed
        i2c_send_buffer[0] = (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL);
        i2c_send_buffer[1] = (read_byte & ~(INEEDMD_COMMS_GREEN_ENABLE)) | INEEDMD_COMMS_RED_ENABLE | INEEDMD_COMMS_BLUE_ENABLE ;
        I2C_transfer (tI2C_handle, &i2cTransaction);
      }
      last_UI_state.comms_led_on = true;
      last_UI_state.comms_led_last_state = COMMS_LED_PURPLE;
      break;

    case COMMS_LED_PINK:
      if ( (last_UI_state.comms_led_on == false)  || (last_UI_state.comms_led_last_state !=  COMMS_LED_PURPLE) )
      {
        i2c_send_buffer[0] = (INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_COMMS_RED_PWM);
        i2c_send_buffer[1] = 0x20;
        I2C_transfer (tI2C_handle, &i2cTransaction);

        i2c_send_buffer[0] = (INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_COMMS_GREEN_PWM);
        i2c_send_buffer[1] = 0x20;
        I2C_transfer (tI2C_handle, &i2cTransaction);

        i2c_send_buffer[0] = (INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_COMMS_BLUE_PWM);
        i2c_send_buffer[1] = 0x20;
        I2C_transfer (tI2C_handle, &i2cTransaction);

        i2c_send_buffer[0] = (INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_COMMS_RED_ILEVEL);
        i2c_send_buffer[1] = 0x14;
        I2C_transfer (tI2C_handle, &i2cTransaction);

        i2c_send_buffer[0] = (INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_COMMS_GREEN_ILEVEL);
        i2c_send_buffer[1] = 0x08;
        I2C_transfer (tI2C_handle, &i2cTransaction);

        i2c_send_buffer[0] = (INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_COMMS_BLUE_ILEVEL);
        i2c_send_buffer[1] = 0x14;
        I2C_transfer (tI2C_handle, &i2cTransaction);

        //switch off all the LEDS not needed
        i2c_send_buffer[0] = (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL);
        i2c_send_buffer[1] = (read_byte | INEEDMD_COMMS_RED_ENABLE | INEEDMD_COMMS_GREEN_ENABLE | INEEDMD_COMMS_BLUE_ENABLE );
        I2C_transfer (tI2C_handle, &i2cTransaction);
      }
      last_UI_state.comms_led_on = true;
      last_UI_state.comms_led_last_state = COMMS_LED_PURPLE;
      break;

//todo, bring these cases in when 3LED pattern is implemented
//    case POWER_LED_OFF:
//      break;
//    case POWER_LED_RED:
//      break;
//    case POWER_LED_GREEN:
//      break;
//    case POWER_LED_BLUE:
//      break;
//    case POWER_LED_ORANGE:
//      break;
//    case POWER_LED_PURPLE:
//      break;
//    case POWER_LED_PINK:
//      break;
    case ALERT_SOUND_ON:
      break;
    case ALERT_SOUND_OFF:
      break;
    default:
      //all leds off
      //sounder off
      break;
  }
#undef vDEBUG_INMD_LED_PAT
  return ER_OK;
}
*/

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

#endif //__INEEDMD_LED_C__
