
#ifndef PART_TM4C1233H6PM
 #define PART_TM4C1233H6PM
#endif

#ifndef TARGET_IS_TM4C1233H6PM
 #define TARGET_IS_TM4C1233H6PM
#endif

// Set us to debug mode
#define DEBUG_CODE
//


#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <driverlib/rom.h>

//Pull the processor
#include <inc/tm4c1233h6pm.h>


//processor pin config files
#include "board.h"
#include <driverlib/i2c.h>

#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "inc/hw_gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom_map.h"
#include "driverlib/gpio.h"


void write_2_byte_i2c (unsigned char device_id, unsigned char first_byte, unsigned char second_byte)

{

	I2CMasterSlaveAddrSet(I2C0_BASE, device_id, false);
    I2CMasterDataPut(I2C0_BASE, first_byte);
    I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_SEND_START);
    while(I2CMasterBusy(I2C0_BASE));
    I2CMasterDataPut(I2C0_BASE, second_byte);
    I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_SEND_FINISH);
    while(I2CMasterBusy(I2C0_BASE));

}

void wait_time (unsigned int tenths_of_seconds)
{
	MAP_SysCtlDelay(( MAP_SysCtlClockGet() / 30  )*tenths_of_seconds );
}

/*
 * main.c
 */
int main(void) {

    volatile uint32_t ui32Loop;
    uint32_t i;

	PortFunctionInit();
	write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_ALL_ILEVEL, 0x05);
	write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_ALL_PWM, 0x20);
	write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED2_OUTPUT_CONTROL), (INEEDMD_LED1_RED_ENABLE | INEEDMD_LED1_BLUE_ENABLE | INEEDMD_LED1_GREEN_ENABLE | INEEDMD_LED2_RED_ENABLE | INEEDMD_LED2_BLUE_ENABLE | INEEDMD_LED2_GREEN_ENABLE) );

    while(1)

    {
        //8 white flashes
        write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_RED_PWM, 0x20);
        write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_BLUE_PWM, 0x20);
        write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_GREEN_PWM, 0x20);

        for(i = 0; i<8; i++ ){
            write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED2_OUTPUT_CONTROL), (INEEDMD_LED1_RED_ENABLE | INEEDMD_LED1_BLUE_ENABLE | INEEDMD_LED1_GREEN_ENABLE));
            wait_time(25);
            write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED2_OUTPUT_CONTROL), (INEEDMD_LED_OFF));
            wait_time(25);
        }

        //8 purple flashes
        write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_RED_PWM, 0x10);
        write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_BLUE_PWM, 0x20);

    	for(i = 0; i<8; i++ ){
    		write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED2_OUTPUT_CONTROL), (INEEDMD_LED1_RED_ENABLE | INEEDMD_LED1_BLUE_ENABLE));
            wait_time(25);
            write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED2_OUTPUT_CONTROL), (INEEDMD_LED_OFF));
            wait_time(25);
        }

        //8 red flashes
        write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_RED_PWM, 0x20);

        for(i = 0; i<8; i++ ){
            write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED2_OUTPUT_CONTROL), (INEEDMD_LED1_RED_ENABLE));
            wait_time(25);
            write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED2_OUTPUT_CONTROL), (INEEDMD_LED_OFF));
            wait_time(25);
        }

        //8 blue flashes
        write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_BLUE_PWM, 0x20);

        for(i = 0; i<8; i++ ){
            write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED2_OUTPUT_CONTROL), (INEEDMD_LED1_BLUE_ENABLE));
            wait_time(25);
            write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED2_OUTPUT_CONTROL), (INEEDMD_LED_OFF));
            wait_time(25);
        }

        //8 green flashes
        write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_GREEN_PWM, 0x20);

        for(i = 0; i<8; i++ ){
            write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED2_OUTPUT_CONTROL), (INEEDMD_LED1_GREEN_ENABLE));
            wait_time(20); //2 second
            write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED2_OUTPUT_CONTROL), (INEEDMD_LED_OFF));
            wait_time(20); //2 second
        }

        // 8 fast red flashes
        write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_RED_PWM, 0x20);

        for(i = 0; i<8; i++ ){
            write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED2_OUTPUT_CONTROL), (INEEDMD_LED1_RED_ENABLE));
            wait_time(10); //1 second
            write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED2_OUTPUT_CONTROL), (INEEDMD_LED_OFF));
            wait_time(10); // 1 second
        }

        // 8 slow orange flashes
        write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_RED_PWM, 0x20);
        write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_GREEN_PWM, 0x10);

        for(i = 0; i<8; i++ ){
            write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED2_OUTPUT_CONTROL), (INEEDMD_LED1_GREEN_ENABLE | INEEDMD_LED1_RED_ENABLE));
            wait_time(50); //5 second
            write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED2_OUTPUT_CONTROL), (INEEDMD_LED_OFF));
            wait_time(50); // 5 second
        }

        // solid green
        write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_GREEN_PWM, 0x20);

        for(i = 0; i<8; i++ ){
            write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED2_OUTPUT_CONTROL), (INEEDMD_LED1_GREEN_ENABLE));
            wait_time(10); //1 second
        }

        //solid yellow
        write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_RED_PWM, 0x20);
        write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_GREEN_PWM, 0x20);

        for(i = 0; i<8; i++ ){
            write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED2_OUTPUT_CONTROL), (INEEDMD_LED1_GREEN_ENABLE | INEEDMD_LED1_RED_ENABLE));
            wait_time(10); //1 second
        }

        //solid blue
        write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_BLUE_PWM, 0x20);

        for(i = 0; i<8; i++ ){
            write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED2_OUTPUT_CONTROL), (INEEDMD_LED1_BLUE_ENABLE));
            wait_time(10); //1 second
        }

         //solid purple
        write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_RED_PWM, 0x10);
        write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_BLUE_PWM, 0x20);

        for(i = 0; i<8; i++ ){
            write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED2_OUTPUT_CONTROL), (INEEDMD_LED1_RED_ENABLE | INEEDMD_LED1_BLUE_ENABLE));
            wait_time(10); //1 second
        }



    }


	return(0);
}
