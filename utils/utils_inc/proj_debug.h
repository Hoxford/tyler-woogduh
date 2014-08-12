//*****************************************************************************
//
// proj_debug.h - project wide debug api include file
//
// Copyright (c) notice
//
//*****************************************************************************
#ifndef DEBUG
#ifndef __PROJ_DEBUG_H__
#define __PROJ_DEBUG_H__
//*****************************************************************************
// includes
//*****************************************************************************

//*****************************************************************************
// defines
//*****************************************************************************
#define __error__(a, b)
//#define vDEBUG(a, __VA_ARGS__)
#define vDEBUG(a, ...)
#define vDEBUG_init()
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
#endif //__PROJ_DEBUG_H__

#else
#ifndef __PROJ_DEBUG_H__
#define __PROJ_DEBUG_H__
//*****************************************************************************
// includes
//*****************************************************************************

//*****************************************************************************
// defines
//*****************************************************************************

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

void __error__(char *pcFilename, uint32_t ui32Line);
//void vUSB_driverlib_out(char *pcFilename, uint32_t ui32Line); //USB driverlib debug api
void vDEBUG(char * cMsg,...);
void vDEBUG_init(void);

#endif //__PROJ_DEBUG_H__
#endif //DEBUG
