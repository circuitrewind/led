#ifndef __LED_GRID_H__
#define __LED_GRID_H__



#include "led_array.h"
#include "led_rainbow.h"
#include "optimize.h"



#define GRID_WIDTH		(5)
#define GRID_HEIGHT		(5)
#define GRID_TOTAL		(GRID_WIDTH*GRID_HEIGHT)
#define GRID_MAX		GRID_TOTAL




class led_grid : public led_array {
	public:




	////////////////////////////////////////////////////////////////////////////
	// CONSTRUCTOR
	////////////////////////////////////////////////////////////////////////////
	led_grid(	uint8_t led_pin, uint16_t led_width, uint16_t led_height,
				LED_MODE led_mode=LED_GRB, uint32_t *mask=nullptr)

			:	led_array(led_pin, led_width*led_height, led_mode),
				grid_width( led_width),
				grid_height(led_height) {


		//SET DEFAULT VALUES
		_type		= LED_GRID;
		mask_x		= 0;
		mask_y		= 0;


		//ALLOCATE MEMORY FOR MASK
		int bytes	= sizeof(uint32_t) * led_height;
		grid_mask	= (uint32_t*) malloc(bytes);
		memset(grid_mask, 0, bytes);

		//IF A MASK IS PROVIDED, USE IT
		if (mask) {
			for (auto y=0; y<led_height; y++) {
				grid_mask[y]	= (y & 0x01)
								? (reverseBits(mask[y]) >> led_width)
								: (mask[y]);
			}


		//OTHERWISE, ASSUME A SQUARE MASK
		} else {
			uint32_t fill = 0x00000000;
			for (auto x=0; x<led_width; x++) {
				fill |= (1 << x);
			}

			for (auto y=0; y<led_height; y++) {
				grid_mask[y]	= (y & 0x01)
								? (reverseBits(fill) >> led_width)
								: (fill);
			}
		}
	}




	////////////////////////////////////////////////////////////////////////////
	// DESTRUCTOR
	////////////////////////////////////////////////////////////////////////////
	virtual ~led_grid() {
		free(grid_mask);
		grid_mask = nullptr;
	}




	////////////////////////////////////////////////////////////////////////////
	// REMIND THE INTERNAL READ POINTER BACK TO FIRST LED
	////////////////////////////////////////////////////////////////////////////
	virtual void rewind() {
		mask_x = 0;
		mask_y = 0;
		led_array::rewind();
	}




	////////////////////////////////////////////////////////////////////////////
	// INCREMENT THE INTERNAL POINTER AND RETURN THE LED
	////////////////////////////////////////////////////////////////////////////
	virtual color_t next() {
		do {
			if (unlikely(mask_y >= height())) break;

			if (likely(grid_mask[mask_y] & (1 << mask_x))) {

				color_t color = read(
					(mask_y * width()) + mask_x
				);

				if (unlikely(++mask_x >= width())) {
					mask_x = 0;
					mask_y++;
				}

				return color;
			}

			if (unlikely(++mask_x >= width())) {
				mask_x = 0;
				mask_y++;
			}
		} while (true);

		return color_t::black();
	}




	////////////////////////////////////////////////////////////////////////////
	// RENGER THE LED GRID OUT TO THE PHYSICAL LED STRIP
	////////////////////////////////////////////////////////////////////////////
	void show();




	////////////////////////////////////////////////////////////////////////////
	// CONVERT X/Y GRID POSITION COORDINATE PAIR INTO A LED STRIP POSITION
	////////////////////////////////////////////////////////////////////////////
	INLINE int16_t index(int16_t x, int16_t y) const {
		if (unlikely(
			x < 0  ||  x >= width()  ||
			y < 0  ||  y >= height()
		)) return -1;

		return (y & 0x01)
			? ((y * width()) + ((width() - 1) - x))
			: ((y * width()) + x);
	}




	////////////////////////////////////////////////////////////////////////////
	// WRITE A SINGLE PIXEL VALUE INTO THE LED GRID
	////////////////////////////////////////////////////////////////////////////
	INLINE void draw(int16_t x, int16_t y, color_t color) {
		write(index(x, y), color);
	}




	////////////////////////////////////////////////////////////////////////////
	// WRITE A SINGLE RAINBOW PIXEL VALUE INTO THE LED GRID
	////////////////////////////////////////////////////////////////////////////
	void draw(int16_t x, int16_t y);




	////////////////////////////////////////////////////////////////////////////
	// DRAW A SINGLE CHARACTER ONTO THE LED GRID
	////////////////////////////////////////////////////////////////////////////
	INLINE void symbol(uint8_t c, int16_t x, int16_t y, color_t color) {
		const char z[2] = {c, 0};
		string(z, x, y, color);
	}




	////////////////////////////////////////////////////////////////////////////
	// DRAW A "C" STRING TO THE GRID
	////////////////////////////////////////////////////////////////////////////
	void string(const char *text, int16_t x_offset, int16_t y_offset);
	void string(const char *text, int16_t x_offset, int16_t y_offset, color_t color);




	////////////////////////////////////////////////////////////////////////////
	// GET THE WIDTH IN PIXELS OF A "C" STRING
	////////////////////////////////////////////////////////////////////////////
	static int16_t stringWidth(const char *text);




	////////////////////////////////////////////////////////////////////////////
	// GET THE WIDTH AND HEIGHT OF THE LED GRID
	////////////////////////////////////////////////////////////////////////////
	INLINE uint16_t width()		const { return grid_width;	}
	INLINE uint16_t height()	const { return grid_height;	}




	////////////////////////////////////////////////////////////////////////////
	// RAINBOW ANIMATOR
	////////////////////////////////////////////////////////////////////////////
	static void increment();
	static void animation(LED_RAINBOW rainbow) { color_anim = rainbow; }




protected:
	const uint16_t	grid_width;
	const uint16_t	grid_height;

	uint32_t		*grid_mask;
	uint16_t		mask_x;
	uint16_t		mask_y;

	static uint8_t		color_offset;
	static LED_RAINBOW	color_anim;
	static color_t		color_palette[GRID_MAX];
};




#endif //__LED_GRID_H__
