/******************************************************************************
*
* ineedmd_led.h - LED driver include file
* Copyright (c) notice
*
******************************************************************************/
#ifndef __INEEDMD_LED_H__
#define __INEEDMD_LED_H__
/******************************************************************************
* includes ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/

/******************************************************************************
*public defines ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/
#define INEEDMD_I2C_ADDRESS_LED_DRIVER  0x32

#define INEEDMD_LED_ENABLE_ADD          0x00
#define INEEDMD_LED_ENABLE              0x40
#define INEEDMD_LED_DISABLE              0x00

#define INEEDMD_LED_CONFIG_ADD          0x36
#define INEEDMD_LED_CONFIG_PWM_POWERSAVE       0x80
#define INEEDMD_LED_CONFIG_AUTO_INCR           0x40
#define INEEDMD_LED_CONFIG_POWER_SAVE_EN       0x20
#define INEEDMD_LED_CONFIG_CP_GAIN1            0x10
#define INEEDMD_LED_CONFIG_CP_GAIN0            0x08
#define INEEDMD_LED_CONFIG_FADE_TO_OFF         0x04
#define INEEDMD_LED_CONFIG_INT_CLK_EN          0x01
#define INEEDMD_LED_CONFIG (INEEDMD_LED_CONFIG_PWM_POWERSAVE | INEEDMD_LED_CONFIG_POWER_SAVE_EN | INEEDMD_LED_CONFIG_CP_GAIN1 | INEEDMD_LED_CONFIG_CP_GAIN0 | INEEDMD_LED_CONFIG_INT_CLK_EN)

#define INEEDMD_LED_PWR_ADD            0x05
#define INEEDMD_LED_PWR_1_6            0x02
#define INEEDMD_LED_PWR_7_9            0x01
#define INEEDMD_LED_PWR (INEEDMD_LED_PWR_1_6 | INEEDMD_LED_PWR_7_9)


#define INEEDMD_HEART_RED_PWM_ADD            0x19
#define INEEDMD_HEART_GREEN_PWM_ADD          0x17
#define INEEDMD_HEART_BLUE_PWM_ADD           0x16

#define INEEDMD_COMMS_RED_PWM_ADD            0x1D
#define INEEDMD_COMMS_GREEN_PWM_ADD          0x1C
#define INEEDMD_COMMS_BLUE_PWM_ADD           0x1E

#define INEEDMD_POWER_RED_PWM_ADD             0x1A
#define INEEDMD_POWER_GREEN_PWM_ADD           0x18
#define INEEDMD_POWER_BLUE_PWM_ADD            0x1B

#define LED_1_SEC_DELAY                      330
#define LED_5_SEC_DELAY                      1650
#define LED_2_SEC_DELAY                      660
#define LED_0_5_SEC_DELAY                    165

#define COLOR_RED_LED_RED                    0x40
#define COLOR_RED_LED_GREEN                  0x00
#define COLOR_RED_LED_BLUE                   0x00

#define COLOR_GREEN_LED_RED                  0x00
#define COLOR_GREEN_LED_GREEN                0x40
#define COLOR_GREEN_LED_BLUE                 0x00

#define COLOR_BLUE_LED_RED                   0x00
#define COLOR_BLUE_LED_GREEN                 0x00
#define COLOR_BLUE_LED_BLUE                  0x40

#define COLOR_ORANGE_LED_RED                 0x40
#define COLOR_ORANGE_LED_GREEN               0x10
#define COLOR_ORANGE_LED_BLUE                0x00

#define COLOR_PURPLE_LED_RED                 0x30
#define COLOR_PURPLE_LED_GREEN               0x00
#define COLOR_PURPLE_LED_BLUE                0x40

#define COLOR_PINK_LED_RED                   0x40
#define COLOR_PINK_LED_GREEN                 0x10
#define COLOR_PINK_LED_BLUE                  0x10

/******************************************************************************
*public variables /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/

/******************************************************************************
* external variables //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/

/******************************************************************************
*public enums /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/

//LED color elements
typedef enum NO_UI_ELEMENT
{
        UI_ALL_OFF,
        HEART_LED_OFF,
        HEART_LED_RED,
        HEART_LED_GREEN,
        HEART_LED_BLUE,
        HEART_LED_ORANGE,
        HEART_LED_PURPLE,
        HEART_LED_PINK,
        COMMS_LED_OFF,
        COMMS_LED_RED,
        COMMS_LED_GREEN,
        COMMS_LED_BLUE,
        COMMS_LED_ORANGE,
        COMMS_LED_PURPLE,
        COMMS_LED_PINK,
        POWER_LED_OFF,
        POWER_LED_RED,
        POWER_LED_GREEN,
        POWER_LED_BLUE,
        POWER_LED_ORANGE,
        POWER_LED_PURPLE,
        POWER_LED_PINK,
        ALERT_SOUND_ON,
        ALERT_SOUND_OFF
} NO_UI_ELEMENT;

#define POWER_LED_YELLOW POWER_LED_ORANGE

/******************************************************************************
*public structures ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/
typedef struct tStruct_UI_State
{
  bool heart_led_on;
  bool comms_led_on;
  bool power_led_on;
  bool sounder_on;
  NO_UI_ELEMENT heart_led_last_state;
  NO_UI_ELEMENT comms_led_last_state;
  NO_UI_ELEMENT power_led_last_state;
  NO_UI_ELEMENT sounder_last_state;
}tStruct_UI_State;

/******************************************************************************
* external functions //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/

/******************************************************************************
* public functions ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/
ERROR_CODE eIneedmd_LED_driver_setup(void);
ERROR_CODE eIneedmd_LED_driver_enable(void);
ERROR_CODE eIneedmd_LED_driver_standby(bool bPowerDown);
ERROR_CODE eIneedmd_LED_pattern(NO_UI_ELEMENT led_pattern);
extern void led_test(void);  //runs the gamut of LED patters


#endif // __INEEDMDLED_H__
