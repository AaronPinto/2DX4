//*****************************************************************************
//
// Copyright (C) 2017 Texas Instruments Incorporated - http://www.ti.com/
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
//
//  Redistributions of source code must retain the above copyright
//  notice, this list of conditions and the following disclaimer.
//
//  Redistributions in binary form must reproduce the above copyright
//  notice, this list of conditions and the following disclaimer in the
//  documentation and/or other materials provided with the
//  distribution.
//
//  Neither the name of Texas Instruments Incorporated nor the names of
//  its contributors may be used to endorse or promote products derived
//  from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
//*****************************************************************************

//*****************************************************************************
//
// MSP432E4 main.c template
//
//*****************************************************************************

#include "msp.h"

int main(void)
{
    // Halt watchdog timer
	SYSCTL->RCGCWD &= ~(SYSCTL_RCGCWD_R1 | SYSCTL_RCGCWD_R0);

	// Enable GPION peripheral
	SYSCTL->RCGCGPIO |= SYSCTL_RCGCGPIO_R12;

	// Check if the peripheral access is enabled.
	while(!(SYSCTL->PRGPIO & SYSCTL_PRGPIO_R12));

	/* Enable the GPIO pin for the LED (PN0). */

	// Set drive strength
	GPION->DR2R |= BIT0;
	GPION->DR4R &= ~(BIT0);
	GPION->DR8R &= ~(BIT0);
	GPION->SLR &= ~(BIT0);
	// Set pin type
	GPION->ODR &= ~(BIT0);
	GPION->PUR &= ~(BIT0);
	GPION->PDR &= ~(BIT0);
	GPION->DEN |= BIT0;
	// Set the analog mode select register.
	GPION->AMSEL &= ~(BIT0);
	// Set the pin direction and mode.
	GPION->DIR |= BIT0;
	GPION->AFSEL &= ~(BIT0);

	uint32_t counter = 0;
	while(1)
	{
	    // Toggle GPIO pin value
	    GPION->DATA ^= BIT0;
	    for(counter = 0; counter < 200000; counter++);
	}
}
