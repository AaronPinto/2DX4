#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include "tm4c1294ncpdt.h"
#include "Systick.h"
#include "ST7735.h"
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

void PortH_Init(void){
	SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R7;
	while((SYSCTL_PRGPIO_R&SYSCTL_PRGPIO_R7) == 0){};	
	GPIO_PORTH_DIR_R |= 0xF;												
	GPIO_PORTH_AFSEL_R &= ~0xF;		 								
	GPIO_PORTH_DEN_R |= 0xF;											
																								
	GPIO_PORTH_AMSEL_R &= ~0xF;		 								
	
	return;
}

//Initialize UART0, based on textbook.  Clock code modified.
void UART_Init(void) {
	SYSCTL_RCGCUART_R |= 0x0001; // activate UART0   
	SYSCTL_RCGCGPIO_R |= 0x0001; // activate port A   
	//UART0_CTL_R &= ~0x0001; // disable UART   

	while((SYSCTL_PRUART_R&SYSCTL_PRUART_R0) == 0){};
		
  UART0_CTL_R &= ~UART_CTL_UARTEN;      // disable UART
  UART0_IBRD_R = 8;                     // IBRD = int(16,000,000 / (16 * 115,200)) = int(8.681)
  UART0_FBRD_R = 44;                    // FBRD = round(0.6806 * 64) = 44
                                        // 8 bit word length (no parity bits, one stop bit, FIFOs)
  UART0_LCRH_R = (UART_LCRH_WLEN_8|UART_LCRH_FEN);
                                        // UART gets its clock from the alternate clock source as defined by SYSCTL_ALTCLKCFG_R
  UART0_CC_R = (UART0_CC_R&~UART_CC_CS_M)+UART_CC_CS_PIOSC;
                                        // the alternate clock source is the PIOSC (default)
  SYSCTL_ALTCLKCFG_R = (SYSCTL_ALTCLKCFG_R&~SYSCTL_ALTCLKCFG_ALTCLK_M)+SYSCTL_ALTCLKCFG_ALTCLK_PIOSC;
  UART0_CTL_R &= ~UART_CTL_HSE;         // high-speed disable; divide clock by 16 rather than 8 (default)

	UART0_LCRH_R = 0x0070;		// 8-bit word length, enable FIFO   
	UART0_CTL_R = 0x0301;			// enable RXE, TXE and UART   
	GPIO_PORTA_PCTL_R = (GPIO_PORTA_PCTL_R&0xFFFFFF00)+0x00000011; // UART   
	GPIO_PORTA_AMSEL_R &= ~0x03;	// disable analog function on PA1-0   
	GPIO_PORTA_AFSEL_R |= 0x03;		// enable alt funct on PA1-0   
	GPIO_PORTA_DEN_R |= 0x03;			// enable digital I/O on PA1-0 
}

// Wait for new input, then return ASCII code 
char UART_InChar(void){
	while((UART0_FR_R&0x0010) != 0);		// wait until RXFE is 0   
	return((char)(UART0_DR_R&0xFF));
} 
	
// Wait for buffer to be not full, then output 
void UART_OutChar(char data){
	while((UART0_FR_R&0x0020) != 0);	// wait until TXFF is 0   
	UART0_DR_R = data;
} 
	
void UART_OutString(char data[], int len){
	uint32_t i;
  for(i=0; i<len; i++){
    UART_OutChar(data[i]);
  }
}

char result;
char lut[4][4] = {{'1', '2', '3', 'A'},{'4', '5', '6', 'B'},{'7', '8', '9', 'C'},{'*', '0', '#', 'D'}};
	
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

void setLED(bool state) {	
	if (state) {
		GPIO_PORTN_DATA_R = 0b00000001;
	} else {
		GPIO_PORTN_DATA_R = 0b00000000;
	}
}

void DutyCycleForward(int delay) {
	GPIO_PORTH_DATA_R = 0b00001100;
	SysTick_Wait10ms(delay);
	GPIO_PORTH_DATA_R = 0b00000110;
	SysTick_Wait10ms(delay);
	GPIO_PORTH_DATA_R = 0b00000011;
	SysTick_Wait10ms(delay);
	GPIO_PORTH_DATA_R = 0b00001001;
	SysTick_Wait10ms(delay);
}

