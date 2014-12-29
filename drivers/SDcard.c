//*****************************************************************************
//
// SDcard.c - SD card driver file
//
// Copyright (c) notice
//
//*****************************************************************************
#ifndef __SDCARD_C__
#define __SDCARD_C__
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
//#include "driverlib/adc.h"
#include "driverlib/uart.h"
#include "driverlib/debug.h"

#include "utils_inc/error_codes.h"
#include "drivers_inc/SDcard.h"

#include "board.h"
//#include "app_inc/ineedmd_power_modes.h"
#include "utils_inc/osal.h"
#include "utils_inc/proj_debug.h"

#include <ti/drivers/SDSPI.h>

/******************************************************************************
* defines /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/
#define TEST_SDCARD_FILE    "mark.txt"
#define BT_FILE_EXT         'dfu'
#define DRIVE_NUM           0
#define Board_SDSPI0        EK_TM4C123GXL_SDSPI0
/*
#ifdef DEBUG
  #define printf        vDEBUG
  #define PrintCommand  vCMND_ECHO_FRAME
  #define debug_out     vDEBUG
#else
//  #define printf(a, __VA_ARGS__)
  #define printf(a,...)
  #define PrintCommand(a,x)
//  #define debug_out(c,__VA_ARGS__)
  #define debug_out(c,...)
#endif //DEBUG
*/

/******************************************************************************
* variables ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/
SDSPI_Handle sdspiHandle;
SDSPI_Params sdspiParams;
FILE *src;
unsigned long bt_file_size;
char SDFilePathName[19];
char bt_DFU_Name[9];

/******************************************************************************
* external variables //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/

/******************************************************************************
* enums ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/

/******************************************************************************
* structures //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/

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

/******************************************************************************
* name: MMC_disk_initialize
* description: Mount SDcard IO and open file
* param description:
* return value description: ERROR_CODE
******************************************************************************/
ERROR_CODE MMC_disk_initialize(void)
{
  ERROR_CODE eEC = ER_OK;
// MJO
//uint8_t fdata[256];
//int rc;
//memset(fdata,0, sizeof(fdata));
  // hard code file name for testing now
//  sprintf(bt_DFU_Name,"mark");
//  sprintf(SDFilePathName,"fat:0:%s.%",bt_DFU_Name,BT_FILE_EXT);
//  sprintf(SDFilePathName,"fat:0:%s.txt",bt_DFU_Name);
sprintf(SDFilePathName,"fat:0:%s",TEST_SDCARD_FILE);

  // Mount and register the SD Card
  SDSPI_Params_init(&sdspiParams);
  sdspiHandle = SDSPI_open(Board_SDSPI0, DRIVE_NUM, &sdspiParams);
  if (sdspiHandle == NULL) {
/*#ifdef DEBUG
    printf("Error starting the SD card!");
#endif
*/
    return ER_FAIL;
  }
/*#ifdef DEBUG
  else {
    printf("SD Card is mounted");
  }
#endif
*/
  // open file
  src = fopen(SDFilePathName, "r");
  if(!src){
/*#ifdef DEBUG
    printf("Failed to open SD card file!");
#endif
*/
    return ER_OPEN;
  }
/*#ifdef DEBUG
  else {
    printf("SD card file opened");
  }
#endif
*/
  // get size of file
  fseek(src, 0, SEEK_END);
  bt_file_size = ftell(src);
  fseek(src, 0, SEEK_SET);
  if(bt_file_size == 0) {
/*#ifdef DEBUG
    printf("Invalid SD card file!");
#endif
*/
    return ER_OPEN;
  }

// MJO test call read
//rc = MMC_disk_read((uint8_t *)fdata,0,bt_file_size);
//rc = MMC_disk_read((uint8_t *)fdata,3,bt_file_size-3);

  return eEC;
}

/******************************************************************************
* name: MMC_disk_status
* description: return status of mounted disk
* param description:
* return value description: ERROR_CODE
******************************************************************************/
ERROR_CODE MMC_disk_status(void)
{
  ERROR_CODE eEC = ER_OK;
  if (sdspiHandle == NULL)
  {
    return ER_FAIL;
  }

  return eEC;
}

/******************************************************************************
* name: MMC_disk_read
* description: read contents of file on SD card
* param description: const uint8_t *buff - pointer to read buffer form disk
*                    uint32_t sector - offset of file in bytes
*                    uint16_t count - number of bytes to read from offset
* return value description: ERROR_CODE
******************************************************************************/
ERROR_CODE MMC_disk_read(const uint8_t *buff, uint32_t sector, uint16_t count)
{
  ERROR_CODE eEC = ER_OK;

  unsigned int bytesRead = 0;

  // move to offset
  fseek(src, sector, SEEK_SET);

  // read file data to buffer
  bytesRead = fread((uint8_t*)buff, 1, count, src);
  if(bytesRead == 0)
  {
    return ER_FAIL;
  }

  return eEC;
}

/******************************************************************************
* name:
* description:
* param description:
* return value description:
******************************************************************************/
ERROR_CODE MMC_disk_write(const uint8_t *buff, uint32_t sector, uint16_t count)
{
  ERROR_CODE eEC = ER_FAIL;

  // ER_FAIL
  return eEC;
}

/******************************************************************************
* name:
* description:
* param description:
* return value description:
******************************************************************************/
ERROR_CODE MMC_disk_ioctl(uint8_t cmd, void * buff)
{
  ERROR_CODE eEC = ER_FAIL;

  // ER_FAIL
  return eEC;
}

/*
 * END
 */
#endif //#define __SDCARD__

