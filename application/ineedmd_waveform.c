//*****************************************************************************
//
// ineedmd_waveform.c - waveform application for the patient EKG data processing
//
// Copyright (c) notice
//
//*****************************************************************************
#ifndef __INEEDMD_WAVEFORM_C__
#define __INEEDMD_WAVEFORM_C__
/******************************************************************************
* includes ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "utils_inc/error_codes.h"
#include "utils_inc/osal.h"

#include "board.h"

#include "app_inc/ineedmd_command_protocol.h"
#include "drivers_inc/ineedmd_bluetooth_radio.h"
#include "drivers_inc/ineedmd_adc.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "app_inc/ineedmd_waveform.h"
#include "app_inc/ineedmd_UI.h"
#include "utils_inc/proj_debug.h"

/******************************************************************************
* defines /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/
//for ramp test waveform
#define ADC_1MV_GAIN_12_VREF_24      163
//todo: more defines if using different gain/reference
#define DATAPACKET_DATA_START 0x07

#define MEASUREMNT_DATA_START 0x03   //index to the measurement packet where the measurement is
#define MEASURMENTS_IN_PACKET 0x02   //measurements count for packets in the datagram  0x00 is 1 measurement...  0x02 is 3
#define DATA_LENGTH           0x10
#define DATAPACKET_TOTAL_LENGTH      (((MEASURMENTS_IN_PACKET+1)*DATA_LENGTH)+8)

#define LEADS_OFF_CLOCK_TIMEOUT 1000


/******************************************************************************
* variables ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/
bool bTest_Mode = false;
uint8_t uiCB_Data[19];

uint16_t uiLead_Off_Data = 0;
bool bIs_Radio_Ready = false;
bool bIs_Radio_On = false;
bool bIs_Carrier_Connection = false;
bool bIs_Protocol_Connection = false;
bool bSend_Error_Packet = false;
static int iTriangle_wave_count = 0;

eEKG_Task_state eCurrent_EKG_Task_state = EKG_TASK_IDLE;

/******************************************************************************
* external variables //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/

extern Mailbox_Handle tEKG_mailbox;
extern Timer_Handle tWaveform_test_pattern_timer;
extern Clock_Handle tWaveform_leads_off_timer;

/******************************************************************************
* enums ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/

typedef enum EKG_MSG_ID
{
  EKG_MSG_NONE,
  EKG_MSG_DATA_READ,
  EKG_MSG_LEAD_OFF_READ,
  EKG_MSG_LIMIT
}EKG_MSG_ID;

/******************************************************************************
* structures //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/

typedef struct tEKG_msg_struct
{
  EKG_MSG_ID eMsg;
}tEKG_msg_struct;

typedef struct tWAVEFORM_Activity_state
{
  bool bTest_Mode;
  uint8_t uiCB_Data[19];

  uint16_t uiLead_Off_Data;
  bool bIs_Radio_Ready;
  bool bIs_Radio_On;
  bool bIs_Carrier_Connection;
  bool bIs_Protocol_Connection;

  eHEART_LED_UI eHeart_led_sequence;
  eEKG_Task_state eCurrent_EKG_Task_state;
}tWAVEFORM_Activity_state;

tWAVEFORM_Activity_state tWF_Activity_state =
{
    false, //bool bTest_Mode =
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, //uint8_t uiCB_Data[19];
    0x0000, //uint16_t uiLead_Off_Data =
    false, //bool bIs_Radio_Ready =
    false, //bool bIs_Radio_On =
    false, //bool bIs_Carrier_Connection =
    false, //bool bIs_Protocol_Connection =

    HEART_LED_NO_UI, //eHEART_LED_UI eHeart_led_sequence = ;

    EKG_TASK_IDLE, //eEKG_Task_state eCurrent_EKG_Task_state = ;
};

/******************************************************************************
* external functions //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/

/******************************************************************************
* private function declarations ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/
ERROR_CODE eIneedmd_EKG_set_task_state(eEKG_Task_state eState);
void    vEKG_continuous_read_cb(uint8_t * uiData);
//radio callbacks
void       vEKG_Radio_connection_callback      (bool bRadio_Carrier_Connection, bool bRadio_Protocol_Connection);
void       vEKG_Radio_change_settings_callback (bool bDid_Setting_Change);
void       vEKG_Radio_sent_frame_callback      (uint32_t uiCount);
void       vEKG_Radio_rcv_frame_callback(uint8_t * pBuff, uint32_t uiRcvd_len);
void       vEKG_Radio_setup_callback(bool bRadio_Ready, bool bRadio_On);
//todo: declarations
/******************************************************************************
* private functions ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/

void vWaveform_timer_INT_Service(void)
{
  tEKG_msg_struct tEKG_msg;
  eEKG_Task_state eTask_state = eIneedmd_EKG_get_task_state();
  if(eTask_state == EKG_TASK_TEST_PATTERN_TRIANGLE)
  {
    tINMD_EKG_req tEKG_req;
    eIneedmd_EKG_request_param_init(&tEKG_req);
    tEKG_msg.eMsg = EKG_MSG_DATA_READ;

    Mailbox_post(tEKG_mailbox, &tEKG_msg, BIOS_NO_WAIT);
  }
}

void vWaveform_leads_off_INT_service(void)
{
  tEKG_msg_struct tEKG_msg;

  eEKG_Task_state eTask_state = eIneedmd_EKG_get_task_state();
  if(eTask_state == EKG_TASK_LEADS_OFF_ONLY)
  {
    //tell task to check lead status
    tINMD_EKG_req tEKG_req;
    eIneedmd_EKG_request_param_init(&tEKG_req);
    tEKG_req.eReq_ID = EKG_REQUEST_LEAD_OFF_READ;

    tEKG_msg.eMsg = EKG_MSG_LEAD_OFF_READ;

//    Mailbox_post(tEKG_mailbox, &tEKG_req, BIOS_NO_WAIT);
    Mailbox_post(tEKG_mailbox, &tEKG_msg, BIOS_NO_WAIT);
  }
}

void       vEKG_Radio_connection_callback      (bool bRadio_Carrier_Connection, bool bRadio_Protocol_Connection)
{
  eEKG_Task_state eTask_State;
  tINMD_EKG_req tEKG_req;
  if(bRadio_Protocol_Connection == false)
  {
    if(bIs_Protocol_Connection == true)
    {
      eTask_State = eIneedmd_EKG_get_task_state();
      if(eTask_State != EKG_TASK_IDLE)
      {
        tEKG_req.eReq_ID = EKG_REQUEST_IDLE;
		tEKG_req.uiLeadOffData = NULL;
		tEKG_req.vEKG_read_callback = NULL;
		eIneedmd_EKG_request(&tEKG_req);

        vDEBUG("vEKG_Radio_connection_callback, Stream stopped");
      }
    }
  }
  else if(bRadio_Protocol_Connection == true)
  {
    if(bIs_Protocol_Connection == false)
    {
      eIneedmd_EKG_request_param_init(&tEKG_req);
      tEKG_req.eReq_ID = EKG_REQUEST_LEADS_OFF_ONLY;
      tEKG_req.uiLeadOffData = NULL;
      tEKG_req.vEKG_read_callback = NULL;
      eIneedmd_EKG_request(&tEKG_req);

      vDEBUG("vEKG_Radio_connection_callback, Lead off detection started");
    }
  }

  bIs_Carrier_Connection = bRadio_Carrier_Connection;
  bIs_Protocol_Connection = bRadio_Protocol_Connection;
}

void       vEKG_Radio_change_settings_callback (bool bDid_Setting_Change)
{
}

void       vEKG_Radio_sent_frame_callback      (uint32_t uiCount)
{
}

void       vEKG_Radio_rcv_frame_callback(uint8_t * pBuff, uint32_t uiRcvd_len)
{
}

void       vEKG_Radio_setup_callback(bool bRadio_Ready, bool bRadio_On)
{
  bIs_Radio_Ready = bRadio_Ready;
  bIs_Radio_On = bRadio_On;
}

void vInit_Data_Packet(uint8_t* pDataPacket)
{
  memset(pDataPacket, 0x00, DATAPACKET_TOTAL_LENGTH );
  //hardcoding packet headers
  pDataPacket[0] = 0x9c;
  pDataPacket[1] = 0x03;
  pDataPacket[2] = DATAPACKET_TOTAL_LENGTH;
  pDataPacket[6] = MEASURMENTS_IN_PACKET + 1;
  pDataPacket[(DATAPACKET_TOTAL_LENGTH-1)] = 0xC9;
}

bool bIs_Radio_Available()
{
  if(bIs_Protocol_Connection && bIs_Carrier_Connection && bIs_Radio_Ready && bIs_Radio_On)
  {
    return true;
  }
  return false;
}

ERROR_CODE eIneedmd_EKG_set_task_state(eEKG_Task_state eState)
{
  ERROR_CODE eEC = ER_FAIL;
  eCurrent_EKG_Task_state = eState;

  if(eCurrent_EKG_Task_state == eState)
  {
    eEC = ER_OK;
  }
  else
  {
    eEC = ER_FAIL;
  }

  return eEC;
}

void vEKG_continuous_read_cb(uint8_t * pData)
{
  tEKG_msg_struct tEKG_msg;

  tEKG_msg.eMsg = EKG_MSG_DATA_READ;

  Mailbox_post(tEKG_mailbox, &tEKG_msg, BIOS_NO_WAIT);
  return;
}

/******************************************************************************
* name: vIneedmd_triangle_wave
* description: Triangle waveform generator
* param description:
* 	pDataPacket - pointer to the data packet to fill
* 	measurement_position is the sample sequence in the super packet in the range 0-3 for BT packets
* return value description:
******************************************************************************/
void vIneedmd_triangle_wave(uint8_t* pDataPacket, uint16_t measurment_position)
{
  //uint32_t gain = 12;
  //todo: get current gain from adc
  iTriangle_wave_count++;
  int i = 0;

  if(iTriangle_wave_count > ADC_1MV_GAIN_12_VREF_24)
  {
    iTriangle_wave_count = -1*ADC_1MV_GAIN_12_VREF_24;
  }

  //put data into datapacket
  for(i = 0; i <= 7; i++)
  {
    pDataPacket[(i*2) + (measurment_position*DATA_LENGTH) + DATAPACKET_DATA_START +1] = iTriangle_wave_count & 0xFF;
    pDataPacket[(i*2) + (measurment_position*DATA_LENGTH) + DATAPACKET_DATA_START ] = (iTriangle_wave_count >> 8) & 0xFF;
  }
}



