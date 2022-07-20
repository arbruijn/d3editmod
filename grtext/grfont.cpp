/*
 THE COMPUTER CODE CONTAINED HEREIN IS THE SOLE PROPERTY OF OUTRAGE
 ENTERTAINMENT, INC. ("OUTRAGE").  OUTRAGE, IN DISTRIBUTING THE CODE TO
 END-USERS, AND SUBJECT TO ALL OF THE TERMS AND CONDITIONS HEREIN, GRANTS A
 ROYALTY-FREE, PERPETUAL LICENSE TO SUCH END-USERS FOR USE BY SUCH END-USERS
 IN USING, DISPLAYING,  AND CREATING DERIVATIVE WORKS THEREOF, SO LONG AS
 SUCH USE, DISPLAY OR CREATION IS FOR NON-COMMERCIAL, ROYALTY OR REVENUE
 FREE PURPOSES.  IN NO EVENT SHALL THE END-USER USE THE COMPUTER CODE
 CONTAINED HEREIN FOR REVENUE-BEARING PURPOSES.  THE END-USER UNDERSTANDS
 AND AGREES TO THE TERMS HEREIN AND ACCEPTS THE SAME BY USE OF THIS FILE.
 COPYRIGHT 1996-2000 OUTRAGE ENTERTAINMENT, INC.  ALL RIGHTS RESERVED.
 */
 

#include <ctype.h>
#include "pserror.h"
#include "renderer.h"
#include "gr.h"
#include "mono.h"
#include "CFILE.H"
#include "bitmap.h"
#include "mem.h"
#include "grtext.h"

#include <string.h>
#include <stdlib.h>

#define FT_COLOR				1
#define FT_PROPORTIONAL		2
#define FT_KERNED				4
#define FT_GRADIENT			8


#define GRFONT_SURFACE_WIDTH	128
#define GRFONT_SURFACE_HEIGHT	128

//	----------------------------------------------------------------------------
//	Macros for file io.
//	----------------------------------------------------------------------------

typedef CFILE* FONTFILE;

#define BITS_TO_BYTES(_c)    (((_c)+7)>>3)
#define BITS_TO_SHORTS(_c)    (((_c)+15)>>4)

inline int READ_FONT_INT(FONTFILE ffile) {
	return cf_ReadInt(ffile);
}

inline short READ_FONT_SHORT(FONTFILE ffile) {
	return cf_ReadShort(ffile);
}

inline ubyte READ_FONT_BYTE(FONTFILE ffile) {
	return (ubyte)cf_ReadByte(ffile);
}

inline int READ_FONT_DATA(FONTFILE ffile, void *buf, int size, int nelem)
{
	int i;

	i = cf_ReadBytes((ubyte *)buf, size*nelem, ffile);

	ASSERT(i == (size*nelem));

	return i;
}  

inline FONTFILE OPEN_FONT(char *filename) {
	FONTFILE fp;
	unsigned file_id;

	fp = (FONTFILE)cfopen(filename, "rb");
	if (!fp) return NULL;

	file_id = READ_FONT_INT(fp);
	if (file_id != 0xfeedbaba) return (FONTFILE)(-1);

	return fp;
}

inline void CLOSE_FONT(FONTFILE ffile) {
	cfclose(ffile);
}



//	----------------------------------------------------------------------------
//	static variables and functions
//	----------------------------------------------------------------------------

gr_font_record Fonts[MAX_FONTS];
bool Font_init;

void grfont_Close()
{
	for (int i = 0; i < MAX_FONTS; i++)
		if (Fonts[i].references)
			grfont_Free(i);
	Font_init = false;
}

void grfont_Reset()
{
	if (Font_init)
		grfont_Close();
	else
		atexit(grfont_Close);

	for (int i = 0; i < MAX_FONTS; i++)
		Fonts[i].references = 0;
	Font_init = true;
}


