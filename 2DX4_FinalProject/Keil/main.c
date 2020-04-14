/*
Aaron Pinto

Bus Speed: 12MHz

Digital I/O LED Status: Distance = PF0, Displacement = PL4
*/
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include "tm4c1294ncpdt.h"
#include "Systick.h"
#include "PLL.h"
#include "uart.h"
#include "onboardLEDs.h"
#include "i2c.h"
#include "lidar.h"
#include "startStop.h"
#include "stepperMotor.h"

// External LED
void PortL_Init(void) {
		SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R10;                 // activate the clock for Port L
	
		while ((SYSCTL_PRGPIO_R & SYSCTL_PRGPIO_R10) == 0);
	
		GPIO_PORTL_DIR_R = 0b00010000;
		GPIO_PORTL_DEN_R = 0b00010000;
	
		return;
}

// External LED
void setDisplacementLED(bool state) {	
		if (state) {
			  GPIO_PORTL_DATA_R = 0b00010000;
		} else {
			  GPIO_PORTL_DATA_R = 0b00000000;
		}
}

void waitForStart() {
		int input = UART_InChar();
	
		while(input != 0x31) {
				input = UART_InChar();
		}
}

double xDistMM = 0.0; // x displacement in mm

int main(void) {
		StartStop_Init();
		PortL_Init();
		PortE_Init();	
		PLL_Init();	// Set System Clock to 12MHz
		SysTick_Init(); // Initialize SysTick configuration
		onboardLEDs_Init();
		I2C_Init();
		UART_Init();
		lidar_Init();
		
		int direction = -1;
		int delay = 2;
		int stepCounter = 0;
		
		bool running = false;
		
		GPIO_PORTE_DATA_R = 0b00000000;
		setDisplacementLED(true);
	
		UART_printf("Press GPIO PJ1 to start\r\n");
		
		uint16_t startStop = 1, prevStartStop = 1;
		while (1) {
				startStop = GPIO_PORTJ_DATA_R & 0x02;
			
				// Check startStop value, if motor isn't already running, make it run
				// If the motor is running, stop it and don't reset anything
				if ((!prevStartStop && startStop) && !running) {
						UART_printf("Starting\r\n");
						running = true;
						setDisplacementLED(false);
				} else if ((!prevStartStop && startStop) && running) {
						UART_printf("Stopping\r\n");
						running = false;
						setDisplacementLED(true);
				}
				
				// Motor should be running
				if (running) {
						outputLidarData(stepCounter, xDistMM, degPerStep);
						rotate(delay, direction);
						stepCounter++;
					
						if (stepCounter % angleInSteps == 0) {
								setDistanceLED(true);
						} else {
								setDistanceLED(false);
						}
				}				
				
				// Done 1 full rotation
				if (stepCounter == stepsPerRot) {
						running = false;
						stepCounter = 0;
						xDistMM += 200.0;
						setDistanceLED(false);
						setDisplacementLED(true);
					
						// Go back to starting, so wires don't get tangled
						for (int i = 0; i < 512; i++) {
							rotate(delay, -direction);
						}
				}
				
				prevStartStop = startStop;
		}
}
