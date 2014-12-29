/******************************************************************************
*
* nvram.h - NV ram include file
* Copyright (c) notice
*
******************************************************************************/
#ifndef __NVRAM_H__
#define __NVRAM_H__
/******************************************************************************
* includes ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/

/******************************************************************************
*public defines ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/

# define BLANK_ID "------------"

/******************************************************************************
*public variables /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/

/******************************************************************************
* external variables //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/

/******************************************************************************
*public enums /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/
//NV RAM request ID's
typedef enum NVRAM_REQUEST_ID
{
  NVR_REQUEST_NONE = 0,
  NVR_REQUEST_GET_FACILITY_ID,
  NVR_REQUEST_SET_FACILITY_ID,
  NVR_REQUEST_GET_PATIENT_ID,
  NVR_REQUEST_SET_PATIENT_ID,
  NVR_REQUEST_GET_DEVICE_ID,
  NVR_REQUEST_SET_DEVICE_ID,
  NVR_REQUEST_GET_CRASH_COUNT,
  NVR_REQUEST_SET_CRASH_COUNT,
  NVR_REQUEST_GET_FW_VERSION,
  NVR_REQUEST_SET_FW_VERSION,
  NVR_REQUEST_GET_RADIO_FW_VERSION,
  NVR_REQUEST_SET_RADIO_FW_VERSION,
  NVR_REQUEST_MASS_ERASE,
  NVR_REQUEST_MAX_MEMORY,
  NVR_REQUEST_LIMIT
}NVRAM_REQUEST_ID;

/******************************************************************************
*public structures ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/
//tExample_struct description
typedef struct tNVRAM_Request
{
    NVRAM_REQUEST_ID eReques_ID;
    char * pFacility_ID;
    char * pPatient_ID;
    char * pDevice_ID;
    char * pFW_Version;
    char * pRadio_FW_Version;
}tNVRAM_Request;

/******************************************************************************
* external functions //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/

/******************************************************************************
* public functions ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/

void read_facilities_ID(char *facilities_ID);
void save_facilities_ID(char *facilities_ID);

void read_dongle_ID(char *dongle_ID);
void save_dongle_ID(char *dongle_ID);

void set_patient_ID(char *patient_ID);
void blank_patient_ID(char *patient_ID);
ERROR_CODE eNVRAM_Request_Param_Init(tNVRAM_Request * pRequest);
ERROR_CODE eNVRAM_Request(tNVRAM_Request * pRequest);

#endif //__NVRAM_H__
