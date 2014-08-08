//*****************************************************************************
//
// error_codes.h - system wide function error codes
//
// Copyright (c) notice
//
//*****************************************************************************
#ifndef __ERROR_CODES_H__
#define __ERROR_CODES_H__
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
//return error codes
typedef enum
{
    ER_OK,
    ER_FAIL,
    ER_TRUE,
    ER_FALSE,
    ER_TIMEOUT,
    ER_PARAM,
    ER_PARAM1,
    ER_PARAM2,
    ER_PARAM3,
    ER_BUFF_SIZE,
    ER_NOMEM,
    ER_CLOSED,
    ER_CLOSE,
    ER_OPENED,
    ER_OPEN,
    ER_NODATA   
}ERROR_CODE;

//*****************************************************************************
// structures
//*****************************************************************************

//*****************************************************************************
// external functions
//*****************************************************************************

//*****************************************************************************
// function declarations
//*****************************************************************************

#endif //__ERROR_CODES_H__
