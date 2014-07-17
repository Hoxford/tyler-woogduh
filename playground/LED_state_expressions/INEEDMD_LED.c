#include <stdint.h>
#include "INEEDMD_LED.h"

void ineedmd_led_pattern(unsigned int led_pattern)
{
    int i;
    switch (led_pattern){
        case PATIENT_ALERT:
            // 8 fast red flashes FLATLINE WARNING
            write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED2_RED_PWM, 0x20);

            for(i = 0; i<8; i++ ){
                write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED2_RED_ENABLE));
                wait_time(5); //1/2 second
                write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED_OFF));
                wait_time(5); // 1/2 second
            }
            break;

        case ACQUIRE_UPLOAD_DATA:
            //slow green flashes
            write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED2_GREEN_PWM, 0x20);

            for(i = 0; i<8; i++ ){
                write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED2_GREEN_ENABLE));
                wait_time(20); //2 second
                write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED_OFF));
                wait_time(20); //2 second
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
                write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED2_GREEN_ENABLE |INEEDMD_LED2_BLUE_ENABLE | INEEDMD_LED2_RED_ENABLE));
                wait_time(20); //2 second
                write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED_OFF));
                wait_time(20);
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
                write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED2_GREEN_ENABLE |INEEDMD_LED2_BLUE_ENABLE | INEEDMD_LED2_RED_ENABLE));
                wait_time(10); //5 second
            }
            break;

        case POWER_ON_BATGOOD:
            // solid green
            write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_GREEN_PWM, 0x20);

            for(i = 0; i<5; i++ ){
                write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED1_GREEN_ENABLE));
                wait_time(10); //1 second
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
                wait_time(10); //1 second
            }
            break;

        case BAT_CHARGING:
            //8 red flashes
            write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_RED_PWM, 0x20);

            for(i = 0; i<8; i++ ){
                write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED1_RED_ENABLE));
                wait_time(10);
                write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED_OFF));
                wait_time(10);
            }
            break;

        case BT_CONNECTED:
            //solid blue
            write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_BLUE_PWM, 0x20);

            for(i = 0; i<5; i++ ){
                write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED1_BLUE_ENABLE));
                wait_time(10); //1 second
            }
            break;

        case BT_ATTEMPTING:
            //8 blue flashes
            write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_BLUE_PWM, 0x20);

            for(i = 0; i<8; i++ ){
                write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED1_BLUE_ENABLE));
                wait_time(10);
                write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED_OFF));
                wait_time(10);
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
                wait_time(10); //1 second
            }
            break;

        case USB_CONNECTED:
            //solid blue
            write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_BLUE_PWM, 0x20);

            for(i = 0; i<5; i++ ){
                write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED1_BLUE_ENABLE));
                wait_time(10); //1 second
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
                wait_time(10); //1 second
            }
            break;

        case DATA_TRANSFER:
            //8 purple flashes
            write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_RED_PWM, 0x10);
            write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_BLUE_PWM, 0x20);

            for(i = 0; i<8; i++ ){
                write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED1_RED_ENABLE | INEEDMD_LED1_BLUE_ENABLE));
                wait_time(10);
                write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED_OFF));
                wait_time(10);
            }
            break;

        case TRANSFER_DONE:
            //solid purple
            write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_RED_PWM, 0x10);
            write_2_byte_i2c(INEEDMD_I2C_ADDRESS_LED_DRIVER, INEEDMD_LED_PROGRAM_SINGLE | INEEDMD_LED1_BLUE_PWM, 0x20);

            for(i = 0; i<8; i++ ){
                write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED1_RED_ENABLE | INEEDMD_LED1_BLUE_ENABLE));
                wait_time(10); //1 second
            }
            break;

        case LED_OFF:
            for(i = 0; i<8; i++ ){
                write_2_byte_i2c (INEEDMD_I2C_ADDRESS_LED_DRIVER, (INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE | INEEDMD_LED_OUTPUT_CONTROL), (INEEDMD_LED_OFF));
            }
            break;

        default:
            break;
    }
    
}
