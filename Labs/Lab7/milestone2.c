/* Studio W7-1 Project Code
		Sample code provided for studio to demonstrate periodic interrupt 
		Based upon interrupt texxtbook code by Valvano.

		This code will use an push button PJ1 to trigger an inteerupt.  The trigger
		will cause the PortJ ISR to execute.  The ISR will
		flash the onboard LED on/off.  However, you can adapt this code
		to perform any operations simply by changing the code in the ISR


		Written by Tom Doyle
		Last Updated: March 3, 2020
*/

#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "tm4c1294ncpdt.h"
#include "PLL.h"
#include "SysTick.h"
#include "i2c0.h" //modified version of Valvano i2c0.c  


//Flash D2
void FlashLED2(int count) {
		while(count--) {
			GPIO_PORTN_DATA_R ^= 0b00000001; 								//hello world!
			SysTick_Wait10ms(10);														//.1s delay
			GPIO_PORTN_DATA_R ^= 0b00000001;			
		}
}

//Flash D1
void FlashLED1(int count) {
		while(count--) {
			GPIO_PORTN_DATA_R ^= 0b00000010; 								//hello world!
			SysTick_Wait10ms(10);														//.1s delay
			GPIO_PORTN_DATA_R ^= 0b00000010;			
		}
}

// Initialize onboard LEDs
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

// Enable interrupts
void EnableInt(void)
{    __asm("    cpsie   i\n");
}

// Disable interrupts
void DisableInt(void)
{    __asm("    cpsid   i\n");
}

// Low power wait
void WaitForInt(void)
{    __asm("    wfi\n");
}

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

// global variable visible in Watch window of debugger
// increments at least once per button press
// GPIO Port J = Vector 67
// Bit in interrupt register = 51
volatile unsigned long FallingEdges = 0;
void ExternalButton_Init(void){
	SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R8;				// activate clock for Port J
	while((SYSCTL_PRGPIO_R&SYSCTL_PRGPIO_R8) == 0){};	// allow time for clock to stabilize
  FallingEdges = 0;             // (b) initialize counter
  GPIO_PORTJ_DIR_R &= ~0x03;    // (c) make PJ1 in 

  GPIO_PORTJ_DEN_R |= 0x03;     //     enable digital I/O on PJ1
	GPIO_PORTJ_PCTL_R &= ~0x000000FF; //  configure PJ1 as GPIO 
	GPIO_PORTJ_AMSEL_R &= ~0x03;	//   disable analog functionality on PJ1		
	GPIO_PORTJ_PUR_R |= 0x03;			//	enable weak pull up resistor
  GPIO_PORTJ_IS_R &= ~0x03;     // (d) PJ1 is edge-sensitive 
  GPIO_PORTJ_IBE_R &= ~0x03;    //     PJ1 is not both edges 
  GPIO_PORTJ_IEV_R &= ~0x03;    //     PJ1 falling edge event 
  GPIO_PORTJ_ICR_R = 0x03;      // (e) clear flag1
  GPIO_PORTJ_IM_R |= 0x03;      // (f) arm interrupt on PJ1
  NVIC_PRI13_R = (NVIC_PRI13_R&0xFF00FFFF)|0x000A0000; // (g) priority 5
  NVIC_EN1_R |= 0x00080000;              // (h) enable interrupt 67 in NVIC
  EnableInt();           				// lets go
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

//This is the Interrupt Service Routine.  This must be included and match the
//	interrupt naming convention in startup_msp432e401y_uvision.s (Note - not the
//	same as Valvano textbook).
char output[100];

void GPIOJ_IRQHandler(void){
  GPIO_PORTJ_ICR_R = 0x03;      // acknowledge flag4
  FallingEdges = FallingEdges + 1; // Observe in Debug Watch Window
	if (GPIO_PORTJ_DATA_R == 0x1) {
		FlashLED2(1);
		snprintf(output, sizeof(output), "%s\r\n", "400190637");
		UART_OutString(output, strlen(output));
	} else if (GPIO_PORTJ_DATA_R == 0x2) {
		FlashLED1(1);
		snprintf(output, sizeof(output), "%s\r\n", "400188200");
		UART_OutString(output, strlen(output));
	}
	//I2C_Send1(0x29, 0x00);		//address, data
}


// The main program -- notice how we are only initializing the micro and nothing else.
// Our configured interrupts are being handled and tasks executed on an event drive basis.
int main(void){
  PLL_Init();               // set system clock to 120 MHz
	SysTick_Init();
	PortN_Init();
	ExternalButton_Init();
	I2C_Init();
	UART_Init();
	
	while(1){
		WaitForInt();
	}		
}


