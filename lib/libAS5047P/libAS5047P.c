//
// Created by thari on 06/09/2021.
//

#include "libAS5047P.h"

void SPIinit_master()
{
	//The AS5047P SPI uses 16 bit mode=1 (CPOL=0, CPHA=1) and MSB first with parity upto 10Mhz
	/*
	Procedure
	1. 	Select the BR[2:0] bits to define the serial clock baud rate (see SPI_CR1 register).
	2. 	Select the CPOL and CPHA bits to define one of the four relationships between the
		data transfer and the serial clock (see Figure 240).
	3. 	Set the DFF bit to define 8- or 16-bit data frame format
	4. 	Configure the LSBFIRST bit in the SPI_CR1 register to define the frame format.
	5. 	If the NSS pin is required in input mode, in hardware mode, connect the NSS pin to a
		high-level signal during the complete byte transmit sequence. In NSS software mode,
		set the SSM and SSI bits in the SPI_CR1 register. If the NSS pin is required in output
		mode, the SSOE bit only should be set.
	6. 	The MSTR and SPE bits must be set (they remain set only if the NSS pin is connected
		to a high-level signal).
	In this configuration the MOSI pin is a data output and the MISO pin is a data input.
	*/
	//SSOE not required (yet)

	RCC->APB1ENR |= RCC_APB1ENR_SPI2EN;		//enable clocks to SPI peripheral

	//enable clocks to GPIO port and configure pins
	//To select the SPI alternate function AF5 must be selected.
	//For SPI2 these are pins PB13 [CLK], PB14 [MISO] and PB15 [MOSI] TODO- SS pin PB9
	//code to enable AF - SPI2
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;		//enable clock to GPIOB
	//PB13
	GPIOB->AFR[1] &= ~(GPIO_AFRH_AFSEL13);
	GPIOB->AFR[1] |= (5<< GPIO_AFRH_AFSEL13_Pos);	//enable SPI CLK to PB13
	GPIOB->MODER &= ~(GPIO_MODER_MODE13);
	GPIOB->MODER |= GPIO_MODER_MODE13_1;			//MODER5[1:0] = 10b, alternate function
	GPIOB->OSPEEDR &= ~(GPIO_OSPEEDER_OSPEEDR13);
	GPIOB->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR13_1;	// 10, 50Mhz speed, plenty
	//PB14
	GPIOB->AFR[1] &= ~(GPIO_AFRH_AFSEL14);
	GPIOB->AFR[1] |= (5<< GPIO_AFRH_AFSEL14_Pos);	//enable MISO to PB14
	GPIOB->MODER &= ~(GPIO_MODER_MODE14);
	GPIOB->MODER |= GPIO_MODER_MODE14_1;			//MODER5[1:0] = 10b, alternate function
	GPIOB->OSPEEDR &= ~(GPIO_OSPEEDER_OSPEEDR14);
	GPIOB->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR14_1;	// 10, 50Mhz speed, plenty
	//PB15
	GPIOB->AFR[1] &= ~(GPIO_AFRH_AFSEL15);
	GPIOB->AFR[1] |= (5<< GPIO_AFRH_AFSEL15_Pos);	//enable MOSI to PB15
	GPIOB->MODER &= ~(GPIO_MODER_MODE15);
	GPIOB->MODER |= GPIO_MODER_MODE15_1;			//MODER5[1:0] = 10b, alternate function
	GPIOB->OSPEEDR &= ~(GPIO_OSPEEDER_OSPEEDR15);
	GPIOB->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR15_1;	// 10b, 50Mhz speed, plenty
	//PB9 - GPIO Open drain 25MHz
	GPIOB->MODER &= ~(GPIO_MODER_MODE9);
	GPIOB->MODER |= GPIO_MODER_MODE9_0;				//MODER5[1:0] = 01 bin, GPIO output
	GPIOB->MODER &= ~(GPIO_OTYPER_OT9);
	GPIOB->MODER |= GPIO_OTYPER_OT9;				//OTYPER 1 for open drain
	GPIOB->OSPEEDR &= ~(GPIO_OSPEEDER_OSPEEDR9);
	GPIOB->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR9_1;		// 10, 25Mhz speed, plenty for SS

	GPIOB->ODR |= (1<<9);		//turn on pin, pulled up so slave gets selected (now oppsite hmmmm)

	SPI2->CR1 = 0;						//Disable and zero all
	//TODO- make this faster by making this all a single bit assignment
	SPI2->CR1 &= ~(SPI_CR1_SPE);		//disable SPI first
	SPI2->CR1 |= SPI_CR1_CPHA;			//CPHA to 1 (capture on falling edge)
	SPI2->CR1 &= ~(SPI_CR1_CPOL);		//CPOL to 0 (clock 0 when idle)
	SPI2->CR1 |= SPI_CR1_MSTR;			//MSTR bit to 1, master mode selected

	//baud rate controls
	SPI2->CR1 &= ~(SPI_CR1_BR);			//zero them all first
	SPI2->CR1 |= SPI_CR1_BR_1;			//Fpclk / 8, (42/8 = 5.25Mhz) TODO- make 011, goto 10.5Mhz if encoder can handle

	SPI2->CR1 &= ~(SPI_CR1_LSBFIRST);	//MSB first data transmission (LSBFIRST to 0)
	SPI2->CR1 |= SPI_CR1_SSM;			//SS controlled by GPIO, so enable software NSS TODO- cheCK
	SPI2->CR1 |= SPI_CR1_SSI;			//SS controlled by software TODO- SSI =1 and SSM = 1?
	SPI2->CR1 &= ~(SPI_CR1_RXONLY);		//enable full duplex (Rx and Tx)
	SPI2->CR1 |= SPI_CR1_DFF;			//16 bit data length

	//no CRC stuff, leave reset

	SPI2->CR1 &= ~(SPI_CR1_BIDIOE);		//we want full duplex
	SPI2->CR1 &= ~(SPI_CR1_BIDIMODE);	//we want full duplex

	SPI2->CR2 = 0;
	//SPI2->CR2 |= SPI_CR2_TXEIE;			//transmitter buffer empty interrupt enable
	SPI2->CR2 &= ~(SPI_CR2_SSOE);		//SS controlled by GPIO so disable master NSS output

	//NVIC_SetPriority (SPI2_IRQn, 1);            // enable interupts
	//NVIC_EnableIRQ (SPI2_IRQn);

	//config done! :D
	SPI2->CR1 |= SPI_CR1_SPE;		//enable the SPI peripheral
}

