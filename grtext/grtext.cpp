#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include "renderer.h"
#include "grtext.h"
#include "grfont.h"

char Grtext_buffer[16384];
int Grtext_ptr;
ubyte Grtext_alpha = 255;
float Grtext_scale;
int Grtext_spacing;
static int grtext_LineSpacing = 1;
int Grtext_bottom, Grtext_right, Grtext_top, Grtext_left, Grtext_tabspace;
int Grtext_font;
bool Grtext_shadow;
sbyte Grtext_alphatype = AT_CONSTANT_TEXTURE;
uint Grtext_colors;

enum {
	OP_PUTS = 1,
	OP_COLOR = 2,
	OP_FANCYCOLOR = 3,
	OP_FONT = 4,
	OP_ALPHA = 5,
	OP_PUTCHAR = 6,
	OP_FLAGS = 7,
	OP_SCALE = 8
};

inline void grtext_AddShort(ushort val) {
	*(ushort *)&Grtext_buffer[Grtext_ptr] = val;
	Grtext_ptr += sizeof(ushort);
}

inline void grtext_AddInt(uint val) {
	*(uint *)&Grtext_buffer[Grtext_ptr] = val;
	Grtext_ptr += sizeof(uint);
}

inline void grtext_AddFloat(float val) {
	*(float *)&Grtext_buffer[Grtext_ptr] = val;
	Grtext_ptr += sizeof(float);
}

inline ushort grtext_GetShort(int pos) {
	return *(ushort *)&Grtext_buffer[pos];
}

inline uint grtext_GetInt(int pos) {
	return *(uint *)&Grtext_buffer[pos];
}

inline float grtext_GetFloat(int pos) {
	return *(float *)&Grtext_buffer[Grtext_ptr];
}

void grtext_SetColor(uint color)
{
	grtext_AddInt(OP_COLOR);
	grtext_AddInt(color);
}

void grtext_SetFancyColor(uint color, uint a, uint b, uint c)
{
	grtext_AddInt(OP_FANCYCOLOR);
	grtext_AddInt(color);
}

void grtext_SetAlpha(ubyte alpha)
{
	Grtext_alpha = alpha;
	grtext_AddShort(OP_ALPHA | (alpha << 8));
}

ubyte grtext_GetAlpha()
{
	return Grtext_alpha;
}

void grtext_SetFlags(int flags)
{
	grtext_AddInt(OP_FLAGS);
	grtext_AddInt(flags);
}

void grtext_SetFont(int font)
{
	Grtext_font = font;
	Grtext_spacing = grfont_GetTracking(font) + 1;
	grtext_AddInt(OP_FONT);
	grtext_AddInt(font);
}

int grtext_GetFont() {
	return Grtext_font;
}

void grtext_Reset()
{
	Grtext_buffer[0] = 0;
	Grtext_ptr = 0;
	Grtext_alpha = 0xff;
	Grtext_scale = 1.0;
	Grtext_spacing = 1;
	grtext_SetFlags(0);
	grtext_SetColor(0);
}

void grtext_Puts(int x, int y, const char *text)
{
	grtext_AddShort(OP_PUTS);
	grtext_AddShort(x);
	grtext_AddShort(y);
	strcpy(Grtext_buffer + Grtext_ptr, text);
	Grtext_ptr += strlen(text) + 1;
}

void grtext_Printf(int x,int y,const char *fmt,...)
{
	char text[512];
	va_list vp;
	va_start(vp, fmt);
	vsnprintf(text, sizeof(text), fmt, vp);
	va_end(vp);
	grtext_Puts(x, y, text);
}

void grtext_PutChar(int x,int y,int ch)
{
	grtext_AddShort(OP_PUTCHAR);
	grtext_AddShort(x);
	grtext_AddShort(y);
	grtext_AddShort(ch);
}


void grtext_Flush(void)
{
	grtext_Render();
	grtext_Reset();
}

inline int grtext_CountLines(const char *str) {
	int lines = 1;
	while ((str = strchr(str, '\n'))) {
		str++;
		lines++;
	}
	return lines;
}

int grtext_GetTextHeightTemplate(tFontTemplate *ft,const char *str)
{
	return grtext_CountLines(str) * (ft->ch_height + grtext_LineSpacing);
}

int grtext_GetTextHeight(const char *str)
{
	return grtext_CountLines(str) * (grfont_GetHeight(Grtext_font) + grtext_LineSpacing);
}

int grtext_GetTextLineWidth(const char *str)
{
	int len = strlen(str);
	int width = 0, max_width = 0;
	for (int i = 0; i < len; i++) {
		char c1 = str[i], c2 = str[i + 1];
		if (c1 == '\1') {
			i += 3;
			continue;
		} else if (c1 == '\t') {
			int w = (grfont_GetCharWidth(Grtext_font, ' ') * Grtext_scale + Grtext_spacing) * Grtext_tabspace;
			width = ((width + w) / w) * w;
		} else if (c1 == '\2') {
			width = c2 * 4;
			i++;
		} else if (c1 == '\n') {
			if (width > max_width)
				max_width = width;
			width = 0;
			continue;
		} else {
			width += grfont_GetCharWidth(Grtext_font, c1) * Grtext_scale + Grtext_spacing +
				grfont_GetKernedSpacing(Grtext_font, c1, c2);
		}
	}
	if (width > max_width)
		max_width = width;
	return max_width ? max_width - Grtext_spacing : 0;
}

void grtext_CenteredPrintf(int x, int y, const char *fmt, ...)
{
	char text[512];
	va_list vp;
	va_start(vp, fmt);
	vsnprintf(text, sizeof(text), fmt, vp);
	va_end(vp);
	grtext_Puts(x + Grtext_left +
		((Grtext_right - Grtext_left) - grtext_GetTextLineWidth(text)) / 2,
		y, text);
}

