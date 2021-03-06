/*
	!gif header 
http://london.harekiet.com/compo/header.txt
*/

#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include <math.h>
//#include <stdint.h>
//#include <unistd.h>
#include <time.h>
//#include <cmath>
#include <climits>
#include <cctype>
#include <cfloat>
#include <vector>
#include <set>
#include <list>
#include <string>
#include <algorithm>
#include <iostream>
#include <complex>
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;

using namespace std;

enum {
        FRAMES = 64,
        WIDTH = 320,
        HEIGHT = 240,
        W = WIDTH,
        H = HEIGHT,
        S = W*H,
};

typedef uint8_t U8;
typedef uint16_t U16;
typedef uint32_t U32;
typedef double D;
typedef int I;
typedef unsigned int UI;

/*
	Main buffer object, this gets saved to a gif!
*/

class Buffer {
	uint8_t data[S];
	uint8_t outside;
	int index;
public:
	Buffer() {
		index = 0;
		c();
	}

	void operator= (int input ) {
		data[ index++ % sizeof(data) ] = input;
	}	
	void operator* (int i ) {
		i = (i >= 0 ) ? ( (i <= 255 ) ? i : ( 255 )) : 0;
		data[ index++ % sizeof(data) ] = i;
	}	
	
	//Clear the buffer
	void c( int v = 0) {
		memset( data, v, sizeof( data ) );
	}

	//Allow direct access
	uint8_t* operator[] ( int index ) {
		return data + index * WIDTH;
	}
	
	uint8_t& operator() (int y, int x) {
		if ( y>=0 && y<H && x>=0 && x<W) {
			return data[y*WIDTH+x];
		} else {
		    return outside;
		}
	}

	uint8_t& i(int y, int x) {
		if ( y>=0 && y<H && x>=0 && x<W) {
			return data[y*WIDTH+x];
		} else {
		    return outside;
		}
	}

	//Acccess through -1,1 ranges
	uint8_t& d( D y, D x) {
	    	return i((y+1)*H*0.5+0.5,((x*H)+W)*0.5+0.5);
	}	

	uint8_t& operator() (int o) {
		return data[o];
	}	

	uint8_t* Get() {
		return data;
	}	
	
	operator uint8_t*() {
		return data;
	}
};

/*
	Palette objbect, defaults to vga table somewhere else
*/

class Palette {
	int index;
	uint8_t data[768];
public:
	Palette() {
		index = 0;
		G();
	}
	
	//Make a grey scale
	void G() {
		for( int i = 0;i<256;i++ ) {
			data[i*3+0]=i;
			data[i*3+1]=i;
			data[i*3+2]=i;
		}
	}
	
	//Allow a single palette entry to be set and forward the index
	int operator= (int input ) {
		data[ index++ % 768 ] = input;
		return input;
	}

	//Allow floats from 0-1 as well...
	double operator*(double input ) {
		data[ index++ % 768 ] = (255*input + 0.5);
		return input;
	}

	uint8_t* operator[] (int index) {
		return data + (index &0xff) * 3;
	}

	//Access to save it
	uint8_t* Get() {
		return data;
	}
};

extern Buffer B;
extern Palette P;

//Save a frame, more than FRAMES will exit();
extern void SaveFrame(bool clear);

#define F SaveFrame(false)
#define C SaveFrame(true)
#define PG P.G()
#define X(_Y_,_X_) B.d(_Y_,_X_)

template<typename T>
void ROT( T&x, T&y, double a) {
    T ca = cos(a); T sa = sin(a);
    T u = x * ca - y * sa;
    T v = x * sa + y * ca;
    x = u; y = v;
}

#define L2 for(D y=-1;y<1;y+=(2.0/H))for(D x=-1/0.75;x<1/0.75;x+=(2/0.75)/W)

#define L3 for(I iy=0;iy<W;iy++)for(I ix=0;ix<W;ix++){ D y=2*iy/H, x=2*ix/W;

//y=2*iy/W;
#define L D x=-1,y=-1/0.75;for(I iy=0;iy<H;iy++,y=-1+2.0*iy/H)for(I ix=0;ix<W;ix++,x=-1/0.75+2.0*ix/H)

#ifndef M_PI
#    define M_PI 3.14159265358979323846
#endif

#ifndef PI
#    define PI 3.14159265358979323846
#endif

