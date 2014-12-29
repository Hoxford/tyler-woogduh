/******************************************************************************
*
* indeedmd_adc.h - adc driver include file
* Copyright (c) notice
*
******************************************************************************/
#ifndef __INEEDMD_ADC_H__
#define __INEEDMD_ADC_H__
/******************************************************************************
* includes ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/

/******************************************************************************
*public defines ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/
#ifndef INEEDMD_PORTA_ADC_SPI_CLK
	#define INEEDMD_PORTA_ADC_SPI_CLK 	0x02
#endif

#ifndef INEEDMD_PORTA_ADC_SPI_CS
	#define INEEDMD_PORTA_ADC_SPI_CS 	0x03
#endif

#ifndef INEEDMD_PORTA_ADC_SPI_MISO
	#define INEEDMD_PORTA_ADC_SPI_MISO 	0x04
#endif

#ifndef INEEDMD_PORTA_ADC_SPI_MOSI
	#define INEEDMD_PORTA_ADC_SPI_MOSI 	0x05
#endif

#ifdef INEEDMD_ADC_SPI
	#define INEEDMD_ADC_SPI			SSI0_BASE
#endif

#define INEEDMD_ADC_DATA_SIZE		19

//lead off update condition
#define UPDATE_DEVICE_CONDITION			0xA0FF
//
////SPI command list
////***************************************************************
//
//////todo: move to board.c
////#define ADS1198_WAKEUP		0x02
////#define ADS1198_NOP			0x02
////#define ADS1198_STANDBY		0x04
////#define ADS1198_RESET		0x06
////#define ADS1198_START		0x08
////#define ADS1198_STOP		0x0A
////#define ADS1198_RDATAC		0x10		//continuous conversion mode
////#define ADS1198_SDATAC		0x11		//stop continuous conversion
////#define ADS1198_RDATA		0x12		//read data by command
//
////register read/write commands
////				byte 1								byte 2
////  RREG | [starting address] 			[number of registers to read]
////***************************************************************
//#define RREG				0x20
//#define WREG				0x40
//
////REGISTER MAP
////***************************************************************
//#define ADS1198_ID_ADDRESS              0x00
//
////device ID (read only)
////#undef  ADS1198_ID
////#define ADS1198_ID			0x00		//different name than datasheet
//
////global settings
//#define CONFIG1				0x01
//#define CONFIG2				0x02
//  #define TEST_REGISTER CONFIG2
//#define CONFIG3				0x03
//#define LOFF				0x04
//
////channel specific settings
//#define CH1SET				0x05
//#define CH2SET				0x06
//#define CH3SET				0x07
//#define CH4SET				0x08
//#define CH5SET				0x09
//#define CH6SET				0x0A
//#define CH7SET				0x0B
//#define CH8SET				0x0C
//#define RLD_SENSP			0x0D
//#define RLD_SENSN			0x0E
//#define LOFF_SENSP			0x0F
//#define LOFF_SENSN			0x10
//#define LOFF_FLIP			0x11
////lead off status, read only
//#define LOFF_STATP			0x12
//#define LOFF_STATN			0x13
////GPIO and OTHER registers
//#define ADS1198_GPIO		0x14		//different name than datasheet
//#define ADS1198_PACE		0x15		//different name than datasheet
//#define CONFIG4				0x17
//#define WCT1				0x18
//#define WCT2				0x19
//
////Register specific bit masks
////***************************************************************
//
////CONFIG1
//#define DAISY_EN			0x40
//#define CLK_EN				0x20
//#define DR2					0x04
//#define DR1					0x02
//#define DR0					0x01
//
////CONFIG2
//#define INT_TEST			0x10
//#define TEST_AMP			0x04
//#define	TEST_FREQ1			0x02
//#define TEST_FREQ0			0x01
//
////CONFIG3
//#define PD_REFBUF			0x80
//#define VREF_4V				0x20
//#define RLD_MEAS			0x10
//#define RLDREF_INT			0x08
//#define PD_RLD				0x04
//#define RLD_LOFF_SENS		0x02
//#define RLD_STAT			0x01
//
////LOFF
//#define COMP_TH2			0x80
//#define COMP_TH1			0x40
//#define COMP_TH0			0x20
//#define VLEAD_OFF_EN		0x10
//#define ILEAD_OFF1			0x08
//#define ILEAD_OFF0			0x04
//#define FLEAD_OFF1			0x02
//#define FLEAD_OFF0			0x01
//
////CHnSET
//#define PD1					0x80
//#define GAIN2				0x40
//#define GAIN1				0x20
//#define GAIN0				0x10
//#define MUX2				0x04
//#define MUX1				0x02
//#define MUX0				0x01
//
//#define GAIN_1_X GAIN0
//#define GAIN_2_X GAIN1
//#define GAIN_3_X (GAIN0 | GAIN1)
//#define GAIN_4_X GAIN2
//#define GAIN_6_X 0x00
//#define GAIN_8_X (GAIN2 | GAIN0)
//#define GAIN_12_X (GAIN2 | GAIN1)
//
//#define MUX_TEST (MUX0 | MUX2)
//#define MUX_ELECTRODE 0x00
//#define MUX_SHORT (MUX0)
//#define MUX_TEMPERATURE (MUX0 | MUX2)
//#define MUX_ELECTRODES Ox00
//
//
//
////RLD_SENSP
//#define RLD8P				0x80
//#define RLD7P				0x40
//#define RLD6P				0x20
//#define RLD5P				0x10
//#define RLD4P				0x08
//#define RLD3P				0x04
//#define RLD2P				0x02
//#define RLD1P				0x01
///* 0.3 Board Pin Definition */
//  #define EKG_V6 RLD8P
//  #define EKG_V5 RLD7P
//  #define EKG_V4 RLD6P
//  #define EKG_V3 RLD5P
//  #define EKG_LA RLD4P
//  #define EKG_LL RLD3P
//  #define EKG_V2 RLD2P
//  #define EKG_V1 RLD1P
///* 0.5 Board Pin Definition
//  #define EKG_V1 RLD8P
//  #define EKG_V2 RLD7P
//  #define EKG_V3 RLD6P
//  #define EKG_V4 RLD5P
//  #define EKG_LL RLD4P
//  #define EKG_LA RLD3P
//  #define EKG_V5 RLD2P
//  #define EKG_V6 RLD1P
//*/
//
////RLD_SENSN
//#define RLD_NO_LEAD 0x00
//
//#define RLD8N				0x80
//#define RLD7N				0x40
//#define RLD6N				0x20
//#define RLD5N				0x10
//#define RLD4N				0x08
//#define RLD3N				0x04
//#define RLD2N				0x02
//#define RLD1N				0x01
//
///* 0.3 Board Pin Definition */
//  #define EKG_WTC1 RLD8N
//  #define EKG_WTC2 RLD7N
//  #define EKG_WTC3 RLD6N
//  #define EKG_WTC4 RLD5N
//  #define EKG_RA1 RLD4N
//  #define EKG_RA2 RLD3N
//  #define EKG_WTC5 RLD2N
//  #define EKG_WTC6 RLD1N
///* 0.5 Board Pin Definition
//  #define EKG_WTC1 RLD8N
//  #define EKG_WTC2 RLD7N
//  #define EKG_WTC3 RLD6N
//  #define EKG_WTC4 RLD5N
//  #define EKG_RA1 RLD4N
//  #define EKG_RA2 RLD3N
//  #define EKG_WTC5 RLD2N
//  #define EKG_WTC6 RLD1N
//*/
//
//
////LOFF_SENSP
//#define LOFF8P				0x80
//#define LOFF7P				0x40
//#define LOFF6P				0x20
//#define LOFF5P				0x10
//#define LOFF4P				0x08
//#define LOFF3P				0x04
//#define LOFF2P				0x02
//#define LOFF1P				0x01
//
////LOFF_SENSN
//#define LOFF8N				0x80
//#define LOFF7N				0x40
//#define LOFF6N				0x20
//#define LOFF5N				0x10
//#define LOFF4N				0x08
//#define LOFF3N				0x04
//#define LOFF2N				0x02
//#define LOFF1N				0x01
//
////ADS1198_GPIO
//#define ADS1198_GPIOD4				0x80
//#define ADS1198_GPIOD3				0x40
//#define ADS1198_GPIOD2				0x20
//#define ADS1198_GPIOD1				0x10
//#define ADS1198_GPIOC4				0x08
//#define ADS1198_GPIOC3				0x04
//#define ADS1198_GPIOC2				0x02
//#define ADS1198_GPIOC1				0x01
//
////PACE
//#define PACEE1				0x10
//#define PACEE0				0x08
//#define PACEO1				0x04
//#define PACEO0				0x02
//#define PD_PACE				0x01
//
////CONFIG4
//#define SINGLE_SHOT			0x08
//#define WCT_TO_RLD			0x04
//#define PD_LOFF_COMP		0x02
//
////WCT1
//#define aVF_CH6				0x80
//#define aVL_CH5				0x40
//#define aVR_CH7				0x20
//#define avR_CH4				0x10
//#define PD_WCTA				0x08
//#define WCTA2				0x04
//#define WCTA1				0x02
//#define WCTA0				0x01
//
////WCT2
//#define PD_WCTC				0x80
//#define PD_WCTB				0x40
//#define WCTB2				0x20
//#define WCTB1				0x10
//#define WCTB0				0x08
//#define WCTC2				0x04
//#define WCTC1				0x02
//#define WCTC0				0x01
//
//#define POWER_UP_WTCA PD_WCTA
//#define POWER_UP_WTCB PD_WCTB
//#define POWER_UP_WTCC PD_WCTC
//#define POWER_DOWN_WTC 0x00


