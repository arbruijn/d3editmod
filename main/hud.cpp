#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include "hud.h"
#include "game.h"
#include "grtext.h"
#include "gamefont.h"
#include "weapon.h"
#include "polymodel.h"
#include "ship.h"
#include "robotfire.h"
#include "pstring.h"
#include "multi.h"
#include "stringtable.h"

tHUDItem HUD_array[MAX_HUD_ITEMS];
float Score_added_timer;
int Score_added;
bool Small_hud_flag;
ushort Hud_stat_mask;

bool AddHUDMessage(char *msg, ...) {
	va_list vp;
	va_start(vp, msg);
	vprintf(msg, vp);
	va_end(vp);
	return false;
}

bool AddFilteredHUDMessage(char *msg, ...) {
	va_list vp;
	va_start(vp, msg);
	vprintf(msg, vp);
	va_end(vp);
	return false;
}

void AddGameMessage(char *msg) {
	printf("game message: %s\n", msg);
}

void RenderHUDTextFlagsNoFormat(int flags,ddgr_color color,ubyte alpha,int sat_count,int x,int y,char *text)
{
	int sx, sy;

	grtext_SetAlpha(alpha);
	grtext_SetFlags(sat_count ? 1 : 0);
	grtext_SetColor(color);
	sx = x * Hud_aspect_x;
	sy = y * Hud_aspect_y;
	for (int i = 0; i < sat_count + 1; i++)
		if (flags & 1)
			grtext_CenteredPrintf(0,sy,text);
		else
			grtext_Puts(sx,sy,text);
}

void RenderHUDText(ddgr_color col,ubyte alpha,int sat_count,int x,int y,char *fmt,...)
{
	char text [128];
	va_list vp;

	va_start(vp, fmt);
	vsnprintf(text,sizeof(text),fmt,vp);
	va_end(vp);
	RenderHUDTextFlagsNoFormat(0,col,alpha,sat_count,x,y,text);
}

void RenderHUDTextFlags(int flags,ddgr_color col,ubyte alpha,int sat_count,int x,int y,char *fmt,...)
{
	char text [128];
	va_list vp;

	va_start(vp, fmt);
	vsnprintf(text,sizeof(text),fmt,vp);
	va_end(vp);
	RenderHUDTextFlagsNoFormat(0,col,alpha,sat_count,x,y,text);
}

int RenderHUDGetTextLineWidth(char *text)
{
	int width, width2;
	int font;
	float scale;

	width = grtext_GetTextLineWidth(text);
	#if 0
	font = grtext_GetFont();
	if (font == Game_fonts[2]) {
		width2 = grtext_GetTextLineWidthTemplate(&Hud_font_template,text);
		scale = width2 / width;
	} else
	#endif
		scale = 1;
	if (scale == 1)
		scale *= 640.0 / Max_window_w;
	return width * scale;
}

float Hud_aspect_x, Hud_aspect_y;

void RenderAuxHUDFrame()
{
}

tHUDItem pmsg;
char pmsg_text[400];
int Hud_persistent_msg_current_len;
float Hud_persistent_msg_char_timer;
float Hud_persistent_msg_timer;

void StartPersistentHUDMessage(ddgr_color color,int x,int y,float time,uint flags,int sound_index,char *msg)
{
	snprintf(pmsg_text,sizeof(pmsg_text),"%s",msg);

	if (x == -1) {
		int w = grtext_GetTextLineWidth(msg);
		x = (Game_window_w - w) / 2 + Game_window_x;
	}
	if (y == -1) {
    	int h = grfont_GetHeight(Game_fonts[2]);
    	y = (Game_window_h - h) / 2 + Game_window_y;
	}
	memset(&pmsg, 0, sizeof(pmsg));
	pmsg.alpha = HUD_ALPHA;
	pmsg.color = color;
	pmsg.x = ((float)x * 640) / Max_window_w;
	pmsg.y = ((float)y * 480) / Max_window_h;
	pmsg.data.text = pmsg_text;
	if (flags & 2) {
		pmsg.alpha = 0xff;
		pmsg.saturation_count = 2;
	}
	Hud_persistent_msg_timer = time;
    Hud_persistent_msg_current_len = 0;
    Hud_persistent_msg_char_timer = 0.03;
}