//0xFFA6E
#define f8 DOSCHAR
unsigned char DOSCHAR[256*8]={
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,

//space char starts here
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x18,0x18,0x18,0x18,0x00,0x00,0x18,0x00,0x66,0x66,0x66,0x00,0x00,0x00,0x00,0x00,0x66,0x66,0xFF,0x66,0xFF,0x66,0x66,0x00,
0x18,0x3E,0x60,0x3C,0x06,0x7C,0x18,0x00,0x62,0x66,0x0C,0x18,0x30,0x66,0x46,0x00,0x3C,0x66,0x3C,0x38,0x67,0x66,0x3F,0x00,0x06,0x0C,0x18,0x00,0x00,0x00,0x00,0x00,
0x0C,0x18,0x30,0x30,0x30,0x18,0x0C,0x00,0x30,0x18,0x0C,0x0C,0x0C,0x18,0x30,0x00,0x00,0x66,0x3C,0xFF,0x3C,0x66,0x00,0x00,0x00,0x18,0x18,0x7E,0x18,0x18,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x18,0x18,0x30,0x00,0x00,0x00,0x7E,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x18,0x18,0x00,0x00,0x03,0x06,0x0C,0x18,0x30,0x60,0x00,
0x3C,0x66,0x6E,0x76,0x66,0x66,0x3C,0x00,0x18,0x18,0x38,0x18,0x18,0x18,0x7E,0x00,0x3C,0x66,0x06,0x0C,0x30,0x60,0x7E,0x00,0x3C,0x66,0x06,0x1C,0x06,0x66,0x3C,0x00,
0x06,0x0E,0x1E,0x66,0x7F,0x06,0x06,0x00,0x7E,0x60,0x7C,0x06,0x06,0x66,0x3C,0x00,0x3C,0x66,0x60,0x7C,0x66,0x66,0x3C,0x00,0x7E,0x66,0x0C,0x18,0x18,0x18,0x18,0x00,
0x3C,0x66,0x66,0x3C,0x66,0x66,0x3C,0x00,0x3C,0x66,0x66,0x3E,0x06,0x66,0x3C,0x00,0x00,0x00,0x18,0x00,0x00,0x18,0x00,0x00,0x00,0x00,0x18,0x00,0x00,0x18,0x18,0x30,
0x0E,0x18,0x30,0x60,0x30,0x18,0x0E,0x00,0x00,0x00,0x7E,0x00,0x7E,0x00,0x00,0x00,0x70,0x18,0x0C,0x06,0x0C,0x18,0x70,0x00,0x3C,0x66,0x06,0x0C,0x18,0x00,0x18,0x00,
0x3C,0x66,0x6E,0x6E,0x60,0x62,0x3C,0x00,0x18,0x3C,0x66,0x7E,0x66,0x66,0x66,0x00,0x7C,0x66,0x66,0x7C,0x66,0x66,0x7C,0x00,0x3C,0x66,0x60,0x60,0x60,0x66,0x3C,0x00,
0x78,0x6C,0x66,0x66,0x66,0x6C,0x78,0x00,0x7E,0x60,0x60,0x78,0x60,0x60,0x7E,0x00,0x7E,0x60,0x60,0x78,0x60,0x60,0x60,0x00,0x3C,0x66,0x60,0x6E,0x66,0x66,0x3C,0x00,
0x66,0x66,0x66,0x7E,0x66,0x66,0x66,0x00,0x3C,0x18,0x18,0x18,0x18,0x18,0x3C,0x00,0x1E,0x0C,0x0C,0x0C,0x0C,0x6C,0x38,0x00,0x66,0x6C,0x78,0x70,0x78,0x6C,0x66,0x00,
0x60,0x60,0x60,0x60,0x60,0x60,0x7E,0x00,0x63,0x77,0x7F,0x6B,0x63,0x63,0x63,0x00,0x66,0x76,0x7E,0x7E,0x6E,0x66,0x66,0x00,0x3C,0x66,0x66,0x66,0x66,0x66,0x3C,0x00,
0x7C,0x66,0x66,0x7C,0x60,0x60,0x60,0x00,0x3C,0x66,0x66,0x66,0x66,0x3C,0x0E,0x00,0x7C,0x66,0x66,0x7C,0x78,0x6C,0x66,0x00,0x3C,0x66,0x60,0x3C,0x06,0x66,0x3C,0x00,
0x7E,0x18,0x18,0x18,0x18,0x18,0x18,0x00,0x66,0x66,0x66,0x66,0x66,0x66,0x3C,0x00,0x66,0x66,0x66,0x66,0x66,0x3C,0x18,0x00,0x63,0x63,0x63,0x6B,0x7F,0x77,0x63,0x00,
0x66,0x66,0x3C,0x18,0x3C,0x66,0x66,0x00,0x66,0x66,0x66,0x3C,0x18,0x18,0x18,0x00,0x7E,0x06,0x0C,0x18,0x30,0x60,0x7E,0x00,0x3C,0x30,0x30,0x30,0x30,0x30,0x3C,0x00,
0x00,0x60,0x30,0x18,0x0C,0x06,0x03,0x00,0x3C,0x0C,0x0C,0x0C,0x0C,0x0C,0x3C,0x00,0x00,0x18,0x3C,0x7E,0x18,0x18,0x18,0x18,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,
0x38,0x78,0xC7,0xC0,0xC7,0xE4,0x78,0x38,0x00,0x00,0x3C,0x06,0x3E,0x66,0x3E,0x00,0x00,0x60,0x60,0x7C,0x66,0x66,0x7C,0x00,0x00,0x00,0x3C,0x60,0x60,0x60,0x3C,0x00,
0x00,0x06,0x06,0x3E,0x66,0x66,0x3E,0x00,0x00,0x00,0x3C,0x66,0x7E,0x60,0x3C,0x00,0x00,0x0E,0x18,0x3E,0x18,0x18,0x18,0x00,0x00,0x00,0x3E,0x66,0x66,0x3E,0x06,0x7C,
0x00,0x60,0x60,0x7C,0x66,0x66,0x66,0x00,0x00,0x18,0x00,0x38,0x18,0x18,0x3C,0x00,0x00,0x06,0x00,0x06,0x06,0x06,0x06,0x3C,0x00,0x60,0x60,0x6C,0x78,0x6C,0x66,0x00,
0x00,0x38,0x18,0x18,0x18,0x18,0x3C,0x00,0x00,0x00,0x66,0x7F,0x7F,0x6B,0x63,0x00,0x00,0x00,0x7C,0x66,0x66,0x66,0x66,0x00,0x00,0x00,0x3C,0x66,0x66,0x66,0x3C,0x00,
0x00,0x00,0x7C,0x66,0x66,0x7C,0x60,0x60,0x00,0x00,0x3E,0x66,0x66,0x3E,0x06,0x06,0x00,0x00,0x7C,0x66,0x60,0x60,0x60,0x00,0x00,0x00,0x3E,0x60,0x3C,0x06,0x7C,0x00,
0x00,0x18,0x7E,0x18,0x18,0x18,0x0E,0x00,0x00,0x00,0x66,0x66,0x66,0x66,0x3E,0x00,0x00,0x00,0x66,0x66,0x66,0x3C,0x18,0x00,0x00,0x00,0x63,0x6B,0x7F,0x3E,0x36,0x00,
0x00,0x00,0x66,0x3C,0x18,0x3C,0x66,0x00,0x00,0x00,0x66,0x66,0x66,0x3E,0x0C,0x78,0x00,0x00,0x7E,0x0C,0x18,0x30,0x7E,0x00,0x1C,0x30,0x30,0x70,0x30,0x30,0x1C,0x00,
0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x38,0x0C,0x0C,0x0E,0x0C,0x0C,0x38,0x00,0x00,0x00,0x00,0x1A,0x2C,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0C,0x18,0x30,0x00,0x0E,0x18,0x3E,0x18,0x18,0x18,0x70,
0x00,0x00,0x00,0x00,0x00,0x66,0x66,0x66,0x00,0x00,0x00,0x00,0x00,0x54,0x54,0x00,0x08,0x3E,0x08,0x08,0x08,0x08,0x08,0x00,0x08,0x3E,0x08,0x08,0x08,0x3E,0x08,0x00,
0x18,0x24,0x00,0x00,0x00,0x00,0x00,0x00,0x62,0x66,0x0C,0x18,0x30,0x65,0x45,0x00,0x1C,0x3C,0x62,0x3C,0x06,0x66,0x3C,0x00,0x00,0x08,0x10,0x20,0x10,0x08,0x00,0x00,
0x3F,0x68,0x68,0x6C,0x68,0x68,0x3F,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x30,0x30,0x18,0x00,0x00,0x00,0x00,0x00,0x18,0x18,0x30,0x00,0x00,0x00,0x00,0x00,0x36,0x6C,0x6C,0x00,0x00,0x00,0x00,0x00,
0x36,0x36,0x6C,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x18,0x3C,0x3C,0x18,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x3C,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x7E,0x00,0x00,
0x1A,0x2C,0x00,0x00,0x00,0x00,0x00,0x00,0xF1,0x5B,0x55,0x55,0x00,0x00,0x00,0x00,0x14,0x08,0x3E,0x60,0x3C,0x06,0x7C,0x00,0x00,0x10,0x08,0x04,0x08,0x10,0x00,0x00,
0x00,0x00,0x3E,0x6B,0x6F,0x68,0x3E,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x24,0x00,0x66,0x66,0x3C,0x18,0x18,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x18,0x00,0x00,0x18,0x18,0x18,0x18,0x00,0x18,0x3C,0x60,0x60,0x60,0x3C,0x18,
0x0C,0x12,0x30,0x7C,0x30,0x62,0xFC,0x00,0x00,0x2A,0x1C,0x36,0x1C,0x2A,0x00,0x00,0x42,0x24,0x18,0x3C,0x18,0x3C,0x18,0x00,0x18,0x18,0x18,0x00,0x00,0x18,0x18,0x18,
0x3E,0x60,0x3C,0x76,0x3C,0x06,0x7C,0x00,0x24,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x3C,0x42,0x99,0xA1,0xA1,0x99,0x42,0x3C,0x3E,0x66,0x3E,0x00,0x7E,0x00,0x00,0x00,
0x00,0x12,0x24,0x48,0x24,0x12,0x00,0x00,0x00,0x00,0x00,0x7F,0x01,0x01,0x01,0x00,0x00,0x00,0x00,0x3C,0x00,0x00,0x00,0x00,0x3C,0x42,0xB9,0xA5,0xB9,0xA5,0x42,0x3C,
0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1C,0x14,0x1C,0x00,0x00,0x00,0x00,0x00,0x18,0x18,0x7E,0x18,0x18,0x00,0x7E,0x00,0x7C,0x06,0x3C,0x60,0x7E,0x00,0x00,0x00,
0x7C,0x06,0x1C,0x06,0x7C,0x00,0x00,0x00,0x08,0x10,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x66,0x66,0x66,0x66,0x7A,0x40,0x3F,0x6A,0x6A,0x2A,0x0A,0x0A,0x0A,0x00,
0x00,0x00,0x00,0x18,0x18,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x18,0x38,0x18,0x38,0x18,0x18,0x7E,0x00,0x00,0x00,0x3C,0x66,0x3C,0x00,0x7E,0x00,0x00,0x00,
0x00,0x48,0x24,0x12,0x24,0x48,0x00,0x00,0x40,0x42,0x45,0x4B,0x15,0x2F,0x41,0x00,0x40,0x42,0x44,0x4A,0x11,0x22,0x43,0x00,0x40,0x22,0x65,0x2B,0x55,0x2F,0x41,0x00,
0x00,0x18,0x00,0x18,0x30,0x60,0x66,0x3C,0x10,0x08,0x18,0x3C,0x66,0x7E,0x66,0x00,0x08,0x10,0x18,0x3C,0x66,0x7E,0x66,0x00,0x18,0x24,0x18,0x3C,0x66,0x7E,0x66,0x00,
0x1A,0x2C,0x18,0x3C,0x66,0x7E,0x66,0x00,0x24,0x00,0x18,0x3C,0x66,0x7E,0x66,0x00,0x18,0x00,0x18,0x3C,0x66,0x7E,0x66,0x00,0x1F,0x3C,0x6C,0x7E,0x6C,0x6C,0x6F,0x00,
0x3C,0x66,0x60,0x60,0x66,0x3C,0x18,0x38,0x10,0x08,0x7E,0x60,0x78,0x60,0x7E,0x00,0x08,0x10,0x7E,0x60,0x78,0x60,0x7E,0x00,0x18,0x24,0x7E,0x60,0x78,0x60,0x7E,0x00,
0x24,0x00,0x7E,0x60,0x78,0x60,0x7E,0x00,0x10,0x08,0x3C,0x18,0x18,0x18,0x3C,0x00,0x08,0x10,0x3C,0x18,0x18,0x18,0x3C,0x00,0x18,0x24,0x3C,0x18,0x18,0x18,0x3C,0x00,
0x24,0x00,0x3C,0x18,0x18,0x18,0x3C,0x00,0x3C,0x36,0x33,0x7B,0x33,0x36,0x3C,0x00,0x1A,0x2C,0x66,0x76,0x7E,0x6E,0x66,0x00,0x10,0x08,0x3C,0x66,0x66,0x66,0x3C,0x00,
0x08,0x10,0x3C,0x66,0x66,0x66,0x3C,0x00,0x18,0x24,0x3C,0x66,0x66,0x66,0x3C,0x00,0x1A,0x2C,0x3C,0x66,0x66,0x66,0x3C,0x00,0x24,0x00,0x3C,0x66,0x66,0x66,0x3C,0x00,
0x00,0x00,0x42,0x24,0x18,0x24,0x42,0x00,0x3D,0x66,0x66,0x6E,0x76,0x66,0x7C,0x00,0x10,0x08,0x66,0x66,0x66,0x66,0x3C,0x00,0x08,0x10,0x66,0x66,0x66,0x66,0x3C,0x00,
0x18,0x24,0x00,0x66,0x66,0x66,0x3C,0x00,0x24,0x00,0x66,0x66,0x66,0x66,0x3C,0x00,0x08,0x10,0x66,0x66,0x3C,0x18,0x18,0x00,0x60,0x7C,0x66,0x66,0x7C,0x60,0x60,0x00,
0x3C,0x66,0x66,0x6C,0x66,0x66,0x6C,0x00,0x10,0x08,0x3C,0x06,0x3E,0x66,0x3E,0x00,0x08,0x10,0x3C,0x06,0x3E,0x66,0x3E,0x00,0x18,0x24,0x3C,0x06,0x3E,0x66,0x3E,0x00,
0x1A,0x2C,0x3C,0x06,0x3E,0x66,0x3E,0x00,0x24,0x00,0x3C,0x06,0x3E,0x66,0x3E,0x00,0x18,0x00,0x3C,0x06,0x3E,0x66,0x3E,0x00,0x00,0x00,0x3E,0x0B,0x3F,0x68,0x3E,0x00,
0x00,0x00,0x3C,0x60,0x60,0x3C,0x18,0x38,0x10,0x08,0x3C,0x66,0x7E,0x60,0x3C,0x00,0x08,0x10,0x3C,0x66,0x7E,0x60,0x3C,0x00,0x18,0x24,0x3C,0x66,0x7E,0x60,0x3C,0x00,
0x24,0x00,0x3C,0x66,0x7E,0x60,0x3C,0x00,0x10,0x08,0x00,0x38,0x18,0x18,0x3C,0x00,0x08,0x10,0x00,0x38,0x18,0x18,0x3C,0x00,0x18,0x24,0x00,0x38,0x18,0x18,0x3C,0x00,
0x24,0x00,0x00,0x38,0x18,0x18,0x3C,0x00,0x06,0x0F,0x06,0x3E,0x66,0x66,0x3E,0x00,0x1A,0x2C,0x00,0x7C,0x66,0x66,0x66,0x00,0x10,0x08,0x00,0x3C,0x66,0x66,0x3C,0x00,
0x08,0x10,0x00,0x3C,0x66,0x66,0x3C,0x00,0x18,0x24,0x00,0x3C,0x66,0x66,0x3C,0x00,0x1A,0x2C,0x00,0x3C,0x66,0x66,0x3C,0x00,0x24,0x00,0x00,0x3C,0x66,0x66,0x3C,0x00,
0x00,0x18,0x00,0x7E,0x00,0x18,0x00,0x00,0x00,0x00,0x3C,0x66,0x6E,0x76,0x3C,0x00,0x10,0x08,0x00,0x66,0x66,0x66,0x3E,0x00,0x08,0x10,0x00,0x66,0x66,0x66,0x3E,0x00,
0x18,0x24,0x00,0x66,0x66,0x66,0x3E,0x00,0x24,0x00,0x66,0x66,0x66,0x66,0x3E,0x00,0x08,0x10,0x66,0x66,0x66,0x3E,0x0C,0x78,0x00,0x00,0x60,0x7C,0x66,0x7C,0x60,
//count=256
};
