
#ifndef __INEEDMD_LED_H__
#define __INEEDMD_LED_H__

#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"

#include "driverlib/gpio.h"
#include "ineedmd_led.h"
#include "board.h"
#include "app_inc/ineedmd_power_modes.h"


///*
//*do a hardware reset by pulling reset low
//*/
//void ineedmd_radio_reset(void)
//{
//	//toggle reset pin
//	GPIOPinWrite(GPIO_PORTE_BASE, INEEDMD_PORTE_RADIO_REST, 0x00);
//	iHW_delay(1);
//	GPIOPinWrite(GPIO_PORTE_BASE, INEEDMD_PORTE_RADIO_REST, INEEDMD_PORTE_RADIO_REST);
//	//wait 18 device clock cycles - 9 usec
//}



void ineedmd_led_pattern(unsigned int led_pattern)
{
    int i;
    switch (led_pattern){
        case PATIENT_ALERT:
            // 8 fast red flashes FLATLINE WARNING
            write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED2_RED_PWM, 0x20);
            write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED2_RED_ILEVEL , 0x05);
            write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED2_GREEN_ILEVEL , 0x00);
            write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED2_BLUE_ILEVEL , 0x00);

            for(i = 0; i<8; i++ ){
                write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED2_RED_ENABLE));
                sleep_for_tenths(5); //1/2 second
                write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED_OFF));
                sleep_for_tenths(5); // 1/2 second
            }
            break;

        case ACQUIRE_UPLOAD_DATA:
            //slow green flashes
            write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED2_GREEN_PWM, 0x20);
            write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED2_RED_ILEVEL , 0x00);
            write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED2_GREEN_ILEVEL , 0x05);
            write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED2_BLUE_ILEVEL , 0x00);

            for(i = 0; i<8; i++ ){
                write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED2_GREEN_ENABLE));
                sleep_for_tenths(20); //2 second
                write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED_OFF));
                sleep_for_tenths(20); //2 second
            }
            break;

        case LEAD_LOOSE:
            //slow yellow flashes
             write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED2_RED_PWM, 30);
             write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED2_GREEN_PWM, 31);
             write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED2_BLUE_PWM, 0x00);
             write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED2_RED_ILEVEL , 0x18);
             write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED2_GREEN_ILEVEL , 0x18);
             write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED2_BLUE_ILEVEL , 0x00);


            for(i = 0; i<8; i++ ){
                write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED2_GREEN_ENABLE | INEEDMD_LED2_RED_ENABLE));
                sleep_for_tenths(20); //2 second
                write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED_OFF));
                sleep_for_tenths(20);
            }
            break;


        case STORAGE_WARNING:
            // solid orange
             write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED2_RED_PWM, 23);
             write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED2_GREEN_PWM, 22);
             write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED2_BLUE_PWM, 0x00);
             write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED2_RED_ILEVEL , 0x1f);
             write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED2_GREEN_ILEVEL , 0x10);
             write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED2_BLUE_ILEVEL , 0x00);

            for(i = 0; i<8; i++ ){
                write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED2_GREEN_ENABLE | INEEDMD_LED2_RED_ENABLE));
                sleep_for_tenths(10); //5 second
            }
            break;

        case POWER_ON_BATGOOD:
            // solid green
            write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_GREEN_PWM, 0x20);
            write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_RED_ILEVEL , 0x00);
            write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_GREEN_ILEVEL , 0x05);
            write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_BLUE_ILEVEL , 0x00);

            for(i = 0; i<5; i++ ){
                write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED1_GREEN_ENABLE));
                sleep_for_tenths(10); //1 second
            }
            break;

        case POWER_ON_BATLOW:
             write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_RED_PWM, 30);
             write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_GREEN_PWM, 31);
             write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_BLUE_PWM, 0x00);
             write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_RED_ILEVEL , 0x18);
             write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_GREEN_ILEVEL , 0x18);
             write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_BLUE_ILEVEL , 0x00);


            for(i = 0; i<8; i++ ){
                write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED1_GREEN_ENABLE |INEEDMD_LED1_BLUE_ENABLE | INEEDMD_LED1_RED_ENABLE));
                sleep_for_tenths(10); //1 second
            }
            break;

        case BAT_CHARGING:
            //8 red flashes
            write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_RED_PWM, 0x20);
            write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_RED_ILEVEL , 0x05);
            write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_GREEN_ILEVEL , 0x00);
            write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_BLUE_ILEVEL , 0x00);

            for(i = 0; i<8; i++ ){
                write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED1_RED_ENABLE));
                sleep_for_tenths(10);
                write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED_OFF));
                sleep_for_tenths(10);
            }
            break;

        case BT_CONNECTED:
            //solid blue
            write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_BLUE_PWM, 0x20);
            write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_RED_ILEVEL , 0x00);
            write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_GREEN_ILEVEL , 0x00);
            write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_BLUE_ILEVEL , 0x005);

            for(i = 0; i<5; i++ ){
                write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED1_BLUE_ENABLE));
                sleep_for_tenths(10); //1 second
            }
            break;

        case BT_ATTEMPTING:
            //8 blue flashes
            write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_BLUE_PWM, 0x20);

            for(i = 0; i<8; i++ ){
                write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED1_BLUE_ENABLE));
                sleep_for_tenths(10);
                write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED_OFF));
                sleep_for_tenths(10);
            }
            break;

        case BT_FAILED:
        //solid yellow
             write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_RED_PWM, 30);
             write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_GREEN_PWM, 31);
             write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_BLUE_PWM, 0x00);
             write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_RED_ILEVEL , 0x18);
             write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_GREEN_ILEVEL , 0x18);
             write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_BLUE_ILEVEL , 0x00);


            for(i = 0; i<8; i++ ){
                write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED1_GREEN_ENABLE |INEEDMD_LED1_BLUE_ENABLE | INEEDMD_LED1_RED_ENABLE));
                sleep_for_tenths(10); //1 second
            }
            break;

        case USB_CONNECTED:
            //solid blue
            write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_BLUE_PWM, 0x20);
            write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_RED_ILEVEL , 0x00);
            write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_GREEN_ILEVEL , 0x00);
            write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_BLUE_ILEVEL , 0x05);


            for(i = 0; i<5; i++ ){
                write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED1_BLUE_ENABLE));
                sleep_for_tenths(10); //1 second
            }
            break;

        case USB_FAILED:
            //solid yellow

             write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_RED_PWM, 30);
             write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_GREEN_PWM, 31);
             write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_BLUE_PWM, 0x00);
             write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_RED_ILEVEL , 0x18);
             write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_GREEN_ILEVEL , 0x18);
             write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_BLUE_ILEVEL , 0x00);


            for(i = 0; i<8; i++ ){
                write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED1_GREEN_ENABLE |INEEDMD_LED1_BLUE_ENABLE | INEEDMD_LED1_RED_ENABLE));
                sleep_for_tenths(10); //1 second
            }
            break;

        case DATA_TRANSFER:
            //8 purple flashes
            write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_RED_PWM, 0x10);
            write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_BLUE_PWM, 0x20);
            write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_RED_ILEVEL , 0x05);
            write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_GREEN_ILEVEL , 0x00);
            write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_BLUE_ILEVEL , 0x05);

            for(i = 0; i<8; i++ ){
                write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED1_RED_ENABLE | INEEDMD_LED1_BLUE_ENABLE));
                sleep_for_tenths(10);
                write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED_OFF));
                sleep_for_tenths(10);
            }
            break;

        case TRANSFER_DONE:
            //solid purple
            write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_RED_PWM, 0x10);
            write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_BLUE_PWM, 0x20);
            write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_RED_ILEVEL , 0x05);
            write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_GREEN_ILEVEL , 0x00);
            write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_BLUE_ILEVEL , 0x05);


            for(i = 0; i<8; i++ ){
                write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED1_RED_ENABLE | INEEDMD_LED1_BLUE_ENABLE));
                sleep_for_tenths(10); //1 second
            }
            break;

        case LED_CHIRP_RED:
          // 1 red flash
          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED2_RED_PWM, 0x20);
          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED2_RED_ILEVEL , 0x05);
          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED2_GREEN_ILEVEL , 0x00);
          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED2_BLUE_ILEVEL , 0x00);

          write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED2_RED_ENABLE));
          sleep_for_tenths(2); //2/10 second
          write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED_OFF));
          break;

        case LED_CHIRP_ORANGE:
          // 1 orange flash
          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED2_RED_PWM, 23);
          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED2_GREEN_PWM, 22);
          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED2_BLUE_PWM, 0x00);
          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED2_RED_ILEVEL , 0x1f);
          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED2_GREEN_ILEVEL , 0x10);
          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED2_BLUE_ILEVEL , 0x00);

          write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED2_GREEN_ENABLE | INEEDMD_LED2_RED_ENABLE));
          sleep_for_tenths(2); //2/10 second
          write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED_OFF));
          break;

        case LED_CHIRP_GREEN:
          // 1 green flash
          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_GREEN_PWM, 0x20);
          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_RED_ILEVEL , 0x00);
          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_GREEN_ILEVEL , 0x05);
          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_BLUE_ILEVEL , 0x00);

          write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED1_GREEN_ENABLE));
          sleep_for_tenths(2); //2/10 second
          write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED_OFF));
          break;

        case DFU_MODE:
          // Sets both LEDS to fussia
          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_RED_PWM, 0x20);
          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_GREEN_PWM, 0x20);
          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_BLUE_PWM, 0x20);
          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_RED_ILEVEL , 0xFF);
          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_GREEN_ILEVEL , 0x60);
          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_BLUE_ILEVEL , 0xBB);

          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED2_RED_PWM, 0x20);
          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED2_GREEN_PWM, 0x20);
          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED2_BLUE_PWM, 0x20);
          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED2_RED_ILEVEL , 0xFF);
          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED2_GREEN_ILEVEL , 0x60);
          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED2_BLUE_ILEVEL , 0xBB);

          write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED1_RED_ENABLE | INEEDMD_LED1_GREEN_ENABLE | INEEDMD_LED1_BLUE_ENABLE | INEEDMD_LED2_RED_ENABLE | INEEDMD_LED2_GREEN_ENABLE | INEEDMD_LED2_BLUE_ENABLE));
          break;

        case DFU_MODE2:
          // Sets both LEDS to fussia
          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_RED_PWM, 0x20);
          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_GREEN_PWM, 0x20);
          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_BLUE_PWM, 0x20);
          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_RED_ILEVEL , 0xFF);
          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_GREEN_ILEVEL , 0x60);
          write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_BLUE_ILEVEL , 0xBB);

          write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED1_RED_ENABLE | INEEDMD_LED1_GREEN_ENABLE | INEEDMD_LED1_BLUE_ENABLE | INEEDMD_LED2_RED_ENABLE | INEEDMD_LED2_GREEN_ENABLE | INEEDMD_LED2_BLUE_ENABLE));
          break;

        default:
        case LED_OFF:
            for(i = 0; i<8; i++ ){
                write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED_OFF));
            }
            break;
    }
    
}


//*****************************************************************************
// name: led_test
// description: runs the gamut of LED patters
// param description: none
// return value description: none
//*****************************************************************************
#ifdef DEBUG
void led_test(void)
{

    //ints to use in switch stmt

    ineedmd_led_pattern(POWER_ON_BATGOOD);
    ineedmd_led_pattern(POWER_ON_BATLOW);
    ineedmd_led_pattern(BAT_CHARGING);
    ineedmd_led_pattern(BT_CONNECTED);
    ineedmd_led_pattern(BT_ATTEMPTING);
    ineedmd_led_pattern(BT_FAILED);
    ineedmd_led_pattern(USB_CONNECTED);
    ineedmd_led_pattern(USB_FAILED);
    ineedmd_led_pattern(DATA_TRANSFER);
    ineedmd_led_pattern(TRANSFER_DONE);
    ineedmd_led_pattern(STORAGE_WARNING);
    ineedmd_led_pattern(LEAD_LOOSE);
    ineedmd_led_pattern(ACQUIRE_UPLOAD_DATA);
    ineedmd_led_pattern(PATIENT_ALERT);
    ineedmd_led_pattern(LED_OFF);

}
#endif //DEBUG

#endif //__INEEDMD_LED_H__
