//*****************************************************************************
//
// ineedmd_command_protocol.h - include file for the indeedMD command protocol
//
// Copyright (c) notice
//
//*****************************************************************************
#ifndef __INEEDMD_COMMAND_PROTOCOL_H__
#define __INEEDMD_COMMAND_PROTOCOL_H__
//*****************************************************************************
// includes
//*****************************************************************************

//*****************************************************************************
// defines
//*****************************************************************************
#define INMD_FRAME_BUFF_SIZE    256
#define NACK_FRAME_SIZE  12
#define ACK_FRAME_SIZE   12
//#define DWORD    unsigned long

#define COMMAND_FLAG      0x01
#define STATUS_FLAG       0x02
#define MEASURE_FLAG      0x03
#define STATUS_HIBERNATE  0x00
#define STATUS_SLEEP      0x20
#define STATUS_ON         0x40
#define STATUS_POWER      0x60

#define REQUEST_TO_TEST   0x18

#define STOP_EKG_TEST_PAT 0x00

#define EKG_TEST_PAT      0x01
#define TRIANGLE_TEST_PAT 0x00
#define SQUARE_TEST_PAT   0x01
#define WAVEFORM_TEST_PAT 0x02

#define LED_TEST_PATTERN  0x02
#define REQ_FOR_DFU       0x03
#define REQ_FOR_RESET     0x04
//*****************************************************************************
// variables
//*****************************************************************************

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
int iIneedmd_Rcv_cmnd_frame(uint8_t * uiCmnd_Frame, uint16_t uiCmnd_Frame_len);
int iIneedmd_Send_cmnd_frame(uint8_t * uiCmnd_Frame, uint16_t uiCmnd_Frame_len);
int iIneedmd_command_process(void);

#endif //__INEEDMD_COMMAND_PROTOCOL_H__
