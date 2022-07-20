#ifndef GRFONT_H
#define GRFONT_H

typedef struct {
	ushort ch;
	ushort use_box;
	int x, y;
	float scale_x, scale_y;
	int sx, sy, sw, sh;
} tCharBlt;

int grfont_BltChar(int fontnum, tCharBlt *blt);

#endif
