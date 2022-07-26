#include "3d.h"
#include "gameloop.h"
#include "game.h"

int dcr_lod_segments[3] = {32, 16, 8};
vector dcr_vecs[3][32];
int dcr_last_w[3], dcr_last_h[3];
float dcr_last_fov[3], dcr_last_zoom[3];

void DrawColoredRing(vector *pos,float r,float g,float b,float inner_alpha,float outer_alpha,float size,
							 float inner_ring_ratio,ubyte saturate,ubyte lod)
{
	int lod_segs;
	g3Point pos_rot;
	g3Point inner_pnts[40], outer_pnts[40];
	g3Point *pnt_ptrs[4];

	lod_segs = dcr_lod_segments[lod];
	rend_SetLighting(LS_GOURAUD);
	rend_SetTextureType(TT_FLAT);
	rend_SetOverlayType(OT_NONE);
	rend_SetColorModel(CM_RGB);
	rend_SetFlatColor(GR_RGB(r * 255, g * 255, b * 255));
	rend_SetAlphaType(saturate ? AT_SATURATE_VERTEX : AT_VERTEX);
	vector *vecs = &dcr_vecs[lod][0];
	if (Game_window_w != dcr_last_w[lod] || Game_window_h != dcr_last_h[lod] ||
		dcr_last_fov[lod] != Render_FOV || dcr_last_zoom[lod] != Render_zoom) {
		angle ang = 0;
		angle inc_ang = 0x10000 / lod_segs;
		vector scale;
		dcr_last_fov[lod] = Render_FOV;
		dcr_last_zoom[lod] = Render_zoom;
		dcr_last_w[lod] = Game_window_w;
		dcr_last_h[lod] = Game_window_h;
		g3_GetMatrixScale(&scale);
		for (int i = 0; i < lod_segs; i++) {
			float s = FixSin(ang), c = FixCos(ang);
			vecs[i] = {c * scale.x, s * scale.y, 0};
			ang += inc_ang;
		}
	}
	g3_RotatePoint(&pos_rot,pos);
	for (int i = 0; i < lod_segs; i++) {
		g3Point *inner = &inner_pnts[i], *outer = &outer_pnts[i];
		inner->p3_vec = vecs[i] * size * inner_ring_ratio + pos_rot.p3_vec;
		inner->p3_flags = PF_RGBA;
		inner->p3_uvl.a = inner_alpha;
		inner->p3_uvl.r = r;
		inner->p3_uvl.g = g;
		inner->p3_uvl.b = b;
		g3_CodePoint(inner);
		outer->p3_vec = vecs[i] * size + pos_rot.p3_vec;
		outer->p3_flags = PF_RGBA;
		outer->p3_uvl.a = outer_alpha;
		outer->p3_uvl.r = r;
		outer->p3_uvl.g = g;
		outer->p3_uvl.b = b;
		g3_CodePoint(outer);
	}

	for (int i = 0; i < lod_segs; i++) {
		pnt_ptrs[0] = outer_pnts + i;
		pnt_ptrs[1] = outer_pnts + (i + 1) % lod_segs;
		pnt_ptrs[2] = inner_pnts + (i + 1) % lod_segs;
		pnt_ptrs[3] = inner_pnts + i;
		g3_DrawPoly(4, pnt_ptrs, 0, 0, NULL);
	}
}


void DrawColoredDisk(vector *pos,float r,float g,float b,float inner_alpha,float outer_alpha,float size,
	ubyte saturate,ubyte lod)
{
	rend_SetZBufferWriteMask(0);
	DrawColoredRing(pos,r,g,b,inner_alpha,outer_alpha,size,0.0,saturate,lod);
	rend_SetZBufferWriteMask(1);
}


void DrawAlphaBlendedScreen(float r,float g,float b,float a)
{
	g3Point *pnt_ptrs [4];
	g3Point pnts [4];

	pnts[0].p3_sx =	pnts[3].p3_sx = 0;
	pnts[0].p3_sy =	pnts[1].p3_sy = 0;
	pnts[2].p3_sx = pnts[1].p3_sx = Game_window_w;
	pnts[3].p3_sy = pnts[2].p3_sy = Game_window_h;
	for (int i = 0; i < 4; i++) {
		pnts[i].p3_vec.z = 0;
		pnts[i].p3_flags = PF_PROJECTED;
		pnt_ptrs[i] = pnts + i;
	}
	rend_SetZBufferState(0);
	rend_SetTextureType(TT_FLAT);
	rend_SetAlphaType(AT_CONSTANT);
	rend_SetAlphaValue(a * 255);
	rend_SetLighting(LS_NONE);
	rend_SetFlatColor(GR_RGB(r * 255, g * 255, b * 255));
	rend_DrawPolygon(0,pnt_ptrs,4,0);
	rend_SetZBufferState(1);
}
