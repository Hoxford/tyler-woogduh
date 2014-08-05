//*****************************************************************************
//
// atadrive.h - atadrive include file for the fatFS file system code
//
// Copyright (c) notice
//
//*****************************************************************************
#ifndef ATADRIVE
#ifndef __ATADRIVE_H__
#define __ATADRIVE_H__

//*****************************************************************************
// includes
//*****************************************************************************
#include "diskio.h"
//*****************************************************************************
// defines
//*****************************************************************************
#define ATA_disk_initialize()    stat = STA_NOINIT
#define ATA_disk_status()        stat = STA_NOINIT
#define ATA_disk_read(b, s, c)   res = RES_PARERR
#define ATA_disk_write(b, s, c)  res = RES_PARERR
#define ATA_disk_ioctl(c, b)     res = RES_PARERR

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
#endif //__ATADRIVE_H__

#else  //def ATADRIVE
#ifndef __ATADRIVE_H__
#define __ATADRIVE_H__

#error "No ATA drive functions created"
#endif //__ATADRIVE_H__
#endif  //ATADRIVE