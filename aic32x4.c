/*
 * aic32x4.c
 *
 *  Created on: 28 рту. 2015 у.
 *      Author: Blink
 */

/*
 * spi.c
 *
 *  Created on: 28 рту. 2015 у.
 *      Author: Blink
 */

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <cc430f5137.h>
#include "spi.h"
#include "port.h"


int8_t AIC32_configSens(uint8_t sensLevel) // 1  - 0db //  2 - 5db  // 3 - 10 db // 4 - 20db
{
	int8_t error = 0;

	uint8_t AIC32X4_Config_page1[][2] =
	{
		{0x00, 0x01},
		{0x3B, 0x00},
		{0x3C, 0x00},
	};

	uint8_t AIC32X4_Config_page1Check[][2] =
	{
			{0x00, 0x01},
			{0x3B, 0x00},
			{0x3C, 0x00},
	};

	switch(sensLevel & 0x0F)
	{
		case 1:
			AIC32X4_Config_page1[1][1] = 0x80;
			break;
		case 2:
			AIC32X4_Config_page1[1][1] = 0x11;
			break;
		case 3:
			AIC32X4_Config_page1[1][1] = 0x1D;
			break;
		case 4:
			AIC32X4_Config_page1[1][1] = 0x28;
			break;
		case 5:
			AIC32X4_Config_page1[1][1] = 0x3B;
			break;
		case 6:
			AIC32X4_Config_page1[1][1] = 0x47;
			break;
		case 7:
			AIC32X4_Config_page1[1][1] = 0x53;
			break;
		case 8:
			AIC32X4_Config_page1[1][1] = 0x5F;
			break;
		default:
			AIC32X4_Config_page1[1][1] = 0x80;
	}

	switch((sensLevel >> 4) & 0x0F)
	{
		case 1:
			AIC32X4_Config_page1[2][1] = 0x80; //0
			break;
		case 2:
			AIC32X4_Config_page1[2][1] = 0x11;  //12Db
			break;
		case 3:
			AIC32X4_Config_page1[2][1] = 0x1D;  //18db
			break;
		case 4:
			AIC32X4_Config_page1[2][1] = 0x28;  //24db
			break;
		case 5:
			AIC32X4_Config_page1[2][1] = 0x3B;  //30Db
			break;
		case 6:
			AIC32X4_Config_page1[2][1] = 0x47;  //36db
			break;
		case 7:
			AIC32X4_Config_page1[2][1] = 0x53;  //42Db
			break;
		case 8:
			AIC32X4_Config_page1[2][1] = 0x5F;  //48Db
			break;
		default:
			AIC32X4_Config_page1[2][1] = 0x80;
	}

	SPI_writeConfig((uint8_t *)AIC32X4_Config_page1, sizeof(AIC32X4_Config_page1));
	SPI_readConfig((uint8_t *)AIC32X4_Config_page1Check, sizeof(AIC32X4_Config_page1));

	uint8_t i = 0;
	for(i = 0; i < (sizeof(AIC32X4_Config_page1) / 2); i++)
	{
		if(AIC32X4_Config_page1[i][0] != AIC32X4_Config_page1Check[i][0])
			error++;
		if(AIC32X4_Config_page1[i][1] != AIC32X4_Config_page1Check[i][1])
			error++;
	}

	return error;
}

