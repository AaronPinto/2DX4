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

void PortE_Init(void){	
	SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R4;		              // activate the clock for Port E
	while((SYSCTL_PRGPIO_R&SYSCTL_PRGPIO_R4) == 0){};	        // allow time for clock to stabilize
  GPIO_PORTE_DEN_R = 0b00001111;                         		// Enabled as digital outputs
	GPIO_PORTE_DIR_R = 0b00001111;
	return;
	}


void PortM_Init(void){
	SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R11;                 //activate the clock for Port M
	while((SYSCTL_PRGPIO_R&SYSCTL_PRGPIO_R11) == 0){};        //allow time for clock to stabilize 
	GPIO_PORTM_DIR_R = 0b00000000;       								    // make PM0 an input, PM0 is reading if the button is pressed or not 
  GPIO_PORTM_DEN_R = 0b00001111;
	return;
}

void PortN0N1_Init(void){
	SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R12;                 //activate the clock for Port N
	while((SYSCTL_PRGPIO_R&SYSCTL_PRGPIO_R12) == 0){};
	GPIO_PORTN_DIR_R=0b00000011;
	GPIO_PORTN_DEN_R=0b00000011;
	return;
}

void PortF0F4_Init(void){
  SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R5;                 //activate the clock for Port F
	while((SYSCTL_PRGPIO_R&SYSCTL_PRGPIO_R5) == 0){};
	GPIO_PORTF_DIR_R=0b00010001;
	GPIO_PORTF_DEN_R=0b00010001;
	return;
}

void PortK_Init(void){
  SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R9;                 //activate the clock for Port K
	while((SYSCTL_PRGPIO_R&SYSCTL_PRGPIO_R9) == 0){};
	GPIO_PORTK_DIR_R=0xFF;
	GPIO_PORTK_DEN_R=0xFF;
	return;
}

uint16_t value = 0;
char result;

char lut[4][4] = {{'1', '2', '3', 'A'},{'4', '5', '6', 'B'},{'7', '8', '9', 'C'},{'*', '0', '#', 'D'}};

uint32_t Flut[4][4] = {{0b000000001, 0b000010000, 0b000010001, 0b000010000},
{0, 0b000000001, 0b000010000, 0b000010001},
{0b000010001, 0, 0b000000001, 0},
{0b000010000, 0, 0b000010001, 0b000000001}};

uint32_t Nlut[4][4] = {{0, 0, 0, 0b000000010},
{0b000000001, 0b000000001, 0b000000001, 0b000000010},
{0b000000001, 0b000000010, 0b000000010, 0b000000011},
{0b000000011, 0, 0b000000011, 0b000000011}};
	
void getChar(uint32_t row, uint32_t col) {
	int rowIndex, colIndex;
	if ((col & 0b00000001) == 0) {
		colIndex = 0;
	} else if ((col & 0b00000010) == 0) {
		colIndex = 1;
	} else if ((col & 0b00000100) == 0) {
		colIndex = 2;
	} else if ((col & 0b00001000) == 0) {
		colIndex = 3;
	}
	
	if ((row & 0b00000001) == 0) {
		rowIndex = 0;
	} else if ((row & 0b00000010) == 0) {
		rowIndex = 1;
	} else if ((row & 0b00000100) == 0) {
		rowIndex = 2;
	} else if ((row & 0b00001000) == 0) {
		rowIndex = 3;
	}
	
	result = lut[rowIndex][colIndex];
}

void setLEDs(uint32_t row, uint32_t col) {
	int rowIndex, colIndex;
	if ((col & 0b00000001) == 0) {
		colIndex = 0;
	} else if ((col & 0b00000010) == 0) {
		colIndex = 1;
	} else if ((col & 0b00000100) == 0) {
		colIndex = 2;
	} else if ((col & 0b00001000) == 0) {
		colIndex = 3;
	}
	
	if ((row & 0b00000001) == 0) {
		rowIndex = 0;
	} else if ((row & 0b00000010) == 0) {
		rowIndex = 1;
	} else if ((row & 0b00000100) == 0) {
		rowIndex = 2;
	} else if ((row & 0b00001000) == 0) {
		rowIndex = 3;
	}
	
	GPIO_PORTN_DATA_R = Nlut[rowIndex][colIndex];
	GPIO_PORTF_DATA_R = Flut[rowIndex][colIndex];
}

