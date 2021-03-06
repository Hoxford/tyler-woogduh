
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "inc/hw_gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom_map.h"

#include "driverlib/ssi.h"
#include "driverlib/gpio.h"
#include "ineedmd_adc.h"
#include "board.h"

extern void wait_time (unsigned int tenths_of_seconds);


//TODO: add function prototypes to header file
//TODO: include directory for delay function
//TODO: set shorter timebases for delay functions

//********************************************************************************
//do a hardware reset by pulling reset low
//********************************************************************************
void ineedmd_adc_Hard_Reset()
{
	//toggle reset pin
	GPIOPinWrite(GPIO_PORTA_BASE, INEEDMD_PORTA_ADC_RESET_OUT_PIN, 0x00);
	//keep low for 2 clock cycles
	wait_time(1);
	GPIOPinWrite(GPIO_PORTA_BASE, INEEDMD_PORTA_ADC_RESET_OUT_PIN, INEEDMD_PORTA_ADC_RESET_OUT_PIN);
	//wait 18 device clock cycles - 9 usec
	wait_time(1);
}

//********************************************************************************
//stop continuous conversion mode
//
//if START pin is high or a START command received (if START pin is low)
//conversions are put into outbound Tx(MISO) FIFO
//when a RDATA command is received
//Clock data out using ineedmd_adc_Receive_Conversion
//********************************************************************************
void ineedmd_adc_Stop_Continuous_Conv()
{
	//set the CS low
	GPIOPinWrite(GPIO_PORTA_BASE, INEEDMD_PORTA_ADC_nCS_PIN, 0x00);
	//2us at fullmspeed
	MAP_SysCtlDelay(60);


	SSIDataPut(INEEDMD_ADC_SPI, ADS1198_SDATAC);
	while(SSIBusy(INEEDMD_ADC_SPI))
	{
	}
	//when done set the CS high
	GPIOPinWrite(GPIO_PORTA_BASE, INEEDMD_PORTA_ADC_nCS_PIN, INEEDMD_PORTA_ADC_nCS_PIN);

}

//********************************************************************************
//start continous conversion mode
//
//If start pin is high or START command is received
//conversions are automatically put into outbound Tx(MISO) FIFO when ready (DRDY goes low)
//use INEEDMD_ADC_Receive_Conversion so shift data through MISO
//********************************************************************************
void ineedmd_adc_Start_Continuous_Conv()
{
	//set the CS low
	GPIOPinWrite(GPIO_PORTA_BASE, INEEDMD_PORTA_ADC_nCS_PIN, 0x00);
	//2us at fullmspeed
	MAP_SysCtlDelay(60);

	SSIDataPut(INEEDMD_ADC_SPI, ADS1198_RDATAC);
	while(SSIBusy(INEEDMD_ADC_SPI))
	{
	}
	//when done set the CS high
	GPIOPinWrite(GPIO_PORTA_BASE, INEEDMD_PORTA_ADC_nCS_PIN, INEEDMD_PORTA_ADC_nCS_PIN);
}

//********************************************************************************
//ads1198 power on sequence
//
//toggles pwdn and reset, waits for device to start
//wakes with continuous conversions enabled
//call INEEDMD_ADC_Stop_Continuous_Conv or send SDATAC before attempting to read/write registers
//********************************************************************************
void ineedmd_adc_Power_On()
{

    //disables clocking into Rx FIFO buffer
    //SSIAdvModeSet(INEEDMD_ADC_SPI, SSI_ADV_MODE_WRITE);

    SSIEnable(INEEDMD_ADC_SPI);


	//power up and raise reset (active low pins)
	GPIOPinWrite(GPIO_PORTA_BASE, INEEDMD_PORTA_ADC_PWRDN_OUT_PIN, INEEDMD_PORTA_ADC_PWRDN_OUT_PIN);

    ineedmd_adc_Hard_Reset();
}

/* --------------------------------------------------------------------------------------------------
*ads1198 power down
*
* Sets the pwdn and resets low
*
* --------------------------------------------------------------------------------------------------
*/
void ineedmd_adc_Power_Off()
{

	//takes the ~rest line low putting the ADS1198 into reset
	GPIOPinWrite(GPIO_PORTA_BASE, INEEDMD_PORTA_ADC_RESET_OUT_PIN, 0x00);
	//takes the ~powerdn line low putting the ADS1198 into low power mode
	GPIOPinWrite(GPIO_PORTA_BASE, INEEDMD_PORTA_ADC_PWRDN_OUT_PIN, 0x00);

}