void AddPersistentHUDMessage(ddgr_color color,int x, int y, float time, int flags, int sound_index, char *fmt, ...) {
	char msg[400];
	va_list vp;
	va_start(vp, fmt);
	vsnprintf(msg,sizeof(msg),fmt,vp);
	va_end(vp);
	printf("persistent msg: %s\n", msg);
	StartPersistentHUDMessage(color, x, y, time, flags, sound_index, msg);
}

void RenderPersistentHUDMessage() {
	if (Hud_persistent_msg_timer <= 0)
		return;

	Hud_persistent_msg_timer -= Frametime;

	if (Hud_persistent_msg_timer < 0.5)
		pmsg.alpha = Hud_persistent_msg_timer * 384;

	for (Hud_persistent_msg_char_timer -= Frametime;
		pmsg_text[Hud_persistent_msg_current_len] && Hud_persistent_msg_char_timer < 0.0;
		Hud_persistent_msg_char_timer += 0.03)
        Hud_persistent_msg_current_len++;

	char c = pmsg_text[Hud_persistent_msg_current_len];
	pmsg_text[Hud_persistent_msg_current_len] = 0;
	tHUDItem *item = &pmsg;
	RenderHUDTextFlags(0,item->color,item->alpha,item->saturation_count,item->x,item->y,item->data.text);
	pmsg_text[Hud_persistent_msg_current_len] = c;
}
int Ret_prim_mask, Ret_sec_mask;

char *Reticle_image_names[][3] = {
	{ "_pr0a.ogf", "_pr0b.ogf", "c" },
	{ "_pr13a.ogf", "_pr1b.ogf", "l" },
	{ "_pr24a.ogf", "_pr2b.ogf", "r" },
	{ "_pr13a.ogf", "_pr3b.ogf", "l" },
	{ "_pr24a.ogf", "_pr4b.ogf", "r" },
	{ "_pr5a.ogf", "_pr5b.ogf", "c" },
	{ NULL, NULL, "l" },
	{ NULL, NULL, "l" },
	{ "_se0a.ogf", "_se0b.ogf", "c" },
	{ "_se1a.ogf", "_se1b.ogf", "l" },
	{ "_se2a.ogf", "_se2b.ogf", "r" },
	{ "_se3a.ogf", "_se3b.ogf", "l" },
	{ "_se4a.ogf", "_se4b.ogf", "r" },
	{ NULL, NULL, "l" },
	{ NULL, NULL, "l" },
	{ NULL, NULL, "l" },
	{ "_lguna.ogf", NULL, "l" },
	{ "_rguna.ogf", NULL, "r" } };
#define ELMS(x) ((int)(sizeof(x)/sizeof(x)[0]))
struct {
	int bm_a, bm_b;
	char pos;
} Reticle_elem_array[ELMS(Reticle_image_names)];
char Reticle_prefix[PAGENAME_LEN + 1];

void RenderHUDQuad(int x,int y,int w,int h,float u0,float v0,float u1,float v1,int bm,
									ubyte alpha,int sat_count)
{
	rend_SetZBufferState(0);
	rend_SetTextureType(TT_LINEAR);
	rend_SetAlphaValue(alpha);
	rend_SetLighting(LS_NONE);
	rend_SetWrapType(WT_CLAMP);
	rend_SetAlphaType(sat_count ? AT_SATURATE_TEXTURE : AT_CONSTANT_TEXTURE);
	int rx = x * Hud_aspect_x, ry = y * Hud_aspect_y;
	int rw = w * Hud_aspect_x, rh = h * Hud_aspect_y;
	for (int i = sat_count + 1; i; i--)
		rend_DrawScaledBitmap(rx,ry,rx + rw,ry + rh,bm,u0,v0,u1,v1,1.0,-1,NULL);
	rend_SetZBufferState(1);
}

void InitReticle(int primary_slots, int secondary_slots)
{
	char *img;
	char filename[PAGENAME_LEN + 1];

	for (int i = 0; i < ELMS(Reticle_image_names); i++) {
		int bm_a = -1, bm_b = -1;
		if ((img = Reticle_image_names[i][0]) && primary_slots >= 0) {
			sprintf(filename, "%s%s", Reticle_prefix, img);
			if ((bm_a = bm_AllocLoadFileBitmap(filename, 0, 0)) < 1)
				bm_a = -1;
		}
		Reticle_elem_array[i].bm_a = bm_a;
		if ((img = Reticle_image_names[i][1]) && primary_slots >= 0) {
			sprintf(filename, "%s%s", Reticle_prefix, img);
			if ((bm_b = bm_AllocLoadFileBitmap(filename, 0, 0)) < 1)
				bm_b = -1;
		}
		Reticle_elem_array[i].bm_b = bm_b;
		Reticle_elem_array[i].pos = *Reticle_image_names[i][2];
	}
}