#ifdef NOT_NOW
/******************************************************************************
* name:
* description:
* param description:
* return value description:
******************************************************************************/
int iIneedmd_waveform_enable_TestSignal(void)
{

  set_system_speed (INEEDMD_CPU_SPEED_FULL_INTERNAL);
  bTest_Mode = true;
  return 1;
}

/******************************************************************************
* name:
* description:
* param description:
* return value description:
******************************************************************************/
int iIneedmd_waveform_disable_TestSignal(void)
{
  set_system_speed (INEEDMD_CPU_SPEED_DEFAULT);
  bTest_Mode = false;
  return 1;
}

/******************************************************************************
* name:
* description:
* param description:
* return value description:
******************************************************************************/
bool iIneedmd_is_test_running(void)
{
  return bTest_Mode;
}
#endif //#ifdef NOT_NOW

/******************************************************************************
* public functions ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/

eEKG_Task_state eIneedmd_EKG_get_task_state(void)
{
  return eCurrent_EKG_Task_state;
}

ERROR_CODE eIneedmd_EKG_get_leads_off(uint16_t * uiLeadStat)
{
  ERROR_CODE eEC = ER_FAIL;
  eEKG_Task_state eTask_State = eIneedmd_EKG_get_task_state();

  if(eTask_State == EKG_TASK_IDLE)
  {
	 vDEBUG("EKG get leads off, EKG TASK IDLE no lead information");
     return eEC;
  }
  else
  {

     *uiLeadStat = uiLead_Off_Data;
     eEC = ER_OK;
  }

  return eEC;
}

ERROR_CODE eIneedmd_EKG_request_param_init(tINMD_EKG_req * ptRequest)
{
  ERROR_CODE eEC = ER_FAIL;

  //verify that the pointer to use is valid
  if(ptRequest == NULL)
  {
    //pointer is invalid
    eEC = ER_PARAM;
  }
  else
  {
    //pointer is valid
    //
    //Init params
    ptRequest->eReq_ID = EKG_REQUEST_NONE;
    ptRequest->vEKG_read_callback = NULL;
    ptRequest->uiLeadOffData = NULL;
    ptRequest->uiADS_Setting = NULL;

    //Check if params were properly set
    if((ptRequest->eReq_ID == EKG_REQUEST_NONE)  &\
       (ptRequest->vEKG_read_callback == NULL)  &\
       (ptRequest->uiLeadOffData == NULL)	&\
       (ptRequest->uiADS_Setting == NULL))
    {
      //Params were properly set
      eEC = ER_OK;
    }
    else
    {
      //Params were not properly set
      eEC = ER_FAIL;
    }
  }

  //return the error code
  return eEC;
}

ERROR_CODE eIneedmd_EKG_request(tINMD_EKG_req * ptRequest)
{
  ERROR_CODE eEC = ER_FAIL;
  tINMD_EKG_req tEKG_req;
  eEKG_Task_state eTask_state = EKG_TASK_NONE;
  ADC_REQUEST tADC_req;

  eTask_state = eIneedmd_EKG_get_task_state();
  if(ptRequest == NULL)
  {
    eEC = ER_PARAM;
  }
  else
  {
    //check if the request ID is valid
    if((ptRequest->eReq_ID >= EKG_REQUEST_LIMIT) |\
       (ptRequest->eReq_ID == EKG_REQUEST_NONE))
    {
      vDEBUG("eIneedmd_EKG_request request invalid");
      eEC = ER_INVALID;
    }
    else
    {
      switch(ptRequest->eReq_ID)
      {
        case EKG_REQUEST_IDLE:
        {
        	if(eTask_state == EKG_TASK_IDLE)
          {
            vDEBUG("ECG already in leads off only mode\n");
          }
          else
          {
            Timer_stop(tWaveform_test_pattern_timer);
            Clock_stop(tWaveform_leads_off_timer);

            //stop any waveform
            eADC_Request_param_init(&tADC_req);
            tADC_req.eRequest_ID = ADC_REQUEST_ADS_STOP;
            eEC = eADC_Request(&tADC_req);

            //turn off continuous mode - not sending any dataright now
            eADC_Request_param_init(&tADC_req);
            tADC_req.eRequest_ID = ADC_REQUEST_ADS_SET_CONVERSION_MODE_SINGLE;
            eEC = eADC_Request(&tADC_req);
            eADC_Request_param_init(&tADC_req);

            //mux inputs to ecg by default
            eADC_Request_param_init(&tADC_req);
            tADC_req.eRequest_ID = ADC_REQUEST_ADS_INPUT_ECG;
            eEC = eADC_Request(&tADC_req);

            eIneedmd_EKG_set_task_state(EKG_TASK_IDLE);
          }
    	    break;
        }

        case EKG_REQUEST_LEADS_OFF_ONLY:
        {
          if(eTask_state == EKG_TASK_LEADS_OFF_ONLY)
          {
            vDEBUG("ECG already in leads off only mode\n");
          }
          else
          {
            Timer_stop(tWaveform_test_pattern_timer);
            Clock_stop(tWaveform_leads_off_timer);

            //stop any waveforms so we can change settings
            eADC_Request_param_init(&tADC_req);
            tADC_req.eRequest_ID = ADC_REQUEST_ADS_STOP;
            eEC = eADC_Request(&tADC_req);

            //turn off continuous mode - not sending any dataright now
            eADC_Request_param_init(&tADC_req);
            tADC_req.eRequest_ID = ADC_REQUEST_ADS_SET_CONVERSION_MODE_SINGLE;
            eEC = eADC_Request(&tADC_req);
            eADC_Request_param_init(&tADC_req);

            //mux inputs to ecg so lead detection works
            eADC_Request_param_init(&tADC_req);
            tADC_req.eRequest_ID = ADC_REQUEST_ADS_INPUT_ECG;
            eEC = eADC_Request(&tADC_req);

            //start pin so lead detection works
            eADC_Request_param_init(&tADC_req);
            tADC_req.eRequest_ID = ADC_REQUEST_ADS_START;
            eEC = eADC_Request(&tADC_req);

            Clock_setTimeout(tWaveform_leads_off_timer, LEADS_OFF_CLOCK_TIMEOUT);
            Clock_start(tWaveform_leads_off_timer);

            eIneedmd_EKG_set_task_state(EKG_TASK_LEADS_OFF_ONLY);
          }
          break;
        }

        case EKG_REQUEST_START_ECG_STREAM:
        {
          if(eTask_state == EKG_TASK_WAVEFORM_CAPTURE)
          {
            vDEBUG("ECG stream already running\n");
          }
          else
          {
            Clock_stop(tWaveform_leads_off_timer);
            Timer_stop(tWaveform_test_pattern_timer);

            //setup adc
            eADC_Request_param_init(&tADC_req);
            tADC_req.eRequest_ID = ADC_REQUEST_ADS_STOP;
            eEC = eADC_Request(&tADC_req);

            //turn off continuous mode - not sending any dataright now
            eADC_Request_param_init(&tADC_req);
            tADC_req.eRequest_ID = ADC_REQUEST_ADS_SET_CONVERSION_MODE_SINGLE;
            eEC = eADC_Request(&tADC_req);
            eADC_Request_param_init(&tADC_req);

            //mux inputs to ecg
            eADC_Request_param_init(&tADC_req);
            tADC_req.eRequest_ID = ADC_REQUEST_ADS_INPUT_ECG;
            eEC = eADC_Request(&tADC_req);

            eADC_Request_param_init(&tADC_req);
            tADC_req.eRequest_ID = ADC_REQUEST_ADS_SET_SAMPLE_RATE;
            tADC_req.eADS_sample_rate = ADS_SAMPLE_RATE_500SPS;
            eEC = eADC_Request(&tADC_req);

            eADC_Request_param_init(&tADC_req);
            tADC_req.eRequest_ID = ADC_REQUEST_ADS_SET_CONVERSION_MODE_CONTINUOUS;
            tADC_req.ADC_continuous_conversion_callback = vEKG_continuous_read_cb;
            eEC = eADC_Request(&tADC_req);

            eADC_Request_param_init(&tADC_req);
            tADC_req.eRequest_ID = ADC_REQUEST_ADS_START;
            eEC = eADC_Request(&tADC_req);

            eIneedmd_EKG_set_task_state(EKG_TASK_WAVEFORM_CAPTURE);

            vDEBUG("ECG stream started\n");
          }

          break;
        }

        case EKG_REQUEST_START_SQUARE_WAVE_STREAM:
        {
          if(eTask_state == EKG_TASK_TEST_PATTERN_SQUARE)
          {
            vDEBUG("ECG test stream already running\n");
          }
          else
          {
            Clock_stop(tWaveform_leads_off_timer);
            Timer_stop(tWaveform_test_pattern_timer);
            //setup adc
            eADC_Request_param_init(&tADC_req);
            tADC_req.eRequest_ID = ADC_REQUEST_ADS_STOP;
            eEC = eADC_Request(&tADC_req);

            eADC_Request_param_init(&tADC_req);
            tADC_req.eRequest_ID = ADC_REQUEST_ADS_SET_CONVERSION_MODE_SINGLE;
            eEC = eADC_Request(&tADC_req);
            eADC_Request_param_init(&tADC_req);

            //mux inputs
            eADC_Request_param_init(&tADC_req);
            tADC_req.eRequest_ID = ADC_REQUEST_ADS_INPUT_SQUARE;
            eEC = eADC_Request(&tADC_req);

            eADC_Request_param_init(&tADC_req);
            tADC_req.eRequest_ID = ADC_REQUEST_ADS_SET_CONVERSION_MODE_CONTINUOUS;
            tADC_req.ADC_continuous_conversion_callback = vEKG_continuous_read_cb;
            eEC = eADC_Request(&tADC_req);

            eADC_Request_param_init(&tADC_req);
            tADC_req.eRequest_ID = ADC_REQUEST_ADS_SET_SAMPLE_RATE;
            tADC_req.eADS_sample_rate = ADS_SAMPLE_RATE_500SPS;
            eEC = eADC_Request(&tADC_req);

            eADC_Request_param_init(&tADC_req);
            tADC_req.eRequest_ID = ADC_REQUEST_ADS_START;
            eEC = eADC_Request(&tADC_req);

            eIneedmd_EKG_set_task_state(EKG_TASK_TEST_PATTERN_SQUARE);

            vDEBUG("Square wave started\n");
          }
          break;
        }

        case EKG_REQUEST_START_TRIANGLE_WAVE_STREAM:
        {
          if(eTask_state == EKG_TASK_TEST_PATTERN_TRIANGLE)
          {
            vDEBUG("ECG Triangle Wave  already running\n");
          }
          else
          {
            Clock_stop(tWaveform_leads_off_timer);
            Timer_stop(tWaveform_test_pattern_timer);
            eADC_Request_param_init(&tADC_req);
            tADC_req.eRequest_ID = ADC_REQUEST_ADS_STOP;
            eEC = eADC_Request(&tADC_req);
            //setup adc
            Timer_start(tWaveform_test_pattern_timer);

            eIneedmd_EKG_set_task_state(EKG_TASK_TEST_PATTERN_TRIANGLE);

            vDEBUG("Triangle wave started\n");
          }
          break;
        }

        case EKG_REQUEST_STOP_STREAM:
        {
          eTask_state = eIneedmd_EKG_get_task_state();
          if(eTask_state != EKG_TASK_IDLE)
          {
            //stop stream, default to IDLE state
            Clock_stop(tWaveform_leads_off_timer);
            Timer_stop(tWaveform_test_pattern_timer);
            eADC_Request_param_init(&tADC_req);
            tADC_req.eRequest_ID = ADC_REQUEST_ADS_STOP;
            eEC = eADC_Request(&tADC_req);

            eADC_Request_param_init(&tADC_req);
            tADC_req.eRequest_ID = ADC_REQUEST_ADS_SET_CONVERSION_MODE_SINGLE;
            eEC = eADC_Request(&tADC_req);
            
            if(ptRequest->vEKG_read_callback != NULL)
            {
              ptRequest->vEKG_read_callback(0, false);
            }
            eIneedmd_EKG_set_task_state(EKG_TASK_IDLE);

            vDEBUG("Stream stopped");
          }
          else
          {
            if(ptRequest->vEKG_read_callback != NULL)
            {
              ptRequest->vEKG_read_callback(0, false);
            }
            vDEBUG("Stream already stopped or not running");
            eEC = ER_OK;
          }

          break;
        }

        case EKG_REQUEST_SAMPLE_RATE:
        {
           //check current state of waveform task
          switch(eTask_state)
          {
            case EKG_TASK_TEST_PATTERN_TRIANGLE:
              vDEBUG("ER: Can't change triangle wave SPS");
              //todo: change timer load value
              break;

            case EKG_TASK_TEST_PATTERN_SQUARE:
              //stop ads to make register read/writes
              eADC_Request_param_init(&tADC_req);
              tADC_req.eRequest_ID = ADC_REQUEST_ADS_STOP;
              eEC = eADC_Request(&tADC_req);

              eADC_Request_param_init(&tADC_req);
              tADC_req.eRequest_ID = ADC_REQUEST_ADS_SET_CONVERSION_MODE_SINGLE;
              eEC = eADC_Request(&tADC_req);

              //adc request, change sample rate
              eADC_Request_param_init(&tADC_req);
              tADC_req.eRequest_ID = ADC_REQUEST_ADS_SET_SAMPLE_RATE;
              tADC_req.eADS_sample_rate = (ADC_SAMPLE_ADS_RATE)*(ptRequest->uiADS_Setting);
              eEC = eADC_Request(&tADC_req);

              eADC_Request_param_init(&tADC_req);
              tADC_req.eRequest_ID = ADC_REQUEST_ADS_SET_CONVERSION_MODE_CONTINUOUS;
              tADC_req.ADC_continuous_conversion_callback = vEKG_continuous_read_cb;
              eEC = eADC_Request(&tADC_req);

              //start ads
              eADC_Request_param_init(&tADC_req);
              tADC_req.eRequest_ID = ADC_REQUEST_ADS_START;
              eEC = eADC_Request(&tADC_req);
              break;

            case EKG_TASK_WAVEFORM_CAPTURE:
              //stop ads to make register read/writes
              eADC_Request_param_init(&tADC_req);
              tADC_req.eRequest_ID = ADC_REQUEST_ADS_STOP;
              eEC = eADC_Request(&tADC_req);

              eADC_Request_param_init(&tADC_req);
              tADC_req.eRequest_ID = ADC_REQUEST_ADS_SET_CONVERSION_MODE_SINGLE;
              eEC = eADC_Request(&tADC_req);

              //adc request, change sample rate
              eADC_Request_param_init(&tADC_req);
              tADC_req.eRequest_ID = ADC_REQUEST_ADS_SET_SAMPLE_RATE;
              tADC_req.eADS_sample_rate = (ADC_SAMPLE_ADS_RATE)*(ptRequest->uiADS_Setting);
              eEC = eADC_Request(&tADC_req);

              eADC_Request_param_init(&tADC_req);
              tADC_req.eRequest_ID = ADC_REQUEST_ADS_SET_CONVERSION_MODE_CONTINUOUS;
              tADC_req.ADC_continuous_conversion_callback = vEKG_continuous_read_cb;
              eEC = eADC_Request(&tADC_req);

              //start ads
              eADC_Request_param_init(&tADC_req);
              tADC_req.eRequest_ID = ADC_REQUEST_ADS_START;
              eEC = eADC_Request(&tADC_req);
              break;

            case EKG_TASK_LEADS_OFF_ONLY:
              vDEBUG("ER: EKG Not streaming");
              break;

            case EKG_TASK_IDLE:
              vDEBUG("ER: EKG Not streaming");
              break;

            default:
              vDEBUG("ER: EKG Task in unknown state =");
              break;
          }
          break;
        }

        case EKG_REQUEST_REFERENCE:
        {
          //check current state of waveform task
          switch(eTask_state)
          {
            case EKG_TASK_TEST_PATTERN_TRIANGLE:
              vDEBUG("ER: Can't change triangle wave reference");
              //todo: change timer load value
              break;

            case EKG_TASK_TEST_PATTERN_SQUARE:
            {
              //stop ads to make register read/writes
              eADC_Request_param_init(&tADC_req);
              tADC_req.eRequest_ID = ADC_REQUEST_ADS_STOP;
              eEC = eADC_Request(&tADC_req);

              eADC_Request_param_init(&tADC_req);
              tADC_req.eRequest_ID = ADC_REQUEST_ADS_SET_CONVERSION_MODE_SINGLE;
              eEC = eADC_Request(&tADC_req);

              //adc request, change sample rate
              eADC_Request_param_init(&tADC_req);
              tADC_req.eRequest_ID = ADC_REQUEST_CHANGE_REFERENCE;
              tADC_req.eVREF_source = (ptRequest->uiADS_Setting);
              eEC = eADC_Request(&tADC_req);

              eADC_Request_param_init(&tADC_req);
              tADC_req.eRequest_ID = ADC_REQUEST_ADS_SET_CONVERSION_MODE_CONTINUOUS;
              tADC_req.ADC_continuous_conversion_callback = vEKG_continuous_read_cb;
              eEC = eADC_Request(&tADC_req);

              //start ads
              eADC_Request_param_init(&tADC_req);
              tADC_req.eRequest_ID = ADC_REQUEST_ADS_START;
              eEC = eADC_Request(&tADC_req);
              vDEBUG("Changing EKG reference");
              break;
            }

            case EKG_TASK_WAVEFORM_CAPTURE:
            {
              //stop ads to make register read/writes
              eADC_Request_param_init(&tADC_req);
              tADC_req.eRequest_ID = ADC_REQUEST_ADS_STOP;
              eEC = eADC_Request(&tADC_req);

              eADC_Request_param_init(&tADC_req);
              tADC_req.eRequest_ID = ADC_REQUEST_ADS_SET_CONVERSION_MODE_SINGLE;
              eEC = eADC_Request(&tADC_req);

              //adc request, change sample rate
              eADC_Request_param_init(&tADC_req);
              tADC_req.eRequest_ID = ADC_REQUEST_CHANGE_REFERENCE;
              tADC_req.eVREF_source = (ptRequest->uiADS_Setting);
              eEC = eADC_Request(&tADC_req);

              eADC_Request_param_init(&tADC_req);
              tADC_req.eRequest_ID = ADC_REQUEST_ADS_SET_CONVERSION_MODE_CONTINUOUS;
              tADC_req.ADC_continuous_conversion_callback = vEKG_continuous_read_cb;
              eEC = eADC_Request(&tADC_req);

              //start ads
              eADC_Request_param_init(&tADC_req);
              tADC_req.eRequest_ID = ADC_REQUEST_ADS_START;
              eEC = eADC_Request(&tADC_req);
              vDEBUG("Changing EKG reference");
              break;
            }

            case EKG_TASK_LEADS_OFF_ONLY:
              //adc request, change sample rate
              Clock_stop(tWaveform_leads_off_timer);
              eADC_Request_param_init(&tADC_req);
              tADC_req.eRequest_ID = ADC_REQUEST_CHANGE_REFERENCE;
              tADC_req.eVREF_source = (ptRequest->uiADS_Setting);
              eEC = eADC_Request(&tADC_req);
              vDEBUG("Changing EKG reference");
              Clock_start(tWaveform_leads_off_timer);
              break;

            case EKG_TASK_IDLE:
              //adc request, change sample rate
              eADC_Request_param_init(&tADC_req);
              tADC_req.eRequest_ID = ADC_REQUEST_CHANGE_REFERENCE;
              tADC_req.eVREF_source = (ptRequest->uiADS_Setting);
              eEC = eADC_Request(&tADC_req);
              vDEBUG("Changing EKG reference");
              break;

            default:
              vDEBUG("ER: EKG Task in unknown state =");
              break;
          }
          break;
        }

        case EKG_REQUEST_CHANGE_GAIN:
        {
          //check current state of waveform task
          switch(eTask_state)
          {
            case EKG_TASK_TEST_PATTERN_TRIANGLE:
              vDEBUG("ER: Can't change triangle wave gain");
              break;

            case EKG_TASK_TEST_PATTERN_SQUARE:
              //stop ads to make register read/writes
              eADC_Request_param_init(&tADC_req);
              tADC_req.eRequest_ID = ADC_REQUEST_ADS_STOP;
              eEC = eADC_Request(&tADC_req);

              eADC_Request_param_init(&tADC_req);
              tADC_req.eRequest_ID = ADC_REQUEST_ADS_SET_CONVERSION_MODE_SINGLE;
              eEC = eADC_Request(&tADC_req);

              //adc request, change sample rate
              eADC_Request_param_init(&tADC_req);
              tADC_req.eRequest_ID = ADC_REQUEST_CHANGE_GAIN;
              tADC_req.eADC_gain = ptRequest->uiADS_Setting;
              eEC = eADC_Request(&tADC_req);

              eADC_Request_param_init(&tADC_req);
              tADC_req.eRequest_ID = ADC_REQUEST_ADS_SET_CONVERSION_MODE_CONTINUOUS;
              tADC_req.ADC_continuous_conversion_callback = vEKG_continuous_read_cb;
              eEC = eADC_Request(&tADC_req);

              //start ads
              eADC_Request_param_init(&tADC_req);
              tADC_req.eRequest_ID = ADC_REQUEST_ADS_START;
              eEC = eADC_Request(&tADC_req);
              break;

            case EKG_TASK_WAVEFORM_CAPTURE:
              //stop ads to make register read/writes
              eADC_Request_param_init(&tADC_req);
              tADC_req.eRequest_ID = ADC_REQUEST_ADS_STOP;
              eEC = eADC_Request(&tADC_req);

              //adc request, change sample rate
              eADC_Request_param_init(&tADC_req);
              tADC_req.eRequest_ID = ADC_REQUEST_CHANGE_GAIN;
              tADC_req.eADC_gain = ptRequest->uiADS_Setting;
              eEC = eADC_Request(&tADC_req);

              eADC_Request_param_init(&tADC_req);
              tADC_req.eRequest_ID = ADC_REQUEST_ADS_SET_CONVERSION_MODE_CONTINUOUS;
              tADC_req.ADC_continuous_conversion_callback = vEKG_continuous_read_cb;
              eEC = eADC_Request(&tADC_req);

              //start ads
              eADC_Request_param_init(&tADC_req);
              tADC_req.eRequest_ID = ADC_REQUEST_ADS_START;
              eEC = eADC_Request(&tADC_req);
              break;

            case EKG_TASK_LEADS_OFF_ONLY:
              vDEBUG("ER: EKG Not streaming");
              break;

            case EKG_TASK_IDLE:
              vDEBUG("ER: EKG Not streaming");
              break;

            default:
              vDEBUG("ER: EKG Task in unknown state =");
              break;
          }
          break;
        }
        case EKG_REQUEST_DATA_READ:
        {
          vDEBUG_ASSERT("eIneedmd_EKG_request EKG_REQUEST_DATA_READ not supported", 0);
          eIneedmd_EKG_request_param_init(&tEKG_req);
          tEKG_req.eReq_ID = EKG_REQUEST_DATA_READ;
          tEKG_req.uiLeadOffData = NULL;
          tEKG_req.vEKG_read_callback = NULL;
          eEC = ER_OK;
          break;
        }
        case EKG_REQUEST_ERROR_PACKET:
        {
          //check current state of waveform task
		  switch(eTask_state)
		  {
			case EKG_TASK_TEST_PATTERN_TRIANGLE:
			  //set error flag
			  bSend_Error_Packet = true;
			  eEC = ER_OK;
			  break;

			case EKG_TASK_TEST_PATTERN_SQUARE:
			  //set error flag
			  bSend_Error_Packet = true;
			  eEC = ER_OK;
			  break;

			case EKG_TASK_WAVEFORM_CAPTURE:
			  bSend_Error_Packet = true;
			  eEC = ER_OK;
			  break;

			case EKG_TASK_LEADS_OFF_ONLY:
			  vDEBUG("ER: EKG Not streaming");
			  break;

			case EKG_TASK_IDLE:
			  vDEBUG("ER: EKG Not streaming");
			  break;

			default:
			  vDEBUG("ER: EKG Task in unknown state =");
			  break;
		  }
		  break;
		}
        case EKG_REQUEST_EKG_SHUTDOWN:
        {
          //todo do EKG shut down
          if(ptRequest->vEKG_read_callback != NULL)
          {
            ptRequest->vEKG_read_callback(0,false);
          }
          eEC = ER_OK;
          break;
        }
        default:
          vDEBUG_ASSERT("eIneedmd_EKG_request invalid request" ,0);
          eEC = ER_REQUEST;
          break;
      }
    }
  }
  return eEC;
}

/******************************************************************************
* name:
* description:
* param description:
* return value description:
******************************************************************************/
void vIneedmd_waveform_task(UArg a0, UArg a1)
{
  ERROR_CODE eEC = ER_FAIL;
  ADC_REQUEST tADC_req;
  tRadio_request tRadio_req;
  tINMD_EKG_req tEKG_req;
  tEKG_msg_struct tEKG_msg;

  uint16_t measurement_packet_count = 0;

  uint16_t uiMsg_size = sizeof(tEKG_msg_struct);
  uint16_t uiMbox_size = 0;

  uint8_t uiAdcDataRead[19];
  uint8_t uiDataPacket[DATAPACKET_TOTAL_LENGTH];

  uint16_t uiLead_off_read = 0;
  tINMD_protocol_req_notify eCommand_Request;
  uint32_t uiClock_time_stop = 0;

  memset(uiAdcDataRead, 0x00, 19);

  uiMbox_size = Mailbox_getMsgSize(tEKG_mailbox);
  vDEBUG_ASSERT("vIneedmd_waveform_task invalid mailbox msg size!", (uiMsg_size == uiMbox_size));

  eADC_Request_param_init(&tADC_req);

  tADC_req.eRequest_ID = ADC_REQUEST_SETUP;
  eEC = eADC_Request(&tADC_req);
  vDEBUG_ASSERT("vIneedmd_waveform_task SYS HALT, ADC setup failure!", (eEC == ER_OK));

  eIneedmd_radio_request_params_init (&tRadio_req);
  tRadio_req.eRequest                    = RADIO_REQUEST_REGISTER_CALLBACKS;
  tRadio_req.vBuff_sent_callback         = &vEKG_Radio_sent_frame_callback;
  tRadio_req.vBuff_receive_callback      = &vEKG_Radio_rcv_frame_callback;
  tRadio_req.vChange_setting_callback    = &vEKG_Radio_change_settings_callback;
  tRadio_req.vConnection_status_callback = &vEKG_Radio_connection_callback;
  tRadio_req.vSetup_state_callback       = &vEKG_Radio_setup_callback;
  eIneedmd_radio_request(&tRadio_req);

  vInit_Data_Packet(uiDataPacket);

  eEC = eIneedmd_EKG_request_param_init(&tEKG_req);
  tEKG_req.eReq_ID = EKG_REQUEST_IDLE;
  tEKG_req.uiLeadOffData = NULL;
  tEKG_req.vEKG_read_callback = NULL;
  eEC = eIneedmd_EKG_request(&tEKG_req);

  while(1)
  {
    if(Mailbox_pend(tEKG_mailbox, &tEKG_msg, BIOS_WAIT_FOREVER) == true)
    {
      switch(tEKG_msg.eMsg)
      {
        case EKG_MSG_DATA_READ:
        {
          eEC = ER_FAIL;
          if(bIs_Radio_Available())
          {
            eEKG_Task_state eTask_state = eIneedmd_EKG_get_task_state();
            //for ECG stream or square wave, get reading from ADC, send to radio
            if(eTask_state == EKG_TASK_WAVEFORM_CAPTURE || eTask_state == EKG_TASK_TEST_PATTERN_SQUARE)
            {
              //read data from adc
              memset(uiAdcDataRead, 0x00, 19);
              eEC = eBSP_ADC_Data_frame_read(uiAdcDataRead, INEEDMD_ADC_DATA_SIZE);
              vDEBUG_ASSERT("EKG_MSG_DATA_READ ADC read fail", eEC == ER_OK);
              if(eEC == ER_OK)
              {
                //copy received data from front end into data packet
                memcpy((void*)&uiDataPacket[(DATAPACKET_DATA_START+(measurement_packet_count*DATA_LENGTH))], (void*)&uiAdcDataRead[MEASUREMNT_DATA_START], DATA_LENGTH);

                if (measurement_packet_count >= MEASURMENTS_IN_PACKET)
                {
                  //if streaming EKG, do lead off checking. If its a square wave, lead off info will be wrong.
                  if(eTask_state == EKG_TASK_WAVEFORM_CAPTURE)
                  {
                    if(Clock_getTicks() >= uiClock_time_stop)
                    {
                      uiLead_off_read = 0xFFFF & ((uiAdcDataRead[0]<<12) | (uiAdcDataRead[1]<<4) | ((uiAdcDataRead[2] & 0xF0)>>4));
                      if(uiLead_off_read != uiLead_Off_Data)
                      {
                        uiLead_Off_Data = uiLead_off_read;
                        //leads are off, notify command protocol
                        eIneedmd_cmnd_Proto_ReqNote_params_init(&eCommand_Request);
                        eCommand_Request.eReq_Notify = CMND_NOTIFY_LEADS_OFF;
                        eIneedmd_cmnd_Proto_Request_Notify(&eCommand_Request);
                      }else{/*do nothing*/}
                      //set stop time to recheck leads in 1 second
                      uiClock_time_stop = Clock_getTicks() + 1000;
                    }else{/*do nothing*/}
                  }
                  else
                  {
                    //for square wave, lead off info is wrong. assume all leads are on
                    uiLead_Off_Data = 0;
                  }

                  //done with leads off checking, request the radio to send the data packet
                  eEC = eIneedmd_radio_request_params_init(&tRadio_req);
                  if(eEC == ER_OK)
                  {
                    tRadio_req.eRequest = RADIO_REQUEST_SEND_FRAME;
                    //copy local data packet into the radio request
                    //if an error packet test request was received, send an error packet
                    if(bSend_Error_Packet)
                    {
                      //copy everything except the last four bytes
                      memcpy((void*)tRadio_req.uiBuff,(void*)uiDataPacket, DATAPACKET_TOTAL_LENGTH - 4);
                      tRadio_req.uiBuff_size = DATAPACKET_TOTAL_LENGTH - 4;
                      //copy end of packet byte, since it wasn't copied in memcpy operation
                      tRadio_req.uiBuff[(DATAPACKET_TOTAL_LENGTH - 4) - 1] = 0xC9;
                      eEC = eIneedmd_radio_request(&tRadio_req);
                      //reset error flag
                      bSend_Error_Packet = false;
                    }
                    //no error request
                    else
                    {
                      //send packet
                      memcpy((void*)tRadio_req.uiBuff,(void*)uiDataPacket, DATAPACKET_TOTAL_LENGTH);
                      tRadio_req.uiBuff_size = DATAPACKET_TOTAL_LENGTH;
                      eEC = eIneedmd_radio_request(&tRadio_req);
                    }
                  }
                  //reset the counter
                  measurement_packet_count = 0;
                }
                else
                {
                  measurement_packet_count++;
                }
              }
              else
              {
                //error in receiving data
                //eIneedmd_set_EKG_led(HEART_LED_LEAD_OFF_NO_DATA);
              }
            }

            //for triangle test pattern, generate the next reading, send to radio
            else if(eTask_state == EKG_TASK_TEST_PATTERN_TRIANGLE)
            {
              //get data
              //setup frame, send to radio
              vIneedmd_triangle_wave(uiDataPacket, measurement_packet_count);

              uiLead_Off_Data = 0;

              if (measurement_packet_count >= MEASURMENTS_IN_PACKET)
              {
                eEC = eIneedmd_radio_request_params_init(&tRadio_req);
                if(eEC == ER_OK)
                {
                  tRadio_req.eRequest = RADIO_REQUEST_SEND_FRAME;
                  //radio request is initailized, check if error packet request was received
                  if(bSend_Error_Packet)
                  {
                	//send an error in packet
                    //copy everything except the last four bytes
                	memcpy((void*)tRadio_req.uiBuff,(void*)uiDataPacket, DATAPACKET_TOTAL_LENGTH - 4);
                	tRadio_req.uiBuff_size = DATAPACKET_TOTAL_LENGTH - 4;
                	//copy end of packet byte, since it wasn't copied in memcpy operation
                	tRadio_req.uiBuff[(DATAPACKET_TOTAL_LENGTH - 4) - 1] = 0xC9;
                	eEC = eIneedmd_radio_request(&tRadio_req);
                	//reset error flag
                	bSend_Error_Packet = false;
                  }
                  //no error request
                  else
                  {
                	//send packet
                	memcpy((void*)tRadio_req.uiBuff,(void*)uiDataPacket, DATAPACKET_TOTAL_LENGTH);
                	tRadio_req.uiBuff_size = DATAPACKET_TOTAL_LENGTH;
                	eEC = eIneedmd_radio_request(&tRadio_req);
                  }
                }
                //reset the counter
                measurement_packet_count = 0;
              }
              else
              {
                measurement_packet_count++;
              }
            }
          }
          break;
        }

        case EKG_MSG_LEAD_OFF_READ:
        {
          //request from adc
          eADC_Request_param_init(&tADC_req);
          tADC_req.eRequest_ID = ADC_REQUEST_ADS_LEAD_DETECT;
          tADC_req.uiLeadStatus = &uiLead_off_read;
          eEC = eADC_Request(&tADC_req);
          //check if any leads are off
          if(uiLead_off_read != uiLead_Off_Data)
          {
            uiLead_Off_Data = uiLead_off_read;
            //leads are off, notify command protocol
            eIneedmd_cmnd_Proto_ReqNote_params_init(&eCommand_Request);
            eCommand_Request.eReq_Notify = CMND_NOTIFY_LEADS_OFF;
            eIneedmd_cmnd_Proto_Request_Notify(&eCommand_Request);
          }
          break;
        }
        default:
          break;
      }
    }
  }
}

/******************************************************************************
* name:
* description:
* param description:
* return value description:
******************************************************************************/

#endif //__INEEDMD_WAVEFORM_C__
