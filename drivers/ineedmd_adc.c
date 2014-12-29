/******************************************************************************
*
* ineedmd_adc.c - the adc driver code for EKG monitoring
* Copyright (c) notice
*
******************************************************************************/
#ifndef __INEEDMD_ADC_C__
#define __INEEDMD_A__
/******************************************************************************
* includes
******************************************************************************/
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include <inc/hw_ints.h>
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "inc/hw_gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom_map.h"
#include "driverlib/ssi.h"
#include "driverlib/gpio.h"
#include "driverlib/adc.h"

#include "utils_inc/error_codes.h"
#include "utils_inc/proj_debug.h"
#include "drivers_inc/ineedmd_adc.h"
#include "board.h"
#include "app_inc/ineedmd_power_modes.h"
#include "utils_inc/osal.h"

/******************************************************************************
* defines /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/

#define ADC_SYS_DATA_MEASUREMENTS 16

#define ADS_SAMPLE_RATE_CLEAR  0x07
#define ADS_CB_ARRAY_LIMIT     3

/******************************************************************************
* variables ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/

bool bIs_adc_ready                 = false;
bool bIs_adc_powered_on            = false;
bool bIs_adc_ads_sampling          = false;
bool bIs_adc_ads_setup             = false;
bool bIs_adc_temp_monitor_setup    = false;
bool bIs_adc_batt_voltage_monitor_setup = false;

uint8_t uiADS_Data[19];

void (*vADC_Continuous_ADS_Conversion_cb[ADS_CB_ARRAY_LIMIT]) (uint8_t * uiData);

/******************************************************************************
* external variables //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/

/******************************************************************************
* enums ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/

/******************************************************************************
* structures //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/

typedef struct tVoltage_Measurement_Data
{
  bool bIs_Data_Initalized;
  uint8_t array_position;
  uint32_t measuremet[ADC_SYS_DATA_MEASUREMENTS];
  uint32_t average_measurement;
  uint8_t uiConverted_measurement;
}tVoltage_Measurement_Data;

typedef struct tTemperature_Measurement_Data
{
  bool bIs_Data_Initalized;
  uint8_t array_position;
  uint32_t measuremet[ADC_SYS_DATA_MEASUREMENTS];
  uint32_t average_measurement;
  int8_t uiConverted_measurement;
}tTemperature_Measurement_Data;

static struct tTemperature_Measurement_Data tSystem_Temperature;
static struct tVoltage_Measurement_Data tSystem_Voltage;

extern Clock_Handle tADC_read_temp_clock;
extern Clock_Handle tADC_read_volt_clock;

/******************************************************************************
* external functions //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/

/******************************************************************************
* private function declarations ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/

/******************************************************************************
* private functions ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/

ERROR_CODE eADC_Continuous_Conversion_Mode       (bool bContinuous_mode);
ERROR_CODE eADC_Register_ADS_continuous_callback (void (*vADC_callback) (uint8_t * uiData));
ERROR_CODE eADC_Notify_ADS_continuous_callback   (uint8_t * uiData);
void       vADS_drdy_int_callback                (void);
ERROR_CODE eADC_Power_On_ADS_Sequence            (void);
ERROR_CODE eADC_Enable_Lead_Detect               (void);
ERROR_CODE eADC_Voltage_Reference                (ADC_VREF_ADS_SOURCE eVref_Source);
ERROR_CODE eADC_Set_ADS_Sample_Rate              (ADC_SAMPLE_ADS_RATE eRate);
ERROR_CODE eADC_Read_ADS_Single                  (uint8_t * pData);
int8_t     iADC_get_unit_temperature             (void); //returns the current system temperature
void       vADC_temperature_read_int_service     (void);
void       vADC_batt_volt_read_int_service       (void);
ERROR_CODE eADC_setup                            (void); //sets up the A to D driver


/******************************************************************************
* name: eBSP_ADC_Continuous_Conv
* description: sets the continuous conversions mode. In continuous mode data will stream
*              out of the A to D. In non-continuous mode data will only be outputted
*              from the A to D when requested via START.
*
* param description: bool - true: If start pin is high or START command is received
*                                 conversions are automatically put into outbound Tx(MISO) FIFO when ready (DRDY goes low)
*                                 use INEEDMD_ADC_Receive_Conversion so shift data through MISO
*                         - false: if START pin is high or a START command received (if START pin is low)
*                                  conversions are put into outbound Tx(MISO) FIFO
*                                  when a RDATA command is received
*                                  Clock data out using ineedmd_adc_Receive_Conversion
* return value description: type - value: value description
******************************************************************************/
ERROR_CODE eADC_Continuous_Conversion_Mode(bool bContinuous_mode)
{
  ERROR_CODE eEC = ER_FAIL;

  if(bContinuous_mode == true)
  {
	eEC = eBSP_ADC_Data_command(ADS1198_RDATAC);
    GPIO_enableInt(EK_TM4C123GXL_ADC_INTERUPT, GPIO_INT_FALLING);
  }
  else
  {
    GPIO_disableInt(EK_TM4C123GXL_ADC_INTERUPT);
    eEC = eBSP_ADC_Data_command(ADS1198_SDATAC);
  }

  return eEC;
}