void FreeReticle()
{
	for (int i = 0; i < ELMS(Reticle_elem_array); i++) {
		if (Reticle_elem_array[i].bm_a > 0)
			bm_FreeBitmap(Reticle_elem_array[i].bm_a);
		Reticle_elem_array[i].bm_a = -1;
		if (Reticle_elem_array[i].bm_b > 0)
			bm_FreeBitmap(Reticle_elem_array[i].bm_b);
		Reticle_elem_array[i].bm_b = -1;
	}
}

void ResetReticle()
{
	player *plr;
	object *plrobj;
	ship *ship;
	otype_wb_info *prim_wb, *sec_wb;
	bool quad;
	int num_gps;

	plr = Players + Player_num;
	plrobj = Objects + plr->objnum;
	if (plrobj->type != OBJ_PLAYER && plrobj->type != OBJ_GHOST && plrobj->type != OBJ_OBSERVER)
		return;
	ship = Ships + plr->ship_index;
	prim_wb = ship->static_wb + plr->weapon[0].index;
	quad = (plrobj->dynamic_wb[plr->weapon[0].index].flags & DWBF_QUAD) != 0;
	sec_wb = ship->static_wb + plr->weapon[1].index;
	num_gps = Poly_models[plrobj->rtype.pobj_info.model_num].poly_wb->num_gps;
	Ret_prim_mask = 0;
	for (int i = 0; i < prim_wb->num_masks; i++) {
		int mask = prim_wb->gp_fire_masks[i], quadmask = prim_wb->gp_quad_fire_mask;
		for (int j = 0; j < num_gps; j++) {
			int bit = 1 << j;
			if ((mask & bit) || (quad && (quadmask & bit)))
				Ret_prim_mask |= bit;
		}
	}
	if (plr->weapon[0].index == MASSDRIVER_INDEX && (Ret_prim_mask & 1))
		Ret_prim_mask = (Ret_prim_mask & ~1) | 0x100;
	Ret_sec_mask = 0;
	for (int i = 0; i < sec_wb->num_masks; i++)
		for (int j = 0; j < num_gps; j++)
			if (sec_wb->gp_fire_masks[i] & (1 << j))
				Ret_sec_mask |= 1 << j;
}

void RenderReticleWeaponType(int x,int y,int w,int h,int ready_mask,int mask,int *gp_to_elem)
{
	char pos;
	int bit;
	int bmy;
	int bmx;
	int bmhandle;
	int elem;

	for (int i = 0; i < 9; i++) {
		int bit = 1 << i;
		if (!(mask & bit))
			continue;
		elem = gp_to_elem[i];
		bmhandle = ready_mask & bit ? Reticle_elem_array[elem].bm_b : Reticle_elem_array[elem].bm_a;
		if (bmhandle < 0)
			continue;
		pos = Reticle_elem_array[elem].pos;
		bmy = y - h / 2;
		if (pos == 'c')
			bmx = x - w / 2;
		else if (pos == 'l')
			bmx = x - w;
		else if (pos =='r')
			bmx = x;
		else {
			bmx = x;
			bmy = y;
		}
		RenderHUDQuad(bmx,bmy,w,h,0.0,0.0,1.0,1.0,bmhandle,HUD_ALPHA,0);
	}
}

short Ret_x_off, Ret_y_off;
int Ret_prim_wb[] = { 0, 2, 1, 4, 3, -1, -1, -1, 5 };
int Ret_sec_wb[] = { 8, 10, 9, 12, 11, -1, -1, -1, -1 };

