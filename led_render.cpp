////////////////////////////////////////////////////////////////////////////////
//INCLUDE FILES
////////////////////////////////////////////////////////////////////////////////
#include <arduino.h>
#include "led_render.h"




////////////////////////////////////////////////////////////////////////////////
//THIS FILE ONLY APPLIES TO THE ESP8266, ESP32, AND NODEMCU CHIPS/BOARDS
////////////////////////////////////////////////////////////////////////////////
#if defined(ESP8266) || defined(ESP32)




#define CYCLES_T0H		(F_CPU /	2500000)
#define CYCLES_T1H		(F_CPU /	1250000)
//#define CYCLES_T0H	(F_CPU /	2350000)
//#define CYCLES_T1H	(F_CPU /	1100000)
#define CYCLES			(F_CPU /	 800000)




////////////////////////////////////////////////////////////////////////////////
// GET THE NUMBER OF CPU CYCLES SINCE POWER ON
////////////////////////////////////////////////////////////////////////////////
static INLINE ICACHE_RAM_ATTR uint32_t esp_cycle() {
	uint32_t ccount;
	__asm__ __volatile__("rsr %0,ccount":"=a" (ccount));
	return ccount;
}




////////////////////////////////////////////////////////////////////////////////
// RENDER ALL OF THE ARRAYS AND GRIDS WITHIN A SINGLE PASS
////////////////////////////////////////////////////////////////////////////////
void ICACHE_RAM_ATTR led_render::render() {
	intr_disable();

	uint32_t	start	= 0;
	uint16_t	count	= 0;
	uint32_t	value	= 0;
	uint32_t	low		= 0;
	uint32_t	data[LED_RENDER_TOTAL];


	//FIND THE MAXIMUM NUMBER OF LEDS TO RENDER
	//ALSO, RESET EACH LED ARRAY'S INTERNAL POINTER
	for (uint16_t x=0; x<LED_RENDER_TOTAL; x++) {
		if (!list[x]) continue;

		list[x]->rewind();

		count = max(count, list[x]->total());
		value |= (1 << list[x]->pin());

		pinMode(list[x]->pin(), OUTPUT);
	}


	for (uint16_t i=0; i<count; i++) {
		for (uint16_t x=0; x<LED_RENDER_TOTAL; x++) {
			if (!list[x]) continue;

			color_t color = list[x]->next();

			data[x] = (list[x]->mode() == LED_RGB)
				? ((uint32_t)color)
				: (color.grb());
		}


		for (int32_t bit=23; bit>=0; bit--) {
			//CALCULATE WHICH CHANNELS SHOULD GO LOW
			low = 0;
			for (uint16_t x=0; x<LED_RENDER_TOTAL; x++) {
				if (!list[x]) continue;

				if (!(data[x] & (1 << bit))) {
					low |= (1 << list[x]->pin());
				}
			}

			//WRITE HIGH VALUE TO **ALL** CHANNELS
			start = esp_cycle();
			GPIO_REG_WRITE(GPIO_OUT_W1TS_ADDRESS, value);

			//PAUSE UNTIL TIME TO GO LOW, THEN WRITE THE LOW VALUE
			//ONLY WRITE TO CHANNELS WITH A **0** BIT
			while ((esp_cycle() - start) < CYCLES_T0H) {}
			GPIO_REG_WRITE(GPIO_OUT_W1TC_ADDRESS, low);

			//PAUSE AND WRITE LOW VALUE TO **ALL** CHANNELS
			while ((esp_cycle() - start) < CYCLES_T1H) {}
			GPIO_REG_WRITE(GPIO_OUT_W1TC_ADDRESS, value);

			//PAUSE DURING LOW CYCLE
			while ((esp_cycle() - start) < CYCLES) {}
		}

		//CLEAR ALL BITS
		GPIO_REG_WRITE(GPIO_OUT_W1TC_ADDRESS, value);
	}

	intr_enable();
}




#endif //defined(ESP8266) || defined(ESP32)