void vADS_drdy_int_callback(void)
{
  GPIO_clearInt(EK_TM4C123GXL_ADC_INTERUPT);

  eADC_Notify_ADS_continuous_callback(uiADS_Data);

  return;
}

ERROR_CODE eADC_Power_On_ADS_Sequence(void)
{
  ERROR_CODE eEC = ER_FAIL;

  eEC = eBSP_ADC_Power(true);
  if(eEC == ER_OK)
  {
    eEC = eBSP_ADC_Reset(false);
    Task_sleep(100);
  }

  if(eEC == ER_OK)
  {
    eEC = eBSP_ADC_Reset(true);
    Task_sleep(1);
  }

  if(eEC == ER_OK)
  {
    eEC = eBSP_ADC_Reset(false);
    Task_sleep(1);
  }

  return eEC;
}

ERROR_CODE eADC_Register_ADS_continuous_callback(void (*vADC_callback) (uint8_t * uiData))
{
  ERROR_CODE eEC = ER_FAIL;
  bool bIs_room = false;
  int i;

  //first verify that the callback is not already registered
  for(i = 0; i < ADS_CB_ARRAY_LIMIT; i++)
  {
    if(vADC_Continuous_ADS_Conversion_cb[i] != NULL)
    {
      if(vADC_Continuous_ADS_Conversion_cb[i] == vADC_callback)
      {
        //The call back was found, set the error code and break
        eEC = ER_OK;
        break;
      }
      else
      {
        eEC = ER_NOT_SET;
        continue;
      }
    }
    else
    {
      bIs_room = true;
      eEC = ER_NOT_SET;
      continue;
    }
  }

  //check if the call back was registered
  if(eEC == ER_NOT_SET)
  {
    //check if there is room in the call back array for the call back function
    if(bIs_room == true)
    {
      for(i = 0; i < ADS_CB_ARRAY_LIMIT; i++)
      {
        if(vADC_Continuous_ADS_Conversion_cb[i] == NULL)
        {
          vADC_Continuous_ADS_Conversion_cb[i] = vADC_callback;
          eEC = ER_OK;
          break;
        }
      }
    }
    else
    {
      eEC = ER_NOMEM;
    }

  }
  return eEC;
}

ERROR_CODE eADC_Notify_ADS_continuous_callback   (uint8_t * uiData)
{
  ERROR_CODE eEC = ER_NOT_SET;
  int i;

  //first verify that the callback is not already registered
  for(i = 0; i < ADS_CB_ARRAY_LIMIT; i++)
  {
    if(vADC_Continuous_ADS_Conversion_cb[i] != NULL)
    {
      vADC_Continuous_ADS_Conversion_cb[i](uiData);
      eEC = ER_OK;
    }
    else
    {
      continue;
    }
  }

  return eEC;
}

//********************************************************************************
//Enable lead off detection
//
//Uses default configuration:
//  Current source detection
//  DC excititation signal
//  4nA current magnitude
//  95% Positive / 5% negative for comparator threshold
//********************************************************************************
ERROR_CODE eADC_Enable_Lead_Detect(void)
{
  ERROR_CODE eEC = ER_OK;

  eEC = eBSP_ADC_Register_Write(LOFF, (ILEAD_OFF0 | ILEAD_OFF1 | FLEAD_OFF1 | FLEAD_OFF0 | COMP_TH0 | COMP_TH1 | COMP_TH2 ));
  //eEC = eBSP_ADC_Register_Write(LOFF, (FLEAD_OFF0));
  if(eEC == ER_OK)
  {
    eEC = eBSP_ADC_Register_Write(CONFIG4, PD_LOFF_COMP);
  }

  if(eEC == ER_OK)
  {
    eEC = eBSP_ADC_Register_Write(LOFF_SENSP, (LOFF8P | LOFF7P | LOFF6P | LOFF5P | LOFF4P | LOFF3P | LOFF2P | LOFF1P));
  }

  if(eEC == ER_OK)
  {
    eEC = eBSP_ADC_Register_Write(LOFF_SENSN, (LOFF8N | LOFF7N | LOFF6N | LOFF5N | LOFF4N | LOFF3N | LOFF2N | LOFF1N));
  }

  return eEC;
}

//********************************************************************************
//
//
//********************************************************************************
ERROR_CODE eADC_Setup_WTC(void)
{
  ERROR_CODE eEC = ER_OK;
  eEC = eBSP_ADC_Register_Write(WCT1, PD_WCTA | WCTA0 | WCTA1 | WCTA2);

  eEC = eBSP_ADC_Register_Write(WCT2, PD_WCTB | PD_WCTC | WCTB2 | WCTC2 | WCT1);

  return eEC;
}

//********************************************************************************
//
//
//********************************************************************************
ERROR_CODE eADC_Setup_RLD(void)
{
  ERROR_CODE eEC = ER_OK;
  eEC = eBSP_ADC_Register_Write(RLD_SENSP,  RLD3P | RLD4P);//RLD1P | RLD2P | RLD3P | RLD4P | RLD5P | RLD6P | RLD7P |RLD8P);//sense from channel 1 to 8 P
  if(eEC == ER_OK)
  {
	eBSP_ADC_Register_Write(RLD_SENSN, RLD3N);//RLD1N | RLD2N | RLD3N | RLD4N | RLD5N | RLD6N | RLD7N | RLD8N);//sense from channels 3, 4 N
  }
  if(eEC == ER_OK)
  {
	eBSP_ADC_Register_Write(CONFIG3, PD_RLD | PD_REFBUF | RLDREF_INT);
  }
  return eEC;
}

