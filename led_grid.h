#ifndef __LED_GRID_H__
#define __LED_GRID_H__



#include "led_array.h"
#include "led_rainbow.h"


#define GRID_WIDTH (5)
#define GRID_HEIGHT (5)
#define GRID_TOTAL (GRID_WIDTH*GRID_HEIGHT)
#define GRID_MAX GRID_TOTAL




class led_grid : public led_array {
public:
	led_grid(uint8_t led_pin, uint16_t led_width, uint16_t led_height, color_t *led_buffer=NULL)
		:	led_array(led_pin, led_width*led_height),
			grid_width( led_width),
			grid_height(led_height) {

		grid	= led_buffer
				? led_buffer
				: (color_t*) malloc(total() * sizeof(color_t));
	}




	////////////////////////////////////////////////////////////////////////////
	//RENGER THE LED GRID OUT TO THE PHYSICAL LED STRIP
	////////////////////////////////////////////////////////////////////////////
	void show();




	////////////////////////////////////////////////////////////////////////////
	//CONVERT X/Y GRID POSITION COORDINATE PAIR INTO A LED STRIP POSITION
	////////////////////////////////////////////////////////////////////////////
	/*
	INLINE int16_t index(int16_t x, int16_t y) const {
		if (x < 0  ||  x >= width()  ||  y < 0  ||  y >= height()) return -1;
		return (y * width()) + x;
	}
	*/
	INLINE int16_t index(int16_t x, int16_t y) const {
		if (x < 0  ||  x >= width()  ||  y < 0  ||  y >= height()) return -1;

		return (y & 0x01)
			? ((y * width()) + ((width() - 1) - x))
			: ((y * width()) + x);
	}





	////////////////////////////////////////////////////////////////////////////
	//WRITE A SINGLE PIXEL VALUE INTO THE LED GRID
	////////////////////////////////////////////////////////////////////////////
	INLINE void draw(int16_t x, int16_t y, color_t color) {
		write(index(x, y), color);
	}




	////////////////////////////////////////////////////////////////////////////
	//WRITE A SINGLE RAINBOW PIXEL VALUE INTO THE LED GRID
	////////////////////////////////////////////////////////////////////////////
	void draw(int16_t x, int16_t y);




	////////////////////////////////////////////////////////////////////////////
	//DRAW A SINGLE CHARACTER ONTO THE LED GRID
	////////////////////////////////////////////////////////////////////////////
	INLINE void symbol(uint8_t c, int16_t x, int16_t y, color_t color) {
		const char z[2] = {c, 0};
		string(z, x, y, color);
	}




	void string(const char *text, int16_t x_offset, int16_t y_offset);
	void string(const char *text, int16_t x_offset, int16_t y_offset, color_t color);
	static int16_t stringWidth(const char *text);





	////////////////////////////////////////////////////////////////////////////
	//GET THE WIDTH AND HEIGHT OF THE LED GRID
	////////////////////////////////////////////////////////////////////////////
	INLINE uint16_t width()		const { return grid_width;	}
	INLINE uint16_t height()	const { return grid_height;	}




	////////////////////////////////////////////////////////////////////////////
	//RAINBOW ANIMATOR
	////////////////////////////////////////////////////////////////////////////
	static void increment();
	static void animation(LED_RAINBOW rainbow) { color_anim = rainbow; }




protected:
	const uint16_t	grid_width;
	const uint16_t	grid_height;
	color_t			*grid;

	static uint8_t		color_offset;
	static LED_RAINBOW	color_anim;
	static color_t		color_palette[GRID_MAX];
};




#endif //__LED_GRID_H__
