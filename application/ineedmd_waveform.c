//*****************************************************************************
//
// ineedmd_waveform.c - waveform application for the patient EKG data processing
//
// Copyright (c) notice
//
//*****************************************************************************
#ifndef __INEEDMD_WAVEFORM_C__
#define __INEEDMD_WAVEFORM_C__
//*****************************************************************************
// includes
//*****************************************************************************
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "board.h"
#include "ineedmd_bluetooth_radio.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "app_inc/ineedmd_waveform.h"

//*****************************************************************************
// defines
//*****************************************************************************

/******************************************************************************
* variables
******************************************************************************/
static bool bTest_Mode = false;
//*****************************************************************************
// external variables
//*****************************************************************************

//*****************************************************************************
// enums
//*****************************************************************************

//*****************************************************************************
// structures
//*****************************************************************************

//*****************************************************************************
// external functions
//*****************************************************************************

//*****************************************************************************
// function declarations
//*****************************************************************************

//*****************************************************************************
// functions
//*****************************************************************************

/******************************************************************************
* name:
* description:
* param description:
* return value description:
******************************************************************************/
int iIneedmd_waveform_enable_TestSignal(void)
{
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

/* ineedmd_measurement_ramp
 * Sends a set of measurement ramps... full scale..
 *
 * This function simply outputs a set of saw tooth measurement packets to test the display.
 */
void ineedmd_measurement_ramp(void)
{
#define PACKET_LENGTH 0x18

  uint32_t uiSys_Tick_value = MAP_SysTickValueGet();
  unsigned char highbyte = uiSys_Tick_value >> 8;
  unsigned char lowbyte = uiSys_Tick_value;
  char test_packet[PACKET_LENGTH];

  test_packet[0x00] = 0x9C;
  test_packet[0x01] = 0x04;
  test_packet[0x02] = PACKET_LENGTH;
  test_packet[PACKET_LENGTH-1] = 0xC9;


//  for (highbyte= 0; highbyte < 0xFF; highbyte++)
//  {
//    for (lowbyte= 0; lowbyte < 0xFF; lowbyte++)
//    {
      //RA measurements
      test_packet[0x03] = highbyte;
      test_packet[0x04] = lowbyte;
      //LA measurements
      test_packet[0x05] = highbyte;
      test_packet[0x06] = lowbyte;
      //RL measurements
      test_packet[0x07] = highbyte;
      test_packet[0x08] = lowbyte;
      //LL measurements
      test_packet[0x09] = highbyte;
      test_packet[0x0A] = lowbyte;
      //V1 measurements
      test_packet[0x0B] = highbyte;
      test_packet[0x0C] = lowbyte;
      //V2 measurements
      test_packet[0x0D] = highbyte;
      test_packet[0x0E] = lowbyte;
      //V3 measurements
      test_packet[0x0F] = highbyte;
      test_packet[0x10] = lowbyte;
      //V4 measurements
      test_packet[0x11] = highbyte;
      test_packet[0x12] = lowbyte;
      //V5 measurements
      test_packet[0x13] = highbyte;
      test_packet[0x14] = lowbyte;
      //V6 measurements
      test_packet[0x15] = highbyte;
      test_packet[0x16] = lowbyte;

      ineedmd_radio_send_frame((uint8_t *)test_packet, PACKET_LENGTH);

//    }
//  }
}

/******************************************************************************
* name:
* description:
* param description:
* return value description:
******************************************************************************/
int iIneedmd_waveform_process(void)
{
  bool bDid_sys_tick = false;

  if(bTest_Mode == true)
  {
    bDid_sys_tick = bWaveform_did_timer_tick();

    if(bDid_sys_tick == true)
    {
      ineedmd_measurement_ramp();
    }
  }
  return 1;
}

/******************************************************************************
* name:
* description:
* param description:
* return value description:
******************************************************************************/

#endif //__INEEDMD_WAVEFORM_C__