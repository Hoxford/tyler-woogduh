#ifndef __INEEDMDLED_H__
#define __INEEDMDLED_H__


#define INEEDMD_I2C_ADDRESS_LED_DRIVER  0x1b
#define INEEDMD_LED_OFF                 0x00
#define INEEDMD_LED1_RED_ENABLE         0x01
#define INEEDMD_LED1_BLUE_ENABLE        0x02
#define INEEDMD_LED1_GREEN_ENABLE       0x04
#define INEEDMD_LED2_RED_ENABLE         0x20
#define INEEDMD_LED2_BLUE_ENABLE        0x40
#define INEEDMD_LED2_GREEN_ENABLE       0x80

#define INEEDMD_LED1_RED_ILEVEL         0x00
#define INEEDMD_LED1_BLUE_ILEVEL        0x01
#define INEEDMD_LED1_GREEN_ILEVEL       0x02

#define INEEDMD_LED2_RED_ILEVEL         0x05
#define INEEDMD_LED2_BLUE_ILEVEL        0x06
#define INEEDMD_LED2_GREEN_ILEVEL       0x07

#define INEEDMD_LED1_RED_PWM            0x08
#define INEEDMD_LED1_BLUE_PWM           0x09
#define INEEDMD_LED1_GREEN_PWM          0x0a

#define INEEDMD_LED2_RED_PWM            0x0d
#define INEEDMD_LED2_BLUE_PWM           0x0e
#define INEEDMD_LED2_GREEN_PWM          0x0f

#define INEEDMD_LED_OUTPUT_CONTROL      0x10
#define INEEDMD_LED_STATUS_REGISTER     0x11

#define INEEDMD_LED_TOGGLE_ALL          0x00
#define INEEDMD_LED_PROGRAM_SINGLE      0x20
#define INEEDMD_LED_PROGRAM_SINGLE_ALL_TOGGLE 0x40
#define INEEDMD_LED_PROGRAM_ALL_ILEVEL  0x60
#define INEEDMD_LED_PROGRAM_ALL_PWM     0x80

//ints to use in switch stmt
#define LED_OFF                         0x00
#define PATIENT_ALERT                   0x01
#define ACQUIRE_UPLOAD_DATA             0x02
#define LEAD_LOOSE                      0x03
#define STORAGE_WARNING                 0x04
#define POWER_ON_BATGOOD                0x05
#define POWER_ON_BATLOW                 0x06
#define BAT_CHARGING                    0x07
#define BT_CONNECTED                    0x08
#define BT_ATTEMPTING                   0x09
#define BT_FAILED                       0x0A
#define USB_CONNECTED                   0x0B
#define USB_FAILED                      0x0C
#define DATA_TRANSFER                   0x0D
#define TRANSFER_DONE                   0x0F
#define LED_CHIRP_RED                   0x11
#define LED_CHIRP_ORANGE                0x12
#define LED_CHIRP_GREEN                 0x13
#define DFU_MODE                        0x14

extern void ineedmd_led_pattern(unsigned int);
extern void ineedmd_radio_reset(void);


#endif // __INEEDMDLED_H__
