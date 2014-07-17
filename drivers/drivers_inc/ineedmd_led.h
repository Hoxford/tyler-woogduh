#ifndef __INEEDMDLED_H__
#define __INEEDMDLED_H__


#define INEEDMD_I2C_ADDRESS_LED_DRIVER 0x1b
#define INEEDMD_LED_OFF 0x00
#define INEEDMD_LED1_RED_ENABLE 0x01
#define INEEDMD_LED1_BLUE_ENABLE 0x02
#define INEEDMD_LED1_GREEN_ENABLE 0x04
#define INEEDMD_LED2_RED_ENABLE 0x20
#define INEEDMD_LED2_BLUE_ENABLE 0x40
#define INEEDMD_LED2_GREEN_ENABLE 0x80

#define INEEDMD_LED1_RED_ILEVEL 0x00
#define INEEDMD_LED1_BLUE_ILEVEL 0x01
#define INEEDMD_LED1_GREEN_ILEVEL 0x02

#define INEEDMD_LED2_RED_ILEVEL 0x05
#define INEEDMD_LED2_BLUE_ILEVEL 0x06
#define INEEDMD_LED2_GREEN_ILEVEL 0x07

#define INEEDMD_LED1_RED_PWM 0x08
#define INEEDMD_LED1_BLUE_PWM 0x09
#define INEEDMD_LED1_GREEN_PWM 0x0a

#define INEEDMD_LED2_RED_PWM 0x0d
#define INEEDMD_LED2_BLUE_PWM 0x0e
#define INEEDMD_LED2_GREEN_PWM 0x0f

#define INEEDMD_LED_OUTPUT_CONTROL 0x10
#define INEEDMD_LED_STATUS_REGISTER 0x11

#define INEEDMD_LED_TOGGLE_ALL 0x00
#define INEEDMD_LED_PROGRAM_SINGLE 0x20
#define INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE 0x40
#define INEEDMD_LED_PROGRAM_ALL_ILEVEL 0x60
#define INEEDMD_LED_PROGRAM_ALL_PWM 0x80

//ints to use in switch stmt
#define PATIENT_ALERT 14
#define ACQUIRE_UPLOAD_DATA 13 
#define LEAD_LOOSE 12
#define STORAGE_WARNING 11
#define POWER_ON_BATGOOD 1
#define POWER_ON_BATLOW 2
#define BAT_CHARGING 3
#define BT_CONNECTED 4
#define BT_ATTEMPTING 5
#define BT_FAILED 6
#define USB_CONNECTED 7
#define USB_FAILED 8
#define DATA_TRANSFER 9
#define TRANSFER_DONE 10
#define LED_OFF 15


extern void ineedmd_led_pattern(unsigned int);
void led_test(void);  //runs the gamut of LED patters


#endif // __INEEDMDLED_H__
