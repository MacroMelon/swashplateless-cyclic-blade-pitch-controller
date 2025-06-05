#include <math.h>
#include "stm32f4xx.h"
#include "libCLCK.h"
#include "libGPIO.h"
#include "libAS5047P.h"
#include "libI2C.h"

//#define MOT1
#define MOT2

#ifdef MOT1
#define OFFSET 9016		//angle at zero:
#define I2CADDR 0x28
#endif

#ifdef MOT2
#define OFFSET 15622	//angle at zero:
#define I2CADDR 0x29
#endif

//uint32_t tick_tester;	//to check if we are skipping ticks (too slow)

//uint16_t esc_value;

uint16_t calculated_esc_value;

uint16_t motor_angle;

uint16_t angle, scaler, throttle_val = 0;
uint32_t safe_packet;

void parsePacket(uint32_t packet)
{
	//structure:
	/*
	 *  [ header(2) | angle(9) | scaler(10) | throttle(11) ]
	 */
	angle = ((packet & (0x3FE00000)) >> 21);
	scaler = ((packet & (0x1FF800)) >> 11);
	throttle_val = (packet & (0x000007FF));
}

uint16_t applyOffset(uint16_t reading, uint16_t offset){
	uint16_t final_value = reading + offset;
	if (final_value > 16383){
		final_value -= 16384;
	}
	return final_value;
}

int initialise()
{
	//__disable_irq();
	set_system_clock_to_168Mhz();
	__enable_irq();

	GPIOinit();
	GPIOC->ODR |= (1<<1);

	packet_byte_counter = -1;		//not synced yet
	I2C_init_client(I2CADDR);

	throttle_val = 0; 	//arm esc disarmed
	transferDone = 1;	//set flag
	//TODO-DShot DMA first
	Dshot_enableConfigureTimer();

	initEncoder();		//configure SPI2 for AS5047P, send first read request

	//TODO- proper arming mechanism

	GPIOC->ODR &= ~(1<<1);
	WaitForTick();
	return 0;
}

void loop()
{
	//WE ARE READY!!!!
	//esc_value = 700;
	//tick_tester = msTicks;

	//TODO- I2C DMA
	if (new_packet){		//transmission finished, whole packet good
		safe_packet = scary_ctrl_packet;	//single instruction copy, basically double buffering
		parsePacket(safe_packet);	//parse the packet and update throttle, angle, scaler
		new_packet = 0;
		GPIOC->ODR ^= (1<<1);		//toggle ye olde debug LED
	}

	//get the motor angle from the encoder
	// set it to 0 to 360
	//motor_angle = (applyOffset(getDECAngle(),OFFSET)) / 45.5111111111;
	motor_angle = (getDECAngle() + OFFSET) / 45.5111111111;

	//TODO- looks like sin is too slow, use fast sin
	//ah yes, the creme de la creme of the system
	//TODO- Use cos
	calculated_esc_value = throttle_val + (sin((angle + motor_angle) * (3.14159265359/180)) * scaler);	//TODO- sin or cos??

	if (((calculated_esc_value >= 48) && (calculated_esc_value <= 2047)) || (calculated_esc_value == 0)){
		esc_value = calculated_esc_value;
	}

	DshotWrite(esc_value);

	//check if we are skipping ticks TODO- remove in production
	//if (tick_tester != msTicks){
	//	GPIOC->ODR |= (1<<1);		//turn on ye olde debug LED
	//}
	//else{

	//	GPIOC->ODR &= ~(1<<1);		//turn on ye olde debug LED
	//}

	//to get it kinda right, we need a loop frequency of about 14Khz
	WaitForTick();	//dont use if you want speeeeeeeeeeeeed
}

int main(void)
{
	initialise();

	while(1)
	{
		loop();
	}
	return 0;
}