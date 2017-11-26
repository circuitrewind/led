#ifndef __PROGMEM_H_
#define __PROGMEM_H_




// SOURCE:
// https://arduino.stackexchange.com/questions/13545/using-progmem-to-store-array-of-structs

template <typename T> INLINE void PROGMEM_readAnything(const T *source, T &destination) {
	memcpy_P (&destination, source, sizeof(T));
}


template <typename T> INLINE T PROGMEM_getAnything(const T *source) {
	static T temp;
	memcpy_P (&temp, source, sizeof(T));
	return temp;
}




#endif //__PROGMEM_H_
