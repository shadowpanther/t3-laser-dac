#include "WProgram.h"
#include "sinetable.h"
#include "laser.h"

int x=0, y=0;
volatile bool stack_full=false;
Laser laser;
IntervalTimer Timer;
void tick(void); // forward declaration

extern "C" int main(void)
{
	int count=0;

	// Status LED pin
	CORE_PIN13_DDRREG |= CORE_PIN13_BITMASK;
	CORE_PIN13_CONFIG = PORT_PCR_SRE | PORT_PCR_DSE | PORT_PCR_MUX(1);
	Timer.begin(tick, (float)33.3333333333);

	while(1) {
		CORE_PIN13_PORTSET = CORE_PIN13_BITMASK;   // LED ON
		x=sine[count];
		y=sine[(count+1024)&0x0FFF];
		count = (count+4)&0x0FFF;
		CORE_PIN13_PORTCLEAR = CORE_PIN13_BITMASK; // LED OFF
		laser.set(x, y, 255, 255, 255);
	}

	// pinMode(13, OUTPUT);
	// while (1) {
	// 	digitalWriteFast(13, HIGH);
	// 	delay(100);
	// 	digitalWriteFast(13, LOW);
	// 	delay(900);
	// }

}

void tick(void) {
	laser.tick();
}
