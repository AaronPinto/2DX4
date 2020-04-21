#include <stdint.h>
#include "tm4c1294ncpdt.h"
#include "PLL.h"
#include "SysTick.h"


void PortN_Init(void){
	//Use PortN onboard LED	
	SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R12;				// activate clock for Port N
	while((SYSCTL_PRGPIO_R&SYSCTL_PRGPIO_R12) == 0){};	// allow time for clock to stabilize
	GPIO_PORTN_DIR_R |= 0x10;												// make PN0 out (PN0 built-in LED1)
	GPIO_PORTN_AFSEL_R &= ~0x10;		 								// disable alt funct on PN0
	GPIO_PORTN_DEN_R |= 0x10;												// enable digital I/O on PN0
																									// configure PN1 as GPIO
	//GPIO_PORTN_PCTL_R = (GPIO_PORTN_PCTL_R&0xFFFFFF0F)+0x00000000;
	GPIO_PORTN_AMSEL_R &= ~0x10;		 								// disable analog functionality on PN0		
	
	GPIO_PORTN_DATA_R ^= 0b00010000; 								//hello world!
	SysTick_Wait10ms(10);														//.1s delay
	GPIO_PORTN_DATA_R ^= 0b00010000;	
	return;
}

int main(void){
	uint32_t 	period = 1000; 												// this is the period of the wave in ms

	PLL_Init();																			// Default Set System Clock to 120MHz
	SysTick_Init();																	// Initialize SysTick configuration
	PortN_Init();																		// Initialize Port N GPIO
	
	while(1){															
		GPIO_PORTN_DATA_R ^= 0b00010000;							// toggle LED for visualization of process
		SysTick_Wait(120000 * period * 0.5);					// 1ms * period * 0.5, multiply by 0.5 for 50% duty cycle
		GPIO_PORTN_DATA_R ^= 0b00010000;							
		SysTick_Wait(120000 * period * 0.5);					// 1ms * period * 0.5
	}
}
