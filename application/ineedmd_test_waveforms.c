/*
 * ineedmd_bluetooth_radio.c
 *
 *  Created on: Jul 7, 2014
 *      Author: BrianS
 */



/*
 * Include Section
 *
 */

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "inc/hw_gpio.h"
#include "inc/hw_sysctl.h"
#include "driverlib/sysctl.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom_map.h"
#include "driverlib/gpio.h"
#include "driverlib/i2c.h"
#include "driverlib/uart.h"
#include "driverlib/debug.h"

//Pull the processor
#include <inc/tm4c1233h6pm.h>

#include "board.h"
#include "ineedmd_bluetooth_radio.h"

/*
* defines
*/
//todo: these don't belong here
//#define INEEDMD_LOW_BATT_VOLTAGE     3300
//#define INEEDMD_SHTDWN_BATT_VOLTAGE  3100
//#define INEEDMD_FULL_BATT_VOLTAGE    3400
//#define INEEDMED_PIO_MASK            01
//
//#define SET_CONTROL_ECHO  "\r\nSET CONTROL ECHO %d\r\n"
//  #define SET_CONTROL_ECHO_BANNER_ON      0x01  //Bit 0 If this bit is set, the start-up banner is visible.
//  #define SET_CONTROL_ECHO_BANNER_OFF     0x00
//  #define SET_CONTROL_ECHO_COMMAND_ON     0x02  //Bit 1 If this bit is set, characters are echoed back to client in command mode.
//  #define SET_CONTROL_ECHO_COMMAND_OFF    0x00
//  #define SET_CONTROL_ECHO_SET_EVENT_ON   0x04  //Bit 2 This bit indicates if set events are displayed in command mode.
//  #define SET_CONTROL_ECHO_SET_EVENT_OFF  0x00
//  #define SET_CONTROL_ECHO_SYNTAX_ERR_ON  0x00  //Bit 3 If this bit is set, SYNTAX ERROR messages are disabled.
//  #define SET_CONTROL_ECHO_SYNTAX_ERR_OFF 0x08
//    #define SET_CONTROL_ECHO_SETTING  ( SET_CONTROL_ECHO_BANNER_OFF    \
//                                      | SET_CONTROL_ECHO_COMMAND_OFF   \
//                                      | SET_CONTROL_ECHO_SET_EVENT_OFF \
//                                      | SET_CONTROL_ECHO_SYNTAX_ERR_ON)
//
////#define SET_BT_NAME       "\r\nSET BT NAME IneedMD-00:07:80:0D:73:E0\r\n"
//#define SET_BT_WHAT_NAME  "\r\nSET BT NAME IneedMD-NUTS\r\n"
//#define SET_BT_NAME       "\r\nSET BT NAME IneedMD-%x%x\r\n"
//#define SET_BT_BDADDR        "\r\nSET BT BDADDR\r\n"  //gets the BT address, yes gets it even though it says SET
//#define SET_BT_BDADDR_PARSE  "%s %s %s %x %c %x %c %x %c %x %c %x %c %x"
//#define SET_BT_SSP      "\r\nSET BT SSP %d %d\r\n"
//  #define SET_BT_SSP_CPBLTES_DISP_ONLY    0  //Display only
//  #define SET_BT_SSP_CPBLTES_DISP_AND_YN  1  //Display + yes/no button
//  #define SET_BT_SSP_CPBLTES_KEYBRD_ONLY  2  //Keyboard only
//  #define SET_BT_SSP_CPBLTES_NONE         3  //None
//  #define SET_BT_SSP_MITM_NO_PROT    0  //no man in the middle protection
//  #define SET_BT_SSP_MITM_PROT       1  //man in the middle protection required
//#define SET_PROFILE_SPP   "\r\nSET PROFILE SPP\r\n"
//#define SET_CONTROL_BATT  "\r\nSET CONTROL BATTERY %d %d %d %.2d\r\n"
//  #define SET_CONTROL_BATT_LOW     INEEDMD_LOW_BATT_VOLTAGE
//  #define SET_CONTROL_BATT_SHTDWN  INEEDMD_SHTDWN_BATT_VOLTAGE
//  #define SET_CONTROL_BATT_FULL    INEEDMD_FULL_BATT_VOLTAGE
//  #define SET_CONTROL_BATT_MASK    INEEDMED_PIO_MASK
//#define SET_SET           "\r\nSET\r\n"
//
//#define BG_SIZE  64

/*
 * Function Section
 *
 */
//int iIneedmd_radio_rcv_string(char *cRcv_string);
int iIneedmd_parse_addr(char * cString_buffer, uint8_t * uiAddr);

/* ineedmd_measurement_ramp
 * Sends a set of measurement ramps... full scale..
 *
 * This function simply outputs a set of saw tooth measurement packets to test the display.
 */
void ineedmd_measurement_ramp()
{
#define PACKET_LENGTH 0x18

	unsigned char highbyte;
	unsigned char lowbyte;
	char test_packet[PACKET_LENGTH];

	test_packet[0x00] = 0x9C;
	test_packet[0x01] = 0x04;
	test_packet[0x02] = PACKET_LENGTH;
	test_packet[PACKET_LENGTH-1] = 0xC9;


	for (highbyte= 0; highbyte < 0xFF; highbyte++)
	{
		for (lowbyte= 0; lowbyte < 0xFF; lowbyte++)
		{
			//RA measurements
			test_packet[0x03] = highbyte;
			test_packet[0x04] = lowbyte;
			//LA measurements
			test_packet[0x05] = highbyte;
			test_packet[0x06] = lowbyte;
			//RL measurements
			test_packet[0x07] = highbyte;
			test_packet[0x08] = lowbyte;
			//LL measurements
			test_packet[0x09] = highbyte;
			test_packet[0x0A] = lowbyte;
			//V1 measurements
			test_packet[0x0B] = highbyte;
			test_packet[0x0C] = lowbyte;
			//V2 measurements
			test_packet[0x0D] = highbyte;
			test_packet[0x0E] = lowbyte;
			//V3 measurements
			test_packet[0x0F] = highbyte;
			test_packet[0x10] = lowbyte;
			//V4 measurements
			test_packet[0x11] = highbyte;
			test_packet[0x12] = lowbyte;
			//V5 measurements
			test_packet[0x13] = highbyte;
			test_packet[0x14] = lowbyte;
			//V6 measurements
			test_packet[0x15] = highbyte;
			test_packet[0x16] = lowbyte;

			//todo: this is the wrong api call to make to send a data frame
//			ineedmd_radio_send_string(test_packet);

		}
	}

}