int8_t AIC32_configRegs(void)
{
	uint8_t i = 0;
	volatile uint8_t error = 0;
	uint8_t AIC32X4_Config_page0[][2] =
	{
		{0x00, 0x00},/////////choosing zero page
		{0x01, 0x01},/////////////////Self clearing software reset
		{0x12, 0x81},/////////NADC clock setting register, NADC value=1
		{0x13, 0x82},/////////MADC clock setting register, MADC value=2
		{0x14, 0x80},////////////ADC Oversampling AOSR = 128
		{0x1B, 0x00},///////////////audio interface - I2S
		{0x3D, 0x01}/////////////: Select ADC Singal Processing Block PRB_R1
	};

	uint8_t AIC32X4_Config_page0Check[][2] =
	{
		{0x00, 0x00},
		{0x01, 0x00},
		{0x12, 0x00},
		{0x13, 0x00},
		{0x14, 0x00},
		{0x1B, 0x00},
		{0x3D, 0x00}
	};

	uint8_t AIC32X4_Config_page1[][2] =
	{
		{0x00, 0x01},/////////first page
		{0x01, 0x08},1: Disabled weak connection of AVDD with DVDD
		{0x02, 0x01},1: AVDD LDO Powered up
		{0x0A, 0x40},1: Full Chip Common Mode is 0.75V
		{0x47, 0x31},: Analog inputs power up time is 3.1 ms
		{0x7B, 0x01},Reference will power up in 40ms when analog blocks are powered up
		{0x34, 0x20},10: IN2L is routed to Left MICPGA with 20k resistance
		{0x36, 0x03},: CM is routed to Left MICPGA via CM2L with 40k resistance
		{0x37, 0x20},: IN2R is routed to Right MICPGA with 20k resistance
		{0x39, 0x03}CM is routed to Right MICPGA via CM2R with 40k resistance
	};

	uint8_t AIC32X4_Config_page1Check[][2] =
	{
		{0x00, 0x01},
		{0x01, 0x00},
		{0x02, 0x00},
		{0x0A, 0x00},
		{0x47, 0x00},
		{0x7B, 0x00},
		{0x34, 0x00},
		{0x36, 0x00},
		{0x37, 0x00},
		{0x39, 0x00}
	};


	uint8_t AIC32X4_Config_page8[][2] =
	{
		{0x00, 0x08},
		{0x18, 0x7F},
		{0x19, 0xFF},
		{0x1A, 0x00},
		{0x1C, 0x80},
		{0x1D, 0x01},
		{0x1E, 0x00},
		{0x20, 0x7F},
		{0x21, 0xFC},
		{0x22, 0x00}
	};

	uint8_t AIC32X4_Config_page8Check[][2] =
	{
		{0x00, 0x08},
		{0x18, 0x00},
		{0x19, 0x00},
		{0x1A, 0x00},
		{0x1C, 0x00},
		{0x1D, 0x00},
		{0x1E, 0x00},
		{0x20, 0x00},
		{0x21, 0x00},
		{0x22, 0x00}
	};


	uint8_t AIC32X4_Config_page9[][2] =
	{
		{0x00, 0x09},
		{0x20, 0x7F},
		{0x21, 0xFF},
		{0x22, 0x00},
		{0x24, 0x80},
		{0x25, 0x01},
		{0x26, 0x00},
		{0x28, 0x7F},
		{0x29, 0xFC},
		{0x2A, 0x00},
	};

	uint8_t AIC32X4_Config_page9Check[][2] =
	{
		{0x00, 0x09},
		{0x20, 0x00},
		{0x21, 0x00},
		{0x22, 0x00},
		{0x24, 0x00},
		{0x25, 0x00},
		{0x26, 0x00},
		{0x28, 0x00},
		{0x29, 0x00},
		{0x2A, 0x00},
	};


	PORT_resetCodec();

	SPI_writeConfig((uint8_t *)AIC32X4_Config_page0, sizeof(AIC32X4_Config_page0));
	SPI_readConfig((uint8_t *)AIC32X4_Config_page0Check, sizeof(AIC32X4_Config_page0));
	for(i = 2; i < ((sizeof(AIC32X4_Config_page0) / 2) - 2); i++)
	{
		if(AIC32X4_Config_page0[i][0] != AIC32X4_Config_page0Check[i][0])
			error++;
		if(AIC32X4_Config_page0[i][1] != AIC32X4_Config_page0Check[i][1])
			error++;
	}

	SPI_writeConfig((uint8_t *)AIC32X4_Config_page1, sizeof(AIC32X4_Config_page1));
	SPI_readConfig((uint8_t *)AIC32X4_Config_page1Check, sizeof(AIC32X4_Config_page1));
	i = 0;
	for(i = 0; i < (sizeof(AIC32X4_Config_page1) / 2); i++)
	{
		if(AIC32X4_Config_page1[i][0] != AIC32X4_Config_page1Check[i][0])
			error++;
		if(AIC32X4_Config_page1[i][1] != AIC32X4_Config_page1Check[i][1])
			error++;
	}

	SPI_writeConfig((uint8_t *)AIC32X4_Config_page8, sizeof(AIC32X4_Config_page8));
	SPI_readConfig((uint8_t *)AIC32X4_Config_page8Check, sizeof(AIC32X4_Config_page8));

	i = 0;
	for(i = 0; i < (sizeof(AIC32X4_Config_page8) / 2); i++)
	{
		if(AIC32X4_Config_page8[i][0] != AIC32X4_Config_page8Check[i][0])
			error++;
		if(AIC32X4_Config_page8[i][1] != AIC32X4_Config_page8Check[i][1])
			error++;
	}


	SPI_writeConfig((uint8_t *)AIC32X4_Config_page9, sizeof(AIC32X4_Config_page9));
	SPI_readConfig((uint8_t *)AIC32X4_Config_page9Check, sizeof(AIC32X4_Config_page9));

	i = 0;
	for(i = 0; i < (sizeof(AIC32X4_Config_page9) / 2); i++)
	{
		if(AIC32X4_Config_page9[i][0] != AIC32X4_Config_page9Check[i][0])
			error++;
		if(AIC32X4_Config_page9[i][1] != AIC32X4_Config_page9Check[i][1])
			error++;
	}


	return error;
}

int8_t AIC32_powerUpADC(uint8_t state)
{
	uint8_t i = 0;
	volatile uint8_t error = 0;

	uint8_t AIC32X4_Config_pageOn[][2] =
	{
		{0x00, 0x00},
		{0x51, 0xC0},
		{0x52, 0x00}
	};
	uint8_t AIC32X4_Config_pageOff[][2] =
	{
		{0x00, 0x00},
		{0x51, 0xC0},
		{0x52, 0x84}
	};

	uint8_t AIC32X4_Config_check[][2] =
	{
		{0x00, 0x00},
		{0x51, 0x00},
		{0x52, 0x00}
	};

	if(state == 1)
	{
		SPI_writeConfig((uint8_t *)AIC32X4_Config_pageOn, sizeof(AIC32X4_Config_pageOn));
		SPI_readConfig((uint8_t *)AIC32X4_Config_check, sizeof(AIC32X4_Config_check));
		for(i = 0; i < (sizeof(AIC32X4_Config_pageOn) / 2); i++)
		{
			if(AIC32X4_Config_pageOn[i][0] != AIC32X4_Config_check[i][0])
				error++;
			if(AIC32X4_Config_pageOn[i][1] != AIC32X4_Config_check[i][1])
				error++;
		}
	}
	else if(state == 0)
	{
		SPI_writeConfig((uint8_t *)AIC32X4_Config_pageOff, sizeof(AIC32X4_Config_pageOff));
		SPI_readConfig((uint8_t *)AIC32X4_Config_check, sizeof(AIC32X4_Config_check));
		for(i = 0; i < (sizeof(AIC32X4_Config_pageOn) / 2); i++)
		{
			if(AIC32X4_Config_pageOff[i][0] != AIC32X4_Config_check[i][0])
				error++;
			if(AIC32X4_Config_pageOff[i][1] != AIC32X4_Config_check[i][1])
				error++;
		}
	}

	return error;
}
