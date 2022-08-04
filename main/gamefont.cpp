#include "gr.h"
#include "grtext.h"
#include "pserror.h"
#include "stringtable.h"
#include "gamefont.h"

int HUD_font_resolution;
bool Hud_font_template_init;
tFontTemplate Hud_font_template;
char *HUD_font_files[] = { "lohud.fnt", "hihud.fnt" };
int Game_fonts[MAX_GAME_FONTS];

void FreeAuxFontData()
{
	if (!Hud_font_template_init)
	    return;
    Hud_font_template_init = false;
    //grfont_FreeTemplate(&Hud_font_template);
}

int LoadFont(char *fname)
{
	int fontnum;
	fontnum = grfont_Load(fname);
	if (fontnum == -1)
	    Error(TXT_ERRNOFONT,fname);
	return fontnum;
}

void LoadAllFonts()
{
	grfont_Reset();
	atexit(FreeAuxFontData);
	//if (Hud_font_template_init)
	//	grfont_FreeTemplate(&Hud_font_template);
	Game_fonts[0] = LoadFont("briefing.fnt");
	Game_fonts[1] = LoadFont("bbriefing.fnt");
	Game_fonts[3] = LoadFont("newmenu.fnt");
	extern int is_demo;
	if (!is_demo) {
		Game_fonts[4] = LoadFont("smallui.fnt");
		Game_fonts[5] = LoadFont("largeui.fnt");
	}
	HUD_font_resolution = 0;
	Game_fonts[2] = LoadFont(HUD_font_files[0]);
	//if (!grfont_LoadTemplate(HUD_font_files[HUD_font_resolution],&Hud_font_template))
	//	Error(TXT_ERRNOFONT,HUD_font_files[HUD_font_resolution]);
	Hud_font_template_init = 1;
}
