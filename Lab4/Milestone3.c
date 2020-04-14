#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include "tm4c1294ncpdt.h"
#include "PLL.h"
#include "SysTick.h"

void PortM_Init(void){
	//Use PortN onboard LED	
	SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R11;				// activate clock for Port M
	while((SYSCTL_PRGPIO_R&SYSCTL_PRGPIO_R11) == 0){};	// allow time for clock to stabilize
	GPIO_PORTM_DIR_R |= 0x7;												// make PM0 out
	GPIO_PORTM_AFSEL_R &= ~0x7;		 								// disable alt funct on PM0
	GPIO_PORTM_DEN_R |= 0x7;												// enable digital I/O on PM0
																									// configure PN0 as GPIO
	//GPIO_PORTN_PCTL_R = (GPIO_PORTN_PCTL_R&0xFFFFFF0F)+0x00000000;
	GPIO_PORTM_AMSEL_R &= ~0x7;		 								// disable analog functionality on PM0		
	
	return;
}

int comp (const void * elem1, const void * elem2) {
    double f = *((double*)elem1);
    double s = *((double*)elem2);
    if (f > s) return  1;
    if (f < s) return -1;
    return 0;
}

uint32_t 	period = 5; 														// this is the period of the wave in ms
void DutyCycle_Percent(double r, double g, double b) {
	double rP = r / 255.0, gP = g / 255.0, bP = b / 255.0;
	double percents[] = {rP, gP, bP};
	qsort(percents, sizeof(percents)/sizeof(*percents), sizeof(*percents), comp);
	
	GPIO_PORTM_DATA_R = 0b00000000;
	SysTick_Wait(120000 * period * percents[0]);
	
	if(rP == percents[0]) {
		GPIO_PORTM_DATA_R |= 0b00000100;
	}
	if(gP == percents[0]) {
		GPIO_PORTM_DATA_R |= 0b00000010;
	} 
	if(bP == percents[0]) {
		GPIO_PORTM_DATA_R |= 0b00000001;
	}
	
	SysTick_Wait(120000 * period * (percents[1] - percents[0]));
	
	if(rP == percents[1]) {
		GPIO_PORTM_DATA_R |= 0b00000100;
	}
	if(gP == percents[1]) {
		GPIO_PORTM_DATA_R |= 0b00000010;
	} 
	if(bP == percents[1]){
		GPIO_PORTM_DATA_R |= 0b00000001;
	}
	
	SysTick_Wait(120000 * period * (percents[2] - percents[1] - percents[0]));
	
	GPIO_PORTM_DATA_R = 0b00000111;
	SysTick_Wait(120000 * period * (1.0 - (percents[2] - percents[1] - percents[0])));
}

int main(void){
	PLL_Init();																			// Default Set System Clock to 120MHz
	SysTick_Init();																	// Initialize SysTick configuration
	PortM_Init();																		// Initialize Port M GPIO
	
	double i = 0;
	double a = 2.0 * 3.14159265 / 3.0;
	double b = 0;
	double c = -2.0 * 3.14159265 / 3.0;
	while(1){
		DutyCycle_Percent(255.0 * pow(sin(0.001 * i + a), 2), 255.0 * pow(sin(0.001 * i + b), 2), 255.0 * pow(sin(0.001 * i + c), 2));
		i++;
	}
}
