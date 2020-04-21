#include <stdint.h>
#include "tm4c1294ncpdt.h"
#include "PLL.h"
#include "SysTick.h"

void PortM_Init(void){
	//Use PortN onboard LED	
	SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R11;				// activate clock for Port M
	while((SYSCTL_PRGPIO_R&SYSCTL_PRGPIO_R11) == 0){};	// allow time for clock to stabilize
	GPIO_PORTM_DIR_R |= 0x1;												// make PM0 out
	GPIO_PORTM_AFSEL_R &= ~0x1;		 								// disable alt funct on PM0
	GPIO_PORTM_DEN_R |= 0x1;												// enable digital I/O on PM0
																									// configure PN0 as GPIO
	//GPIO_PORTN_PCTL_R = (GPIO_PORTN_PCTL_R&0xFFFFFF0F)+0x00000000;
	GPIO_PORTM_AMSEL_R &= ~0x1;		 								// disable analog functionality on PM0		
	
	GPIO_PORTM_DATA_R ^= 0b00000001; 								//hello world!
	SysTick_Wait10ms(10);														//.1s delay
	GPIO_PORTM_DATA_R ^= 0b00000001;	
	return;
}

uint32_t 	period = 1; 														// this is the period of the wave in ms
void DutyCycle_Percent(double value) {
	double percent = value / 255.0;
	GPIO_PORTM_DATA_R = 0b00000000;								// toggle LED for visualization of process
	SysTick_Wait(120000 * period * percent);				// 1ms * period * percent, percent is the duty high time
	GPIO_PORTM_DATA_R = 0b00000001;							
	SysTick_Wait(120000 * period * (1.0 - percent)); // 1ms * period * (1 - percent), 1 - percent is the duty low time
}

void IntensitySteps() {
	double i;
	uint32_t j;
	for(i = 25.5; i < 255.0; i += 25.5) { // Increases from 10% to 90% duty cycle
		for(j = 0; j < 10; j++) { // This runs 50 times, and the period of the wave is 10ms, for a total time of 0.5s
			DutyCycle_Percent(i);
		}	
	}
	for(i = 255; i > 25.5; i -= 25.5) { // Decreases from 100% to 20% duty cycle
		for(j = 0; j < 10; j++) {
			DutyCycle_Percent(i);
		}
	}
	for(j = 0; j < 50; j++) { // Last 10% duty cycle
			DutyCycle_Percent(i);
	}
}

int main(void){
	PLL_Init();																			// Default Set System Clock to 120MHz
	SysTick_Init();																	// Initialize SysTick configuration
	PortM_Init();																		// Initialize Port M GPIO
	
	uint32_t i, j;
	while(1) {
		IntensitySteps();
	}
}
