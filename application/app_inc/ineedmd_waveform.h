//*****************************************************************************
//
// ineedmd_waveform.c - waveform application for the patient EKG data processing
//
// Copyright (c) notice
//
//*****************************************************************************
#ifndef __INEEDMD_WAVEFORM_H__
#define __INEEDMD_WAVEFORM_H__
//*****************************************************************************
// includes
//*****************************************************************************

//*****************************************************************************
// defines
//*****************************************************************************

/******************************************************************************
* variables
******************************************************************************/

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
int iIneedmd_waveform_enable_TestSignal(void);
int iIneedmd_waveform_disable_TestSignal(void);
bool iIneedmd_is_test_running(void);
void ineedmd_measurement_ramp(void);
int iIneedmd_waveform_process(void);

#endif// __INEEDMD_WAVEFORM_H__
