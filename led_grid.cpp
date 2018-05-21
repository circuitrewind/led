#include <arduino.h>
#include "led_grid.h"
#include "led_font.h"
#include "progmem.h"




uint8_t		led_grid::color_offset	= 0;
LED_RAINBOW	led_grid::color_anim	= PR_LEFT;
color_t		led_grid::color_palette[GRID_MAX];




#ifdef TEENSYDUINO
void led_grid::show() {
	begin();
	for (int y=height()-1; y>=0; y--) {
		uint16_t	row = layout[y];
		uint16_t	set = y * width();
		for (int x=0; x<width(); x++) {
			if (row & (1<<x)) {
				if (y & 0x01) {
					pixel(grid[set + ((width() - 1) - x)]);
				} else {
					pixel(grid[set + x]);
				}
			}
		}
	}
	end();
}
#endif //TEENSYDUINO




#ifdef __AVR__
void led_grid::show() {
	begin();
	for (int i=0; i<total(); i++) {
		pixel(grid[i]);
	}
	/*
	for (int y=height()-1; y>=0; y--) {
//		uint16_t	row = layout[y];
		uint16_t	set = y * width();
		for (int x=width(); x>=0; x--) {
//			if (row & (1<<x)) {
				if (y & 0x01) {
					pixel(grid[set + x]);
				} else {
					pixel(grid[set + ((width() - 1) - x)]);
				}
//			}
		}
	}
	*/
	end();
}
#endif //__AVR__




void led_grid::draw(int16_t x, int16_t y) {
	int16_t pos = index(x, y);
	if (unlikely(pos == -1)) return;

	switch (color_anim) {
		case PR_LEFT:
		case PR_RIGHT:
		case PR_LEFT_ANIM:
		case PR_RIGHT_ANIM:
			write(pos, color_palette[x]);
			break;

		case PR_TOP:
		case PR_TOP_ANIM:
		case PR_BOTTOM:
		case PR_BOTTOM_ANIM:
			write(pos, color_palette[y]);
			break;

		case PR_TOP_LEFT:
		case PR_TOP_LEFT_ANIM:
		case PR_BOTTOM_RIGHT:
		case PR_BOTTOM_RIGHT_ANIM:
			write(pos, color_palette[x+y]);
			break;

		case PR_TOP_RIGHT:
		case PR_TOP_RIGHT_ANIM:
		case PR_BOTTOM_LEFT:
		case PR_BOTTOM_LEFT_ANIM:
			write(pos, color_palette[(GRID_WIDTH-x)+y]);
			break;

		default: //DO NOTHING
			break;
	}
}




void led_grid::string(const char *_text, int16_t x_offset, int16_t y_offset) {
	const uint8_t *text = (const uint8_t *) _text;
	if (!text) return;

	while (*text) {

		if (*text < 0x21) {
			x_offset += 3;
			text++;
			continue;
		}

		uint8_t index = ((*text > 0x7E) ? 0x7F : *text) - 0x21;

		#if defined(ESP8266) || defined(ESP32) || __AVR__
			led_character item = PROGMEM_getAnything(&led_font[index]);
		#else
			led_character item = led_font[index];
		#endif

		for (int x=0; x<item.width; x++) {
			uint8_t column = item.data[x];

			for (int y=0; y<6; y++) {
				if (column & (1<<y)) {
					draw(x+x_offset, y+y_offset);
				}
			}
		}

		x_offset += item.width + 1;
		text++;
	}
}




void led_grid::string(const char *_text, int16_t x_offset, int16_t y_offset, color_t color) {
	const uint8_t *text = (const uint8_t *) _text;
	if (!text) return;

	while (*text) {

		if (*text < 0x21) {
			x_offset += 3;
			text++;
			continue;
		}

		uint8_t index = ((*text > 0x7E) ? 0x7F : *text) - 0x21;

		#if defined(ESP8266) || defined(ESP32) || __AVR__
			led_character item = PROGMEM_getAnything(&led_font[index]);
		#else
			led_character item = led_font[index];
		#endif

		for (int x=0; x<item.width; x++) {
			uint8_t column = item.data[x];

			for (int y=0; y<6; y++) {
				if (column & (1<<y)) {
					draw(x+x_offset, y+y_offset, color);
				}
			}
		}

		x_offset += item.width + 1;
		text++;
	}
}




int16_t led_grid::stringWidth(const char *_text) {
	const uint8_t *text = (const uint8_t *) _text;
	if (!text) return 0;

	int16_t x_offset = 0;
	while (*text) {

		if (*text < 0x21) {
			x_offset += 3;
			text++;
			continue;
		}

		uint8_t index = ((*text > 0x7E) ? 0x7F : *text) - 0x21;

		#if defined(ESP8266) || defined(ESP32) || __AVR__
			led_character item = PROGMEM_getAnything(&led_font[index]);
		#else
			led_character item = led_font[index];
		#endif

		x_offset += item.width + 1;
		text++;
	}

	return x_offset;
}




void led_grid::increment() {
	int16_t	total	= 0;
	int16_t	cycle	= 0;
	bool	dir		= false;


	//FIGURE OUT WHICH BASE ANIMATION WE'RE USING
	switch (color_anim) {
		case PR_NONE:
			return;

		case PR_BOTTOM:
		case PR_BOTTOM_ANIM:
			dir = true;
			//no break
		case PR_TOP:
		case PR_TOP_ANIM:
			total = GRID_HEIGHT;
			break;

		case PR_RIGHT:
		case PR_RIGHT_ANIM:
			dir = true;
			//no break
		case PR_LEFT:
		case PR_LEFT_ANIM:
			total = GRID_WIDTH;
			break;

		case PR_BOTTOM_LEFT:
		case PR_BOTTOM_LEFT_ANIM:
			dir = true;
			//no break;
		case PR_TOP_RIGHT:
		case PR_TOP_RIGHT_ANIM:
			total = GRID_WIDTH + GRID_HEIGHT - 1;
			break;

		case PR_TOP_LEFT:
		case PR_TOP_LEFT_ANIM:
			dir = true;
			//no break
		case PR_BOTTOM_RIGHT:
		case PR_BOTTOM_RIGHT_ANIM:
			total = GRID_WIDTH + GRID_HEIGHT - 1;
			break;
	}


	//IF INVALID ANIMATION SELECTED, DON'T WASTE CPU CYCLES!
	if (!total) return;


	//CHECK IF WE'RE DOING PALLETE ANIMATION OR NOT
	switch (color_anim) {
		case PR_BOTTOM_ANIM:
		case PR_TOP_ANIM:
		case PR_RIGHT_ANIM:
		case PR_LEFT_ANIM:
		case PR_TOP_RIGHT_ANIM:
		case PR_BOTTOM_RIGHT_ANIM:
		case PR_BOTTOM_LEFT_ANIM:
		case PR_TOP_LEFT_ANIM:
			cycle = ++color_offset;
			break;

		default: //DO NOTHING!
			break;
	}


	//GENERATE AND STORE THE PALLETE TO RAM
	if (dir) {
		for (int i=0; i<total; i++) {
			uint16_t hue = (256*3) / total * (i + cycle);
			color_palette[total-i-1] = color_t::hue(hue % 768);
		}
	} else {
		for (int i=0; i<total; i++) {
			uint16_t hue = (256*3) / total * (i + cycle);
			color_palette[i] = color_t::hue(hue % 768);
		}

	}
}