void grtext_SetParameters(int left,int top,int right,int bottom,int tabspace)
{
	Grtext_bottom = bottom;
	Grtext_right = right;
	Grtext_top = top;
	Grtext_left = left;
	Grtext_tabspace = tabspace;
}


void grtext_DrawTextLine(int x,int y,char *text)
{
	int len;
	int pos;
	tCharBlt blt;
	ubyte ch, ch2;
	
	len = strlen(text);
	for (pos = 0; pos < len; pos++) {
		ch = text[pos];
		ch2 = text[pos + 1];
		//grfont_GetCharWidth(Grtext_font, ch);
		if (ch == '\x01') {
			if (pos + 3 >= len)
				return;
			rend_SetFlatColor(text[pos + 3] + text[pos + 2] * 0x100 + text[pos + 1] * 0x10000);
			pos += 3;
		} else if (ch == '\t') {
			int w = grfont_GetCharWidth(Grtext_font, ' ');
			w = (w * Grtext_scale + Grtext_spacing) * Grtext_tabspace;
			x = (x + w) * w / w;
		} else if (ch == '\2') {
			pos = pos + 1;
			if (pos >= len)
				return;
			x = x + text[pos];
		} else if (ch == ' ') {
			int w = grfont_GetCharWidth(Grtext_font, ' ');
			x += (w * Grtext_scale) + Grtext_spacing;
		} else {
			blt.ch = ch;
			blt.x = x;
			blt.y = y;
			blt.scale_y = Grtext_scale;
			blt.scale_x = Grtext_scale;
			blt.use_box = 0;
			x = grfont_BltChar(Grtext_font,&blt);
			x += grfont_GetKernedSpacing(Grtext_font,ch,ch2) + Grtext_spacing;
		}
	}
}

void grtext_RenderString(int x,int y,char *str)
{
	int width;
	int height;
	char *line_end;
	
	while (str) {
		line_end = strchr(str,'\n');
		if (line_end)
			*line_end = '\0';
		width = grtext_GetTextLineWidth(str);
		height = grfont_GetHeight(Grtext_font);
		if ((Grtext_top <= height * Grtext_scale + y) && (y <= Grtext_bottom) &&
				(Grtext_left <= width + x) && (x <= Grtext_right)) {
			#if 0
			if (y < Grtext_top || Grtext_bottom < grfont_GetHeight(Grtext_font) * Grtext_scale + y ||
					x < Grtext_left || Grtext_right < width + x)
				grtext_DrawTextLineClip(x,y,str);
			else
			#endif
				grtext_DrawTextLine(x,y,str);
		}
		x = Grtext_left;
		y = y + grtext_LineSpacing + grfont_GetHeight(Grtext_font) * Grtext_scale;
		if (!line_end)
			break;
		*line_end = '\n';
		str = line_end + 1;
	}
}



void grtext_Render()
{
	int x, y, flags;
	char *text;
	int len;
	int end;
	int pos;
	tCharBlt blt;
	
	rend_SetTextureType(TT_LINEAR);
	rend_SetOverlayType(OT_NONE);
	rend_SetFiltering(0);
	rend_SetLighting(LS_FLAT_GOURAUD);
	rend_SetAlphaType(AT_CONSTANT_TEXTURE);
	rend_SetColorModel(CM_MONO);
	rend_SetZBufferState(0);
	rend_SetAlphaValue(Grtext_alpha);
	for (int pos = 0; pos < Grtext_ptr; ) {
		switch(Grtext_buffer[pos]) {
		default:
			abort();
			break;
		case OP_PUTS:
			x = grtext_GetShort(pos + 2);
			y = grtext_GetShort(pos + 4);
			text = Grtext_buffer + pos + 6;
			if (Grtext_shadow) {
				rend_SetFlatColor(0);
				grtext_RenderString(x + 1,y + 1,text);
				rend_SetFlatColor(Grtext_colors);
			}
			grtext_RenderString(x,y,text);
			pos += 7 + strlen(text);
			break;
		case OP_COLOR:
		case OP_FANCYCOLOR:
			Grtext_colors = grtext_GetInt(pos + 4);
			rend_SetFlatColor(Grtext_colors);
			pos += 8;
			break;
		case OP_FONT:
			Grtext_font = grtext_GetInt(pos + 4);
			pos += 8;
			break;
		case OP_ALPHA:
			rend_SetAlphaValue(Grtext_buffer[pos + 1]);
			pos += 2;
			break;
		case OP_PUTCHAR:
			blt.x = grtext_GetShort(pos + 2);
			blt.y = grtext_GetShort(pos + 4);
			blt.use_box = 0;
			blt.ch = Grtext_buffer[pos + 6];
			if (Grtext_shadow) {
				rend_SetFlatColor(0);
				blt.x++;
				blt.y++;
				grfont_BltChar(Grtext_font,&blt);
				rend_SetFlatColor(Grtext_colors);
				blt.x--;
				blt.y--;
			}
			grfont_BltChar(Grtext_font,&blt);
			pos += 8;
			break;
		case OP_FLAGS:
			flags = grtext_GetInt(pos + 4);
			pos += 8;
			Grtext_alphatype = flags & 1 ? AT_SATURATE_TEXTURE : AT_CONSTANT_TEXTURE;
			rend_SetAlphaType(Grtext_alphatype);
			Grtext_shadow = flags & 2 ? 1 : 0;
			break;
		case OP_SCALE:
			Grtext_scale = grtext_GetFloat(pos + 4);
			pos += 8;
		}
	}
	rend_SetFiltering(1);
	rend_SetZBufferState(1);
}
