/*******************************************************************************
* SOURCE CODE BASED ON: https://github.com/bigjosh/SimpleNeoPixelDemo
********************************************************************************
* LICENSE:
* https://github.com/bigjosh/SimpleNeoPixelDemo/blob/master/licence.txt
*
* The MIT License (MIT)
*
* Copyright (c) 2015 Josh Levine
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*******************************************************************************/




////////////////////////////////////////////////////////////////////////////////
//INCLUDE FILES
////////////////////////////////////////////////////////////////////////////////
#include <arduino.h>
#include "led.h"




////////////////////////////////////////////////////////////////////////////////
//THIS FILE ONLY APPLIES TO THE ATTINY AND DIGISPARK AVR BOARDS
////////////////////////////////////////////////////////////////////////////////
#if defined ARDUINO_attiny || defined ARDUINO_AVR_DIGISPARK





////////////////////////////////////////////////////////////////////////////////
//BIT CALCULATIONS
////////////////////////////////////////////////////////////////////////////////
#define		T1H		 900	// Width of a 1 bit in ns
#define		T1L		 600	// Width of a 1 bit in ns
#define		T0H		 400	// Width of a 0 bit in ns
#define		T0L		 900	// Width of a 0 bit in ns
#define		RES		6000	// Width of the low gap between bits to cause a frame to latch

#define NS_PER_SEC		(1000000000L)
#define CYCLES_PER_SEC	(F_CPU)
#define NS_PER_CYCLE	(NS_PER_SEC / CYCLES_PER_SEC)
#define NS_TO_CYCLES(n)	((n) / NS_PER_CYCLE)




////////////////////////////////////////////////////////////////////////////////
// DYNAMIC CLOCK CALCULATIONS
// NOT NEEDED, THIS MODULE USES STATIC CALCULATIONS FOR PERFORMANCE REASONS
// THERE ISN'T ENOUGH CLOCK CYCLES TO CALCULATE THIS ON THE ATTINY CHIPS
////////////////////////////////////////////////////////////////////////////////
void led::clock() {}




////////////////////////////////////////////////////////////////////////////////
// BITBANG SEND A SINGLE BIT TO THE LED STRIP
////////////////////////////////////////////////////////////////////////////////
static INLINE void led_send_bit(const bool value) {

	if (value) {

		asm volatile (
			"sbi %[port], %[bit] \n\t"
			".rept %[onCycles] \n\t"
			"nop \n\t"
			".endr \n\t"
			"cbi %[port], %[bit] \n\t"
			".rept %[offCycles] \n\t"
			"nop \n\t"
			".endr \n\t"
			::
			[port]		"I" (_SFR_IO_ADDR(PORTB)),
			[bit]		"I" (4),
			[onCycles]	"I" (NS_TO_CYCLES(T1H) - 2),
			[offCycles]	"I" (NS_TO_CYCLES(T1L) - 2)
		);

	} else {

		asm volatile (
			"sbi %[port], %[bit] \n\t"
			".rept %[onCycles] \n\t"
			"nop \n\t"
			".endr \n\t"
			"cbi %[port], %[bit] \n\t"
			".rept %[offCycles] \n\t"
			"nop \n\t"
			".endr \n\t"
			::
			[port]		"I" (_SFR_IO_ADDR(PORTB)),
			[bit]		"I" (4),
			[onCycles]	"I" (NS_TO_CYCLES(T0H) - 2),
			[offCycles]	"I" (NS_TO_CYCLES(T0L) - 2)
		);

	}
}




////////////////////////////////////////////////////////////////////////////////
// SEND A SINGLE BYTE TO THE LED STRIP
////////////////////////////////////////////////////////////////////////////////
static INLINE void led_send_byte(const uint8_t byte) {
	for (int8_t bit=7; bit>=0; bit--) {
		led_send_bit(bitRead(byte, bit));
	}
}




////////////////////////////////////////////////////////////////////////////////
// SEND A FULL PIXEL TO THE LED STRIP
////////////////////////////////////////////////////////////////////////////////
void led::pixel(const color_t &color) {
	led_send_byte(color.g);
	led_send_byte(color.r);
	led_send_byte(color.b);
}




#endif // ARDUINO_attiny || ARDUINO_AVR_DIGISPARK