/********************************************************************************
//send single byte command to ADC
//
//ensure device is not in continuous conversion mode
//using INEEDMD_ADC_Stop_Continuous_Conv() prior to calling this function
//
// ********************************************************************************
*/
void ineedmd_adc_Send_Command(uint32_t command)
{
	//put in write only mode
	//SSIAdvModeSet(INEEDMD_ADC_SPI, SSI_ADV_MODE_WRITE);
	//send single byte command

	//set the CS low
	GPIOPinWrite(GPIO_PORTA_BASE, INEEDMD_PORTA_ADC_nCS_PIN, 0x00);
	//2us at fullmspeed
	MAP_SysCtlDelay(60);

	SSIDataPut(INEEDMD_ADC_SPI, command);
	while(SSIBusy(INEEDMD_ADC_SPI))
	{
	}
	//when done set the CS high
	GPIOPinWrite(GPIO_PORTA_BASE, INEEDMD_PORTA_ADC_nCS_PIN, INEEDMD_PORTA_ADC_nCS_PIN);
}

//********************************************************************************
//returns the contents of the register specified by "address"
//
//see ads1198.h for register definitions
//
//ensure device is not in continuous conversion mode
//using INEEDMD_ADC_Stop_Continuous_Conv() prior to calling this function
//********************************************************************************
uint32_t ineedmd_adc_Register_Read(uint32_t address)
{
	//Write only SPI mode is enabled
	//SSIAdvModeSet(INEEDMD_ADC_SPI, SSI_ADV_MODE_WRITE);
	//setup bytes for a register read command
	uint32_t txData[2];
	txData[0] = (RREG | address); 	//mask with read command
	txData[1] = 0x00;				//read n + 1 bytes (1)

	int i;

	//set the CS low
	GPIOPinWrite(GPIO_PORTA_BASE, INEEDMD_PORTA_ADC_nCS_PIN, 0x00);
	//2us at fullmspeed
	MAP_SysCtlDelay(60);


	//send SPI command to tell ADC to output register content
	for(i = 0; i < 2; i++)
	{
		SSIDataPut(INEEDMD_ADC_SPI, txData[i]);
		while(SSIBusy(INEEDMD_ADC_SPI))
		{
		}
	}

	//flush receive FIFO
	uint32_t flush;
	while(SSIDataGetNonBlocking(SSI0_BASE, &flush))
	{
	}

	//enable R/W mode - data can be clocked into rx FIFO
	//SSIAdvModeSet(INEEDMD_ADC_SPI, SSI_ADV_MODE_READ_WRITE);

	//send NOP to clock data in to FIFO
	SSIDataPut(INEEDMD_ADC_SPI, ADS1198_NOP);
	while(SSIBusy(INEEDMD_ADC_SPI))
			{
			}

	//read FIFO
	uint32_t rxData;
	SSIDataGet(INEEDMD_ADC_SPI, &rxData);
	rxData &= 0x00FF;

	//when done set the CS high
	GPIOPinWrite(GPIO_PORTA_BASE, INEEDMD_PORTA_ADC_nCS_PIN, INEEDMD_PORTA_ADC_nCS_PIN);
	//CS
	//put back into write only mode
	//SSIAdvModeSet(INEEDMD_ADC_SPI, SSI_ADV_MODE_WRITE);
	return rxData;
}

//********************************************************************************
//writes the value to register address in ads1198
//
//see ads1198.h for register definitions
//
//ensure device is not in continuous conversion mode
//using INEEDMD_ADC_Stop_Continuous_Conv() prior to calling this function
//********************************************************************************
void ineedmd_adc_Register_Write(uint32_t address, uint32_t value)
{
	//Write only SPI mode is enabled
	//SSIAdvModeSet(INEEDMD_ADC_SPI, SSI_ADV_MODE_WRITE);
	//setup bytes for a register write command
	uint32_t txData[3];
	txData[0] = (WREG | address); 	//mask with read command
	txData[1] = 0x00;				//write n + 1 bytes (1)
	txData[2] = ineedmd_adc_Register_Read(address);	//get current value of register
	txData[2] |= value;								//mask with new value
	int i;

	//set the CS low
	GPIOPinWrite(GPIO_PORTA_BASE, INEEDMD_PORTA_ADC_nCS_PIN, 0x00);
	//2us at fullmspeed
	MAP_SysCtlDelay(60);


	//send write command followed by data
	for(i = 0; i < 3; i++)
	{
		SSIDataPut(INEEDMD_ADC_SPI, txData[i]);
		while(SSIBusy(INEEDMD_ADC_SPI))
		{
		}
	}

	//when done set the CS high
	GPIOPinWrite(GPIO_PORTA_BASE, INEEDMD_PORTA_ADC_nCS_PIN, INEEDMD_PORTA_ADC_nCS_PIN);

}

