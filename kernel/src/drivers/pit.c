#include <amd64/ports.h>
#include <drivers/pit.h>

void pit_wait(uint16_t ms) {
	outb(0x43, 0x30);
	// count
	uint16_t val = 0x4a9 * ms;
	outb(0x40, (uint8_t)val);
	outb(0x40, (uint8_t)(val >> 8));
	while (true) {
		outb(0x43, 0xe2);
		uint8_t status = inb(0x40);
		if ((status & (1 << 7)) != 0) {
			break;
		}
	}
}