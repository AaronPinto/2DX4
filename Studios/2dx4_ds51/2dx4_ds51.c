/* Studio W5-1 Project Code
		Sample code provided for studio to demonstrate UART 
		Uses UART texxtbook code by Valvano.

		U0Rx (VCP receive) connected to PA0
		U0Tx (VCP transmit) connected to PA1
		Note: Connected LaunchPad JP4 and JP5 inserted parallel with long side of board.

		Don't forget Gnd to Gnd

		Written by Tom Doyle
		Last Updated: Feb 13, 2020
*/

#include <stdint.h>
#include <string.h>
#include "tm4c1294ncpdt.h"
#include "PLL.h"
#include "SysTick.h"


//Used as indicition of code start with onboard LED
void FlashLED1(int count) {
		while(count--) {
			GPIO_PORTN_DATA_R ^= 0b00000001; 								//hello world!
			SysTick_Wait10ms(10);														//.1s delay
			GPIO_PORTN_DATA_R ^= 0b00000001;			
		}
}

void FlashLED2(int count) {
		while(count--) {
			GPIO_PORTN_DATA_R ^= 0b00000010; 								//hello world!
			SysTick_Wait10ms(10);														//.1s delay
			GPIO_PORTN_DATA_R ^= 0b00000010;			
		}
}

void PortN_Init(void){
	//Use PortN onboard LED	
	SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R12;				// activate clock for Port N
	while((SYSCTL_PRGPIO_R&SYSCTL_PRGPIO_R12) == 0){};	// allow time for clock to stabilize
	GPIO_PORTN_DIR_R |= 0x03;        								// make PN0 out (PN0 built-in LED1)
  GPIO_PORTN_AFSEL_R &= ~0x03;     								// disable alt funct on PN0
  GPIO_PORTN_DEN_R |= 0x03;        								// enable digital I/O on PN0
																									// configure PN1 as GPIO
  //GPIO_PORTN_PCTL_R = (GPIO_PORTN_PCTL_R&0xFFFFFF00)+0x00000000;
  GPIO_PORTN_AMSEL_R &= ~0x03;     								// disable analog functionality on PN0		
	FlashLED1(1);
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


//main test block
static char i=0;
int main(void){
	char string[] = "Aaron Pinto";
  uint32_t n;

  PLL_Init();               // set system clock to 120 MHz
	SysTick_Init();
  UART_Init();              // initialize UART
	PortN_Init();
  
	while(1){
		FlashLED2(1);
		//UART_OutString(string, strlen(string));
		i = UART_InChar();
		UART_OutChar(i);
		//SysTick_Wait(120000);
  }
}

