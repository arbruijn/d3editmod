#ifndef GAMEDLL_H
#define GAMEDLL_H

#include "vecmat.h"
#include "d3events.h"

typedef struct
{
	vector point,normal;
	float hitspeed, hit_dot;
	int hitseg, hitwall;
}game_collide_info;

typedef struct 
{
	int me_handle;
	int it_handle;
	ubyte *special_data;
	char	*input_string;
	int input_key;
	union{
		int iRet;
		float fRet;
	};
	float fParam;
	int iParam;
	game_collide_info collide_info;
	int newseg,oldseg;
} dllinfo;

extern dllinfo DLLInfo;

void CallGameDLL(int evt, dllinfo *info);

#endif