ERROR_CODE eADC_Setup_Inputs_ECG(ADC_CHANNEL_GAIN gain)
{
  //todo: error code checking
  ERROR_CODE eEC = ER_OK;
  eEC = eBSP_ADC_Register_Clear(CH1SET, PD1 | GAIN0 | GAIN1 | GAIN2 | MUX0 | MUX1 | MUX2);
  if(eEC == ER_OK)
  {
	eEC = eBSP_ADC_Register_Clear(CH2SET, PD1 | GAIN0 | GAIN1 | GAIN2 | MUX0 | MUX1 | MUX2);
  }
  if(eEC == ER_OK)
  {
	eEC = eBSP_ADC_Register_Clear(CH3SET, PD1 | GAIN0 | GAIN1 | GAIN2 | MUX0 | MUX1 | MUX2);
  }
  if(eEC == ER_OK)
  {
	eEC = eBSP_ADC_Register_Clear(CH4SET, PD1 | GAIN0 | GAIN1 | GAIN2 | MUX0 | MUX1 | MUX2);
  }
  if(eEC == ER_OK)
  {
	  eEC = eBSP_ADC_Register_Clear(CH5SET, PD1 | GAIN0 | GAIN1 | GAIN2 | MUX0 | MUX1 | MUX2);
  }
  if(eEC == ER_OK)
  {
	  eEC = eBSP_ADC_Register_Clear(CH6SET, PD1 | GAIN0 | GAIN1 | GAIN2 | MUX0 | MUX1 | MUX2);
  }
  if(eEC == ER_OK)
  {
	  eEC = eBSP_ADC_Register_Clear(CH7SET, PD1 | GAIN0 | GAIN1 | GAIN2 | MUX0 | MUX1 | MUX2);
  }
  if(eEC == ER_OK)
  {
	  eEC = eBSP_ADC_Register_Clear(CH8SET, PD1 | GAIN0 | GAIN1 | GAIN2 | MUX0 | MUX1 | MUX2);
  }
  if(eEC == ER_OK)
  {
	  eEC = eBSP_ADC_Register_Write(CH1SET, gain);
  }
  if(eEC == ER_OK)
  {
	  eEC = eBSP_ADC_Register_Write(CH2SET, gain);
  }
  if(eEC == ER_OK)
  {
	  eEC = eBSP_ADC_Register_Write(CH3SET, gain);
  }
  if(eEC == ER_OK)
  {
	  eEC = eBSP_ADC_Register_Write(CH4SET, gain);
  }
  if(eEC == ER_OK)
  {
	  eEC = eBSP_ADC_Register_Write(CH5SET, gain);
  }
  if(eEC == ER_OK)
  {
	  eEC = eBSP_ADC_Register_Write(CH6SET, gain);
  }
  if(eEC == ER_OK)
  {
	  eEC = eBSP_ADC_Register_Write(CH7SET, gain);
  }
  if(eEC == ER_OK)
  {
	  eEC = eBSP_ADC_Register_Write(CH8SET, gain);
  }
  return eEC;
}

ERROR_CODE eADC_Change_Gain(ADC_CHANNEL_GAIN gain)
{
	//todo: error codes on register read backs fail but gain is still set, figure out why
	ERROR_CODE eEC = ER_OK;

	eEC = eBSP_ADC_Register_Clear(CH1SET, GAIN0 | GAIN1 | GAIN2);
	eEC = eBSP_ADC_Register_Clear(CH2SET, GAIN0 | GAIN1 | GAIN2);
	eEC = eBSP_ADC_Register_Clear(CH3SET, GAIN0 | GAIN1 | GAIN2);
	eEC = eBSP_ADC_Register_Clear(CH4SET, GAIN0 | GAIN1 | GAIN2);
	eEC = eBSP_ADC_Register_Clear(CH5SET, GAIN0 | GAIN1 | GAIN2);
	eEC = eBSP_ADC_Register_Clear(CH6SET, GAIN0 | GAIN1 | GAIN2);
	eEC = eBSP_ADC_Register_Clear(CH7SET, GAIN0 | GAIN1 | GAIN2);
	eEC = eBSP_ADC_Register_Clear(CH8SET, GAIN0 | GAIN1 | GAIN2);

	eEC = eBSP_ADC_Register_Write(CH1SET, gain);
	eEC = eBSP_ADC_Register_Write(CH2SET, gain);
	eEC = eBSP_ADC_Register_Write(CH3SET, gain);
	eEC = eBSP_ADC_Register_Write(CH4SET, gain);
	eEC = eBSP_ADC_Register_Write(CH5SET, gain);
	eEC = eBSP_ADC_Register_Write(CH6SET, gain);
	eEC = eBSP_ADC_Register_Write(CH7SET, gain);
	eEC = eBSP_ADC_Register_Write(CH8SET, gain);
	return eEC;
}

