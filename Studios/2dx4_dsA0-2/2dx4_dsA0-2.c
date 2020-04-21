/* Half-Duplex Serial Communication Example for 2DX4


		Written by Tom Doyle
		Last Updated: March 28, 2020
*/

#include <stdint.h>
#include "tm4c1294ncpdt.h"
#include "PLL.h"
#include "SysTick.h"
#include "uart.h"
#include "onboardLEDs.h"

int main(void) {
	char i;
	int input = 0, offset = 0;
	PLL_Init();	
	SysTick_Init();
	onboardLEDs_Init();
	UART_Init();
	
	input = UART_InChar();
	
	if (input == 0x31)			//ASCII for '1' is 0x31 or decimal 49
		offset = 32;				//ASCII offset between A and a
	else
		offset = 0;
	
	while(1){	
		for(i = 'A' + offset; i < ('A' + 27 + offset); i++) {			//Sends capital letters A - Z repeatedly
			UART_OutChar(i);
			FlashLED3(1);
			SysTick_Wait10ms(50);
		}
	}
}




