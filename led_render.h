#ifndef __LED_RENDER_H__
#define __LED_RENDER_H__


#include "led_array.h"


class led_render {
public:

	led_render(led_array **_list, uint16_t _total) {
		list	= _list;
		total	= _total;
	};


	virtual ~led_render() {
		total = 0;
		list = NULL;
	}


	void render();


private:
	led_array **list;
	uint16_t total;

};


#endif //__LED_RENDER_H__
