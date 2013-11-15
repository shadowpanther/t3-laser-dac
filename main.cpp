#include "WProgram.h"
#include "sinetable.h"
#include "laser.h"

int x=0, y=0;
volatile bool stack_full=false;
Laser laser;

extern "C" int main(void)
{
	int count=0;

	// Status LED pin
	CORE_PIN13_DDRREG |= CORE_PIN13_BITMASK;
	CORE_PIN13_CONFIG = PORT_PCR_SRE | PORT_PCR_DSE | PORT_PCR_MUX(1);


	while(1) {
		// while(stack_full==true)yield();
		CORE_PIN13_PORTSET = CORE_PIN13_BITMASK;   // LED ON
		x=sine[count];
		y=sine[(count+1024)&0x0FFF];
		count = (count+1)&0x0FFF;
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

void pit0_isr(void) {
	PIT_TFLG0 = 0x01; // TIF (Timer Interrupt Flag)
	laser.tick();
}
