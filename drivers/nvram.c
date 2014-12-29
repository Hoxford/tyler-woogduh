//*****************************************************************************
//
// nvram.c - stores ID and operating modes in the NVRAM that allows .
//
// Copyright (c) notice
//
//*****************************************************************************
#ifndef __NVRAM_C__
#define __NVRAM_C__
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

#include "driverlib/eeprom.h"
#include <ti/sysbios/knl/Clock.h>

#include "utils_inc/error_codes.h"
#include "utils_inc/proj_debug.h"
#include "utils_inc/osal.h"

#include "inc/tm4c1233h6pm.h"
#include "board.h"

#include "drivers_inc/nvram.h"

/******************************************************************************
* defines /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/

//Persistent settings defines
#define PS_START       0xAA
#define PS_END         0x55
#define ID_STRING_LEN  12

/******************************************************************************
* variables ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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

typedef struct tNVRAM_Activity_State
{
  bool bIs_NVRAM_Setup;
}tNVRAM_Activity_State;

tNVRAM_Activity_State tNVR_Activity_State =
{
  false, //bool bIs_NVRAM_Setup;
};

//persistent settings
typedef struct tSystem_Persistent_Settings
{
  uint8_t  uiSettings_Start;

  char     cFacility_ID[13];
  char     cDevice_ID[13];
  char     cPatient_ID[13];
  char     cDevice_FW_Ver[13];

  uint16_t uiCrash_Counter;

  uint8_t  uiSettings_End;

  uint8_t  uiFiller[2];
}tSystem_Persistent_Settings;

tSystem_Persistent_Settings tSystem_Settings;

tSystem_Persistent_Settings tDefault_System_Settings =
{
  0xAA,//uint8_t  uiSettings_Start;

  {'-','-','F','A','C','I','L','I','T','Y','-','-',0x00},  //char     cFacility_ID[13];
  {'-','-','D','E','V','I','C','E','I','D','-','-',0x00},  //char     cDevice_ID[13];
  {'-','-','P','A','T','I','E','N','T',' ','-','-',0x00},  //char     cPatient_ID[13];
  {'A','E','R','O',' ','V','5','.','X','.','X','X',0x00},  //char     cDevice_FW_Ver[13];

  0x0000,  //uint16_t uiCrash_Counter;

  0x55, //uint8_t  uiSettings_End;
};

/******************************************************************************
* external functions //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/

/******************************************************************************
* private function declarations ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/
ERROR_CODE eNVRAM_Init(void);

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
ERROR_CODE eNVRAM_Init(void)
{
  ERROR_CODE eEC = ER_FAIL;
  uint32_t EEERER  = EEPROM_INIT_OK;
  uint32_t eeprom_block_size;
  uint32_t eeprom_size;
  uint32_t eeprom_block_count;
  uint32_t eeprom_facilities_address;
  tSystem_Persistent_Settings tRead_System_Settings;

  uint16_t uiPS_Size = 0;
  uint16_t uiPS_Word_Check = 0;
  uint16_t uiPS_Word_Size = 0;

  int iEC = 0;

  if(tNVR_Activity_State.bIs_NVRAM_Setup == false)
  {
    EEERER = EEPROMInit();
    if (EEERER != EEPROM_INIT_ERROR)
    {
      eeprom_block_count = EEPROMBlockCountGet();
      eeprom_size = EEPROMSizeGet();
      eeprom_block_size = eeprom_size / eeprom_block_count;
      eeprom_facilities_address = (eeprom_block_count - 1) * eeprom_block_size;

      uiPS_Size = sizeof(tSystem_Settings);
      uiPS_Word_Size = uiPS_Size/4;

      uiPS_Word_Check = (uiPS_Size & 0x03);
      if(uiPS_Word_Check > 0)
      {
        uiPS_Word_Size += 1;
      }

      uiPS_Word_Size = uiPS_Word_Size * 2;

      EEPROMRead((uint32_t *)&tRead_System_Settings, eeprom_facilities_address, uiPS_Size);

      if((tRead_System_Settings.uiSettings_Start != PS_START) &
         (tRead_System_Settings.uiSettings_End != PS_END))
      {

        EEPROMProgram((uint32_t *)&tRead_System_Settings, eeprom_facilities_address, uiPS_Size);

        EEPROMRead((uint32_t *)&tRead_System_Settings, eeprom_facilities_address, uiPS_Size);

        iEC = memcmp (&tDefault_System_Settings, &tRead_System_Settings, sizeof(tSystem_Persistent_Settings) - 2 );
        if(iEC == 0)
        {
          eEC = ER_OK;
        }
        else
        {
          eEC = ER_FAIL;
        }
      }
      else
      {
        eEC = ER_OK;
      }
    }

    tNVR_Activity_State.bIs_NVRAM_Setup = true;
  }
  else
  {
    eEC = ER_OK;
  }
  return eEC;
}


/******************************************************************************
* public functions ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/

ERROR_CODE eNVRAM_Request_Param_Init(tNVRAM_Request * pRequest)
{
  ERROR_CODE eEC = ER_FAIL;
  int iSize = 0;

  if(pRequest != NULL)
  {
    iSize = sizeof(tNVRAM_Request);
    memset(pRequest, 0x00, iSize);

    if(pRequest->eReques_ID == NVR_REQUEST_NONE)
    {
      eEC = ER_OK;
    }
    else
    {
      eEC = ER_FAIL;
    }
  }
  else
  {
    eEC = ER_PARAM;
  }

  return eEC;
}

/******************************************************************************
* name: Filename_or_abreviation_funciton
* description:
* param description: type - value: value description (in order from left to right)
*                    bool - true: do action when set to true
* return value description: type - value: value description
******************************************************************************/
ERROR_CODE eNVRAM_Request(tNVRAM_Request * pRequest)
{
  ERROR_CODE eEC = ER_FAIL;

  eNVRAM_Init();

  switch(pRequest->eReques_ID)
  {
    case NVR_REQUEST_GET_FACILITY_ID:
      memcpy(pRequest->pFacility_ID, tSystem_Settings.cFacility_ID, ID_STRING_LEN);
      break;
    case NVR_REQUEST_SET_FACILITY_ID:
    case NVR_REQUEST_GET_PATIENT_ID:
    case NVR_REQUEST_SET_PATIENT_ID:
    case NVR_REQUEST_GET_DEVICE_ID:
    case NVR_REQUEST_SET_DEVICE_ID:
    case NVR_REQUEST_GET_CRASH_COUNT:
    case NVR_REQUEST_SET_CRASH_COUNT:
    case NVR_REQUEST_GET_FW_VERSION:
    case NVR_REQUEST_SET_FW_VERSION:
    case NVR_REQUEST_GET_RADIO_FW_VERSION:
    case NVR_REQUEST_SET_RADIO_FW_VERSION:
    case NVR_REQUEST_MASS_ERASE:
    case NVR_REQUEST_MAX_MEMORY:
      vDEBUG("eNVRAM_Request Command not supported yet");
      eEC = ER_FAIL;
      break;
    default:
      vDEBUG_ASSERT("eNVRAM_Request invalid ID", 0);
      break;
  }
  return eEC;
}

