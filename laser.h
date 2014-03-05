#ifndef _laser_h_
#define _laser_h_

#include <stdint.h>

#define LASER_FIFO_SIZE 16

union dac_channel_t {
	struct
	{
		uint16_t data  :12;
		uint8_t  shdn  :1 ;
		uint8_t  gain  :1 ;
		uint8_t  buf   :1 ;
		uint8_t  sel   :1 ;
	};
	uint16_t cmd;
};

struct laser_t {
	dac_channel_t x;
	dac_channel_t y;
	uint8_t r,g,b;
};

class Laser
{
public:
	Laser();
	// ~Laser();

	void set(int16_t x, int16_t y, int8_t r, int8_t g, int8_t b);
	void tick();
	bool is_fifo_empty() { return fifo_empty; }
	bool is_fifo_full() { return fifo_full; }
private:
	laser_t fifo[LASER_FIFO_SIZE];
	volatile uint8_t fifo_head;
	volatile uint8_t fifo_tail;
	volatile bool fifo_full;
	volatile bool fifo_empty;
};

// Laser laser;

#endif