#if 0
int register_font(char *fontname, char *filename)
{
/* look for a slot with no fontname and place fontname in it */
	ASSERT(fontname != NULL);

	int i;

	for (i = 0; i < MAX_FONTS; i++)
	{
		if (Fonts[i].filename[0] == 0) {
			strcpy(Fonts[i].filename, filename);
			strcpy(Fonts[i].name, fontname);
			load(Fonts[i].filename, i);
			Fonts[i].references=1;
			break;
		}
	}

	if (i == MAX_FONTS) i = -1;

	return i;
}

//	----------------------------------------------------------------------------
//	high level font initialization and cleanup routines
//	----------------------------------------------------------------------------

void init(const char *fontname)
{
	int i, slot = -1;

//	Look for this font in the list, check if it has any references, and load it if
//	there are none.
	if (fontname == NULL) {
		slot = 0;
	}
	else {
		for (i = 0; i < MAX_FONTS; i++)
		{
			if (stricmp(Fonts[i].name, fontname) == 0) {
				slot = i;

				if (Fonts[i].references == 0) {
					break;
				}
				else {
					m_FontHandle = slot;
					return;
				}
			}
		}

		ASSERT(slot != -1);

	//	clear up surface and bitmap lists
		Fonts[slot].ch_w = NULL;
		Fonts[slot].ch_h = NULL;
		Fonts[slot].ch_u = NULL;
		Fonts[slot].ch_v = NULL;
	
		for (i = 0; i < MAX_FONT_BITMAPS; i++)
		{
			Fonts[slot].bmps[i] = -1;
			Fonts[slot].surfs[i] = NULL;
		}

	//	We must load this font from the 'hogfile'
		load(Fonts[slot].filename, slot);

		Fonts[slot].references++;
	}

	m_FontHandle = slot;
}


void free()
{
	if (m_FontHandle == -1) return;

	return;

//!! Removed the following assert because there are bugs in the reference count system. Matt, 11-15-97.
//!!	ASSERT(Fonts[m_FontHandle].references > 0);
//@@
//	free font if there was only one reference left.
//@@	if (Fonts[m_FontHandle].references == 1) {
//@@		gr_font_record *ft;
//@@
//@@	// free up all memory allocated to this font.
//@@		ft = &Fonts[m_FontHandle];
//@@
//@@ 		free_font(ft);
//@@	}
//@@
//@@	Fonts[m_FontHandle].references--;
	m_FontHandle = -1;					
}
#endif

void grfont_Free(int slot)
{
	int i;
	gr_font_record *ft = &Fonts[slot];

//	delete font surface info.
	if (ft->ch_wf) delete[] ft->ch_wf;
	if (ft->ch_hf) delete[] ft->ch_hf;
	if (ft->ch_uf) delete[] ft->ch_uf;
	if (ft->ch_vf) delete[] ft->ch_vf;
	if (ft->ch_w) delete[] ft->ch_w;
	if (ft->ch_h) delete[] ft->ch_h;
	if (ft->ch_u) delete[] ft->ch_u;
	if (ft->ch_v) delete[] ft->ch_v;
	if (ft->ch_surf) delete[] ft->ch_surf;
	
	for (i = 0; i < MAX_FONT_BITMAPS; i++)
		if (ft->bmps[i] != -1) bm_FreeBitmap(ft->bmps[i]);

//	delete font file info.
	if (ft->font.flags & FT_PROPORTIONAL) {
		delete[] ft->font.char_widths;
	}

	mem_free(ft->font.raw_data);
	mem_free(ft->font.char_data);
}

void grfont_XlateMonoChar(int bm_handle, int x, int y, int index, gr_font_file_record *ft, int width);
void grfont_XlateColorChar(int bm_handle, int x, int y, int index, gr_font_file_record *ft, int width);
void grfont_XlateColorGrayChar(int bm_handle, int x, int y, int index, gr_font_file_record *ft, int width);
void grfont_TranslateToBitmaps(int slot);