void read_facilities_ID(char *facilities_ID)
{
    uint32_t EEERER  = EEPROM_INIT_OK;
    uint32_t eeprom_block_size;
    uint32_t eeprom_size;
    uint32_t eeprom_block_count;
    uint32_t eeprom_facilities_address;
    uint32_t eeprom_buffer[12];


    facilities_ID[0] = 45;
    facilities_ID[1] = 45;
    facilities_ID[2] = 70;
    facilities_ID[3] = 97;
    facilities_ID[4] = 99;
    facilities_ID[5] = 105;
    facilities_ID[6] = 108;
    facilities_ID[7] = 105;
    facilities_ID[8] = 116;
    facilities_ID[9] = 121;
    facilities_ID[10] = 45;
    facilities_ID[11] = 45;
    EEERER = EEPROMInit();
    if (EEERER != EEPROM_INIT_ERROR)
    {
        eeprom_block_count = EEPROMBlockCountGet();
        eeprom_size = EEPROMSizeGet();
        eeprom_block_size = eeprom_size / eeprom_block_count;

        eeprom_facilities_address = (eeprom_block_count - 1) * eeprom_block_size;

        EEPROMRead(eeprom_buffer, eeprom_facilities_address, 24 );

        if (eeprom_buffer[0] != 0xFFFFFFFF)
        {
            memcpy(facilities_ID, eeprom_buffer, 24 );
        }

    }

    return;
}