int ReticleReadyMask(object *plrobj, otype_wb_info *wb, int idx)
{
	player *plr = Players + plrobj->id;
	int ready_mask = 0;
	if (WBIsBatteryReady(plrobj, wb, idx) &&
		(!wb->ammo_usage || plr->weapon_ammo[idx]) &&
		(!wb->energy_usage || plr->energy >= wb->energy_usage)) {
		bool quad = (plrobj->dynamic_wb[idx].flags & DWBF_QUAD) != 0;
		int mask_idx = plrobj->dynamic_wb[idx].cur_firing_mask;
		int num_gps = Poly_models[plrobj->rtype.pobj_info.model_num].poly_wb->num_gps;
		for (int i = 0; i < num_gps; i++) {
			int bit = 1 << i;
			if ((wb->gp_fire_masks[mask_idx] & bit) ||
				(quad && (wb->gp_quad_fire_mask & bit)))
				ready_mask |= bit;
		}
		if (idx == MASSDRIVER_INDEX && (ready_mask & 1))
			ready_mask = (ready_mask & ~1) | 0x100;
	}
	return ready_mask;
}

void RenderReticle()
{
	player *plr;
	object *plrobj;
	ship *ship;
	otype_wb_info *prim_wb, *sec_wb;
	bool quad;
	int num_gps;
	int ready_mask;
	int prim_idx, sec_idx;
	static bool quad_prim_last_frame;
	static int prim_idx_last_frame;

	plr = Players + Player_num;
	plrobj = Objects + plr->objnum;
	ship = Ships + plr->ship_index;
	prim_idx = plr->weapon[0].index;
	sec_idx = plr->weapon[1].index;
	prim_wb = ship->static_wb + prim_idx;
	quad = (plrobj->dynamic_wb[prim_idx].flags & DWBF_QUAD) != 0;
	sec_wb = ship->static_wb + sec_idx;
	if (quad != quad_prim_last_frame || prim_idx_last_frame != prim_idx) {
		ResetReticle();
		quad_prim_last_frame = quad;
	}
	prim_idx_last_frame = prim_idx;

	ready_mask = ReticleReadyMask(plrobj, prim_wb, prim_idx);
	RenderReticleWeaponType(Ret_x_off + 320, Ret_y_off + 246, 32, 32, ready_mask, Ret_prim_mask, Ret_prim_wb);

	ready_mask = ReticleReadyMask(plrobj, sec_wb, sec_idx);
	RenderReticleWeaponType(Ret_x_off + 320, Ret_y_off + 246, 32, 32, ready_mask, Ret_sec_mask, Ret_sec_wb);

	if (Reticle_elem_array[16].bm_a >= 0)
	    RenderHUDQuad(Ret_x_off + 288,Ret_y_off + 230,32,32,0.0,0.0,1.0,1.0,Reticle_elem_array[16].bm_a,HUD_ALPHA,0);
	if (Reticle_elem_array[17].bm_a >= 0)
	    RenderHUDQuad(Ret_x_off + 320,Ret_y_off + 230,32,32,0.0,0.0,1.0,1.0,Reticle_elem_array[17].bm_a,HUD_ALPHA,0);
}

void HudDisplayRouter(tHUDItem *item) {
}

void FreeHUDItem(int item)
{
	tHUDItem *itemp = HUD_array + item;
	itemp->stat = 0;
	itemp->flags = 0;
	itemp->id = 255;
	if ((itemp->type == HUD_ITEM_CUSTOMTEXT || itemp->type == HUD_ITEM_CUSTOMTEXT2) && itemp->data.text) {
		free(itemp->data.text);
		itemp->data.text = NULL;
	}
}

void FreeHUDItems()
{
	for (int i = 0; i < MAX_HUD_ITEMS; i++)
		if (HUD_array[i].stat && !(HUD_array[i].flags & (HUD_FLAG_PERSISTANT | HUD_FLAG_LEVEL)))
			FreeHUDItem(i);
}