void displayASCII(uint32_t row, uint32_t col) {
	int rowIndex, colIndex;
	if ((col & 0b00000001) == 0) {
		colIndex = 0;
	} else if ((col & 0b00000010) == 0) {
		colIndex = 1;
	} else if ((col & 0b00000100) == 0) {
		colIndex = 2;
	} else if ((col & 0b00001000) == 0) {
		colIndex = 3;
	}
	
	if ((row & 0b00000001) == 0) {
		rowIndex = 0;
	} else if ((row & 0b00000010) == 0) {
		rowIndex = 1;
	} else if ((row & 0b00000100) == 0) {
		rowIndex = 2;
	} else if ((row & 0b00001000) == 0) {
		rowIndex = 3;
	}
	
	GPIO_PORTK_DATA_R = (int) lut[rowIndex][colIndex];
}

int main(void){
	PortE_Init();
	PortM_Init();
	PortN0N1_Init();
	PortF0F4_Init();
	PortK_Init();
	
	int i = 0;
	GPIO_PORTE_DATA_R = 0b00000000;
	
	while(1){
		value = 0;
		result = '\0';
		GPIO_PORTN_DATA_R = 0b00000000;
		GPIO_PORTF_DATA_R = 0b00000000;
		GPIO_PORTK_DATA_R = 0b00000000;
		// 0001 0010 0100 1000
		
		for(i = 0; i < 4; i++) {		
			if (i == 0) {
				GPIO_PORTE_DATA_R = 0b00001110;
			} else if (i == 1) {
				GPIO_PORTE_DATA_R = 0b00001101;
			} else if (i == 2) {
				GPIO_PORTE_DATA_R = 0b00001011;
			} else {
				GPIO_PORTE_DATA_R = 0b00000111;
			}
			
			while((GPIO_PORTM_DATA_R & 0b00000001)==0){
				if (value == 0) {
					value = GPIO_PORTM_DATA_R & 0b00001111;
					value <<= 4;
					value += GPIO_PORTE_DATA_R & 0b00001111;
					getChar(GPIO_PORTE_DATA_R, GPIO_PORTM_DATA_R);
					setLEDs(GPIO_PORTE_DATA_R, GPIO_PORTM_DATA_R);
					displayASCII(GPIO_PORTE_DATA_R, GPIO_PORTM_DATA_R);
				}
			}
			
			while((GPIO_PORTM_DATA_R & 0b00000010)==0){
				if (value == 0) {
					value = GPIO_PORTM_DATA_R & 0b00001111;
					value <<= 4;
					value += GPIO_PORTE_DATA_R & 0b00001111;
					getChar(GPIO_PORTE_DATA_R, GPIO_PORTM_DATA_R);
					setLEDs(GPIO_PORTE_DATA_R, GPIO_PORTM_DATA_R);
					displayASCII(GPIO_PORTE_DATA_R, GPIO_PORTM_DATA_R);
				}
			}
			
			while((GPIO_PORTM_DATA_R & 0b00000100)==0){
				if (value == 0) {
					value = GPIO_PORTM_DATA_R & 0b00001111;
					value <<= 4;
					value += GPIO_PORTE_DATA_R & 0b00001111;
					getChar(GPIO_PORTE_DATA_R, GPIO_PORTM_DATA_R);
					setLEDs(GPIO_PORTE_DATA_R, GPIO_PORTM_DATA_R);
					displayASCII(GPIO_PORTE_DATA_R, GPIO_PORTM_DATA_R);
				}
			}
			
			while((GPIO_PORTM_DATA_R & 0b00001000)==0){
				if (value == 0) {
					value = GPIO_PORTM_DATA_R & 0b00001111;
					value <<= 4;
					value += GPIO_PORTE_DATA_R & 0b00001111;
					getChar(GPIO_PORTE_DATA_R, GPIO_PORTM_DATA_R);
					setLEDs(GPIO_PORTE_DATA_R, GPIO_PORTM_DATA_R);
					displayASCII(GPIO_PORTE_DATA_R, GPIO_PORTM_DATA_R);
				}
			}
		}
	}
}
