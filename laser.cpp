#include "laser.h"
#include "mk20dx128.h"
#include "core_pins.h"
#include "wiring.h"
#include "IntervalTimer.h"

IntervalTimer Timer;

Laser::Laser(){
#ifdef LASER_DEBUG
	// Status LED pin
	CORE_PIN13_DDRREG |= CORE_PIN13_BITMASK;
	CORE_PIN13_CONFIG = PORT_PCR_SRE | PORT_PCR_DSE | PORT_PCR_MUX(1);
#endif

	// Init SPI
	SIM_SCGC6 |= SIM_SCGC6_SPI0; // Clock to SPI module enable
	CORE_PIN2_CONFIG = PORT_PCR_DSE | PORT_PCR_MUX(2); // Setup pins CS
	CORE_PIN7_CONFIG = PORT_PCR_DSE | PORT_PCR_MUX(2); //            MOSI
	CORE_PIN8_CONFIG = PORT_PCR_MUX(2);                //            MISO
	CORE_PIN14_CONFIG = PORT_PCR_DSE | PORT_PCR_MUX(2);//            SCK
	SPI0_CTAR0 = SPI_CTAR_FMSZ(15) | SPI_CTAR_PBR(0) | SPI_CTAR_BR(0) | SPI_CTAR_CSSCK(0); // SPI mode setup
	SPI0_MCR |= SPI_MCR_MDIS | SPI_MCR_HALT; // SPI stop
	SPI0_MCR |= SPI_MCR_DCONF(0) | SPI_MCR_MSTR | SPI_MCR_PCSIS(31); // SPI config
	SPI0_MCR &= ~(SPI_MCR_MDIS | SPI_MCR_HALT); // SPI start

	Timer.begin((void(*)())&Laser::tick, (float)33.3333333333);

	// // Init PIT (Periodic Interrupt Timer)
	// SIM_SCGC6 |= SIM_SCGC6_PIT; // Clock to PIT enable
	// NVIC_ENABLE_IRQ(IRQ_PIT_CH0);
	// PIT_MCR = 0; // PIT enable
	// PIT_TCTRL0 = 0;
	// PIT_LDVAL0 = 1600; // Clock interval
	// PIT_TFLG0 = 0x01; // TIF (Timer Interrupt Flag)
	// PIT_TCTRL0 = 0x02; // TIE (Timer Interrupt Enable)
	// PIT_TCTRL0 |= 0x01; // TEN (Timer Enable)

	for(int i=0; i<LASER_FIFO_SIZE; i++) {
		fifo[i].x.sel = 0;
		fifo[i].x.buf = 1;
		fifo[i].x.gain = 0;
		fifo[i].x.shdn = 1;
		fifo[i].x.data = 2047;
		fifo[i].y.sel = 1;
		fifo[i].y.buf = 1;
		fifo[i].y.gain = 0;
		fifo[i].y.shdn = 1;
		fifo[i].y.data = 2047;
		fifo[i].r = 0;
		fifo[i].g = 0;
		fifo[i].b = 0;
	}
	fifo_head = 0;
	fifo_tail = 0;
	fifo_full = false;
	fifo_empty = true;
}

void Laser::set(int16_t x, int16_t y, int8_t r, int8_t g, int8_t b) {
#ifdef LASER_DEBUG
	CORE_PIN13_PORTSET = CORE_PIN13_BITMASK;   // LED ON
#endif
	while(fifo_full) yield();
	fifo[fifo_head].x.data = constrain(x, 0, 4095);
	fifo[fifo_head].y.data = constrain(y, 0, 4095);
	fifo[fifo_head].r = constrain(r, 0, 255);
	fifo[fifo_head].g = constrain(g, 0, 255);
	fifo[fifo_head].b = constrain(b, 0, 255);
	fifo_empty = false;
	if(++fifo_head==LASER_FIFO_SIZE) fifo_head = 0;
	if(fifo_head==fifo_tail) fifo_full = true;
#ifdef LASER_DEBUG
	CORE_PIN13_PORTCLEAR = CORE_PIN13_BITMASK; // LED OFF
#endif
}

void Laser::tick() {
	int reg_x, reg_y;
	if(fifo_empty) return;
	reg_x = fifo[fifo_tail].x.cmd;
	reg_y = fifo[fifo_tail].y.cmd;
	fifo_full = false;
	if(++fifo_tail==LASER_FIFO_SIZE) fifo_tail = 0;
	if(fifo_head==fifo_tail) fifo_empty = true;

	SPI0_PUSHR = SPI_PUSHR_CTAS(0) | SPI_PUSHR_PCS(1) | reg_x;
	SPI0_PUSHR = SPI_PUSHR_CTAS(0) | SPI_PUSHR_PCS(1) | reg_y;
}

