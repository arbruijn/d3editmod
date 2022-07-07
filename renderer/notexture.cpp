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
 
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "grdefs.h"
#include "texture.h"
#include "pstypes.h"
#include "fix.h"
#include "mono.h"
#include "3d.h"
#include "pserror.h"
#include "bitmap.h"
#include "ddio.h"
#include "renderer.h"
#include "macros.h"
#include "mem.h"
void tex_SetMipState (int mipstate)
{
}

int tex_Init()
{
	return 1;
}

// Gets the correct AND value so our textures wrap
int tex_GetAnder (int bm,int mip)
{
	return 0;
}

// Given nv points, draws that polygon according to the rendertype currently set
// Handle is a bitmap handle
void tex_DrawPointList (int handle,g3Point **p,int nv)
{
}

// Draws a solid color polygon.  "color" is a color in 5-6-5 format.
void tex_DrawFlatPolygon (g3Point **pv,int nv)
{
}

void tex_SetFlatColor (ddgr_color color)
{
}

// Sets the fog state to TRUE or FALSE
void tex_SetFogState (int on)
{
}

// Sets the near and far plane of fog
void tex_SetFogBorders (float fog_near,float fog_far)
{
}

void tex_SetLighting (light_state state)
{
}
void tex_SetColorModel (color_model state)
{
}
void tex_SetTextureType (texture_type state)
{
}

void tex_SetAlphaType (sbyte atype)
{
}


void tex_SetZBufferState (int state)
{
}
// Clears the zbuffer
void tex_ClearZBuffer ()
{
}

void tex_EndFrame()
{

}
void tex_StartFrame(int x1,int y1,int x2,int y2)
{
}

// Sets where the software renderer should draw to
void tex_SetSoftwareParameters (float aspect_ratio,int width,int height,int pitch,ubyte *framebuffer)
{
}

void tex_GetLFBLock(renderer_lfb *lfb)
{
}


void tex_ReleaseLFBLock(renderer_lfb *lfb)
{
}

// Fills in projection variables
void tex_GetProjectionParameters (int *width,int *height)
{
}

// Returns the aspect ratio of the physical screen
float tex_GetAspectRatio ()
{
	return 1.0f;
}


//	software primative renderers
//	fill rect supported.
void tex_FillRect(ddgr_color color, int x1, int y1, int x2, int y2)
{
}


//	set pixel
void tex_SetPixel(ddgr_color color, int x, int y)
{
}


//	get pixel
ddgr_color tex_GetPixel(int x, int y)
{
	return 0;
}


//	fillcircle
void tex_FillCircle(ddgr_color col, int xc, int yc, int r)
{
}


void tex_DrawCircle(int xc, int yc, int r)
{
}


void tex_DrawLine(int x1, int y1, int x2, int y2)
{
}


int tex_ClipRect(int &l, int &t, int &r, int &b)
{
	return 0;
}


//	grPen::clip_line
//		returns 2 if totally clipped
//		returns 1 if partially clipped
//		returns 0 if not clipped.

# define FSCALE(var,arg,num,den) ((var) = (arg)*(num)/(den))

int tex_ClipLine(int &l, int &t, int &r, int &b)
{
	return 0;
}

void tex_GetRenderState (rendering_state *rs)
{
	rs->screen_width=640;
	rs->screen_height=480;
}
