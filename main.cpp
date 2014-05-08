#include "WProgram.h"
#include "sinetable.h"
#include "laser.h"
#include "math.h"

int x=0, y=0;
volatile bool stack_full=false;
Laser laser;
// IntervalTimer Timer;
void tick(void); // forward declaration
void laser_line(int16_t x1, int16_t y1, int16_t x2, int16_t y2, int16_t step, int8_t r, int8_t g, int8_t b);

extern "C" int main(void)
{
	int count=0;

	// Timer.begin(tick, (float)33.3333333333);
	randomSeed(analogRead(0));

	// while(1) {
	// 	x=sine[count];
	// 	y=sine[(count+1024)&0x0FFF];
	// 	count = (count+32)&0x0FFF;
	// 	laser.set(x, y, 255, 255, 255);
	// }
	int16_t step = 20;
	int x, y, xo = 0, yo = 0;
	while (1) {
		// laser_line(2048,    0, 3072, 4095, step, 255, 255, 255);
		// laser_line(3072, 4095,    0, 1920, step, 255, 255, 255);
		// laser_line(   0, 1920, 4095, 1920, step, 255, 255, 255);
		// laser_line(4095, 1920, 1024, 4095, step, 255, 255, 255);
		// laser_line(1024, 4095, 2048,    0, step, 255, 255, 255);

		// laser_line(2000, 2000, 3000, 3000, step, 255, 255, 255);
		// laser_line(3000, 3000, 2000, 3000, step, 255, 255, 255);
		// laser_line(2000, 3000, 3000, 2000, step, 255, 255, 255);
		// laser_line(3000, 2000, 2000, 2000, step, 255, 255, 255);

		x = random(4096);
		y = random(4096);
		laser_line(xo, yo, x, y, step, 255, 255, 255);
		xo = x;
		yo = y;
	}
}

#if 1
void laser_line(int16_t x1, int16_t y1, int16_t x2, int16_t y2, int16_t step, int8_t r, int8_t g, int8_t b) {
	int dx = x2 - x1;
	int dy = y2 - y1;
	int steps = ceil(sqrtf(dx * dx + dy * dy)*16/step);
	int x, y;
	int speed = 0;
	for(int count = 0; count <= steps; count+=speed) {
		if (count <= 136 && count < steps/2) speed++;
		if (count >= steps - 136 && count > steps/2) speed--;
		if (speed==0) speed = 1;
		x = count * (x2 - x1) / steps + x1;
		y = count * (y2 - y1) / steps + y1;
		laser.set(x, y, r, g, b);
	}
}
#else
void laser_line(int16_t x1, int16_t y1, int16_t x2, int16_t y2, int16_t step, int8_t r, int8_t g, int8_t b) {
	int16_t dx = abs(x2 - x1);
	int16_t dy = abs(y2 - y1);
	bool x_is_bigger = dx > dy;
	int8_t dirx = (x2 - x1) >= 0 ? 1 : -1;
	int8_t diry = (y2 - y1) >= 0 ? 1 : -1;

	int16_t steps = ceil(sqrtf((int32_t)dx * dx + (int32_t)dy * dy)/step);

	int16_t sx = floor((float)dx / steps);
	int16_t sy = floor((float)dy / steps);
	int16_t cx = dx - sx * steps;
	int16_t cy = dy - sy * steps;

	int16_t ex = 0;
	int16_t ey = 0;

	int16_t x = x1;
	int16_t y = y1;
	laser.set(x, y, r, g, b);

	bool run = true;

	while (run) {
		if (dirx > 0) {
			x += sx;
			ex += cx;
			if (ex > steps) {
				x++;
				ex -= steps;
			}
			if (x >= x2 && x_is_bigger) {
				run = false;
				x = x2;
			}
		} else {
			x -= sx;
			ex += cx;
			if (ex > steps) {
				x--;
				ex -= steps;
			}
			if (x <= x2 && x_is_bigger) {
				run = false;
				x = x2;
			}
		}
		if (diry > 0) {
			y += sy;
			ey += cy;
			if (ey > steps) {
				y++;
				ey -= steps;
			}
			if (y >= y2 && !x_is_bigger) {
				run = false;
				y = y2;
			}
		} else {
			y -= sy;
			ey += cy;
			if (ey > steps) {
				y--;
				ey -= steps;
			}
			if (y <= y2 && !x_is_bigger) {
				run = false;
				y = y2;
			}
		}
		// CORE_PIN13_PORTSET = CORE_PIN13_BITMASK;   // LED ON
		laser.set(x, y, r, g, b);
		// CORE_PIN13_PORTCLEAR = CORE_PIN13_BITMASK; // LED OFF
	}
}
#endif

void tick(void) {
	laser.tick();
}
