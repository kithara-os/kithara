#include <video/video.h>

#define VIDEO_WIDTH 80
#define VIDEO_HEIGHT 25
#define VIDEO_FB_CHAR (VIDEO_WIDTH * VIDEO_HEIGHT)
#define VIDEO_FB_SIZE (VIDEO_FB_CHAR * 2)
#define VIDEO_COLOR 0x1f

PRIVATE uint64_t video_x, video_y;
PRIVATE char *const video_fb = (char *)(0xb8000 + KERNEL_MAPPING_BASE);
PRIVATE char copy_fb[VIDEO_WIDTH * VIDEO_HEIGHT * 2];

inline static uint64_t video_get_fb_offset(uint64_t x, uint64_t y) {
	return 2 * (y * VIDEO_WIDTH + x);
}

inline static void video_drawc(uint64_t x, uint64_t y, char c) {
	*(video_fb + video_get_fb_offset(x, y)) = c;
	*(video_fb + video_get_fb_offset(x, y) + 1) = (char)VIDEO_COLOR;
	*(copy_fb + video_get_fb_offset(x, y)) = c;
	*(copy_fb + video_get_fb_offset(x, y) + 1) = (char)VIDEO_COLOR;
}

inline static void video_scroll(void) {
	memcpy(copy_fb, copy_fb + (2 * VIDEO_WIDTH),
	       VIDEO_FB_SIZE - (2 * VIDEO_WIDTH));
	char *last_line = copy_fb + 2 * ((VIDEO_HEIGHT - 1) * VIDEO_WIDTH);
	for (uint64_t i = 0; i < VIDEO_WIDTH; ++i) {
		last_line[2 * i] = ' ';
		last_line[2 * i + 1] = VIDEO_COLOR;
	}
	memcpy(video_fb, copy_fb, VIDEO_FB_SIZE);
	video_x = 0;
	video_y = VIDEO_HEIGHT - 1;
}

inline static void video_clear(void) {
	for (uint64_t i = 0; i < VIDEO_FB_CHAR; ++i) {
		copy_fb[2 * i] = ' ';
		copy_fb[2 * i + 1] = VIDEO_COLOR;
	}
	memcpy(video_fb, copy_fb, VIDEO_FB_SIZE);
	video_x = 0;
	video_y = 0;
}

inline static void video_adjust_y(void) {
	if (video_y >= VIDEO_HEIGHT) {
		video_scroll();
	}
}

inline static void video_newline(void) {
	++video_y;
	video_x = 0;
}

void video_putc(char c) {
	video_adjust_y();
	if (c == '\n') {
		video_newline();
	} else {
		video_drawc(video_x, video_y, c);
		if (++video_x == VIDEO_WIDTH) {
			video_newline();
		}
	}
}

void video_init(void) {
	video_clear();
}
