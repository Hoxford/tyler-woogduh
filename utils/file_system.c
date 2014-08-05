//*****************************************************************************
//
// file_system.c - File system abstraction functions
//
// Copyright (c) notice
//
//*****************************************************************************
#ifndef __FILE_SYSTEM_C__
#define __FILE_SYSTEM_C__
//*****************************************************************************
// includes
//*****************************************************************************
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "utils_inc/file_system.h"

//*****************************************************************************
// defines
//*****************************************************************************
#ifdef USE_FatFs_DEFINES

#else
#error "File system module defines not declared!"
#endif
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

//*****************************************************************************
// functions
//*****************************************************************************

/******************************************************************************
* name:
* description: Open or create a file
* param description:
* return value description:
******************************************************************************/
int iFileSys_open (sFileSys* fp, const char* path, uint8_t mode)
{
  int iEC = -1;
  return iEC;
}

/******************************************************************************
* name:
* description: Close an open file object
* param description:
* return value description:
******************************************************************************/
int iFileSys_close (sFileSys* fp)
{
  int iEC = -1;
  return iEC;
}

/* Read data from a file */
/******************************************************************************
* name:
* description:
* param description:
* return value description:
******************************************************************************/
int iFileSys_read  (sFileSys* fp, void* buff, uint16_t btr, uint16_t* br)
{
  int iEC = -1;
  return iEC;
}

/* Write data to a file */
/******************************************************************************
* name:
* description:
* param description:
* return value description:
******************************************************************************/
int iFileSys_write (sFileSys* fp, const void* buff, uint16_t btw, uint16_t* bw)
{
  int iEC = -1;
  return iEC;
}

//int iFileSys_forward (sFileSys* fp, UINT(*func)(const BYTE*,UINT), UINT btf, UINT* bf); /* Forward data to the stream */
//int iFileSys_lseek (sFileSys* fp, DWORD ofs);               /* Move file pointer of a file object */
//int iFileSys_truncate (sFileSys* fp);                   /* Truncate file */

/* Flush cached data of a writing file */
/******************************************************************************
* name:
* description:
* param description:
* return value description:
******************************************************************************/
int iFileSys_sync (sFileSys* fp)
{
  int iEC = -1;
  return iEC;
}

/* Open a directory */
/******************************************************************************
* name:
* description:
* param description:
* return value description:
******************************************************************************/
int iFileSys_opendir (void * dp, const char* path)
{
  int iEC = -1;
  return iEC;
}

/* Close an open directory */
/******************************************************************************
* name:
* description:
* param description:
* return value description:
******************************************************************************/
int iFileSys_closedir (void * dp)
{
  int iEC = -1;
  return iEC;
}

/* Read a directory item */
/******************************************************************************
* name:
* description:
* param description:
* return value description:
******************************************************************************/
int iFileSys_readdir (void * dp, sFileSys * fno)
{
  int iEC = -1;
  return iEC;
}

/* Create a sub directory */
/******************************************************************************
* name:
* description:
* param description:
* return value description:
******************************************************************************/
int iFileSys_mkdir (const char* path)
{
  int iEC = -1;
  return iEC;
}

/* Delete an existing file or directory */
/******************************************************************************
* name:
* description:
* param description:
* return value description:
******************************************************************************/
int iFileSys_unlink (const char* path)
{
  int iEC = -1;
  return iEC;
}

//int iFileSys_rename (const TCHAR* path_old, const TCHAR* path_new);  /* Rename/Move a file or directory */

/* Get file status */
/******************************************************************************
* name:
* description:
* param description:
* return value description:
******************************************************************************/
int iFileSys_stat (const char* path, sFileSys * fno)
{  int iEC = -1;
return iEC;

}
//int iFileSys_chmod (const TCHAR* path, BYTE value, BYTE mask);     /* Change attribute of the file/dir */
//int iFileSys_utime (const TCHAR* path, const FILINFO* fno);      /* Change times-tamp of the file/dir */

/* Change current directory */
/******************************************************************************
* name:
* description:
* param description:
* return value description:
******************************************************************************/
int iFileSys_chdir (const char* path)
{
  int iEC = -1;
  return iEC;
}

//int iFileSys_chdrive (const TCHAR* path);                /* Change current drive */
//int iFileSys_getcwd (TCHAR* buff, UINT len);             /* Get current directory */
//int iFileSys_getfree (const TCHAR* path, DWORD* nclst, FATFS** fatfs); /* Get number of free clusters on the drive */
//int iFileSys_getlabel  (const TCHAR* path, TCHAR* label, DWORD* vsn); /* Get volume label */
//int iFileSys_setlabel  (const TCHAR* label);              /* Set volume label */

/* Mount/Unmount a logical drive */
/******************************************************************************
* name:
* description:
* param description:
* return value description:
******************************************************************************/
int iFileSys_mount(sFileSys* fs, const char* path, uint8_t opt)
{
  int iEC = -1;
#ifdef USE_FatFs_FUNCTIONS
  f_mount (fs->FileSys, NULL, FatFs_FORCE_MOUNT);
#else
#error "module mount file system is undefined!"
#endif
  return iEC;
}
//int iFileSys_mkfs (const TCHAR* path, BYTE sfd, UINT au);        /* Create a file system on the volume */
//int iFileSys_fdisk (BYTE pdrv, const DWORD szt[], void* work);     /* Divide a physical drive into some partitions */
//int iFileSys_putc (TCHAR c, sFileSys* fp);                    /* Put a character to the file */
//int iFileSys_puts (const TCHAR* str, sFileSys* cp);               /* Put a string to the file */
//int iFileSys_printf (sFileSys* fp, const TCHAR* str, ...);            /* Put a formatted string to the file */
//TCHAR* iFileSys_gets (TCHAR* buff, int len, sFileSys* fp);            /* Get a string from the file */

#ifdef USE_FatFs_USR_DEF_FUNCTIONS
unsigned long get_fattime(void)
{
  return 0;
}
#endif //USE_FatFs

#endif //__FILE_SYSTEM_C__

