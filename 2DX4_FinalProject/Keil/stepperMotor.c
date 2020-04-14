#include <stdint.h>
#include "SysTick.h"
#include "tm4c1294ncpdt.h"
#include "stepperMotor.h"

const int stepsPerRot = 512;
const double degPerStep = 360.0 / stepsPerRot;
const int angleInSteps = 45.0 / 360.0 * stepsPerRot;

// Stepper motor
void PortE_Init(void) {
		SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R4;
	
		while ((SYSCTL_PRGPIO_R & SYSCTL_PRGPIO_R4) == 0);
	
		GPIO_PORTE_DIR_R |= 0xF;												
		GPIO_PORTE_AFSEL_R &= ~0xF;		 								
		GPIO_PORTE_DEN_R |= 0xF;																		
		GPIO_PORTE_AMSEL_R &= ~0xF;
	
		return;
}

// Stepper motor sequence for forward, CW
void DutyCycleForward(int delay) {	
		GPIO_PORTE_DATA_R = 0b00001100;
		SysTick_Wait1ms(delay);
	
		GPIO_PORTE_DATA_R = 0b00000110;
		SysTick_Wait1ms(delay);
	
		GPIO_PORTE_DATA_R = 0b00000011;
		SysTick_Wait1ms(delay);
	
		GPIO_PORTE_DATA_R = 0b00001001;
		SysTick_Wait1ms(delay);
}

// Stepper motor sequence for reverse, CCW
void DutyCycleReverse(int delay) {	
	  GPIO_PORTE_DATA_R = 0b00001001;
	  SysTick_Wait1ms(delay);
	
  	GPIO_PORTE_DATA_R = 0b00000011;
	  SysTick_Wait1ms(delay);
	
	  GPIO_PORTE_DATA_R = 0b00000110;
	  SysTick_Wait1ms(delay);
	
	  GPIO_PORTE_DATA_R = 0b00001100;
	  SysTick_Wait1ms(delay);
}

// @param number of 10ms delays, 1 = CCW, -1 = CW
void rotate(int delay, int dir) {
	  if (dir == 1) {
		  	DutyCycleForward(delay);
	  } else if (dir == -1) {
		  	DutyCycleReverse(delay);
	  }
}
