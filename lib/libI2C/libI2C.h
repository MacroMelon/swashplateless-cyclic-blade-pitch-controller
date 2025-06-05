//
// Created by thari on 25/08/2021.
//

#ifndef TESTING_SWASHPLATELESS_CONTROLLER_1_LIBI2C_H
#define TESTING_SWASHPLATELESS_CONTROLLER_1_LIBI2C_H

#include <memory.h>		//memcpy
#include "stm32f4xx.h"

#define motorCtrl1_addr 0x28
#define motorCtrl2_addr 0x29

volatile uint_fast8_t packet_byte_counter;
volatile uint32_t scary_ctrl_packet;

volatile uint8_t new_packet;		//gets set to 1 at every STOPF, can be used to identify unread packets

void I2C_init_client(uint8_t address);
void I2C1_EV_IRQHandler(void);

#endif //TESTING_SWASHPLATELESS_CONTROLLER_1_LIBI2C_H
