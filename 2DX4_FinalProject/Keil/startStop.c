#include <stdint.h>
#include "tm4c1294ncpdt.h"
#include "startStop.h"

// Enable interrupts
void EnableInt(void){
	__asm("    cpsie   i\n");
}

// Disable interrupts
void DisableInt(void){
	__asm("    cpsid   i\n");
}

// Low power wait
void WaitForInt(void){
	__asm("    wfi\n");
}

// GPIO Port J = Vector 67
// Bit in interrupt register = 51
void StartStop_Init(void){
	SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R8;				// activate clock for Port J
	while((SYSCTL_PRGPIO_R&SYSCTL_PRGPIO_R8) == 0){};	// allow time for clock to stabilize
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
//  NVIC_PRI13_R = (NVIC_PRI13_R&0xFF00FFFF)|0x000A0000; // (g) priority 5
  NVIC_PRI12_R = (NVIC_PRI12_R&0xFF00FFFF)|0xA0000000; // (g) priority 5
  NVIC_EN1_R |= 0x00080000;              // (h) enable interrupt 67 in NVIC
  EnableInt();           				// lets go
}

//This is the Interrupt Service Routine.  This must be included and match the
//	interrupt naming convention in startup_msp432e401y_uvision.s (Note - not the
//	same as Valvano textbook).
void GPIOJ_IRQHandler(void){
  GPIO_PORTJ_ICR_R = 0x02;      // acknowledge flag4
}
