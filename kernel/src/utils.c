#include <amd64/ports.h>
#include <spinlock.h>
#include <utils.h>
#include <video/video.h>

void panic(const char *str) {
	asm("cli");
	printf("%s\n", str);
	while (1) {
		asm volatile("hlt");
	}
	outw(0x604, 0x2000);
}

char char_from_digit(uint8_t digit) {
	if (digit >= 10) {
		return 'A' - 10 + digit;
	}
	return '0' + digit;
}

void putui(uint64_t val, uint8_t base, bool rec) {
	if (val == 0 && rec) {
		return;
	}
	putui(val / base, base, true);
	video_putc(char_from_digit(val % base));
}

void puti(int64_t val, int8_t base) {
	if (val < 0) {
		video_putc('-');
		putui((uint64_t)-val, base, false);
	} else {
		putui((uint64_t)val, base, false);
	}
}

void puts(const char *str) {
	for (uint64_t i = 0; str[i] != '\0'; ++i) {
		video_putc(str[i]);
	}
}

void putp(uint64_t pointer, int depth) {
	if (depth == 0) {
		return;
	}
	putp(pointer / 16, depth - 1);
	video_putc(char_from_digit(pointer % 16));
}

void printf(const char *fmt, ...) {
	static spinlock_t spinlock = 0;
	spinlock_lock(&spinlock);
	va_list args;
	va_start(args, fmt);
	for (uint64_t i = 0; fmt[i] != '\0'; ++i) {
		if (fmt[i] != '%') {
			video_putc(fmt[i]);
		} else {
			++i;
			switch (fmt[i]) {
			case '%':
				video_putc('%');
				break;
			case 'd':
				puti(va_arg(args, int64_t), 10);
				break;
			case 'u':
				putui(va_arg(args, uint32_t), 10, false);
				break;
			case 'p':
				putp(va_arg(args, uint64_t), 16);
				break;
			case 's':
				puts(va_arg(args, char *));
				break;
			case 'c':
				video_putc((char)va_arg(args, int));
				break;
			case 'l':
				++i;
				switch (fmt[i]) {
				case 'u':
					putui(va_arg(args, uint64_t), 10,
					      false);
					break;
				case 'd':
					puti(va_arg(args, int64_t), 10);
					break;
				case 'l':
					++i;
					switch (fmt[i]) {
					case 'u':
						putui(va_arg(args, uint64_t),
						      10, false);
						break;
					case 'd':
						puti(va_arg(args, int64_t), 10);
						break;
					default:
						break;
					}
					break;
				default:
					break;
				}
			default:
				break;
			}
		}
	}
	spinlock_unlock(&spinlock);
	va_end(args);
}

void write(const char *str, uint64_t size) {
	for (uint64_t i = 0; i < size; ++i) {
		video_putc(str[i]);
	}
}