ERROR_CODE eADC_Setup_Inputs_Test(ADC_CHANNEL_GAIN gain)
{
  //todo: error code checking
  ERROR_CODE eEC = ER_OK;

  eEC = eBSP_ADC_Register_Write(CONFIG1, DR2 | DR1);
  eEC = eBSP_ADC_Register_Write(CONFIG2, INT_TEST);

  eEC = eBSP_ADC_Register_Clear(CH1SET, PD1 | GAIN0 | GAIN1 | GAIN2 | MUX0 | MUX1 | MUX2);
  eEC = eBSP_ADC_Register_Clear(CH2SET, PD1 | GAIN0 | GAIN1 | GAIN2 | MUX0 | MUX1 | MUX2);
  eEC = eBSP_ADC_Register_Clear(CH3SET, PD1 | GAIN0 | GAIN1 | GAIN2 | MUX0 | MUX1 | MUX2);
  eEC = eBSP_ADC_Register_Clear(CH4SET, PD1 | GAIN0 | GAIN1 | GAIN2 | MUX0 | MUX1 | MUX2);
  eEC = eBSP_ADC_Register_Clear(CH5SET, PD1 | GAIN0 | GAIN1 | GAIN2 | MUX0 | MUX1 | MUX2);
  eEC = eBSP_ADC_Register_Clear(CH6SET, PD1 | GAIN0 | GAIN1 | GAIN2 | MUX0 | MUX1 | MUX2);
  eEC = eBSP_ADC_Register_Clear(CH7SET, PD1 | GAIN0 | GAIN1 | GAIN2 | MUX0 | MUX1 | MUX2);
  eEC = eBSP_ADC_Register_Clear(CH8SET, PD1 | GAIN0 | GAIN1 | GAIN2 | MUX0 | MUX1 | MUX2);

  eBSP_ADC_Register_Clear(CH1SET, PD1 | GAIN0 | GAIN1 | GAIN2 | MUX0 | MUX1 | MUX2);
  eBSP_ADC_Register_Clear(CH2SET, PD1 | GAIN0 | GAIN1 | GAIN2 | MUX0 | MUX1 | MUX2);
  eBSP_ADC_Register_Clear(CH3SET, PD1 | GAIN0 | GAIN1 | GAIN2 | MUX0 | MUX1 | MUX2);
  eBSP_ADC_Register_Clear(CH4SET, PD1 | GAIN0 | GAIN1 | GAIN2 | MUX0 | MUX1 | MUX2);
  eBSP_ADC_Register_Clear(CH5SET, PD1 | GAIN0 | GAIN1 | GAIN2 | MUX0 | MUX1 | MUX2);
  eBSP_ADC_Register_Clear(CH6SET, PD1 | GAIN0 | GAIN1 | GAIN2 | MUX0 | MUX1 | MUX2);
  eBSP_ADC_Register_Clear(CH7SET, PD1 | GAIN0 | GAIN1 | GAIN2 | MUX0 | MUX1 | MUX2);
  eBSP_ADC_Register_Clear(CH8SET, PD1 | GAIN0 | GAIN1 | GAIN2 | MUX0 | MUX1 | MUX2);

  eBSP_ADC_Register_Write(CH1SET, gain | MUX2 | MUX0);
  eBSP_ADC_Register_Write(CH2SET, gain | MUX2 | MUX0);
  eBSP_ADC_Register_Write(CH3SET, gain | MUX2 | MUX0);
  eBSP_ADC_Register_Write(CH4SET, gain | MUX2 | MUX0);
  eBSP_ADC_Register_Write(CH5SET, gain | MUX2 | MUX0);
  eBSP_ADC_Register_Write(CH6SET, gain | MUX2 | MUX0);
  eBSP_ADC_Register_Write(CH7SET, gain | MUX2 | MUX0);
  eBSP_ADC_Register_Write(CH8SET, gain | MUX2 | MUX0);

  return eEC;
}

//********************************************************************************
//start the internal voltage reference at 2.7V
//sets PD_REFBUF bit in CONFIG3 register
//
//********************************************************************************
ERROR_CODE eADC_Voltage_Reference(ADC_VREF_ADS_SOURCE eVref_Source)
{
  ERROR_CODE eEC = ER_FAIL;

  //must ensure conversions are not going on when changing vref
  eEC = eBSP_ADC_Start(false);

  if(eEC == ER_OK)
  {
    switch(eVref_Source)
    {
     case VREF_OFF:
     case VREF_ADC_DEFAULT:
        eEC = eBSP_ADC_Register_Clear(CONFIG3, (PD_REFBUF | VREF_4V       | RLD_MEAS | RLDREF_INT |\
                                                PD_RLD    | RLD_LOFF_SENS | RLD_STAT));
        break;
      case VREF_INTERNAL:
      {
        //perform a recursive call to put the voltage reference register into default settings
        eADC_Voltage_Reference(VREF_ADC_DEFAULT);

        eEC = eBSP_ADC_Register_Write(CONFIG3, PD_REFBUF);
        eEC = eBSP_ADC_Register_Write(EXTERNAL_REF_POWER_ADDRESS, EXTERNAL_REF_OFF);
        Task_sleep(150);

        break;
      }
      case VREF_EXTERNAL:
        //perform a recursive call to put the voltage reference register into default settings
        eADC_Voltage_Reference(VREF_ADC_DEFAULT);
        eEC = eBSP_ADC_Register_Clear(EXTERNAL_REF_POWER_ADDRESS, 0x0F);
        eEC = eBSP_ADC_Register_Write(EXTERNAL_REF_POWER_ADDRESS, EXTERNAL_REF_ON);
        break;
      default:
        eEC = ER_PARAM;
        break;
    }
  }
  else
  {
    eEC = ER_FAIL;
  }


  return eEC;
}

