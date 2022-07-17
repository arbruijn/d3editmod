#include "object.h"
#include "aimain.h"

void osipf_ObjectCustomAnim(int objnum,float start,float end,float time,char flags,int sound,char next_type)
{
	object *obj;
	
	obj = ObjGet(objnum);
	if (!obj)
		return;
	obj->rtype.pobj_info.anim_flags |= AIAF_NOTIFY;
	obj->rtype.pobj_info.custom_anim_info.flags = flags;
	obj->rtype.pobj_info.custom_anim_info.anim_sound_index = sound;
	obj->rtype.pobj_info.custom_anim_info.anim_time = time;
	obj->rtype.pobj_info.custom_anim_info.next_anim_type = next_type;
	obj->rtype.pobj_info.custom_anim_info.anim_start_frame = start;
	obj->rtype.pobj_info.custom_anim_info.anim_end_frame = end;
	if (obj->ai_info)
		obj->ai_info->next_animation_type = AS_CUSTOM;
	//if (flags & AIAF_IMMEDIATE)
	//	AIUpdateAnim(obj);
}

void osipf_LGoalValue(char action,char type,void *val,int goal,int item) {
}
