#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include "hud.h"
#include "game.h"
#include "grtext.h"
#include "gamefont.h"

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
}

void RenderHUDTextFlags(int flags,ddgr_color color,ubyte alpha,int sat_count,int x,int y,char *fmt,...)
{
	int sx, sy;
	char text[128];
	va_list vp;

	va_start(vp,fmt);
	snprintf(text,sizeof(text),fmt,vp);
	va_end(vp);
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

	memset(&pmsg, 0, sizeof(pmsg));
	pmsg.alpha = 0xc0;
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
}