/******************************************************************************
* set the adc sampling rate
* 0 = 8kSPS
* 1 = 4k
* 2 = 2k
* 3 = 1k
* 4 = 500
* 5 = 250
* 6 = 125
*
* ensure device is not in continuous conversion mode
* using INEEDMD_ADC_Stop_Continuous_Conv() prior to calling this function
******************************************************************************/
ERROR_CODE eADC_Set_ADS_Sample_Rate(ADC_SAMPLE_ADS_RATE eRate)
{
  ERROR_CODE eEC = ER_FAIL;
  uint8_t uiSPS;

  //if out of bounds, default to 500 SPS
  if(eRate >= ADS_SAMPLE_RATE_LIMIT)
  {
    uiSPS = 4;
  }
  else
  {
    switch(eRate)
    {
      case ADS_SAMPLE_RATE_8KSPS:
        uiSPS = 0;
        break;
      case ADS_SAMPLE_RATE_4KSPS:
        uiSPS = 1;
        break;
      case ADS_SAMPLE_RATE_2KSPS:
        uiSPS = 2;
        break;
      case ADS_SAMPLE_RATE_1KSPS:
        uiSPS = 3;
        break;
      case ADS_SAMPLE_RATE_500SPS:
        uiSPS = 4;
        break;
      case ADS_SAMPLE_RATE_250SPS:
        uiSPS = 5;
        break;
      case ADS_SAMPLE_RATE_125SPS:
        uiSPS = 6;
        break;
      default:
        uiSPS = 4;
        break;
    }
  }

  //clear out the old sample rate
  eEC = eBSP_ADC_Register_Clear(CONFIG1, ADS_SAMPLE_RATE_CLEAR);

  if(eEC == ER_OK)
  {
    //write the new sample rate
      eEC = eBSP_ADC_Register_Write(CONFIG1, uiSPS);
  }

  return eEC;
}

//********************************************************************************
//read a full conversion
//call after ineedmd_adc_DRDY_PIN goes high
//takes a pointer to an array of integers
//fills array with received data from ADC
//********************************************************************************
ERROR_CODE eADC_Read_ADS_Single(uint8_t * pData)
{
  ERROR_CODE eEC = ER_FAIL;

  eEC = eBSP_ADC_Data_command(ADS1198_RDATA);

  eBSP_ADC_Data_frame_read(pData, INEEDMD_ADC_DATA_SIZE);
  return eEC;
}

ERROR_CODE eADC_Leads_Off_Single_Read(uint16_t * pLeadStat)
{
  ERROR_CODE eEC = ER_FAIL;
  uint8_t uiLeadP = 0;
  uint8_t uiLeadN = 0;
  eEC = eBSP_ADC_Register_Read(LOFF_STATP, &uiLeadP);
  eEC = eBSP_ADC_Register_Read(LOFF_STATN, &uiLeadN);
  if(eEC == ER_OK)
  {
    *pLeadStat = (uiLeadP << 8) | uiLeadN;
    return eEC;
  }
  else
  {
    return eEC;
  }
}

eADC_Read_ADS_Continuous(uint8_t * pData)
{
//  eEC = eBSP_ADC_Data_ADS_read(pData, INEEDMD_ADC_DATA_SIZE);
}


/******************************************************************************
* name: iADC_get_unit_tempoerature
* description: returns the current system temperature in tenths of a degree
*              fahrenheit resolution.
* param description: none
*
* return value description: int - 0: temperature not read
*                               - -1: temperature read failure
*                               - integer: temperature in tenths of a degree fahrenheit
******************************************************************************/
int8_t iADC_get_unit_temperature(void)
{
  uint8_t TempValueC = 180;
//  uint32_t temp_adc_return_value;
//
//  eBSP_TemperatureMeasureADCEnable();
//
//  ADCProcessorTrigger(TEMPERATURE_ADC, 3);
//  //
//  // Wait for conversion to be completed.
//  //
//  while(!ADCIntStatus(TEMPERATURE_ADC, 3, false))
//  {
//  }
//  //
//  // Clear the ADC interrupt flag.
//  //
//  ADCIntClear(TEMPERATURE_ADC, 3);
//  //
//  // Read ADC Value.
//  //
//  ADCSequenceDataGet(TEMPERATURE_ADC, 3, &temp_adc_return_value);
//
//  eBSP_TemperatureMeasureADCDisable();
//  TempValueC = ((1475 * 1023) - (2250 * temp_adc_return_value)) / 10230;

  TempValueC = tSystem_Temperature.uiConverted_measurement;

  return TempValueC;

}