void AddHUDItemAt(int pos, tHUDItem *item)
{
	char *text;
	tHUDItem *dst;
	int stat;

	switch (item->type) {
	case HUD_ITEM_PRIMARY:
		stat = STAT_PRIMARYLOAD;
		break;
	case HUD_ITEM_SECONDARY:
		stat = STAT_SECONDARYLOAD;
		break;
	case HUD_ITEM_SHIELD:
		stat = STAT_SHIELDS;
		break;
	case HUD_ITEM_ENERGY:
		stat = STAT_ENERGY;
		break;
	case HUD_ITEM_AFTERBURNER:
		stat = STAT_AFTERBURN;
		break;
	case HUD_ITEM_INVENTORY:
		stat = STAT_INVENTORY;
		break;
	case HUD_ITEM_SHIPSTATUS:
		stat = STAT_SHIP;
		break;
	case HUD_ITEM_WARNINGS:
		stat = STAT_WARNING;
		break;
	case HUD_ITEM_GOALS:
	case HUD_ITEM_GOALSTATES:
		stat = STAT_GOALS;
		break;
	case HUD_ITEM_CNTRMEASURE:
		stat = STAT_CNTRMEASURE;
		break;
	case HUD_ITEM_SCORE:
		stat = STAT_TIMER;
		break;
	case HUD_ITEM_CUSTOMTEXT:
		if (item->data.text)
			HUD_array[pos].data.text = strdup(item->data.text);
		stat = STAT_CUSTOM;
		break;
	case HUD_ITEM_CUSTOMIMAGE:
		stat = STAT_CUSTOM;
		break;
	case HUD_ITEM_TIMER:
		HUD_array[pos].data.timer_handle = item->data.timer_handle;
		stat = STAT_CUSTOM;
		break;
	case HUD_ITEM_CUSTOMTEXT2:
		text = (char *)malloc(item->buffer_size);
		HUD_array[pos].data.text = text;
		*text = 0;
		HUD_array[pos].buffer_size = item->buffer_size;
		stat = STAT_CUSTOM;
		break;
	}
	dst = HUD_array + pos;
	dst->x = item->x;
	dst->y = item->y;
	dst->xa = item->xa;
	dst->ya = item->ya;
	dst->xb = item->xb;
	dst->yb = item->yb;
	dst->tx = item->tx;
	dst->ty = item->ty;
	dst->grscalex = item->grscalex;
	dst->grscaley = item->grscaley;
	dst->type = item->type;
	dst->stat = item->stat | stat;
	dst->flags = item->flags;
	dst->alpha = item->alpha;
	dst->color = item->color;
	dst->tcolor = item->tcolor;
	dst->saturation_count = item->saturation_count;
	dst->render_fn = item->render_fn;
	dst->id = item->id;
}

void AddHUDItem(tHUDItem *item)
{
	for (int i = 0; i < MAX_HUD_ITEMS; i++)
		if (!HUD_array[i].stat) {
			AddHUDItemAt(i, item);
			break;
		}
}

void RenderHUDScore(tHUDItem *item)
{
	int right_pad;
	int width, height;
	float alpha;
	char text [100];

	if ((Game_mode & 0x24) && !(Netgame.flags & 4))
		return;
	sprintf(text,"%s: %d ",TXT_SCORE,Players[Player_num].score);
	right_pad = (Max_window_w - Game_window_w) * Hud_aspect_x;
	width = RenderHUDGetTextLineWidth(text);
	RenderHUDText(item->color,HUD_ALPHA,0,item->x - width - right_pad,item->y,text);

	if (Score_added_timer > 0) {
		height = grfont_GetHeight(Game_fonts[2]);
		sprintf(text,"%d   ",Score_added);
		width = RenderHUDGetTextLineWidth(text);
		alpha = Score_added_timer * 768.0 / 2;
		if (alpha > HUD_ALPHA)
			alpha = HUD_ALPHA;
		RenderHUDText(item->color,alpha,0,item->x - width - right_pad,item->y + height,text);
		Score_added_timer -= Frametime;
	}
}

void InitDefaultHUDItems()
{
	tHUDItem item;

	memset(&item, 0, sizeof(item));
	item.type = HUD_ITEM_SCORE;
	item.stat = STAT_TIMER;
	item.x = 640;
	item.y = 5;
	item.color = HUD_COLOR;
	item.render_fn = RenderHUDScore;
	AddHUDItem(&item);
}

typedef bool (tParseFun)(const char *keyword, const char *value, void *data);

