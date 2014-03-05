#include "WProgram.h"
#include "sinetable.h"
#include "laser.h"
#include "math.h"

int x=0, y=0;
volatile bool stack_full=false;
Laser laser;
IntervalTimer Timer;
void tick(void); // forward declaration
void laser_line(int16_t x1, int16_t y1, int16_t x2, int16_t y2, int16_t step, int8_t r, int8_t g, int8_t b);

extern "C" int main(void)
{
	int count=0;

	Timer.begin(tick, (float)33.3333333333);

	// while(1) {
	// 	CORE_PIN13_PORTSET = CORE_PIN13_BITMASK;   // LED ON
	// 	x=sine[count];
	// 	y=sine[(count+1024)&0x0FFF];
	// 	count = (count+4)&0x0FFF;
	// 	CORE_PIN13_PORTCLEAR = CORE_PIN13_BITMASK; // LED OFF
	// 	laser.set(x, y, 255, 255, 255);
	// }
	int16_t step = 10;
	while (1) {
		laser_line(2048,    0, 3072, 4095, step, 255, 255, 255);
		laser_line(3072, 4095,    0, 1920, step, 255, 255, 255);
		laser_line(   0, 1920, 4095, 1920, step, 255, 255, 255);
		laser_line(4095, 1920, 1024, 4095, step, 255, 255, 255);
		laser_line(1024, 4095, 2048,    0, step, 255, 255, 255);
	}

	// pinMode(13, OUTPUT);
	// while (1) {
	// 	digitalWriteFast(13, HIGH);
	// 	delay(100);
	// 	digitalWriteFast(13, LOW);
	// 	delay(900);
	// }

}

#if 1
void laser_line(int16_t x1, int16_t y1, int16_t x2, int16_t y2, int16_t step, int8_t r, int8_t g, int8_t b) {
	int dx = x2 - x1;
	int dy = y2 - y1;
	int steps = ceil(sqrtf(dx * dx + dy * dy)/step);
	int x, y;
	for(int count = 0; count <= steps; count++) {
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
			if (ex > steps) x++;
			if (x >= x2 && x_is_bigger) {
				run = false;
				x = x2;
			}
		} else {
			x -= sx;
			ex += cx;
			if (ex > steps) x--;
			if (x <= x2 && x_is_bigger) {
				run = false;
				x = x2;
			}
		}
		if (diry > 0) {
			y += sy;
			ey += cy;
			if (ey > steps) y++;
			if (y >= y2 && !x_is_bigger) {
				run = false;
				y = y2;
			}
		} else {
			y -= sy;
			ey += cy;
			if (ey > steps) y--;
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