void vADC_temperature_read_int_service(void)
{
  uint32_t uiTemp = 0;
  uint32_t summed_value = 0;
  int i;

  if(tSystem_Temperature.bIs_Data_Initalized == false)
  {
    for(i = 0; i < ADC_SYS_DATA_MEASUREMENTS; i++)
    {
        eBSP_ADC_temperature_read(&uiTemp);
        eBSP_ADC_temperature_read(&uiTemp);
        eBSP_ADC_temperature_read(&uiTemp);
      tSystem_Temperature.measuremet[i] = uiTemp;
    }

    tSystem_Temperature.bIs_Data_Initalized = true;
  }
  else
  {
    memcpy(&tSystem_Temperature.measuremet[0], &tSystem_Temperature.measuremet[1], ((ADC_SYS_DATA_MEASUREMENTS - 1)*4) );

    eBSP_ADC_temperature_read(&uiTemp);
    eBSP_ADC_temperature_read(&uiTemp);
    eBSP_ADC_temperature_read(&uiTemp);
    tSystem_Temperature.measuremet[(ADC_SYS_DATA_MEASUREMENTS - 1)] = uiTemp;
  }

  for(i = 0; i < ADC_SYS_DATA_MEASUREMENTS; i++)
  {
    summed_value = summed_value + tSystem_Temperature.measuremet[i];
  }

  tSystem_Temperature.average_measurement = summed_value / ADC_SYS_DATA_MEASUREMENTS;

  //todo: moved to a requested funciton
  tSystem_Temperature.uiConverted_measurement =(uint8_t)( 0xff & (148 - ((225 * tSystem_Temperature.average_measurement) / 4095)));

  return;
}

void vADC_batt_volt_read_int_service(void)
{
//#define DEBUG_vADC_batt_volt_read_int_service
#ifdef DEBUG_vADC_batt_volt_read_int_service
  #define  vSNPRINTF             snprintf
  #define  vDEBUG_BATT_READ_INT  vDEBUG
  char cSend_buff[113];
  memset(cSend_buff, 0x00, 113);
#else
  #define vSNPRINTF(a, n, ...)
  #define vDEBUG_BATT_READ_INT(a)
#endif
  uint32_t uiVoltage = 0;
  uint32_t summed_value = 0;
  int i;

  if(tSystem_Voltage.bIs_Data_Initalized == false)
  {
    for(i = 0; i < ADC_SYS_DATA_MEASUREMENTS; i++)
    {
        eBSP_ADC_batt_volt_read(&uiVoltage);

      tSystem_Voltage.measuremet[i] = uiVoltage;
    }

    tSystem_Voltage.bIs_Data_Initalized = true;
  }
  else
  {
    memcpy(&tSystem_Voltage.measuremet[0], &tSystem_Voltage.measuremet[1], ((ADC_SYS_DATA_MEASUREMENTS - 1)*4) );
    eBSP_ADC_batt_volt_read(&uiVoltage);
    tSystem_Voltage.measuremet[(ADC_SYS_DATA_MEASUREMENTS - 1)] = uiVoltage;
  }

  for(i = 0; i < ADC_SYS_DATA_MEASUREMENTS; i++)
  {
    summed_value = summed_value + tSystem_Voltage.measuremet[i];
  }

  tSystem_Voltage.average_measurement = summed_value / ADC_SYS_DATA_MEASUREMENTS;

  //todo: moved to requested function
  if (tSystem_Voltage.average_measurement > 1500 )
  {
	  tSystem_Voltage.uiConverted_measurement = (uint8_t)( 0xff & ( ( ( tSystem_Voltage.average_measurement * 165 ) - 250000 ) / 1000 ));
  }
  else
  {
	  tSystem_Voltage.uiConverted_measurement = 0;
  }

  vSNPRINTF(cSend_buff, 113, "V %.3d | %.5d - %.4x | %.4X,%.4X,%.4X,%.4X,%.4X,%.4X,%.4X,%.4X,%.4X,%.4X,%.4X,%.4X,%.4X,%.4X,%.4X,%.4X \n", tSystem_Voltage.uiConverted_measurement, tSystem_Voltage.average_measurement, tSystem_Voltage.average_measurement, tSystem_Voltage.measuremet[0], tSystem_Voltage.measuremet[1], tSystem_Voltage.measuremet[2], tSystem_Voltage.measuremet[3], tSystem_Voltage.measuremet[4], tSystem_Voltage.measuremet[5], tSystem_Voltage.measuremet[6], tSystem_Voltage.measuremet[7], tSystem_Voltage.measuremet[8], tSystem_Voltage.measuremet[9], tSystem_Voltage.measuremet[10], tSystem_Voltage.measuremet[11], tSystem_Voltage.measuremet[12], tSystem_Voltage.measuremet[13], tSystem_Voltage.measuremet[14], tSystem_Voltage.measuremet[15]);
  vDEBUG_BATT_READ_INT(cSend_buff);

  return;
}