int grfont_Load(char *filename)
{
	gr_font_record *fnt;
	gr_font_file_record *ft;
	FONTFILE ff;
 	int num_char;
	int i;
	int slot;
	char fontname[32];

	for (slot = 0; slot < MAX_FONTS; slot++)
		if (!Fonts[slot].references)
			break;

	fnt = &Fonts[slot];

	ft = &fnt->font;

	ff = OPEN_FONT(filename);
	if (!ff) {
		Error("Unable to open font in file %s.\n", filename);
	
	}
	else if (ff == (FONTFILE)0xffffffff) {
		Error("Illegal font file: %s.\n", filename);
	}

	mprintf((0, "%s font.\n", Fonts[slot].name));

	ft->width = READ_FONT_SHORT(ff);
	ft->height = READ_FONT_SHORT(ff);
	ft->flags = READ_FONT_SHORT(ff);
	ft->baseline = READ_FONT_SHORT(ff);
	ft->min_ascii = READ_FONT_BYTE(ff);
	ft->max_ascii = READ_FONT_BYTE(ff);
	READ_FONT_DATA(ff, fontname, 32, 1);
	if (ft->flags & FT_FFI2) {
		ft->ffi2.tracking = READ_FONT_SHORT(ff);
		READ_FONT_DATA(ff, ft->ffi2.reserved, sizeof(ft->ffi2.reserved), 1);
	} else
		ft->ffi2.tracking = 0;
	ft->brightness = (ft->baseline >> 8) * 0.1f;

	mprintf((0, "  <ht %d>::<min %d>::<max %d>::<base %d>", ft->height, ft->min_ascii, ft->max_ascii, ft->baseline));

	num_char = ft->max_ascii-ft->min_ascii+1;
	if (ft->max_ascii < 'a')
		ft->flags |= FT_UPPER;

//	Read in all widths 
	if (ft->flags & FT_PROPORTIONAL) {
		ft->char_widths = new short[num_char];
		for (i = 0; i < num_char; i++)
			ft->char_widths[i] = READ_FONT_SHORT(ff);
		mprintf((0, "::proportional"));
	}
	else {
		ft->char_widths = NULL;
	}

//	Read in kerning data
	if (ft->flags & FT_KERNED) {
		int num = READ_FONT_SHORT(ff);
		ft->kern_data = (ubyte *)malloc(num * 3 + 3);
		for (int i = 0; i < num; i++) {
			ft->kern_data[i * 3] = READ_FONT_BYTE(ff);
			ft->kern_data[i * 3 + 1] = READ_FONT_BYTE(ff);
			ft->kern_data[i * 3 + 2] = READ_FONT_BYTE(ff);
		}
		ft->kern_data[num * 3] = 0xff;
		ft->kern_data[num * 3 + 1] = 0xff;
		ft->kern_data[num * 3 + 2] = 0;
	} else
		ft->kern_data = NULL;

//	Read in pixel data.
//	for color fonts, read in byte count and then the data,
//		generate character data pointer table
//	for mono fonts, read in byte count, then the data, convert to bits and store
//		generate character data pointer table
	int bytesize = READ_FONT_INT(ff);

	ft->raw_data = (ubyte *)mem_malloc(bytesize);
	ft->char_data = (ubyte **)mem_malloc(num_char * sizeof(ubyte *));

	READ_FONT_DATA(ff, ft->raw_data, bytesize, 1);

	if (ft->flags & FT_COLOR) {
		int off = 0;
		mprintf((0, "::color"));
		for (i = 0; i < num_char; i++) 
		{
			ft->char_data[i] = ft->raw_data + off;
			if (ft->flags & FT_PROPORTIONAL) 
				off += (ft->char_widths[i]*ft->height*BITS_TO_BYTES(BPP_16));
			else 
				off += (ft->width*ft->height*BITS_TO_BYTES(BPP_16));
		}
	}
	else {			// Monochrome
		ubyte *ptr = ft->raw_data;
		mprintf((0, "::mono"));
		for (i = 0; i < num_char; i++)
		{
			ft->char_data[i] = ptr;
			if (ft->flags & FT_PROPORTIONAL)
				ptr += BITS_TO_BYTES(ft->char_widths[i]) * ft->height;
			else 
				ptr += BITS_TO_BYTES(ft->width) * ft->height;
		}
	}

	CLOSE_FONT(ff);

	mprintf((0, "\n"));

	Fonts[slot].references = 1;
	//Fonts[slot].font = fnt;

//	draw font to bitmaps
	grfont_TranslateToBitmaps(slot);
	
	return slot;
}

