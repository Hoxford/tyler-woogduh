//*****************************************************************************
//
// usbdisk.h - usbdisk include file for the fatFS file system code
//
// Copyright (c) notice
//
//*****************************************************************************
#ifndef USBDISK
#ifndef __USBDISK_H__
#define __USBDISK_H__

//*****************************************************************************
// includes
//*****************************************************************************
#include "diskio.h"
//*****************************************************************************
// defines
//*****************************************************************************
#define USB_disk_initialize()    stat = STA_NOINIT
#define USB_disk_status()        stat = STA_NOINIT
#define USB_disk_read(b, s, c)   res = RES_PARERR
#define USB_disk_write(b, s, c)  res = RES_PARERR
#define USB_disk_ioctl(c, b)     res = RES_PARERR

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
#endif //__USBDISK_H__

#else //USBDISK
#ifndef __USBDISK_H__
#define __USBDISK_H__

#error "No USB disk functions created"
#endif //__USBDISK_H__
#endif //USBDISK
