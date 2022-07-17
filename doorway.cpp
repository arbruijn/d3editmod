#include <stdlib.h>
#include "game.h"
#include "doorway.h"
#include "door.h"
#include "osiris_common.h"
#include "osiris_dll.h"
#include "hlsoundlib.h"
#include "polymodel.h"

void SetDoorObjectRate(room *rp);
object *GetDoorObject(room *rp);
doorway *GetDoorwayFromObject(int door_obj_handle);

int Num_active_doorways;
int Active_doorways[MAX_ACTIVE_DOORWAYS];

int GetDoorModelHandle(int doornum)
{
	return Doors[doornum].model_handle;
}

void RemoveActiveDoorway(int activenum)
{
	int *active;
	int *next;
	
	Rooms[Active_doorways[activenum]].doorway_data->activenum = -1;
	while (activenum + 1 < Num_active_doorways) {
		int doorroom = Active_doorways[activenum + 1];
		Active_doorways[activenum] = doorroom;
		Rooms[doorroom].doorway_data->activenum--;
		activenum++;
	}
	Num_active_doorways--;
}

void AddActiveDoorway(int roomnum)
{
	int activenum;
	doorway *dw;
	
	activenum = Num_active_doorways;
	dw = Rooms[roomnum].doorway_data;
	if (dw->activenum == -1) {
		Active_doorways[Num_active_doorways] = roomnum;
		dw->activenum = (sbyte)activenum;
		Num_active_doorways = Num_active_doorways + 1;
	}
}

void DoorwayPlaySound(object *obj)
{
	int doornum;
	doorway *dw;
	
	dw = Rooms[obj->roomnum].doorway_data;
	doornum = dw->doornum;
	if (dw->sound_handle != -1) {
		Sound_system.StopSoundImmediate(dw->sound_handle);
		dw->sound_handle = -1;
	}
	if ((dw->state == 1) || (dw->state == 4)) {
		if (Doors[doornum].open_sound != -1) {
			doornum = Sound_system.Play3dSound(Doors[dw->doornum].open_sound,3,obj,1.0,0);
			dw->sound_handle = doornum;
		}
	}
	else if (Doors[doornum].close_sound != -1) {
		doornum = Sound_system.Play3dSound(Doors[dw->doornum].close_sound,3,obj,1.0,0);
		dw->sound_handle = doornum;
	}
}

void DoorwayActivate(int objnum)
{
	object *obj;
	doorway *dp;
	tOSIRISEventInfo info;
	ubyte state;
	
	obj = ObjGet(objnum);
	if (!obj)
		return;
	dp = GetDoorwayFromObject(objnum);
	if (!dp || (dp->flags & DF_BLASTED))
		return;
	state = dp->state;
	if (state == DOORWAY_OPENING || state == DOORWAY_WAITING || state == DOORWAY_OPENING_AUTO)
		return;
	dp->dest_pos = 1.0;
	AddActiveDoorway(obj->roomnum);
	dp->state = dp->flags & DF_AUTO ? DOORWAY_OPENING_AUTO : DOORWAY_OPENING;
	DoorwayPlaySound(obj);
	Osiris_CallEvent(obj,EVT_DOOR_ACTIVATE,&info);
}

void DoorwaySetPosition(int objnum,float position)
{
	object *obj;
	doorway *dp;
	
	obj = ObjGet(objnum);
	if (obj != (object *)0x0) {
		dp = GetDoorwayFromObject(objnum);
		if ((dp != (doorway *)0x0) && (dp->dest_pos = position, dp->position != position)) {
			AddActiveDoorway(obj->roomnum);
			if ((ushort)((ushort)(dp->dest_pos < dp->position) << 8 |
									(ushort)(dp->dest_pos == dp->position) << 0xe) == 0) {
				dp->state = DOORWAY_OPENING;
				DoorwayPlaySound(obj);
				return;
			}
			dp->state = DOORWAY_CLOSING;
			DoorwayPlaySound(obj);
		}
	}
}

void DoorwayStop(int objnum)
{
	doorway *dp;
	
	dp = GetDoorwayFromObject(objnum);
	if (dp != (doorway *)0x0) {
		dp->dest_pos = dp->position;
		dp->state = DOORWAY_STOPPED;
		if (dp->sound_handle != -1) {
			Sound_system.StopSoundImmediate(dp->sound_handle);
			dp->sound_handle = -1;
		}
		if (dp->activenum != -1) {
			RemoveActiveDoorway((int)dp->activenum);
		}
	}
}