//********************************************************************************
//start the internal voltage reference at 2.7V
//sets PD_REFBUF bit in CONFIG3 register
//
//ensure device is not in continuous conversion mode
//using INEEDMD_ADC_Stop_Continuous_Conv() prior to calling this function
//********************************************************************************
void ineedmd_adc_Start_Internal_Reference()
{
	uint32_t val = ineedmd_adc_Register_Read(CONFIG3);
	val |= PD_REFBUF;

	//set the CS low
	GPIOPinWrite(GPIO_PORTA_BASE, INEEDMD_PORTA_ADC_nCS_PIN, 0x00);
	//2us at fullmspeed
	MAP_SysCtlDelay(60);


	ineedmd_adc_Register_Write(CONFIG3, val);
	//wait for reference to start
	wait_time(15);

	//when done set the CS high
	GPIOPinWrite(GPIO_PORTA_BASE, INEEDMD_PORTA_ADC_nCS_PIN, INEEDMD_PORTA_ADC_nCS_PIN);

}
//********************************************************************************
//Stops the internal voltage reference at 2.7V
//sets PD_REFBUF bit in CONFIG3 register
//
//ensure device is not in continuous conversion mode
//using INEEDMD_ADC_Stop_Continuous_Conv() prior to calling this function
//********************************************************************************
void ineedmd_adc_Stop_Internal_Reference()
{
}

//********************************************************************************
//raise start pin to begin ADC conversions
//DRDY will indicate conversion result is ready
//********************************************************************************
void ineedmd_adc_Start_High()
{
	GPIOPinWrite(GPIO_PORTA_BASE, INEEDMD_PORTA_ADC_START_PIN, INEEDMD_PORTA_ADC_START_PIN);
}

//********************************************************************************
//lower start pin to stop ADC conversions
//********************************************************************************
void ineedmd_adc_Start_Low()
{
	GPIOPinWrite(GPIO_PORTA_BASE, INEEDMD_PORTA_ADC_START_PIN, 0x00);
	//wait for 2 device clocks - 1uSec
	wait_time(1);
}


//send RDATA command
////
//ensure device is not in continuous conversion mode
//using ineedmd_adc_Stop_Continuous_Conv() prior to calling this function
//
//start pin must be high
//********************************************************************************
void ineedmd_adc_Request_Data()
{


	ineedmd_adc_Send_Command(ADS1198_RDATA);

}

//********************************************************************************
//read a full conversion
//call after ineedmd_adc_DRDY_PIN goes high
//takes a pointer to an array of integers
//fills array with received data from ADC
//********************************************************************************
void ineedmd_adc_Receive_Data(char* data)
{
	//enable read to clock data into rx FIFO
	//SSIAdvModeSet(INEEDMD_ADC_SPI, SSI_ADV_MODE_READ_WRITE);
	//flush receive FIFO
	uint32_t flush;


	//when set the CS low
	GPIOPinWrite(GPIO_PORTA_BASE, INEEDMD_PORTA_ADC_nCS_PIN, 0x00);
	//2us at fullmspeed
	MAP_SysCtlDelay(60);

	while(SSIDataGetNonBlocking(SSI0_BASE, &flush))
	{
	}
	uint32_t dat;
	int i;
	//read 19 bytes of data
	for(i = 0; i < INEEDMD_ADC_DATA_SIZE; i++)
	{
		//send NOP to clock data into Rx FIFO
		SSIDataPut(INEEDMD_ADC_SPI, ADS1198_NOP);
		while(SSIBusy(INEEDMD_ADC_SPI))
		{
		}
		//read Rx FIFO
		SSIDataGet(INEEDMD_ADC_SPI, &dat);
		data[i] = dat;
	}
	while(SSIDataGetNonBlocking(SSI0_BASE, &flush))
		{
		}
	//SSIAdvModeSet(INEEDMD_ADC_SPI, SSI_ADV_MODE_WRITE);

	//when done set the CS high
	GPIOPinWrite(GPIO_PORTA_BASE, INEEDMD_PORTA_ADC_nCS_PIN, INEEDMD_PORTA_ADC_nCS_PIN);


}


//*****************************special register read functions*******************


//********************************************************************************
//read ID register of ADS1198
//
//ensure device is not in continuous conversion mode
//using INEEDMD_ADC_Stop_Continuous_Conv() prior to calling this function
//********************************************************************************
uint32_t ineedmd_adc_Get_ID()
{
	return ineedmd_adc_Register_Read(ADS1198_ID);


}

