//
// Created by thari on 06/09/2021.
//

#ifndef SWASHPLATELESS_CYCLIC_BLADE_PITCH_CONTROLLER_LIBAS5047P_H
#define SWASHPLATELESS_CYCLIC_BLADE_PITCH_CONTROLLER_LIBAS5047P_H

#include "stm32f4xx.h"

//as given in pg 15 of datasheet:

#define NOP_addr		0x0000			//no operation register

#define ERRFL_addr 		0x0001			//error register
#define ERRFL_PARERR_Pos	0x02		//parity error
#define ERRFL_PARERR	0x04			//bitmask

#define PROG_addr 		0x0003			//programming register
//TODO- Programming register bitmasks

#define DIAAGC_addr 	0x3FFC			//Diagnostic and AGC register, default = 0x0180
//TODO- DIAAGC register bitmasks

#define MAG_addr 		0x3FFD			//CORDIC magnitude (magnitude of feild)
//TODO- MAG register bitmasks

#define ANGLEUNC_addr	0x3FFE			//Measured angle without dynamic angle error compensation
//TODO- ANGLEUNC register bitmasks

#define ANGLECOM_addr	0x3FFF			//Measured angle with dynamic angle error compensation
#define ANGLECOM_DAECANG_Pos	0x00	//Position of measured angle with error comp
#define ANGLECOM_DAECANG		0x3FFF	//bitmask of lower 13 bits (actual angle value)

void SPIinit_master();
void SPIwrite(uint16_t val);
uint16_t SPIread();
void SPI2_IRQHandler(void);

uint16_t addParity(uint16_t data_packet);

void initEncoder();
uint16_t getDECAngle();

#endif //SWASHPLATELESS_CYCLIC_BLADE_PITCH_CONTROLLER_LIBAS5047P_H