void grfont_ClearBitmap(int handle)
{
	memset(bm_data(handle, 0), 0, bm_rowsize(handle, 0) * bm_h(handle, 0));
}



//	translates a font to surfaces
void grfont_TranslateToBitmaps(int slot)
{
	gr_font_file_record *fntfile;
	gr_font_record *fnt;

	fnt = &Fonts[slot];
	fntfile = &Fonts[slot].font;

//	start creating font surfaces, map these surfaces onto bitmaps created via bitmap library
//	this is needed for the renderer library.
//	create a 128x128 surface first.
//	draw each character into surface until we need to create another
//	surface.
	ubyte u=0, v=0, w;
	int ch, num_ch;
	ubyte surf_index = 0;

	num_ch = fntfile->max_ascii-fntfile->min_ascii+1;

//	initialize memory
	fnt->ch_w = new ubyte[num_ch];
	fnt->ch_h = new ubyte[num_ch];
	fnt->ch_u = new ubyte[num_ch];
	fnt->ch_v = new ubyte[num_ch];	  
	fnt->ch_surf = new ubyte[num_ch];	  
	fnt->ch_uf = new float[num_ch];
	fnt->ch_vf = new float[num_ch];
	fnt->ch_wf = new float[num_ch];
	fnt->ch_hf = new float[num_ch];	  

	fnt->bmps[surf_index] = bm_AllocBitmap(GRFONT_SURFACE_WIDTH, GRFONT_SURFACE_HEIGHT, 0);
	if (fnt->bmps[surf_index] == -1 || !fnt->bmps[surf_index])
		Error("translate_to_surfaces <Bitmap allocation error>");
	if (fntfile->flags & FT_FMT4444)
		GameBitmaps[fnt->bmps[surf_index]].format = BITMAP_FORMAT_4444;
	grfont_ClearBitmap(fnt->bmps[surf_index]);
	surf_index++;

	for (ch = 0; ch < num_ch; ch++)
	{
		if (fntfile->flags & FT_PROPORTIONAL) w = (int)fntfile->char_widths[ch];
		else w = (int)fntfile->width;

		if ((u+w) > GRFONT_SURFACE_WIDTH) {
			u = 0;
			v += fntfile->height;
			if ((v+fntfile->height) > GRFONT_SURFACE_HEIGHT) {
				if (surf_index == MAX_FONT_BITMAPS) Int3();
				fnt->bmps[surf_index] = bm_AllocBitmap(GRFONT_SURFACE_WIDTH, GRFONT_SURFACE_HEIGHT, 0);																
				if (fnt->bmps[surf_index] == -1 || !fnt->bmps[surf_index])
					Error("translate_to_surfaces <Bitmap allocation error>");
				if (fntfile->flags & FT_FMT4444)
					GameBitmaps[fnt->bmps[surf_index]].format = BITMAP_FORMAT_4444;
				grfont_ClearBitmap(fnt->bmps[surf_index]);
				surf_index++;
				v = 0;
			}
		}


	// blt each character
		if (fntfile->flags & FT_COLOR)
			grfont_XlateColorChar(fnt->bmps[surf_index-1], u, v, ch, fntfile, w);
		else if (fntfile->flags & FT_GRADIENT)
			grfont_XlateColorGrayChar(fnt->bmps[surf_index-1], u,v,ch,fntfile,w);
		else
			grfont_XlateMonoChar(fnt->bmps[surf_index-1], u,v,ch,fntfile,w);

		fnt->ch_h[ch] = fntfile->height;
 		fnt->ch_w[ch] = w;
		fnt->ch_u[ch] = u;
		fnt->ch_v[ch] = v;
		fnt->ch_surf[ch] = surf_index-1;
		fnt->ch_hf[ch] = ((float)fntfile->height)/((float)GRFONT_SURFACE_HEIGHT);
 		fnt->ch_wf[ch] = ((float)w)/((float)GRFONT_SURFACE_WIDTH);
		fnt->ch_uf[ch] = ((float)u)/((float)GRFONT_SURFACE_WIDTH);
		fnt->ch_vf[ch] = ((float)v)/((float)GRFONT_SURFACE_HEIGHT);

	//	check to adjust uv's if we are outside surface.
		u+= w;
	}
}