//setting up the numbers for the Test mode

#define TEST_MODE_NORMAL_FREQUENCY 0x00
#define TEST_MODE_FAST_FREQUENCY   0x01
#define TEST_MODE_NOMINAL_VOLTAGE  0x00
#define TEST_MODE_HIGH_VOLTAGE     0x04
#define TEST_MODE_ENABLE           0x30
#define TEST_MODE_DISABLE          0x20

//Part ID
//#define ADS1198_ID              0xB6

/******************************************************************************
*public variables /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/

/******************************************************************************
* external variables //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/

/******************************************************************************
*public enums /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/

typedef enum ADC_REQUEST_ID
{
   ADC_REQUEST_NONE,
   ADC_REQUEST_SETUP,                              //sets up the ADC interface
   ADC_REQUEST_ADS_SET_CONVERSION_MODE_CONTINUOUS, //sets the ADS conversion mode to continuous on start, requires a callback function to notify requester when new data is available to read
   ADC_REQUEST_ADS_SET_CONVERSION_MODE_SINGLE,     //sets the ADS conversion mode to single on start
   ADC_REQUEST_ADS_SET_SAMPLE_RATE,                //sets the ADS sample rate according to the sample rate setting in the request
   ADC_REQUEST_ADS_SET_VREF_SOURCE,                //sets the ADS voltage reference source according the the voltage reference setting in the request
   ADC_REQUEST_ADS_START,                          //start ADC conversions from the ADS module
   ADC_REQUEST_ADS_STOP,                           //stop ADC conversions from the ADS module
   ADC_REQUEST_ADS_POWER_ON,                       //powers on the ADS
   ADC_REQUEST_ADS_POWER_OFF,                      //powers off the ADS
   ADC_REQUEST_ADS_PERMANENT_POWER_OFF,            //PERMANENTLY power off the ADS - !!!WARNING!!! this request will not allow any task or process to turn the ADS back on unless the system is reset
   ADC_REQUEST_ADS_RESET,                          //resets the ADS to default settings
   ADC_REQUEST_ADS_SINGLE_READ,                    //reads data from the ADS
   ADC_REQUEST_ADS_LEAD_DETECT,                    //Sets the ADS to detect EKG leads
   ADC_REQUEST_ADS_INPUT_ECG,                      //mux ADS inputs to exernal ECG leads (default)
   ADC_REQUEST_ADS_INPUT_SQUARE,                   //mux ADS inputs to internal test square wave
   ADC_REQUEST_DEVICE_TEMPERATURE,                 //reads the device temperature and sets the value in the device temperature variable pointer
   ADC_REQUEST_DEVICE_SUPPLY_VOLTAGE,              //reads the device voltage and sets the value in the device supply voltage variable pointer
   ADC_REQUEST_CHANGE_GAIN,
   ADC_REQUEST_CHANGE_REFERENCE,
   ADC_REQUEST_LIMIT
}ADC_REQUEST_ID;

typedef enum ADC_VREF_ADS_SOURCE
{
  VREF_ADC_DEFAULT,
  VREF_INTERNAL,
  VREF_EXTERNAL,
  VREF_OFF,
  VREF_LIMIT
}ADC_VREF_ADS_SOURCE;

typedef enum ADC_SAMPLE_ADS_RATE
{
  ADS_SAMPLE_RATE_NONE,
  ADS_SAMPLE_RATE_8KSPS,
  ADS_SAMPLE_RATE_4KSPS,
  ADS_SAMPLE_RATE_2KSPS,
  ADS_SAMPLE_RATE_1KSPS,
  ADS_SAMPLE_RATE_500SPS,
  ADS_SAMPLE_RATE_250SPS,
  ADS_SAMPLE_RATE_125SPS,
  ADS_SAMPLE_RATE_LIMIT
}ADC_SAMPLE_ADS_RATE;

typedef enum ADC_CHANNEL_GAIN
{
  ADS_GAIN_1 = 0x10,
  ADS_GAIN_2 = 0x20,
  ADS_GAIN_3 = 0x30,
  ADS_GAIN_4 = 0x40,
  ADS_GAIN_6 = 0x00,
  ADS_GAIN_8 = 0x50,
  ADS_GAIN_12 = 0x60
}ADC_CHANNEL_GAIN;


/******************************************************************************
*public structures ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/

typedef struct ADC_REQUEST
{
  ADC_REQUEST_ID eRequest_ID;
  ADC_SAMPLE_ADS_RATE eADS_sample_rate;
  uint8_t * uiADS_data;
  int8_t * uiDevice_temperature;
  uint8_t * uiDevice_supp_voltage;
  uint16_t * uiLeadStatus;
  ADC_CHANNEL_GAIN * eADC_gain;
  ADC_VREF_ADS_SOURCE * eVREF_source;
  void (*ADC_continuous_conversion_callback) (uint8_t * uiData);
}ADC_REQUEST;

/******************************************************************************
* external functions //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/

/******************************************************************************
* public functions ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
******************************************************************************/
//void ineedmd_adc_Stop_Continuous_Conv();
//void ineedmd_adc_Start_Continuous_Conv();
//
//void ineedmd_adc_Send_Command(uint32_t command);
//uint32_t ineedmd_adc_Register_Read(uint32_t address);
//void ineedmd_adc_Register_Write(uint32_t address, uint32_t value);
//void ineedmd_adc_Stop_Internal_Reference();
//void ineedmd_adc_Start_High();
//
//void ineedmd_adc_Request_Data();
//void ineedmd_adc_Receive_Data(char* data);
//
//int ineedmd_adc_gain_set(int gain_position);
//int ineedmd_adc_mux_set(int mux_position);
//
//ERROR_CODE eIneedmd_adc_Power_status(void);
//
//uint32_t ineedmd_adc_Get_ID();
//void     switch_on_adc_for_lead_detection(void);
//uint32_t ineedmd_adc_Check_Lead_Off();
//uint32_t ineedmd_adc_Check_RLD_Lead();
//int ineedmd_adc_Check_Update();
//
//void ineedmd_adc_Set_Sample_Rate(uint32_t SPS);
//void ineedmd_adc_Enable_Lead_Detect();

ERROR_CODE eADC_ADS_Power_status(void);  //returns
ERROR_CODE eADC_Request_param_init(ADC_REQUEST * tParam);
ERROR_CODE eADC_Request(ADC_REQUEST * tRequest);
ERROR_CODE eADC_Read_ADS_Single(uint8_t * pData);
#endif //#define __INEEDMD_ADC_H__
