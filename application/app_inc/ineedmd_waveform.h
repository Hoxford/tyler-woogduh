//*****************************************************************************
//
// ineedmd_waveform.h - waveform application for the patient EKG data processing
//
// Copyright (c) notice
//
//*****************************************************************************
#ifndef __INEEDMD_WAVEFORM_H__
#define __INEEDMD_WAVEFORM_H__
/******************************************************************************
* includes ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/

/******************************************************************************
* public defines ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/

/******************************************************************************
* public variables ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/

/******************************************************************************
* external variables //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/

/******************************************************************************
* public enums ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/
//EKG request ID's
typedef enum eEKG_Request_ID
{
  EKG_REQUEST_NONE,
  EKG_REQUEST_IDLE,
  EKG_REQUEST_LEADS_OFF_ONLY,
  EKG_REQUEST_START_ECG_STREAM,
  EKG_REQUEST_START_TRIANGLE_WAVE_STREAM,
  EKG_REQUEST_START_SQUARE_WAVE_STREAM,
  EKG_REQUEST_STOP_STREAM,
  EKG_REQUEST_DATA_READ,
  EKG_REQUEST_LEAD_OFF_READ,
  EKG_REQUEST_SAMPLE_RATE,
  EKG_REQUEST_CHANGE_GAIN,
  EKG_REQUEST_REFERENCE,
  EKG_REQUEST_ERROR_PACKET,
//  EKG_REQUEST_SHORTING_BAR,
//  EKG_REQUEST_DFU,
//  EKG_REQUEST_SHIPPING_HOLD,
//  EKG_REQUEST_TEST_PATTERN,
  EKG_REQUEST_EKG_MONITOR,
  EKG_REQUEST_EKG_SHUTDOWN,
  EKG_REQUEST_LIMIT
}eEKG_Request_ID;

//EKG task state
typedef enum eEKG_Task_state
{
  EKG_TASK_NONE,
  EKG_TASK_IDLE,
  EKG_TASK_TEST_PATTERN_SQUARE,
  EKG_TASK_TEST_PATTERN_TRIANGLE,
  EKG_TASK_STORED_TEST_PATTERN_STREAM,
  EKG_TASK_LEADS_OFF_ONLY,
  EKG_TASK_WAVEFORM_CAPTURE,
  EKG_TASK_LIMIT
}eEKG_Task_state;


/******************************************************************************
* public structures //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/
//EKG request structure
typedef struct tINMD_EKG_req
{
  eEKG_Request_ID eReq_ID;
  uint16_t* uiLeadOffData;
  uint8_t * uiSingle_Read;
  uint8_t * uiADS_Setting;
  void (* vEKG_read_callback) (uint32_t uiEKG_data, bool bEKG_read);
}tINMD_EKG_req;

/******************************************************************************
* external functions //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/

/******************************************************************************
* public functions ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/
//int iIneedmd_waveform_enable_TestSignal(void);
//int iIneedmd_waveform_disable_TestSignal(void);
//bool iIneedmd_is_test_running(void);
//void ineedmd_measurement_ramp(void);
eEKG_Task_state eIneedmd_EKG_get_task_state(void);
ERROR_CODE eIneedmd_EKG_request_param_init(tINMD_EKG_req * ptRequest);
ERROR_CODE eIneedmd_EKG_request(tINMD_EKG_req * ptRequest);
ERROR_CODE eIneedmd_EKG_get_leads_off(uint16_t * uiLeadStat);

#endif// __INEEDMD_WAVEFORM_H__
