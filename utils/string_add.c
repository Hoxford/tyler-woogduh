//*****************************************************************************
//
// string_add.c - additional string manimulation functions
//
// Copyright (c) notice
//
//*****************************************************************************
#ifndef __STRING_ADD_C__
#define __STRING_ADD_C__
//*****************************************************************************
// includes
//*****************************************************************************
#include <stdint.h>
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

//*****************************************************************************
// functions
//*****************************************************************************

//*****************************************************************************
// name: itoa
// description: Converts an integer value to a null-terminated string using the
//  specified base and stores the result in the array given by str parameter.
// param description:
//    value - Value to be converted to a string.
//    str   - Array in memory where to store the resulting null-terminated string.
//    base  - Numerical base used to represent the value as a string, between 2 and 36, where 10 means decimal base, 16 hexadecimal, 8 octal, and 2 binary.
// return value description: none
//*****************************************************************************
void itoa(long unsigned int value, char* result, int base)
    {
      // check that the base if valid
      if (base < 2 || base > 36) { *result = '\0';}

      char* ptr = result, *ptr1 = result, tmp_char;
      int tmp_value;

      do {
        tmp_value = value;
        value /= base;
        *ptr++ = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz" [35 + (tmp_value - value * base)];
      } while ( value );

      // Apply negative sign
      if (tmp_value < 0) *ptr++ = '-';
      *ptr-- = '\0';
      while(ptr1 < ptr) {
        tmp_char = *ptr;
        *ptr--= *ptr1;
        *ptr1++ = tmp_char;
      }

    }

#endif //__STRING_ADD_C__
