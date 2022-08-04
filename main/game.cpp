#include "game.h"

int Game_window_x,Game_window_y,Game_window_w,Game_window_h;
int Max_window_w,Max_window_h;

void InitGameScreen(int w,int h)
{
	if (w > Max_window_w)
		w = Max_window_w;
	if (h > Max_window_h)
		h = Max_window_h;
	Game_window_h = h;
	Game_window_w = w;
	Game_window_y = (Max_window_h - h) / 2;
	Game_window_x = (Max_window_w - w) / 2;
}

void SetScreenSize(int w, int h)
{
	Max_window_w = w;
	Max_window_h = h;
	InitGameScreen(w, h);
}
