#include "laser.h"
#include "mk20dx128.h"
#include "core_pins.h"
#include "IntervalTimer.h"

Laser::Laser(){
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

void Laser::set(uint16_t x, uint16_t y, uint8_t r, uint8_t g, uint8_t b) {
	while(fifo_full) yield();
	fifo[fifo_head].x.data = x;
	fifo[fifo_head].y.data = y;
	fifo[fifo_head].r = r;
	fifo[fifo_head].g = g;
	fifo[fifo_head].b = b;
	fifo_empty = false;
	if(++fifo_head==LASER_FIFO_SIZE) fifo_head = 0;
	if(fifo_head==fifo_tail) fifo_full = true;
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

