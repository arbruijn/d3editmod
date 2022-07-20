#include "object.h"
#include "aimain.h"
#include "osiris_common.h"
#include "levelgoal.h"

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


void osipf_LGoalValue(char op,char vtype,void *value,int goal,int item)
{
	int cur, clear, set;
	char *ptype;
	int *phandle;
	bool *pdone;
	switch(vtype) {
	case LGV_I_STATUS:
		if (op == VF_GET)
			Level_goals.LGStatus(LO_GET_SPECIFIED,(int *)value);
		else if (op == VF_SET_FLAGS)
			Level_goals.LGStatus(LO_SET_SPECIFIED,(int *)value);
		else if (op == VF_CLEAR_FLAGS)
			Level_goals.LGStatus(LO_CLEAR_SPECIFIED,(int *)value);
		else if (op == VF_SET) {
			Level_goals.LGStatus(LO_GET_SPECIFIED,&cur);
			clear = (*(int *)value ^ cur) & cur;
			set = ~cur & (*(int *)value ^ cur);
			if (clear)
				Level_goals.LGStatus(LO_CLEAR_SPECIFIED,&clear);
			if (set)
				Level_goals.LGStatus(LO_SET_SPECIFIED,&set);
		}
		break;
	case LGV_I_NUM_ACTIVE_PRIMARIES:
		if (op == VF_GET)
			*(int *)value = Level_goals.GetNumActivePrimaryGoals();
		break;
	case LGSV_I_ACTIVE_PRIMARY_GOAL:
		if (op == VF_GET)
			*(int *)value = Level_goals.GetActivePrimaryGoal(goal);
		break;
	case LGV_I_NUM_ACTIVE_SECONDARIES:
		if (op == VF_GET)
			*(int *)value = Level_goals.GetNumActiveSecondaryGoals();
		break;
	case LGSV_I_ACTIVE_SECONDARY_GOAL:
		if (op == VF_GET)
			*(int *)value = Level_goals.GetActiveSecondaryGoal(goal);
		break;
	case LGV_I_NUM_GOALS:
		if (op == VF_GET)
			*(int *)value = Level_goals.GetNumGoals();
		break;
	case LGSV_PC_GOAL_NAME:
		if (op == VF_GET)
			Level_goals.GoalGetName(goal,(char *)value,256);
		else if (op == VF_SET)
			Level_goals.GoalSetName(goal,(char *)value);
		break;
	case LGSV_PC_LOCATION_NAME:
		if (op == VF_GET)
			Level_goals.GoalGetItemName(goal,(char *)value,0x100);
		else if (op == VF_SET)
			Level_goals.GoalSetItemName(goal,(char *)value);
		break;
	case LGSV_PC_DESCRIPTION:
		if (op == VF_GET)
			Level_goals.GoalGetDesc(goal,(char *)value,0x100);
		else if (op == VF_SET)
			Level_goals.GoalSetDesc(goal,(char *)value);
		break;
	case LGSV_PC_COMPLETION_MESSAGE:
		if (op == VF_GET)
			Level_goals.GoalGetCompletionMessage(goal,(char *)value,0x100);
		else if (op == VF_SET)
			Level_goals.GoalSetCompletionMessage(goal,(char *)value);
		break;
	case LGSV_I_PRIORITY:
		if (op == VF_GET)
			Level_goals.GoalPriority(goal,LO_GET_SPECIFIED,(int *)&value); // bug?
		else if (op == VF_SET)
			Level_goals.GoalPriority(goal,LO_SET_SPECIFIED,(int *)&value); // bug?
		break;
	case LGSV_C_GOAL_LIST:
		if (op == VF_GET)
			Level_goals.GoalGoalList(goal,LO_GET_SPECIFIED,(char *)value);
		if (op == VF_SET)
			Level_goals.GoalGoalList(goal,LO_SET_SPECIFIED,(char *)value);
		break;
	case LGSV_I_STATUS:
		if (op == VF_GET)
			Level_goals.GoalStatus(goal,LO_GET_SPECIFIED,(int *)value,true);
		else if (op == VF_SET_FLAGS)
			Level_goals.GoalStatus(goal,LO_SET_SPECIFIED,(int *)value,true);
		else if (op == VF_CLEAR_FLAGS)
			Level_goals.GoalStatus(goal,LO_CLEAR_SPECIFIED,(int *)value,true);
		else if (op == VF_SET) {
			Level_goals.GoalStatus(goal,LO_GET_SPECIFIED,&cur);
			clear = (*(int *)value ^ cur) & cur;
			set = ~cur & (*(int *)value ^ cur);
			if (clear)
				Level_goals.GoalStatus(goal,LO_CLEAR_SPECIFIED,&clear,true);
			if (set)
				Level_goals.GoalStatus(goal,LO_SET_SPECIFIED,&set,true);
		}
		break;
	case LGSV_I_NUM_ITEMS:
		if (op == VF_GET)
			*(int *)value = Level_goals.GoalGetNumItems(goal);
		break;
	case LGSSV_C_ITEM_TYPE:
	case LGSSV_I_ITEM_HANDLE:
	case LGSSV_B_ITEM_DONE:
		ptype = NULL;
		phandle = NULL;
		pdone = NULL;
		switch (vtype) {
			case LGSSV_C_ITEM_TYPE:
				ptype = (char *)value;
				break;
			case LGSSV_I_ITEM_HANDLE:
				phandle = (int *)value;
				break;
			case LGSSV_B_ITEM_DONE:
				pdone = (bool *)value;
		}
		if (op == VF_GET || op == VF_SET)
			Level_goals.GoalItemInfo(goal,item,op==VF_GET?LO_GET_SPECIFIED:LO_SET_SPECIFIED,ptype,phandle,pdone);
	}
}
