#ifndef __INEEDMDLED_H__
#define __INEEDMDLED_H__

/******************************************************************************
* defines
******************************************************************************/
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

/******************************************************************************
* variables
******************************************************************************/

/******************************************************************************
* external variables
******************************************************************************/

/******************************************************************************
* enums
******************************************************************************/

typedef enum
{
  LED_OFF,
  POWER_ON_BATT_LOW_BLOCKING,
  POWER_ON_BATT_LOW,
  POWER_ON_BATT_LOW_ON,
  POWER_ON_BATT_LOW_OFF,
  POWER_ON_BATT_GOOD_BLOCKING,
  POWER_ON_BATT_GOOD,
  POWER_ON_BATT_GOOD_ON,
  POWER_ON_BATT_GOOD_OFF,
  BATT_CHARGING_BLOCKING,
  BATT_CHARGING,
  BATT_CHARGING_ON,
  BATT_CHARGING_OFF,
  BATT_CHARGING_LOW_BLOCKING,
  BATT_CHARGING_LOW,
  BATT_CHARGING_LOW_ON,
  BATT_CHARGING_LOW_OFF,
  LEAD_LOOSE_BLOCKING,
  LEAD_LOOSE,
  LEAD_LOOSE_ON,
  LEAD_LOOSE_OFF,
  LEAD_GOOD_UPLOADING_BLOCKING,
  LEAD_GOOD_UPLOADING,
  LEAD_GOOD_UPLOADING_ON,
  LEAD_GOOD_UPLOADING_OFF,
  DIG_FLATLINE_BLOCKING,
  DIG_FLATLINE,
  DIG_FLATLINE_ON,
  DIG_FLATLINE_OFF,
  BT_CONNECTED_BLOCKING,
  BT_CONNECTED,
  BT_CONNECTED_ON,
  BT_CONNECTED_OFF,
  BT_ATTEMPTING_BLOCKING,
  BT_ATTEMPTING,
  BT_ATTEMPTING_ON,
  BT_ATTEMPTING_OFF,
  BT_FAILED_BLOCKING,
  BT_FAILED,
  BT_FAILED_ON,
  BT_FAILED_OFF,
  USB_CONNECTED_BLOCKING,
  USB_CONNECTED,
  USB_CONNECTED_ON,
  USB_CONNECTED_OFF,
  USB_FAILED_BLOCKING,
  USB_FAILED,
  USB_FAILED_ON,
  USB_FAILED_OFF,
  DATA_TRANSFER_BLOCKING,
  DATA_TRANSFER,
  DATA_TRANSFER_ON,
  DATA_TRANSFER_OFF,
  TRANSFER_DONE_BLOCKING,
  TRANSFER_DONE,
  TRANSFER_DONE_ON,
  TRANSFER_DONE_OFF,
  STORAGE_WARNING_BLOCKING,
  STORAGE_WARNING,
  STORAGE_WARNING_ON,
  STORAGE_WARNING_OFF,
  ERASING_BLOCKING,
  ERASING,
  ERASING_ON,
  ERASING_OFF,
  ERASE_DONE_BLOCKING,
  ERASE_DONE,
  ERASE_DONE_ON,
  ERASE_DONE_OFF,
  DFU_MODE_BLOCKING,
  DFU_MODE,
  DFU_MODE_ON,
  DFU_MODE_OFF,
  MV_CAL_BLOCKING,
  MV_CAL,
  MV_CAL_ON,
  MV_CAL_OFF,
  TRI_WVFRM_BLOCKING,
  TRI_WVFRM,
  TRI_WVFRM_ON,
  TRI_WVFRM_OFF,
  REBOOT_BLOCKING,
  REBOOT,
  REBOOT_ON,
  REBOOT_OFF,
  HIBERNATE_BLOCKING,
  HIBERNATE,
  HIBERNATE_ON,
  HIBERNATE_OFF,
  LEADS_ON_BLOCKING,
  LEADS_ON,
  LEADS_ON_ON,
  LEADS_ON_OFF,
  MEMORY_TEST_BLOCKING,
  MEMORY_TEST,
  MEMORY_TEST_ON,
  MEMORY_TEST_OFF,
  COM_BUS_TEST_BLOCKING,
  COM_BUS_TEST,
  COM_BUS_TEST_ON,
  COM_BUS_TEST_OFF,
  CPU_CLOCK_TEST_BLOCKING,
  CPU_CLOCK_TEST,
  CPU_CLOCK_TEST_ON,
  CPU_CLOCK_TEST_OFF,
  FLASH_TEST_BLOCKING,
  FLASH_TEST,
  FLASH_TEST_ON,
  FLASH_TEST_OFF,
  TEST_PASS_BLOCKING,
  TEST_PASS,
  TEST_PASS_ON,
  TEST_PASS_OFF,
  POWER_UP_GOOD_BLOCKING,
  POWER_UP_GOOD,
  POWER_UP_GOOD_ON,
  POWER_UP_GOOD_OFF,
  ACTUAL_DFU_BLOCKING,
  ACTUAL_DFU,
  ACTUAL_DFU_ON,
  ACTUAL_DFU_OFF
}LED_MODE;

/******************************************************************************
* public functions
******************************************************************************/
void ineedmd_led_pattern(LED_MODE led_pattern);
extern void led_test(void);  //runs the gamut of LED patters


#endif // __INEEDMDLED_H__
