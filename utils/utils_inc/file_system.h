//*****************************************************************************
//
// file_system.h - File system abstraction functions include file
//
// Copyright (c) notice
//
//*****************************************************************************
#ifndef __FILE_SYSTEM_H__
#define __FILE_SYSTEM_C__
//*****************************************************************************
// includes
//*****************************************************************************
#ifdef USE_FatFs
#include "ff.h"
#else
#error "File system module inclued not declared!"
#endif
//*****************************************************************************
// defines
//*****************************************************************************
#ifdef USE_FatFs
  #define USE_FatFs_INCLUDES
  #define USE_FatFs_DEFINES
  #define USE_FatFs_VARIABLES
  #define USE_FatFs_EXTERN_VARIABLES
  #define USE_FatFs_ENUMS
  #define USE_FatFs_STRUCTS
  #define USE_FatFs_EXTERN_FCNS
  #define USE_FatFs_FUNCTIONS
  #define USE_FatFs_USR_DEF_FUNCTIONS

  #define FatFs_FORCE_MOUNT 1
  #define FatFs_MOUNT_LATER 0
#else
  #error "File system module undefined!"
#endif
//#define iFileSys_eof(fp) (((fp)->fptr == (fp)->fsize) ? 1 : 0)
//#define iFileSys_error(fp) ((fp)->err)
//#define iFileSys_tell(fp) ((fp)->fptr)
#define iFileSys_size(sFileSys) (sFileSys->fsize)

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
typedef struct
{
#ifdef USE_FatFs_STRUCTS
  FATFS   * FileSys;
  FIL     * FilePtr;
  DIR     * DirPtr;
  FILINFO * FileInfo;
#else
  #error "File system module structures undefined!"
#endif
}sFileSys;
//*****************************************************************************
// external functions
//*****************************************************************************

//*****************************************************************************
// function declarations
//*****************************************************************************
int iFileSys_open      (sFileSys* fp, const char * path, uint8_t mode);                      /* Open or create a file */
int iFileSys_close     (sFileSys* fp);                                                    /* Close an open file object */
int iFileSys_read      (sFileSys* fp, void* buff, uint16_t btr, uint16_t* br);                    /* Read data from a file */
int iFileSys_write     (sFileSys* fp, const void* buff, uint16_t btw, uint16_t* bw);              /* Write data to a file */
//int iFileSys_forward   (FIL* fp, UINT(*func)(const BYTE*,UINT), UINT btf, UINT* bf); /* Forward data to the stream */
//int iFileSys_lseek     (FIL* fp, DWORD ofs);                                         /* Move file pointer of a file object */
//int iFileSys_truncate  (FIL* fp);                                                    /* Truncate file */
int iFileSys_sync      (sFileSys* fp);                                                    /* Flush cached data of a writing file */
int iFileSys_opendir   (void * dp, const char * path);                                 /* Open a directory */
int iFileSys_closedir  (void * dp);                                                    /* Close an open directory */
int iFileSys_readdir   (void * dp, sFileSys * fno);                                      /* Read a directory item */
int iFileSys_mkdir     (const char * path);                                          /* Create a sub directory */
int iFileSys_unlink    (const char * path);                                          /* Delete an existing file or directory */
//int iFileSys_rename    (const TCHAR* path_old, const TCHAR* path_new);               /* Rename/Move a file or directory */
int iFileSys_stat      (const char * path, sFileSys * fno);                            /* Get file status */
//int iFileSys_chmod     (const TCHAR* path, BYTE value, BYTE mask);                   /* Change attribute of the file/dir */
//int iFileSys_utime     (const TCHAR* path, const FILINFO* fno);                      /* Change times-tamp of the file/dir */
int iFileSys_chdir     (const char * path);                                          /* Change current directory */
//int iFileSys_chdrive   (const TCHAR* path);                                          /* Change current drive */
//int iFileSys_getcwd    (TCHAR* buff, UINT len);                                      /* Get current directory */
//int iFileSys_getfree   (const TCHAR* path, DWORD* nclst, FATFS** fatfs);             /* Get number of free clusters on the drive */
//int iFileSys_getlabel  (const TCHAR* path, TCHAR* label, DWORD* vsn);                /* Get volume label */
//int iFileSys_setlabel  (const TCHAR* label);                                         /* Set volume label */
int iFileSys_mount     (sFileSys* fs, const char * path, uint8_t opt);                     /* Mount/Unmount a logical drive */
//int iFileSys_mkfs      (const TCHAR* path, BYTE sfd, UINT au);                       /* Create a file system on the volume */
//int iFileSys_fdisk     (BYTE pdrv, const DWORD szt[], void* work);                   /* Divide a physical drive into some partitions */
//int iFileSys_putc      (TCHAR c, FIL* fp);                                           /* Put a character to the file */
//int iFileSys_puts      (const TCHAR* str, FIL* cp);                                  /* Put a string to the file */
//int iFileSys_printf    (FIL* fp, const TCHAR* str, ...);                             /* Put a formatted string to the file */
//TCHAR* iFileSys_gets   (TCHAR* buff, int len, FIL* fp);                              /* Get a string from the file */

#endif //__FILE_SYSTEM_C__

