#include "object.h"
#include "osiris_dll.h"
#include "game.h"

#define OTFI_USED	0x0001
#define OTFI_REPEATER	0x0002
#define OTFI_TRIGGER		0x0004
#define OTFI_LEVEL		0x0008
#define OTFI_CANCELONDEAD 0x0010

static void CallTimer(int timer_id, int flags, int handle)
{
	tOSIRISEventInfo data;
	data.evt_timer.id = timer_id;
	data.evt_timer.game_time = Gametime;
	printf("CallTimer %d %s %d\n", timer_id, flags & OTFI_TRIGGER ? "trig" : flags & OTFI_LEVEL ? "lvl" : "obj", handle);
	if (flags & OTFI_TRIGGER)
		Osiris_CallTriggerEvent(handle,EVT_TIMER,&data);
	else if (flags & OTFI_LEVEL)
		Osiris_CallLevelEvent(EVT_TIMER,&data);
	else {
		object *obj = ObjGet(handle);
		if (obj)
			Osiris_CallEvent(obj,EVT_TIMER,&data);
	}
}

int Osiris_CreateTimer(tOSIRISTIMER *timer_info)
{
	CallTimer(timer_info->id, timer_info->flags << 1, timer_info->object_handle);
	return 1;
}
