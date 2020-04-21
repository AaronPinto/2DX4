/* Simplex Serial Communication Example for 2DX4


		Written by Tom Doyle
		Last Updated: March 27, 2020
*/

#include <stdint.h>
#include "tm4c1294ncpdt.h"
#include "PLL.h"
#include "SysTick.h"
#include "uart.h"
#include "onboardLEDs.h"

int main(void) {
	char i;
	PLL_Init();	
	SysTick_Init();
	onboardLEDs_Init();
	UART_Init();
	while(1){	
		for(i = 'A'; i < ('Z' + 1); i++) {			//Sends capital letters A - Z repeatedly
			UART_OutChar(i);
			FlashLED3(1);
			SysTick_Wait10ms(50);
		}
	}
}




