// 2DX4Studio4
// This program illustrates detecting a single button press


#include <stdint.h>
#include "tm4c1294ncpdt.h"

void PortE0_Init(void){	
	SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R4;		  // activate the clock for Port E
	while((SYSCTL_PRGPIO_R&SYSCTL_PRGPIO_R4) == 0){};	  // allow time for clock to stabilize
	GPIO_PORTE_DEN_R= 0b00000001;
	GPIO_PORTE_DIR_R |= 0b00000001;                           // make PE0 output  
	GPIO_PORTE_DATA_R=0b00000000;                             // setting state to zero to drive the row, one to disable 
	return;
	}


void PortM0_Init(void){
	SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R11;                 //activate the clock for Port M
	while((SYSCTL_PRGPIO_R&SYSCTL_PRGPIO_R11) == 0){};        //allow time for clock to stabilize 
	GPIO_PORTM_DIR_R |= 0b00000000;       			  // make PM0 an input, PM0 is reading the column 
        GPIO_PORTM_DEN_R |= 0b00000001;
	return;
}


//Turns on D2
void PortN0_Init(void){
	SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R12;                 //activate the clock for Port N to use D2
	while((SYSCTL_PRGPIO_R&SYSCTL_PRGPIO_R12) == 0){};
	GPIO_PORTN_DIR_R|=0b00000011;                             //Here D2 and D1 are ready for output but we will only be using D2
	GPIO_PORTN_DEN_R|=0b00000011;
	return;
}



int main(void){
	PortE0_Init();
	PortM0_Init();
	PortN0_Init();

	
	
	while(1){//keep checking if the button is pressed 
 
	//Checks if Button 1 is pressed, if pressed D2 lights up	
	if(((GPIO_PORTM_DATA_R&0b00000001)==0&&(GPIO_PORTE_DATA_R&0b000000001)==0)){
	GPIO_PORTN_DATA_R = 0b000000001;
	}
	
  else{
	GPIO_PORTN_DATA_R=0b00000000;
	}

}
}

