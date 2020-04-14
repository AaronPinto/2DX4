// // 2DX4StudioW30E1_Decoding a Button Press
// This program illustrates detecting a single button press.
// This program uses code directly from your course textbook.
//
// This example will be extended for in W21E0 and W21E1.
//
//  Written by Ama Simons
//  January 30, 2020
//  Last Update: January 21, 2020

#include <stdint.h>
#include "tm4c1294ncpdt.h"
#include "Systick.h"
#include "PLL.h"

void PortE0E1_Init(void){	
	SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R4;		              // activate the clock for Port E
	while((SYSCTL_PRGPIO_R&SYSCTL_PRGPIO_R4) == 0){};	        // allow time for clock to stabilize
  GPIO_PORTE_DEN_R = 0b00000011;                         		// Enabled both as digital outputs
	GPIO_PORTE_DIR_R = 0b00000011;
	return;
	}


void PortM0M1_Init(void){
	SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R11;                 //activate the clock for Port M
	while((SYSCTL_PRGPIO_R&SYSCTL_PRGPIO_R11) == 0){};        //allow time for clock to stabilize 
	GPIO_PORTM_DIR_R = 0b00000000;       								    // make PM0 an input, PM0 is reading if the button is pressed or not 
  GPIO_PORTM_DEN_R = 0b00000011;
	return;
}


//Turns on D2, D1
void PortN0N1_Init(void){
	SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R12;                 //activate the clock for Port N
	while((SYSCTL_PRGPIO_R&SYSCTL_PRGPIO_R12) == 0){};
	GPIO_PORTN_DIR_R=0b00000011;
	GPIO_PORTN_DEN_R=0b00000011;
	return;
}

//Turns on D3, D4
void PortF0F4_Init(void){
  SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R5;                 //activate the clock for Port F
	while((SYSCTL_PRGPIO_R&SYSCTL_PRGPIO_R5) == 0){};
	GPIO_PORTF_DIR_R=0b00010001;
	GPIO_PORTF_DEN_R=0b00010001;
	return;
}




int main(void){
	//PLL_Init();
	//SysTick_Init();
	PortE0E1_Init();
	PortM0M1_Init();
	PortN0N1_Init();
	PortF0F4_Init();
	
	while(1){
	
//Row 1	
		  
  GPIO_PORTE_DATA_R =  0b00000010;
	
	//Checks If Button 3 is pressed - D2 lights up
	//Unique code is:	1010 - In the order of PE1 PE0 PM1 PM0
	while((GPIO_PORTM_DATA_R&0b00000001)==0){
	GPIO_PORTN_DATA_R = 0b00000001;
	}

	
	
	//Checks If Button 4 is pressed - D1 lights up
	//Unique code is: 1001 - In the order of PE1 PE0 PM1 PM0  
	while((GPIO_PORTM_DATA_R&0b00000010)==0){
	  GPIO_PORTN_DATA_R = 0b00000010;
	}
   
 
	
	
	//Row 2
	GPIO_PORTE_DATA_R =  0b00000001;            //Data is still as registers 

	
	//Checks if Button 1 is pressed - D3 lights up 
	//Unique code is: 0110 - In order of PE1 PE0 PM1 PM0
  while((GPIO_PORTM_DATA_R&0b00000001)==0){
	GPIO_PORTF_DATA_R=0b000010000;}
	
 
	
//Checks if Button 2 is pressed - D4 Lights up 
//Unique Code is: 0101  - In order of PE1 PE0 PM1 PM0 
   while((GPIO_PORTM_DATA_R&0b00000010)==0){
		GPIO_PORTF_DATA_R=0b000000001;
	}


GPIO_PORTN_DATA_R=0b00000000;
GPIO_PORTF_DATA_R=0b00000000;

}
}
