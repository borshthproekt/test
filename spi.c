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


void SPI_init(void)
{

	UCA0CTL1 |= UCSWRST;                      // **Put state machine in reset**
	UCA0CTL0 |= UCMST + UCSYNC + UCMSB ;    // 3-pin, 8-bit SPI master
											// Clock polarity high, MSB
	UCA0CTL1 |= UCSSEL_2;                     // SMCLK
	UCA0BR0 = 0x02;                           // /2
	UCA0BR1 = 0;                              //
	UCA0MCTL = 0;                             // No modulation
	UCA0CTL1 &= ~UCSWRST;                     // **Initialize USCI state machine**
	// Lock port mapping registers
}


void SPI_writeConfig(uint8_t *data, uint8_t length)
{
	uint8_t i = 0;
	volatile uint8_t dataToWrite = 0;
	uint8_t (*pData)[2] = (uint8_t (*)[2]) data;
	volatile uint8_t dummyRead = 0;

	for(i = 0 ; i < length / 2; i++)
	{
		P1OUT &=  ~SPI_CS_CODEC;
		while (!(UCA0IFG & UCTXIFG));
		{
			dataToWrite = (*pData)[0];
			dataToWrite = dataToWrite << 1;
			UCA0TXBUF = dataToWrite;
		}
		while  (!(UCA0IFG &   UCRXIFG));    //  USCI_A0 RX  Received?
			dummyRead    =   UCA0RXBUF;  //  Store   received    data
		while (!(UCA0IFG&UCTXIFG));
		{
			dataToWrite = (*pData)[1];
			UCA0TXBUF = dataToWrite;
		}
		while  (!(UCA0IFG &   UCRXIFG));    //  USCI_A0 RX  Received?
			dummyRead  =  UCA0RXBUF;  //  Store   received    data

		P1OUT |= SPI_CS_CODEC;
		pData++;
	}
}

void SPI_readConfig(uint8_t *data, uint8_t length)
{
	uint8_t i = 0;
	volatile uint8_t dataToWrite = 0;
	volatile uint8_t dataToRead = 0;
	volatile uint8_t dummyRead = 0;
	uint8_t (*pData)[2] = (uint8_t (*)[2]) data;

	P1OUT &=  ~SPI_CS_CODEC;
	while (!(UCA0IFG & UCTXIFG));
	{
		dataToWrite = (*pData)[0];
		dataToWrite = dataToWrite << 1;
		UCA0TXBUF = dataToWrite;
	}
	while  (!(UCA0IFG &   UCRXIFG));    //  USCI_A0 RX  Received?
		dummyRead    =   UCA0RXBUF;  //  Store   received    data
	while (!(UCA0IFG&UCTXIFG));
	{
		dataToWrite = (*pData)[1];
		UCA0TXBUF = dataToWrite;
	}
	while  (!(UCA0IFG &   UCRXIFG));    //  USCI_A0 RX  Received?
		dummyRead  =  UCA0RXBUF;  //  Store   received    data

	P1OUT |= SPI_CS_CODEC;
	pData++;

	for(i = 0 ; i < ((length / 2) - 1); i++)
	{
		P1OUT &=  ~SPI_CS_CODEC;
		while (!(UCA0IFG & UCTXIFG));
		{
			dataToWrite = (*pData)[0];
			dataToWrite = dataToWrite << 1;
			dataToWrite |= SPI_READ;
			UCA0TXBUF = dataToWrite;
		}
		while  (!(UCA0IFG &   UCRXIFG));    //  USCI_A0 RX  Received?
			dummyRead = UCA0RXBUF;  //  Store   received    data
		while (!(UCA0IFG & UCTXIFG));
			UCA0TXBUF = 0x00;
		while  (!(UCA0IFG &   UCRXIFG));    //  USCI_A0 RX  Received?
			dataToRead = UCA0RXBUF;  //  Store   received    data

		P1OUT |= SPI_CS_CODEC;

		(*pData)[1] = dataToRead;
		pData++;
	}
}