//	translate mono font data to the surface
void grfont_XlateMonoChar(int bm_handle, int x, int y, int index, gr_font_file_record *ft, int width)
{
	int row,col;					// byte width of char
	int rowsize;
	ubyte bit_mask=0, byte;
	ubyte *fp;

	fp = ft->char_data[index];

	/*	draw one-bit one color. */
	ushort *dest_ptr;
	ushort col_w = GR_COLOR_TO_16(GR_RGB(255,255,255));
	int rowsize_w;

	dest_ptr = (ushort *)bm_data(bm_handle, 0);
	rowsize_w = bm_rowsize(bm_handle, 0)/2;
	dest_ptr += (y*rowsize_w)+x;

	for (row = 0; row < ft->height; row++)
	{
		bit_mask = 0;
		for (col = 0; col < width; col++)
		{
			if (bit_mask == 0) {
				byte = *fp++;
				bit_mask = 0x80;
			}

			if (byte & bit_mask)
				dest_ptr[col] = col_w;
			bit_mask >>=1;
		}
		dest_ptr += rowsize_w;
	}
}

//	translate color font data to the surface
void grfont_XlateColorChar(int bm_handle, int x, int y, int index, gr_font_file_record *ft, int width)
{
	int row,col;					// byte width of char
	int rowsize;
	ubyte bit_mask=0, byte;
	ushort *fp;

	fp = (ushort *)ft->char_data[index];

	/*	draw short one color. */
	ushort *dest_ptr;
	int rowsize_w;

	dest_ptr = (ushort *)bm_data(bm_handle, 0);
	rowsize_w = bm_rowsize(bm_handle, 0)/2;
	dest_ptr += (y*rowsize_w)+x;

	if (ft->flags & FT_FMT4444)
		for (row = 0; row < ft->height; row++) {
			memcpy(dest_ptr, fp, width * 2);
			fp += width;
			dest_ptr += rowsize_w;
		}
	else // 565 -> 1555
		for (row = 0; row < ft->height; row++) {
			for (col = 0; col < width; col++) {
				ushort c = *fp++;
				if (c == 0x7e0) // full green
					dest_ptr[col] = 0;
				else
					dest_ptr[col] = (c & 0x1f) | ((c & 0x7c0) >> 1) | ((c & 0xf800) >> 1) |  0x8000;
			}
			dest_ptr += rowsize_w;
		}
}

