#include <stdint.h>
#include "tm4c1294ncpdt.h"
#include "PLL.h"
#include "SysTick.h"

void PortM_Init(void){
	//Use PortN onboard LED	
	SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R11;				// activate clock for Port M
	while((SYSCTL_PRGPIO_R&SYSCTL_PRGPIO_R11) == 0){};	// allow time for clock to stabilize
	GPIO_PORTM_DIR_R |= 0xF;												// make PM0 out
	GPIO_PORTM_AFSEL_R &= ~0xF;		 								// disable alt funct on PM0
	GPIO_PORTM_DEN_R |= 0xF;												// enable digital I/O on PM0
																									// configure PN0 as GPIO
	//GPIO_PORTN_PCTL_R = (GPIO_PORTN_PCTL_R&0xFFFFFF0F)+0x00000000;
	GPIO_PORTM_AMSEL_R &= ~0xF;		 								// disable analog functionality on PM0		
	
	return;
}

uint32_t delay = 200000;
void DutyCycleForward() {
	GPIO_PORTM_DATA_R = 0b00001100;
	SysTick_Wait(delay);
	GPIO_PORTM_DATA_R = 0b00000110;
	SysTick_Wait(delay);
	GPIO_PORTM_DATA_R = 0b00000011;
	SysTick_Wait(delay);
	GPIO_PORTM_DATA_R = 0b00001001;
	SysTick_Wait(delay);
}

void DutyCycleReverse() {
	GPIO_PORTM_DATA_R = 0b00001001;
	SysTick_Wait(delay);
	GPIO_PORTM_DATA_R = 0b00000011;
	SysTick_Wait(delay);
	GPIO_PORTM_DATA_R = 0b00000110;
	SysTick_Wait(delay);
	GPIO_PORTM_DATA_R = 0b00001100;
	SysTick_Wait(delay);
}

// @param number of rotations, 1 = CW, -1 = CCW
void IntensitySteps(double numRots, int dir) {
	uint32_t i;
	numRots = 512 * numRots;
	if(dir == 1) {
		for(i = 0; i < numRots; i++) {
			DutyCycleForward();
		}
	} else if(dir == -1) {
		for(i = 0; i < numRots; i++) {
			DutyCycleReverse();
		}
	}
}

int main(void){
	PLL_Init();																			// Default Set System Clock to 120MHz
	SysTick_Init();																	// Initialize SysTick configuration
	PortM_Init();																		// Initialize Port M GPIO
	
	IntensitySteps(1, 1);
	SysTick_Wait10ms(25);
	IntensitySteps(1, -1);
}