void DutyCycleReverse(int delay) {
	GPIO_PORTH_DATA_R = 0b00001001;
	SysTick_Wait10ms(delay);
	GPIO_PORTH_DATA_R = 0b00000011;
	SysTick_Wait10ms(delay);
	GPIO_PORTH_DATA_R = 0b00000110;
	SysTick_Wait10ms(delay);
	GPIO_PORTH_DATA_R = 0b00001100;
	SysTick_Wait10ms(delay);
}

// @param number of rotations, 1 = CW, -1 = CCW
void rotate(int delay, int dir) {
	if(dir == 1) {
			DutyCycleForward(delay);
	} else if(dir == -1) {
			DutyCycleReverse(delay);
	}
}

int angleInSteps = 0;
int direction = 0;
int delay = 0;

bool setAngle = false;
bool setDir = false;
bool setDelay = false;
bool sendOutputToUART = true;

double stepsPerRot = 512.0;

int main(void){
	PortE_Init();
	PortM_Init();
	PortN0N1_Init();
	PortH_Init();
	PLL_Init();																			// Default Set System Clock to 120MHz
	SysTick_Init();																	// Initialize SysTick configuration
	UART_Init();              // initialize UART
	ST7735_InitR(INITR_REDTAB);
	ST7735_FillScreen(0);                 // set screen to black
	ST7735_OutString("\n");
	
	char output[100];
	
	int i = 0, tickCounter = 0;
	GPIO_PORTE_DATA_R = 0b00000000;
	
	while(1){
		result = '\0';
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
					getChar(GPIO_PORTE_DATA_R, GPIO_PORTM_DATA_R);
			}
			
			while((GPIO_PORTM_DATA_R & 0b00000010)==0){
					getChar(GPIO_PORTE_DATA_R, GPIO_PORTM_DATA_R);
			}
			
			while((GPIO_PORTM_DATA_R & 0b00000100)==0){
					getChar(GPIO_PORTE_DATA_R, GPIO_PORTM_DATA_R);
			}
			
			while((GPIO_PORTM_DATA_R & 0b00001000)==0){
					getChar(GPIO_PORTE_DATA_R, GPIO_PORTM_DATA_R);
			}
		}
		
		if (!setAngle) {
			if (result == 'A') {
				setAngle = true;
				angleInSteps = (11.25 / 360.0) * stepsPerRot;
			} else if (result == 'B') {
				setAngle = true;
				angleInSteps = (45.0 / 360.0) * stepsPerRot;
			} else if (result == 'C') {
				setAngle = true;
				angleInSteps = (90.0 / 360.0) * stepsPerRot;
			} else if (result == 'D') {
				setAngle = true;
				angleInSteps = stepsPerRot;
			}
		}
		
		if (!setDir && setAngle) {
			if (result == '*') {
				setDir = true;
				direction = 1;
			} else if (result == '#') {
				setDir = true;
				direction = -1;
			}
		}
		
		if (!setDelay && setAngle && setDir) {
			if (result >= '1' && result <= '9') {
				setDelay = true;
				delay = result - '0';
			}
		}
		
		if (setAngle && setDir && setDelay) {
			rotate(delay, direction);
			tickCounter++;
			if (tickCounter % angleInSteps == 0) {
				setLED(true);
			} else {
				setLED(false);
			}
			
			if (sendOutputToUART) {
				snprintf(output, sizeof(output), " %f %d %d\r", (angleInSteps / stepsPerRot) * 360.0, direction, delay);
				sendOutputToUART = false;
				UART_OutString(output, strlen(output));
				ST7735_OutString(output);
			}
		}
		
		if (result == '0') {
			setAngle = false;
			setDir = false;
			setDelay = false;
			sendOutputToUART = true;
			angleInSteps = 0;
			direction = 0;
			delay = 0;
			tickCounter = 0;
		}
	}
}
