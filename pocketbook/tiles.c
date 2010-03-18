typedef struct ibitmap_s {
	short width;
	short height;
	short depth;
	short scanline;
	unsigned char data[];
} ibitmap;