//*****************************************************************************
// name: iADC_setup
// description: sets up the A to D converter driver
// param description: none
// return value description: 1 if success
//*****************************************************************************
ERROR_CODE eADC_setup(void)
{
  ERROR_CODE eEC = ER_FAIL;
  int i = 0;
  bool bIs_read_temp_clock_running = false;
  bool bIs_read_voltage_clock_running = false;
  uint8_t testRead = 0;

  //check if ADS ads is already setup
  if(bIs_adc_ads_setup == false)
  {
    //ADS is not setup, perform the setup sequence
    //

    //Init the callback function array
    for(i = 0; i < ADS_CB_ARRAY_LIMIT; i++)
    {
      vADC_Continuous_ADS_Conversion_cb[i] = NULL;
    }

    eEC = eBSP_ADC_SPI_Enable();

    if(eEC == ER_OK)
    {
      //halt ADC conversions
      eEC = eBSP_ADC_Start(false);
    }

    if(eEC == ER_OK)
    {
      //power on ADC

      eEC = eADC_Power_On_ADS_Sequence();
      if(eEC == ER_OK)
      {
        bIs_adc_powered_on = true;
      }
    }

    if(eEC == ER_OK)
    {
      //set up callback for continuous conversion mode

      GPIO_setupCallbacks(&Board_gpioCallbacks1);

      //set read data mode to single read

      eEC = eADC_Continuous_Conversion_Mode(false);
    }

    //todo: check id register, redo powerup if not success

    eBSP_ADC_Register_Read(ADS1198_ID_ADDRESS, &testRead);
    if(testRead != ADS1198_IDENTIFIER)
    {
      eEC = ER_FAIL;
    }
    else
    {
      eEC = ER_OK;
    }

    if(eEC == ER_OK)
    {
      //set the internal reference voltage
      eEC = eADC_Voltage_Reference(VREF_INTERNAL);
    }

    //setup WTC and RLD  - needed for ECG and lead off detect
    if(eEC == ER_OK)
    {
      eEC = eADC_Setup_WTC();
    }
    if(eEC == ER_OK)
    {
      eEC = eADC_Setup_RLD();
    }
    //enable lead detect
    if(eEC == ER_OK)
    {
      eEC = eADC_Enable_Lead_Detect();
    }

    eADC_Setup_Inputs_ECG(ADS_GAIN_12);

  }
  else
  {
    //ADS is already setup, only stop converstions and turn the ADS power on
    //

    //halt ADC conversions
    eEC = eBSP_ADC_Start(false);

    if(eEC == ER_OK)
    {
      //power on ADC
      eEC = eBSP_ADC_Power(true);
      if(eEC == ER_OK)
      {
        bIs_adc_powered_on = true;
      }
    }
  }

  //check if ADS setup completed successfully and set the control variables
  if(eEC == ER_OK)
  {
    bIs_adc_ads_sampling = false;
    bIs_adc_ads_setup = true;
  }
  else
  {
    bIs_adc_ads_sampling = false;
    bIs_adc_ads_setup = false;
  }

  //set up the temperature monitoring ADC
  //
  if(bIs_adc_temp_monitor_setup == false)
  {
    eEC = eBSP_ADC_temperature_init();
    if(eEC == ER_OK)
    {
      tSystem_Temperature.bIs_Data_Initalized = false;
      Clock_stop(tADC_read_temp_clock);
      Clock_start(tADC_read_temp_clock);
      bIs_adc_temp_monitor_setup = true;
    }
  }
  else
  {
    bIs_read_temp_clock_running = Clock_isActive(tADC_read_temp_clock);
    if(bIs_read_temp_clock_running == false)
    {
      Clock_start(tADC_read_temp_clock);
    }
  }

  //set up the battery voltage monitoring ADC
  //
  if(bIs_adc_batt_voltage_monitor_setup == false)
  {
    eEC = eBSP_ADC_batt_volt_init();
    if(eEC == ER_OK)
    {
      tSystem_Voltage.bIs_Data_Initalized = false;
      Clock_stop(tADC_read_volt_clock);
      Clock_start(tADC_read_volt_clock);
      bIs_adc_batt_voltage_monitor_setup = true;
    }
  }
  else
  {
    bIs_read_voltage_clock_running = Clock_isActive(tADC_read_temp_clock);
    if(bIs_read_voltage_clock_running == false)
    {
      Clock_start(tADC_read_volt_clock);
    }
  }

  return eEC;
}

/******************************************************************************
* public functions ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/

/******************************************************************************
* name: eADC_ADS_Power_status
* description: returns the power status of the external ADS module
* param description: none
*
* return value description: ERROR_CODE - ER_ON: ADS power is on
*                                      - ER_OFF: ADS power is off
******************************************************************************/
ERROR_CODE eADC_ADS_Power_status(void)
{
  ERROR_CODE eEC = ER_OFF;

  if(bIs_adc_powered_on == true)
  {
    eEC = ER_ON;
  }
  else
  {
    eEC = ER_OFF;
  }

  return eEC;
}

ERROR_CODE eADC_Request_param_init(ADC_REQUEST * tParam)
{
  ERROR_CODE eEC = ER_FAIL;

  tParam->eRequest_ID = ADC_REQUEST_NONE;
  tParam->uiADS_data = NULL;
  tParam->uiDevice_temperature = NULL;
  tParam->uiDevice_supp_voltage = NULL;
  tParam->ADC_continuous_conversion_callback = NULL;
  tParam->eVREF_source = NULL;

  if((tParam->eRequest_ID != ADC_REQUEST_NONE) |\
     (tParam->uiADS_data != NULL)              |\
     (tParam->uiDevice_temperature != NULL)    |\
     (tParam->uiDevice_supp_voltage != NULL)   |\
     (tParam->uiLeadStatus != NULL)   |\
     (tParam->eADC_gain != NULL)   |\
     (tParam->ADC_continuous_conversion_callback != NULL) |\
     (tParam->eVREF_source != NULL))
  {
    eEC = ER_FAIL;
  }
  else
  {
    eEC = ER_OK;
  }
  return eEC;
}