//********************************************************************************
//manually check if any of the leads are off
//lower 16 bits of data indicates which leads are off (if any)
//data format:
//[0x0000] [ 8P 7P 6P 5P 4P 3P 2P 1P ] [ 8N 7N 6N 5N 4N 3N 2N 1N ]
//returns 0 if all leads on
//
//ensure device is not in continuous conversion mode
//using INEEDMD_ADC_Stop_Continuous_Conv() prior to calling this function
//********************************************************************************
uint32_t ineedmd_adc_Check_Lead_Off()
{
	uint32_t leadStat = 0;

	leadStat |= ineedmd_adc_Register_Read(LOFF_STATN);
	leadStat |= (ineedmd_adc_Register_Read(LOFF_STATP) << 8);
	//mask lower 16bits
	leadStat &= 0xFFFF;

	return leadStat;
}

//********************************************************************************
//checks if Right Leg Drive was connected during power up
//returns 0 if RLD connected
//returns 1 if RLD disconnected
//
//ensure device is not in continuous conversion mode
//using INEEDMD_ADC_Stop_Continuous_Conv() prior to calling this function
//********************************************************************************
uint32_t ineedmd_adc_Check_RLD_Lead()
{

	uint32_t statRLD = ineedmd_adc_Register_Read(CONFIG3);
	//mask unwanted bits
	statRLD &= RLD_STAT;
	return statRLD;
}

//********************************************************************************
//checks if lead off status matches the update device condition
//returns 0 if no update required
//returns 1 if update required
//
//ensure device is not in continuous conversion mode
//using INEEDMD_ADC_Stop_Continuous_Conv() prior to calling this function
//********************************************************************************
int ineedmd_adc_Check_Update()
{
	uint32_t leadStat = ineedmd_adc_Check_Lead_Off();
	if(leadStat == UPDATE_DEVICE_CONDITION)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}


//********************************************************************************
//set the adc sampling rate
//0 = 8kSPS
//1 = 4k
//2 = 2k
//3 = 1k
//4 = 500
//5 = 250
//6 = 125
//
//ensure device is not in continuous conversion mode
//using INEEDMD_ADC_Stop_Continuous_Conv() prior to calling this function
//********************************************************************************
void ineedmd_adc_Set_Sample_Rate(uint32_t SPS)
{

	//if out of bounds, default to 500 SPS
	if(SPS > 6)
	{
		SPS = 4;
	}
	//mask SPS value with current CONFIG1 register to preserve other settings
	SPS |= ineedmd_adc_Register_Read(CONFIG1);
	//write new SPS value
	ineedmd_adc_Register_Write(CONFIG1, SPS);
}

//********************************************************************************
//Enable lead off detection
//
//Uses default configuration:
//	Current source detection
//	DC excititation signal
//	4nA current magnitude
//	95% Positive / 5% negative for comparator threshold
//********************************************************************************
void ineedmd_adc_Enable_Lead_Detect()
{
	ineedmd_adc_Register_Write(LOFF, FLEAD_OFF0 | FLEAD_OFF1);
	ineedmd_adc_Register_Write(CONFIG4, PD_LOFF_COMP);
	ineedmd_adc_Register_Write(LOFF_SENSP, 0xFFFF);
	ineedmd_adc_Register_Write(LOFF_SENSN, 0xFFFF);
}

//configure to use 3 lead setup
//turns off unused channels
//
//ensure device is not in continuous conversion mode
//using INEEDMD_ADC_Stop_Continuous_Conv() prior to calling this function
//********************************************************************************
void ineedmd_adc_3_Lead_Config()
{

}

//configure to use 12 lead setup
//turns on all channels
//
//ensure device is not in continuous conversion mode
//using INEEDMD_ADC_Stop_Continuous_Conv() prior to calling this function
//********************************************************************************
void ineedmd_adc_12_Lead_Config()
{

}

//*****************************************************************************
// name: iADC_setup
// description: sets up the A to D converter driver
// param description: none
// return value description: 1 if success
//*****************************************************************************
int iADC_setup(void)
{
  uint32_t regVal;

  ineedmd_adc_Start_Low();
  //power on ADC, disable continuous conversions
  ineedmd_adc_Power_On();
  //turn off continuous conversion for register read/writes
  ineedmd_adc_Stop_Continuous_Conv();

  ineedmd_adc_Enable_Lead_Detect();

  //increase comparator threshold for lead off detect
  regVal = ineedmd_adc_Register_Read(LOFF);
  ineedmd_adc_Register_Write(LOFF, (regVal | ILEAD_OFF0 | ILEAD_OFF1));

  //start conversions
  ineedmd_adc_Start_Internal_Reference();
  ineedmd_adc_Start_High();

  return 1;
}