//	translate grayscale font data to the surface
void grfont_XlateColorGrayChar(int bm_handle, int x, int y, int index, gr_font_file_record *ft, int width)
{
	int row,col;					// byte width of char
	int rowsize;
	ubyte bit_mask=0, byte;
	ushort *fp;

	fp = (ushort *)ft->char_data[index];

	/*	draw short one color. */
	ushort *dest_ptr;
	int rowsize_w;

	dest_ptr = (ushort *)bm_data(bm_handle, 0);
	rowsize_w = bm_rowsize(bm_handle, 0)/2;
	dest_ptr += (y*rowsize_w)+x;

	for (row = 0; row < ft->height; row++) {
		for (col = 0; col < width; col++) {
			ushort c = *fp++;
			if (c == 0x7e0) // full green
				dest_ptr[col] = 0;
			else {
				float fval = ((c & 0x1f) * 0.11f +
					   ((c >> 6) & 0x1f) * 0.59f +
					   (c >> 11) * 0.3f) / 32 * ft->brightness;
				ubyte val = fval > 1 ? 255 : fval * 255;
				val >>= 3;
				dest_ptr[col] = 0x8000 | (val << 10) | (val << 5) | val;
			}
		}
		dest_ptr += rowsize_w;
	}
}

typedef struct {
	ushort ch;
	ushort use_box;
	int x, y;
	float scale_x, scale_y;
	int sx, sy, sw, sh;
} tCharBlt;

int grfont_BltChar(int fontnum, tCharBlt *blt)
{
	gr_font_record *ft;
	int index, ch;

	ft = &Fonts[fontnum];
	ch = blt->ch;
	if (ch > ft->font.max_ascii && (ft->font.flags & FT_UPPER))
		ch = toupper(ch);
	
	if ((ch < ft->font.min_ascii) || (ch > ft->font.max_ascii))
		return blt->x+1;

	index = ch - ft->font.min_ascii;

	if (blt->use_box) {
		rend_DrawFontCharacter (ft->bmps[ft->ch_surf[index]], blt->x,blt->y,blt->x+blt->sw,blt->y+blt->sh,
							ft->ch_uf[index]+(((float)blt->sx)/((float)GRFONT_SURFACE_WIDTH)), 
							ft->ch_vf[index]+(((float)blt->sy)/((float)GRFONT_SURFACE_HEIGHT)), 
							((float)blt->sw)/((float)GRFONT_SURFACE_WIDTH), 
							((float)blt->sh)/((float)GRFONT_SURFACE_HEIGHT));
	} else {
		blt->sw = ft->font.flags & FT_PROPORTIONAL ? ft->font.char_widths[index] : ft->font.width;
		blt->sh = ft->font.height;
		rend_DrawFontCharacter (ft->bmps[ft->ch_surf[index]],
				blt->x,blt->y,blt->x+blt->sw * blt->scale_x,blt->y+blt->sh * blt->scale_y,
				ft->ch_uf[index], ft->ch_vf[index],
				ft->ch_wf[index], ft->ch_hf[index]);
	}

	//	adjust next x value with kerning and return it.
	return blt->x + blt->sw;
}


//	----------------------------------------------------------------------------
//	accessor functions
//	----------------------------------------------------------------------------

int grfont_GetCharWidth(int font, int ch)
{
	gr_font_file_record *ft;

	ft = &Fonts[font].font;

	if (ch < ft->min_ascii || ch > ft->max_ascii) 
		return 0;
	else if (ft->flags & FT_PROPORTIONAL) 
		return ft->char_widths[ch-ft->min_ascii];
	else
		return ft->width;
}

int grfont_GetHeight(int font)
{
	return Fonts[font].font.height;
}

int grfont_GetTracking(int font)
{
	return Fonts[font].font.ffi2.tracking;
}

int grfont_GetKernedSpacing(int font, int c1, int c2)
{
	ubyte *p = Fonts[font].font.kern_data;

	if (p)
		while (*p!=255) {
			if (p[0]==c1 && p[1]==c2) return (sbyte)p[2];
			p+=3;
		}

	return 0;
}

int grfont_GetKernedSpacingTemp(gr_font_file_record *ft, int c1, int c2)
{
	ubyte *p = ft->kern_data;

	while (*p!=255) 	{
		if (p[0]==c1 && p[1]==c2) return p[2];
		p+=3;
	}

	return 0;
}