void SPIwrite(uint16_t val)
{
	//TODO- check if pull up worked on SS
	while(SPI2->SR & SPI_SR_BSY){}		//wait for SPI to finish transfer
	GPIOB->ODR &= ~(1<<9);		//turn off pin, pulled up so slave gets deselect TODO- why oppsite??
	while (!(SPI2->SR & SPI_SR_TXE)) {}
	SPI2->DR = val;
	while (!(SPI2->SR & SPI_SR_TXE)) {}
	while(SPI2->SR & SPI_SR_BSY){}		//wait for SPI to finish transfer
	GPIOB->ODR |= (1<<9);		//turn on pin, pulled up so slave gets selected
}

uint16_t SPIread()
{
	return 0;
}

void SPI2_IRQHandler(void){

}

//encoder control API-
//commands are:
// bit 15 - Parity
// bit 14 - Read from address or Write to address
// bits 13:0 - Address to read from / write to

//return / read vales are:
//bit 15- parity
//bit 14- frame error?
//bit 13:0 - data from address

/*
 * So, first send command, set CS high, then set low, then read incoming data
 */

uint16_t addParity(uint16_t data_packet)
{
	//todo- fix parity function
	return 0;
}

void initEncoder()
{
	SPIinit_master();
	//TODO- check for errors etc

	//send command to make sure our getangle function gets a value when run
	while(SPI2->SR & SPI_SR_BSY){}		//wait for SPI to finish transfers
	GPIOB->ODR &= ~(1<<9);		//turn off pin, pulled up so slave gets deselect TODO- why oppsite??
	//SPI2->DR = addParity(ANGLECOM_addr | 0x2000);	//read anglecom register with proper parity
	SPI2->DR = 0xFFFF;
	while (!(SPI2->SR & SPI_SR_TXE)) {}		//TODO- use && instead of &
	while(SPI2->SR & SPI_SR_BSY){}		//wait for SPI to finish transfers
	GPIOB->ODR |= (1<<9);		//turn on pin, pulled up so slave gets selected
}

//get the dynamic error compensated angle
//TODO- check for errors etc
//TDOO- set MOSI as GPIO and set to high
uint16_t getDECAngle()
{
	uint16_t angle;

	while(SPI2->SR & SPI_SR_BSY){}		//wait for SPI to finish transfers TODO- use && instead of &
	GPIOB->ODR &= ~(1<<9);		//turn off pin, pulled up so slave gets deselect TODO- why oppsite??
	while (!(SPI2->SR & SPI_SR_TXE)) {}
	//SPI2->DR = addParity(ANGLECOM_addr | 0x2000);	//read anglecom register with proper parity
	SPI2->DR = 0xFFFF;
	//while (!(SPI2->SR & SPI_SR_TXE)) {}
	while (!(SPI2->SR & SPI_SR_RXNE)) {}
	angle = (SPI2->DR & ANGLECOM_DAECANG);
	while(SPI2->SR & SPI_SR_BSY){}		//wait for SPI to finish transfers
	GPIOB->ODR |= (1<<9);		//turn on pin, pulled up so slave gets selected

	return  angle;
}
