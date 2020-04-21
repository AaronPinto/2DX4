// 2DX4StudioW20E1_ADCDemo
// This program illustrates the use of the ADC in the C language.
// Note the library headers asscoaited are PLL.h and SysTick.h,
// which define functions and variables used in PLL.c and SysTick.c.
// This program uses code directly from your course textbook.
//
// This example will be extended for in W21E0 and W21E1.
//
//  Written by Tom Doyle
//  January 18, 2020
//  Last Update: January 21, 2020

#include <stdint.h>
#include "tm4c1294ncpdt.h"
#include "PLL.h"
#include "SysTick.h"

volatile uint32_t ADCvalue;

//-ADC0_InSeq3-
// Busy-wait analog to digital conversion. 0 to 3.3V maps to 0 to 4095 
// Input: none 
// Output: 12-bit result of ADC conversion 
uint32_t ADC0_InSeq3(void){
	uint32_t result;
	
	ADC0_PSSI_R = 0x0008;														// 1) initiate SS3   
	while((ADC0_RIS_R&0x08)==0){}										// 2) wait for conversion done   
	result = ADC0_SSFIFO3_R&0xFFF;									// 3) read 12-bit result   
	ADC0_ISC_R = 0x0008;														// 4) acknowledge completion   
	
	return result; 
} 

void ADC_Init(void){
	//config the ADC from Valvano textbook
	SYSCTL_RCGCGPIO_R |= 0b00010000;								// 1. activate clock for port E
	while ((SYSCTL_PRGPIO_R & 0b00010000) == 0) {}	//		wait for clock/port to be ready
	GPIO_PORTE_DIR_R &= ~0x10;											// 2) make PE4 input   
	GPIO_PORTE_AFSEL_R |= 0x10;											// 3) enable alternate function on PE4   
	GPIO_PORTE_DEN_R &= ~0x10;											// 4) disable digital I/O on PE4   
	GPIO_PORTE_AMSEL_R |= 0x10;											// 5) enable analog function on PE4   
	SYSCTL_RCGCADC_R |= 0x01;												// 6) activate ADC0   
	ADC0_PC_R = 0x01;																// 7) maximum speed is 125K samples/sec   
	ADC0_SSPRI_R = 0x0123;													// 8) Sequencer 3 is highest priority   
	ADC0_ACTSS_R &= ~0x0008;												// 9) disable sample sequencer 3   
	ADC0_EMUX_R &= ~0xF000;													// 10) seq3 is software trigger 
	ADC0_SSMUX3_R = 9;															// 11) set channel Ain9 (PE4)   
	ADC0_SSCTL3_R = 0x0006;													// 12) no TS0 D0, yes IE0 END0   
	ADC0_IM_R &= ~0x0008;														// 13) disable SS3 interrupts   
	ADC0_ACTSS_R |= 0x0008;													// 14) enable sample sequencer 3 
	//==============================================================================	
	
	return;
}

void PortN_Init(void){
	//Use PortN onboard LED	
	SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R12;				// activate clock for Port N
	while((SYSCTL_PRGPIO_R&SYSCTL_PRGPIO_R12) == 0){};	// allow time for clock to stabilize
	GPIO_PORTN_DIR_R |= 0x01;        								// make PN0 out (PN0 built-in LED1)
  GPIO_PORTN_AFSEL_R &= ~0x01;     								// disable alt funct on PN0
  GPIO_PORTN_DEN_R |= 0x01;        								// enable digital I/O on PN0
																									// configure PN1 as GPIO
  //GPIO_PORTN_PCTL_R = (GPIO_PORTN_PCTL_R&0xFFFFFF0F)+0x00000000;
  GPIO_PORTN_AMSEL_R &= ~0x01;     								// disable analog functionality on PN0		
	
	GPIO_PORTN_DATA_R ^= 0b00000001; 								//hello world!
	SysTick_Wait10ms(10);														//.1s delay
	GPIO_PORTN_DATA_R ^= 0b00000001;	
	return;
}


uint32_t func_debug[100]; 											// array used to store realtime data 	

int main(void){
  				
	uint32_t 	halfdelay = 25, 												// this is a multiplier used for SysTick = half of delay 
						count = 0;														// my loop control variable initialized to 0	
	 
	PLL_Init();																			// Default Set System Clock to 120MHz
	SysTick_Init();																	// Initialize SysTick configuration
	ADC_Init();																			// Initialize ADC as per Vavano text
	PortN_Init();																		// Initialize Port N GPIO
	while(count < 100){															// run program for fixed number of steps/samples
		GPIO_PORTN_DATA_R ^= 0b00000001;							// toggle LED for visualization of process
		SysTick_Wait10ms(halfdelay);									// halfdelay*10ms
		func_debug[count++] = ADC0_InSeq3();					// Acquire sample
		GPIO_PORTN_DATA_R ^= 0b00000001;							// toggle LED for visualization of process
		SysTick_Wait10ms(halfdelay);									// halfdelay*10ms		
	}

}



