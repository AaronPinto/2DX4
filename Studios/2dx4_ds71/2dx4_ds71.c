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
#include "tm4c1294ncpdt.h"
#include "PLL.h"
#include "SysTick.h"


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

// global variable visible in Watch window of debugger
// increments at least once per button press
// GPIO Port J = Vector 67
// Bit in interrupt register = 51
volatile unsigned long FallingEdges = 0;
void ExternalButton_Init(void){
	SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R8;				// activate clock for Port J
	while((SYSCTL_PRGPIO_R&SYSCTL_PRGPIO_R8) == 0){};	// allow time for clock to stabilize
  FallingEdges = 0;             // (b) initialize counter
  GPIO_PORTJ_DIR_R &= ~0x02;    // (c) make PJ1 in 

  GPIO_PORTJ_DEN_R |= 0x02;     //     enable digital I/O on PJ1
	GPIO_PORTJ_PCTL_R &= ~0x000000F0; //  configure PJ1 as GPIO 
	GPIO_PORTJ_AMSEL_R &= ~0x02;	//   disable analog functionality on PJ1		
	GPIO_PORTJ_PUR_R |= 0x02;			//	enable weak pull up resistor
  GPIO_PORTJ_IS_R &= ~0x02;     // (d) PJ1 is edge-sensitive 
  GPIO_PORTJ_IBE_R &= ~0x02;    //     PJ1 is not both edges 
  GPIO_PORTJ_IEV_R &= ~0x02;    //     PJ1 falling edge event 
  GPIO_PORTJ_ICR_R = 0x02;      // (e) clear flag1
  GPIO_PORTJ_IM_R |= 0x02;      // (f) arm interrupt on PJ1
  NVIC_PRI13_R = (NVIC_PRI13_R&0xFF00FFFF)|0x000A0000; // (g) priority 5
  NVIC_EN1_R |= 0x00080000;              // (h) enable interrupt 67 in NVIC
  EnableInt();           				// lets go
}

//This is the Interrupt Service Routine.  This must be included and match the
//	interrupt naming convention in startup_msp432e401y_uvision.s (Note - not the
//	same as Valvano textbook).
void GPIOJ_IRQHandler(void){
  GPIO_PORTJ_ICR_R = 0x02;      // acknowledge flag4
  FallingEdges = FallingEdges + 1; // Observe in Debug Watch Window
	FlashLED2(1);
}


// The main program -- notice how we are only initializing the micro and nothing else.
// Our configured interrupts are being handled and tasks executed on an event drive basis.

int main(void){
  PLL_Init();               // set system clock to 120 MHz
	SysTick_Init();
	PortN_Init();
	ExternalButton_Init();
	
	while(1){
		WaitForInt();
	}

		
}