void save_facilities_ID(char *facilities_ID)
{
    uint32_t EEERER  = EEPROM_INIT_OK;
    uint32_t eeprom_block_size;
    uint32_t eeprom_size;
    uint32_t eeprom_block_count;
    uint32_t eeprom_facilities_address;


    EEERER = EEPROMInit();
    if (EEERER != EEPROM_INIT_ERROR)
    {
        eeprom_block_count = EEPROMBlockCountGet();
        eeprom_size = EEPROMSizeGet();
        eeprom_block_size = eeprom_size / eeprom_block_count;

        eeprom_facilities_address = (eeprom_block_count - 1) * eeprom_block_size;


        if (facilities_ID[0] != 0x00)
        {
            EEPROMProgram((uint32_t *)facilities_ID, eeprom_facilities_address, 24 );
        }

    }

    return;
}

void read_dongle_ID(char *dongle_ID)
{
    uint32_t EEERER  = EEPROM_INIT_OK;
    uint32_t eeprom_block_size;
    uint32_t eeprom_size;
    uint32_t eeprom_block_count;
    uint32_t eeprom_dongle_id_address;
    uint32_t eeprom_buffer[12];


    dongle_ID[0] = 45;
    dongle_ID[1] = 68;
    dongle_ID[2] = 111;
    dongle_ID[3] = 110;
    dongle_ID[4] = 103;
    dongle_ID[5] = 108;
    dongle_ID[6] = 101;
    dongle_ID[7] = 45;
    dongle_ID[8] = 45;
    dongle_ID[9] = 73;
    dongle_ID[10] = 68;
    dongle_ID[11] = 45;

    EEERER = EEPROMInit();
    if (EEERER != EEPROM_INIT_ERROR)
    {
        eeprom_block_count = EEPROMBlockCountGet();
        eeprom_size = EEPROMSizeGet();
        eeprom_block_size = eeprom_size / eeprom_block_count;

        eeprom_dongle_id_address = ((eeprom_block_count - 2) * eeprom_block_size);

        EEPROMRead(eeprom_buffer, eeprom_dongle_id_address, 24 );

        if (eeprom_buffer[0] != 0xFFFFFFFF)
        {
            memcpy(dongle_ID, eeprom_buffer, 24 );
        }
    }


    return;

}
void save_dongle_ID(char *dongle_ID)
{
    uint32_t EEERER  = EEPROM_INIT_OK;
    uint32_t eeprom_block_size;
    uint32_t eeprom_size;
    uint32_t eeprom_block_count;
    uint32_t eeprom_dongle_id_address;
    uint32_t eeprom_buffer[12];

    EEERER = EEPROMInit();
    if (EEERER != EEPROM_INIT_ERROR)
    {
        eeprom_block_count = EEPROMBlockCountGet();
        eeprom_size = EEPROMSizeGet();
        eeprom_block_size = eeprom_size / eeprom_block_count;

        eeprom_dongle_id_address = ((eeprom_block_count - 2) * eeprom_block_size);

        EEPROMRead(eeprom_buffer, eeprom_dongle_id_address, 24 );

        if (dongle_ID[0] != 0x00 )
        {
            EEPROMProgram((uint32_t *)dongle_ID, eeprom_dongle_id_address, 24 );
        }
    }


    return;

}

void set_patient_ID(char *patient_ID)
{

    uint32_t clock_time;

    clock_time = Clock_getTicks();

    patient_ID[0] = (char)((0xff000000 & clock_time) >> 24);
    patient_ID[1] = (char)((0x00ff0000 & clock_time) >> 16);
    patient_ID[2] = (char)((0x0000ff00 & clock_time) >> 8);
    patient_ID[3] = (char)((0x000000ff & clock_time) >> 0);
    patient_ID[4] = (char)((0xff000000 & clock_time) >> 24);
    patient_ID[5] = (char)((0x00ff0000 & clock_time) >> 16);
    patient_ID[6] = (char)((0x0000ff00 & clock_time) >> 8);
    patient_ID[7] = (char)((0x000000ff & clock_time) >> 0);
    patient_ID[8] = (char)((0xff000000 & clock_time) >> 24);
    patient_ID[9] = (char)((0x00ff0000 & clock_time) >> 16);
    patient_ID[10] = (char)((0x0000ff00 & clock_time) >> 8);
    patient_ID[11] = (char)((0x000000ff & clock_time) >> 0);

}

void blank_patient_ID(char *patient_ID)
{

    memset((char *)patient_ID, 45,12);

    //memcpy((char *)patient_ID, (char *)BLANK_ID,12);
}

#endif //__NVRAM_C__
