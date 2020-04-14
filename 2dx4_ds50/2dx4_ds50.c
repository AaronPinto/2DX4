/* Studio W5-0 Project Code
		Sample code provided for studio to demonstrate SSI with AD2

		MISO: DIO3 connected to PD0 ((SSI2XDAT1/SSI2Rx)) 
		SCK: DIO1 connected to PD3 (SSI2Clk)
		MOSI: DIO2 connected to PD1 (SSI2XDAT0/SSI2Tx)
		TCS: DIO0 connected to PD2 (SSI2Fss)
		Don't forget Gnd to Gnd

		Written by Tom Doyle
		Last Updated: Feb 12, 2020
*/

#include <stdint.h>
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


void static SSI_Init(void) {
	// TED Start clk PortD
	SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R3;
																					// allow time for clock to stabilize
  while((SYSCTL_PRGPIO_R&SYSCTL_PRGPIO_R3) == 0){};	
				
	// TED Modify for SSI2		
                                        // activate clock for SSI2
  SYSCTL_RCGCSSI_R |= SYSCTL_RCGCSSI_R2;
                                        // allow time for clock to stabilize
  while((SYSCTL_PRSSI_R&SYSCTL_PRSSI_R2) == 0){};

 	  // initialize SSI2 TED PD 0,1,2,3
  GPIO_PORTD_AFSEL_R |= 0x0F;           // enable alt funct on PD1,2,3 0b00001111
  GPIO_PORTD_DEN_R |= 0x0F;             // enable digital I/O  
	
	// configure PD 0,1,2,3 as SSI
  GPIO_PORTD_PCTL_R = (GPIO_PORTD_PCTL_R&0xFFFF0000)+0x0000FFFF;
  GPIO_PORTD_AMSEL_R &= ~0x0F;          // disable analog functionality on PD0,1,2,3
  
 
	SSI2_CR1_R &= ~SSI_CR1_SSE;           // disable SSI
  //SSI2_CR1_R |= (SSI_CR1_MS | SSI_CR1_MODE_BI);            // master mode
	SSI2_CR1_R &= ~(SSI_CR1_MS | SSI_CR1_MODE_LEGACY);            // master mode	
                                        // configure for clock from source as defined by SYSCTL_ALTCLKCFG_R for baud clock source
  SSI2_CC_R = (SSI2_CC_R&~SSI_CC_CS_M)+SSI_CC_CS_PIOSC;
                                        // the alternate clock source is the PIOSC (default)
  SYSCTL_ALTCLKCFG_R = (SYSCTL_ALTCLKCFG_R&~SYSCTL_ALTCLKCFG_ALTCLK_M)+SYSCTL_ALTCLKCFG_ALTCLK_PIOSC;
                                        // clock divider for 8 MHz SSIClk (16 MHz PIOSC/2)
  SSI2_CPSR_R = (SSI2_CPSR_R&~SSI_CPSR_CPSDVSR_M)+2;
  SSI2_CR0_R &= ~(SSI_CR0_SCR_M |       // SCR = 0 (8 Mbps data rate)
                  SSI_CR0_SPH |         // SPH = 0
                  SSI_CR0_SPO);         // SPO = 0
                                        // FRF = Freescale format
  SSI2_CR0_R = (SSI2_CR0_R&~SSI_CR0_FRF_M)+SSI_CR0_FRF_MOTO;
                                        // DSS = 8-bit data
  SSI2_CR0_R = (SSI2_CR0_R&~SSI_CR0_DSS_M)+SSI_CR0_DSS_8;
  SSI2_CR1_R |= SSI_CR1_SSE;            // enable SSI
}




void static writedata(unsigned char c) {
  while((SSI2_SR_R&SSI_SR_TNF)==0){};   // wait until transmit FIFO not full
  SSI2_DR_R = c;                        // data out
}




int main(void){
	
  PLL_Init();    // 120 MHz
	SysTick_Init();
	PortN_Init();
	SSI_Init();

	/*while(1){
		FlashLED2(1);
		writedata('P');
		SysTick_Wait10ms(10);
	}*/
	
	// 41 61 72 6f 6e 20 50 69 6e 74 6f
	uint8_t str[] = {0x41, 0x61, 0x72, 0x6f, 0x6e, 0x20, 0x50, 0x69, 0x6e, 0x74, 0x6f}; 
	int i=0;
  while(str[i]){
		FlashLED2(1);
		writedata(str[i++]);
		SysTick_Wait10ms(10);
	}
}