ERROR_CODE eADC_Request(ADC_REQUEST * tRequest)
{
  ERROR_CODE eEC = ER_FAIL;
  int i = 0;

  if((bIs_adc_ready == false) &\
     (tRequest->eRequest_ID != ADC_REQUEST_SETUP))
  {
    eEC = ER_NOT_READY;
  }
  else
  {
    switch(tRequest->eRequest_ID)
    {
      case ADC_REQUEST_SETUP:

        for(i = 0; i < 100; i++)
        {
          eEC = eADC_setup();
          if(eEC != ER_OK)
          {
            Task_sleep(100);
            eEC = eADC_setup();
          }
          else
          {
            break;
          }
        }

        if(eEC == ER_OK)
        {
          bIs_adc_ready = true;
        }
        else
        {
          bIs_adc_ready = false;
        }
        break;
      case ADC_REQUEST_ADS_SET_CONVERSION_MODE_CONTINUOUS:
        if(tRequest->ADC_continuous_conversion_callback == NULL)
        {
          eEC = ER_PARAM;
        }
        else
        {
          eEC = eADC_Continuous_Conversion_Mode(true);

          if(eEC == ER_OK)
          {
            eEC = eADC_Register_ADS_continuous_callback(tRequest->ADC_continuous_conversion_callback);
          }
        }
        break;
      case ADC_REQUEST_ADS_SET_CONVERSION_MODE_SINGLE:
        eEC = eADC_Continuous_Conversion_Mode(false);
        break;
      case ADC_REQUEST_ADS_SET_SAMPLE_RATE:
        eEC = eADC_Set_ADS_Sample_Rate(tRequest->eADS_sample_rate);
        break;
      case ADC_REQUEST_ADS_START:
        eEC = eBSP_ADC_Start(true);
        if(eEC == ER_OK)
        {
          bIs_adc_ads_sampling = true;
        }
        else
        {
          bIs_adc_ads_sampling = false;
        }
        break;
      case ADC_REQUEST_ADS_STOP:
        eEC = eBSP_ADC_Start(false);
        if(eEC == ER_OK)
        {
          bIs_adc_ads_sampling = false;
        }
        break;
      case ADC_REQUEST_ADS_POWER_ON:
        eEC = eBSP_ADC_Power(true);
        if(eEC == ER_OK)
        {
          bIs_adc_powered_on = true;
        }else{/*do nothing*/}
        break;
      case ADC_REQUEST_ADS_POWER_OFF:
        eEC = eBSP_ADC_Power(false);
        if(eEC == ER_OK)
        {
          bIs_adc_powered_on = false;
        }else{/*do nothing*/}
        break;
      case ADC_REQUEST_ADS_RESET:
        eEC = eBSP_ADC_Hard_Reset();
        break;
      case ADC_REQUEST_ADS_SINGLE_READ:
      {
        if(bIs_adc_ads_sampling == true)
        {
          eADC_Read_ADS_Single(tRequest->uiADS_data);
        }
        else
        {
          eEC = ER_NOT_ENABLED;
        }
        break;
      }
      case ADC_REQUEST_ADS_LEAD_DETECT:
        eEC = eADC_Leads_Off_Single_Read(tRequest->uiLeadStatus);
        break;
      case ADC_REQUEST_ADS_INPUT_ECG:
        eEC = eADC_Setup_Inputs_ECG(ADS_GAIN_12);
        if(eEC == ER_OK)
        {
          vDEBUG("ADC ECG inputs set");
        }
        break;

      case ADC_REQUEST_ADS_INPUT_SQUARE:
        eADC_Setup_Inputs_Test(ADS_GAIN_12);
        if(eEC == ER_OK)
        {
          vDEBUG("ADC Square Wave inputs set");
        }
        break;
      case ADC_REQUEST_DEVICE_TEMPERATURE:
      {
        *tRequest->uiDevice_temperature = iADC_get_unit_temperature();

        if(*tRequest->uiDevice_temperature != 0)
        {
          eEC = ER_OK;
        }
        else
        {
          eEC = ER_FAIL;
        }
        break;
      }
      case ADC_REQUEST_DEVICE_SUPPLY_VOLTAGE:
      {
        *tRequest->uiDevice_supp_voltage = tSystem_Voltage.uiConverted_measurement;

        if(*tRequest->uiDevice_supp_voltage != 0)
        {
          eEC = ER_OK;
        }
        else
        {
          eEC = ER_FAIL;
        }
        break;
      }
      case ADC_REQUEST_CHANGE_REFERENCE:
      {
      	eEC = eADC_Voltage_Reference(*tRequest->eVREF_source);
      	break;
      }
      case ADC_REQUEST_CHANGE_GAIN:
        eEC = eADC_Change_Gain(*tRequest->eADC_gain);
        break;
      default:
        eEC = ER_FAIL;
    }
  }

  return eEC;
}

#endif //#define __INEEDMD_A__