void LoadHUDConfig(const char *config_name,tParseFun *extra_fun,void *extra_data)
{
	CFILE *cf;
	char keyword [32];
	tHUDItem item;
	char value [96];
	char line [128];
	bool have_textpos;
	int i1, i2, i3;

	FreeHUDItems();
	InitDefaultHUDItems();
	cf = cfopen(config_name,"rt");
	if (!cf)
		return;
	cf_ReadString(line,sizeof(line),cf);
	if (stricmp(line, "[hud file]") != 0)
		goto end;
	memset(&item, 0, sizeof(item));
	item.alpha = HUD_ALPHA;
	item.color = HUD_COLOR;
	item.tcolor = HUD_COLOR;
	have_textpos = false;
	while (!cfeof(cf)) {
		if (cf_ReadString(line,sizeof(line),cf) == 0)
			continue;
		strtok(line," \t=");
		CleanupStr(keyword,line,sizeof(keyword));
		CleanupStr(value,line + strlen(keyword) + 1,sizeof(value));
		if (keyword[0] == '@')
			continue;
		if (stricmp(keyword,"type") == 0) {
			item.type = atoi(value);
		} else if (stricmp(keyword,"pos") == 0) {
			sscanf(value,"%d,%d",&i1,&i2);
			item.x = i1;
			item.y = i2;
		} else if (stricmp(keyword,"posA") == 0) {
			sscanf(value,"%d,%d",&i1,&i2);
			item.xa = i1;
			item.ya = i2;
		} else if (stricmp(keyword,"posB") == 0) {
			sscanf(value,"%d,%d",&i1,&i2);
			item.xb = i1;
			item.yb = i2;
		} else if (stricmp(keyword,"grscale") == 0) {
			sscanf(value,"%f,%f",&item.grscalex,&item.grscaley);
		} else if (stricmp(keyword,"textpos") == 0) {
			sscanf(value,"%d,%d",&i1,&i2);
			item.tx = i1;
			item.ty = i2;
			have_textpos = true;
		} else if (stricmp(keyword,"alpha") == 0) {
			item.alpha = atoi(value);
		} else if (stricmp(keyword,"sat") == 0) {
			item.saturation_count = atoi(value);
		} else if (stricmp(keyword,"rgb") == 0) {
			sscanf(value,"%d,%d,%d",&i1,&i2,&i3);
			item.color = GR_RGB(i1,i2,i3);
		} else if (stricmp(keyword,"textrgb") == 0) {
			sscanf(value,"%d,%d,%d",&i1,&i2,&i3);
			item.tcolor = GR_RGB(i1,i2,i3);
		} else if (stricmp(keyword,"special") == 0) {
			item.stat |= STAT_SPECIAL;
		} else if (stricmp(keyword,"create") == 0) {
			if (!have_textpos) {
				item.tx = item.x;
				item.ty = item.y;
			}
			item.render_fn = HudDisplayRouter;
			AddHUDItem(&item);
			memset(&item, 0, sizeof(item));
			item.alpha = HUD_ALPHA;
			item.color = HUD_COLOR;
			item.tcolor = HUD_COLOR;
			have_textpos = false;
		} else if (stricmp(keyword,"reticleprefix") == 0) {
			strcpy(Reticle_prefix, value);
		} else if (stricmp(keyword,"reticleoffset") == 0) {
			sscanf(value,"%d,%d",&i1,&i2);
			Ret_x_off = i1;
			Ret_y_off = i2;
		} else if (extra_fun) {
			if (!extra_fun(keyword,value,extra_data))
				break;
		}
	}
end:
	FreeReticle();
	InitReticle(0,0);
	cfclose(cf);
}

void RenderHUDItems(ushort stat_mask)
{
	static float framerate_timer, last_fps;
	grtext_Reset();
	grtext_SetFont(Game_fonts[2]);

	framerate_timer -= Frametime;
	while (framerate_timer < 0) {
		framerate_timer += 0.25f;
		last_fps = GetFPS();
	}

	if (stat_mask & 1)
		RenderHUDText(HUD_COLOR,HUD_ALPHA,0,10,10, "FPS: %.1f", last_fps);

	for (int i = 0; i < MAX_HUD_ITEMS; i++) {
		tHUDItem *item = HUD_array + i;
		if ((!(item->flags & HUD_FLAG_SMALL) || Small_hud_flag) && (item->stat & stat_mask)) {
			if (item->render_fn)
				item->render_fn(item);
			else
				HudDisplayRouter(item);
		}
	}

	grtext_Flush();
}

void RenderHUDFrame()
{
	rend_SetOverlayType(OT_NONE);
	Hud_aspect_x = Game_window_w / 640;
	Hud_aspect_y = Game_window_h / 480;

	grtext_Reset();
	grtext_SetFont(Game_fonts[2]);

	//RenderHUDTextFlags(1, 0xffffff, 0xff, 0, 320, 200, "hello world");
	//grtext_Flush();
	RenderPersistentHUDMessage();
	grtext_Flush();

	RenderHUDItems(Hud_stat_mask);
	RenderReticle();
}
