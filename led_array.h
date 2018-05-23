#ifndef __LED_ARRAY_H__
#define __LED_ARRAY_H__



#include "led.h"
#include "optimize.h"




class led_array : public led {
	public:



	////////////////////////////////////////////////////////////////////////////
	// CONSTRUCTOR - PASS IN THE PIN FOR THE LEDS, AS WELL AS THE LED COUNT
	////////////////////////////////////////////////////////////////////////////
	led_array(	uint8_t led_pin, uint16_t led_total,
				LED_MODE led_mode=LED_GRB)

			:	led(led_pin, led_total, led_mode) {

		_array		= (color_t*) malloc(sizeof(color_t) * led_total);
		_type		= LED_ARRAY;
		_pointer	= 0;
	}




	////////////////////////////////////////////////////////////////////////////
	// DESTRUCTOR - TURN OFF ALL LEDS AND FREE ALL OF THE LED BUFFER MEMORY
	////////////////////////////////////////////////////////////////////////////
	virtual ~led_array() {
		clear();
		show();
		free(_array);
		_array = nullptr;
	}




	////////////////////////////////////////////////////////////////////////////
	// REMIND THE INTERNAL READ POINTER BACK TO FIRST LED
	////////////////////////////////////////////////////////////////////////////
	virtual void rewind() {
		_pointer = 0;
	}




	////////////////////////////////////////////////////////////////////////////
	// INCREMENT THE INTERNAL POINTER AND RETURN THE LED
	////////////////////////////////////////////////////////////////////////////
	virtual color_t next() {
		return read(_pointer++);
	}




	////////////////////////////////////////////////////////////////////////////
	//READ A SINGLE PIXEL FROM THE LED STRIP
	////////////////////////////////////////////////////////////////////////////
	INLINE color_t read(int16_t index) const {
		return (likely(index >= 0  &&  index < total()))
			? _array[index]
			: color_t::black();
	}

	INLINE color_t read_unsafe(int16_t index) const {
		return _array[index];
	}





	////////////////////////////////////////////////////////////////////////////
	//WRITE A SINGLE PIXEL VALUE INTO THE LED STRIP
	////////////////////////////////////////////////////////////////////////////
	INLINE void write(int16_t index, color_t color) {
		if (likely(index >=0  &&  index < total())) {
			_array[index] = color;
		}
	}

	INLINE void write(int16_t index, uint8_t r, uint8_t g, uint8_t b) {
		write(index, color_t(r, g, b));
	}

	INLINE void write_unsafe(int16_t index, color_t color) {
		_array[index] = color;
	}

	INLINE void write_unsafe(int16_t index, uint8_t r, uint8_t g, uint8_t b) {
		write_unsafe(index, color_t(r, g, b));
	}




	////////////////////////////////////////////////////////////////////////////
	//WRITE A PIXEL VALUE TO THE GRID, AND RETURN THE PREVIOUSLY WRITTEN VALUE
	////////////////////////////////////////////////////////////////////////////
	INLINE color_t swap(int16_t index, color_t color) {
		color_t ret = read(index);
		write(index, color);
		return ret;
	}

	INLINE color_t swap_unsafe(int16_t index, color_t color) {
		color_t ret = read_unsafe(index);
		write_unsafe(index, color);
		return ret;
	}




	////////////////////////////////////////////////////////////////////////////
	//RENDER LED ARRAY OUT TO GRID
	////////////////////////////////////////////////////////////////////////////
	void show() {
		begin();
		for (int16_t i=0; i<total(); i++) {
			pixel(read_unsafe(i));
		}
		end();
	}




	////////////////////////////////////////////////////////////////////////////
	//CLEAR THE ENTIRE ARRAY
	////////////////////////////////////////////////////////////////////////////
	INLINE void clear() { fill(color_t::black()); }




	////////////////////////////////////////////////////////////////////////////
	//FILL THE ENTIRE STRIP WITH A SINGLE COLOR
	////////////////////////////////////////////////////////////////////////////
	void fill(color_t color) {
		for (int16_t i=0; i<total(); i++) write_unsafe(i, color);
	}




	////////////////////////////////////////////////////////////////////////////
	//FADE THE BRIGHTNESS TO A DARKER LEVEL
	////////////////////////////////////////////////////////////////////////////
	void fade(uint8_t amount = 0x80) {
		if (amount == 0x00) amount = 0x80;

		for (int16_t i=0; i<total(); i++) {
			color_t read = read_unsafe(i);
			color_t write;

			for (uint8_t x=0; x<8; x++) {
				if (amount & (0x80 >> x)) {
					write.r += (read.r >> (x+1));
					write.g += (read.g >> (x+1));
					write.b += (read.b >> (x+1));
				}
			}

			write_unsafe(i, write);
		}
	}




	////////////////////////////////////////////////////////////////////////////
	// RUN COLOR_T::ADD() ON ALL LEDS IN STRIP
	////////////////////////////////////////////////////////////////////////////
	INLINE void add(uint8_t x) { add(color_t(x, x, x)); }
	INLINE void add(uint8_t r, uint8_t g, uint8_t b) { add(color_t(r, g, b)); }

	INLINE void add(color_t color) {
		for (auto i=0; i<total(); i++) {
			_array[i].add(color);
		}
	}




	////////////////////////////////////////////////////////////////////////////
	// RUN COLOR_T::ADD() ON ALL LEDS IN STRIP
	////////////////////////////////////////////////////////////////////////////
	INLINE void sub(uint8_t x) { sub(color_t(x, x, x)); }
	INLINE void sub(uint8_t r, uint8_t g, uint8_t b) { sub(color_t(r, g, b)); }

	INLINE void sub(color_t color) {
		for (auto i=0; i<total(); i++) {
			_array[i].sub(color);
		}
	}




	////////////////////////////////////////////////////////////////////////////
	// RUN COLOR_T::SCREEN() ON ALL LEDS IN STRIP
	////////////////////////////////////////////////////////////////////////////
	INLINE void screen(uint8_t x) { screen(color_t(x, x, x)); }
	INLINE void screen(uint8_t r, uint8_t g, uint8_t b) { screen(color_t(r, g, b)); }

	INLINE void screen(color_t color) {
		for (auto i=0; i<total(); i++) {
			_array[i].screen(color);
		}
	}




	////////////////////////////////////////////////////////////////////////////
	// RUN COLOR_T::MULTIPLY() ON ALL LEDS IN STRIP
	////////////////////////////////////////////////////////////////////////////
	INLINE void multiply(uint8_t x) { multiply(color_t(x, x, x)); }
	INLINE void multiply(uint8_t r, uint8_t g, uint8_t b) { multiply(color_t(r, g, b)); }

	INLINE void multiply(color_t color) {
		for (auto i=0; i<total(); i++) {
			_array[i].multiply(color);
		}
	}




	////////////////////////////////////////////////////////////////////////////
	//GET A POINTER TO THE RAW BINARY PIXEL DATA
	////////////////////////////////////////////////////////////////////////////
	INLINE color_t *pixels() const {
		return _array;
	}




	protected:
		color_t *_array;
		int16_t _pointer;
};




#endif //__LED_ARRAY_H__