void DoorwayDestroy(object *obj)
{
	doorway *dp;
	
	dp = Rooms[obj->roomnum].doorway_data;
	dp->position = 1.0;
	dp->state = 0;
	dp->flags = dp->flags | DF_BLASTED;
	if (dp->activenum != -1) {
		RemoveActiveDoorway((int)dp->activenum);
	}
}


void DoorwayDeactivateAll()
{
	int roomnum;
	doorway *dp;
	
	roomnum = 0;
	dp = Rooms[0].doorway_data;
	if (Highest_room_index < 0) {
		Num_active_doorways = 0;
		return;
	}
	do {
		if (Rooms[0].used && (Rooms[0].flags & RF_DOOR) && dp->state) {
			dp->activenum = -1;
			dp->position = dp->dest_pos;
			dp->state = 0;
			if (dp->sound_handle != -1) {
				Sound_system.StopSoundImmediate(dp->sound_handle);
				dp->sound_handle = -1;
			}
			SetDoorObjectRate(Rooms);
			dp = Rooms[0].doorway_data;
		}
		roomnum = roomnum + 1;
	} while (roomnum <= Highest_room_index);
	Num_active_doorways = 0;
}

void SetDoorObjectRate(room *rp)
{
	object *obj;
	int model;
	float pos;
	
	obj = GetDoorObject(rp);
	if (!obj)
		return;
	model = GetDoorModelHandle((uint)obj->id);
	pos = DoorwayGetPosition(rp);
	if ((Poly_models[model].flags & PMF_TIMED))
		obj->rtype.pobj_info.anim_frame = Poly_models[model].frame_max * pos;
	else
		obj->rtype.pobj_info.anim_frame = Poly_models[model].max_keys * pos;
}

void DoorwayDoFrame()
{
	room *rp;
	tOSIRISEventInfo info;
	int doornum;
	doorway *dp;
	float pos;
	
	for (int i = 0; i < Num_active_doorways; i++) {
		rp = Rooms + Active_doorways[i];
		dp = rp->doorway_data;
		doornum = dp->doornum;
		switch(dp->state) {
			case DOORWAY_OPENING:
			case DOORWAY_OPENING_AUTO:
				dp->position += Frametime / Doors[doornum].total_open_time;
				if (dp->position >= dp->dest_pos) {
					dp->position = dp->dest_pos;
					if (dp->state == DOORWAY_OPENING_AUTO) {
						dp->state = DOORWAY_WAITING;
						dp->dest_pos = Doors[doornum].total_time_open;
					} else {
						dp->state = DOORWAY_STOPPED;
						RemoveActiveDoorway(i);
					}
				}
				break;
			case DOORWAY_CLOSING:
				dp->position -= Frametime / Doors[doornum].total_close_time;
				if (dp->position <= dp->dest_pos) {
					dp->position = dp->dest_pos;
					dp->state = 0;
					RemoveActiveDoorway(i);
					Osiris_CallEvent(GetDoorObject(rp),EVT_DOOR_CLOSE,&info);
				}
				break;
			case DOORWAY_WAITING:
				dp->dest_pos -= Frametime;
				if (dp->dest_pos <= 0) {
					object *obj = GetDoorObject(rp);
					if (obj->next == -1 && obj->prev == -1) {
						dp->dest_pos = 0.0;
						dp->state = DOORWAY_CLOSING;
						DoorwayPlaySound(obj);
					}
				}
		}
		SetDoorObjectRate(rp);
	}
}

int DoorwayState(int door_obj_handle)
{
	doorway *dp;
	
	dp = GetDoorwayFromObject(door_obj_handle);
	return dp ? dp->state : DOORWAY_STOPPED;
}


void DoorwayLockUnlock(int door_obj_handle,bool lock)
{
	doorway *dp;
	
	dp = GetDoorwayFromObject(door_obj_handle);
	if (!dp)
		return;
	if (lock)
		dp->flags |= DF_LOCKED;
	else
		dp->flags &= ~DF_LOCKED;
}
