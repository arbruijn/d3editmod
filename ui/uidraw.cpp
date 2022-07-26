#include "renderer.h"
#include "uidraw.h"
#include "3d.h"

ubyte m_UIDrawAlpha;

void ui_DrawSetAlpha(ubyte alpha)
{
	m_UIDrawAlpha = alpha;
}

void ui_DrawRect(ddgr_color color,int x1,int y1,int x2,int y2)
{
	g3Point *ptr_pnts [4];
	g3Point pnts [4];
	
	if (color != 0xff00) {
		pnts[0].p3_sx = x1;
		pnts[0].p3_sy = y1;
		pnts[0].p3_vec.z = 0.0;
		pnts[0].p3_flags = PF_PROJECTED;
		pnts[1].p3_sx = x2;
		pnts[1].p3_sy = y1;
		pnts[1].p3_vec.z = 0.0;
		pnts[1].p3_flags = PF_PROJECTED;
		pnts[2].p3_sx = x2;
		pnts[2].p3_sy = y2;
		pnts[2].p3_vec.z = 0.0;
		pnts[2].p3_flags = PF_PROJECTED;
		pnts[3].p3_sx = x1;
		pnts[3].p3_sy = y2;
		pnts[3].p3_vec.z = 0.0;
		pnts[3].p3_flags = PF_PROJECTED;
		ptr_pnts[0] = pnts;
		ptr_pnts[1] = pnts + 1;
		ptr_pnts[2] = pnts + 2;
		ptr_pnts[3] = pnts + 3;
		rend_SetZBufferState(0);
		rend_SetTextureType(TT_FLAT);
		rend_SetAlphaType(AT_CONSTANT);
		rend_SetLighting(LS_NONE);
		rend_SetFlatColor(color);
		rend_SetAlphaValue(m_UIDrawAlpha);
		rend_DrawPolygon(0,ptr_pnts,4,0);
	}
}
