#include "object.h"
#include "aimain.h"
#include "osiris_dll.h"
#include "levelgoal.h"
#include "objinfo.h"
#include "game.h"
#include "door.h"
#include "multi.h"
#include "matcen.h"
#include "damage.h"
#include "psrand.h"

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

void InitObjectScripts(object *objp,bool do_evt_created)
{
	tOSIRISEventInfo info;
	
	Osiris_BindScriptsToObject(objp);
	if (do_evt_created) {
		if (objp->control_type == CT_AI) {
			Osiris_CallEvent(objp,EVT_AI_INIT,&info);
		}
		Osiris_CallEvent(objp,EVT_CREATED,&info);
	}
}


struct osiris_mod_funs {
	ubyte flags;
	ubyte lib_temp_idx;
	ushort pad;
	char (* dInitializeDLL)(tOSIRISModuleInit *func_list);
	void (* dShutdownDLL)(void);
	int (* dGetGOScriptID)(char *name, ubyte isdoor);
	void * (* dCreateInstance)(int id);
	void (* dDestroyInstance)(int id,void *ptr);
	short (* dCallInstanceEvent)(int id,void *ptr,int event,tOSIRISEventInfo *data);
	int (* dGetTriggerScriptID)(int trigger_room,int trigger_face);
	int (* dGetCOScriptList)(int **list, int **id_list);
	int (* dSaveRestoreState)(void *file_ptr, ubyte saving_state);
	void * module;
	char * lib_name;
	char * * string_tbl;
	int string_tbl_len;
};

struct osiris_mod_funs OSIRIS_loaded_modules[64];

struct tOSIRISCurrentMission {
	ubyte loaded;
	ubyte b;
	ushort module_id;
} tOSIRISCurrentMission;

struct tOSIRISCurrentLevel {
	ubyte loaded;
	ubyte b;
	ushort obj_script_count;
	ushort module_id;
	int *obj_script_ids;
	int *obj_handles;
	void *script_inst;
} tOSIRISCurrentLevel;

int Osiris_LoadGameModule(char *module_name) {
	return -1;
}

void Osiris_UnloadModule(int id) {
}

bool Osiris_BindScriptsToObject(object *obj)
{
	int obj_type;
	uint module_id;
	int script_id;
	void *script_instance;
	tOSIRISScript *script;
	char *module_name;
	bool is_door;
	bool not_camera;
	char *script_name;
	int idx;
	ushort module_id2;
	short mission_mod_id;
	ushort obj_id;
	
	if (obj->osiris_script)
		return true;
	if (((((Game_mode & 0x24) == 0) || (Netgame.local_role == 1)) && (obj != (object *)0x0)) &&
		 (((((obj_type = obj->type, obj_type == OBJ_ROBOT || (obj_type == OBJ_BUILDING)) ||
				((obj_type == OBJ_POWERUP || ((obj_type == OBJ_CLUTTER || (obj_type == OBJ_DOOR)))))) ||
			 (obj_type == OBJ_CAMERA)) || (obj_type == OBJ_DUMMY)))) {
		if (obj_type == OBJ_DUMMY) {
			obj_type = obj->dummy_type;
		}
		if (((((obj_type == OBJ_ROBOT) || (obj_type == OBJ_BUILDING)) || (obj_type == OBJ_POWERUP)) ||
				((obj_type == OBJ_CLUTTER || (obj_type == OBJ_DOOR)))) || (obj_type == OBJ_CAMERA)) {
			is_door = obj_type == OBJ_DOOR;
			not_camera = obj_type != OBJ_CAMERA;
			if (not_camera) {
				if (is_door) {
					module_name = Doors[obj->id].module_name;
					script_name = (char *)(Doors + obj->id);
				}
				else {
					module_name = obj->custom_default_module_name;
					if (module_name == (char *)0x0) {
						module_name = Object_info[obj->id].module_name;
					}
					script_name = obj->custom_default_script_name;
					if (script_name == (char *)0x0) {
						obj_id = obj->id;
						script_name = Object_info[obj_id].script_name_override;
						if (Object_info[obj_id].script_name_override[0] == '\0') {
							script_name = (char *)(Object_info + obj_id);
						}
					}
				}
				module_id = Osiris_LoadGameModule(module_name);
				if (-1 < (int)module_id) {
					script = (tOSIRISScript *)malloc(sizeof(*script));
					obj->osiris_script = script;
					if (script == (tOSIRISScript *)0x0) {
						return false;
					}
					(script->default_script).script_instance = (void *)0x0;
					(obj->osiris_script->custom_script).script_instance = (void *)0x0;
					(obj->osiris_script->mission_script).script_instance = (void *)0x0;
					(obj->osiris_script->level_script).script_instance = (void *)0x0;
					script = obj->osiris_script;
					script_id = (*OSIRIS_loaded_modules[module_id].dGetGOScriptID)(script_name,is_door);
					if (script_id == -1) {
						Osiris_UnloadModule(module_id);
					}
					else {
						script_instance = (void *)(*OSIRIS_loaded_modules[module_id].dCreateInstance)(script_id)
						;
						if (script_instance == (void *)0x0) {
							Osiris_UnloadModule(module_id);
						}
						else {
							(script->default_script).DLLID = (ushort)module_id;
							(script->default_script).script_id = (ushort)script_id;
							(script->default_script).script_instance = script_instance;
						}
					}
				}
			}
			else {
				script_name = NULL;
			}
			if ((tOSIRISCurrentLevel.loaded != '\0') &&
				 (module_id = tOSIRISCurrentLevel.module_id,
				 (OSIRIS_loaded_modules[module_id].flags & 1) != 0)) {
				module_id2 = (ushort)module_id;
				if ((not_camera) &&
					 (script_id = (*OSIRIS_loaded_modules[module_id].dGetGOScriptID)(script_name,is_door),
					 script_id != -1)) {
					if (obj->osiris_script == (tOSIRISScript *)0x0) {
						script = (tOSIRISScript *)malloc(sizeof(*script));
						obj->osiris_script = script;
						if (script == (tOSIRISScript *)0x0) {
							return false;
						}
						(script->default_script).script_instance = (void *)0x0;
						(obj->osiris_script->custom_script).script_instance = (void *)0x0;
						(obj->osiris_script->mission_script).script_instance = (void *)0x0;
						(obj->osiris_script->level_script).script_instance = (void *)0x0;
					}
					script = obj->osiris_script;
					script_instance = (void *)(*OSIRIS_loaded_modules[module_id].dCreateInstance)(script_id);
					if (script_instance != (void *)0x0) {
						(script->level_script).script_id = (ushort)script_id;
						(script->level_script).DLLID = module_id2;
						(script->level_script).script_instance = script_instance;
					}
				}
				idx = 0;
				if (tOSIRISCurrentLevel.obj_script_count != 0) {
					do {
						if ((obj->handle == tOSIRISCurrentLevel.obj_handles[idx]) &&
							 (script_id = tOSIRISCurrentLevel.obj_script_ids[idx], script_id != -1)) {
							if (obj->osiris_script == (tOSIRISScript *)0x0) {
								script = (tOSIRISScript *)malloc(sizeof(*script));
								obj->osiris_script = script;
								if (script == (tOSIRISScript *)0x0) {
									return false;
								}
								(script->default_script).script_instance = (void *)0x0;
								(obj->osiris_script->custom_script).script_instance = (void *)0x0;
								(obj->osiris_script->mission_script).script_instance = (void *)0x0;
								(obj->osiris_script->level_script).script_instance = (void *)0x0;
							}
							script = obj->osiris_script;
							script_instance =
									 (void *)(*OSIRIS_loaded_modules[module_id].dCreateInstance)(script_id);
							if (script_instance != (void *)0x0) {
								(script->custom_script).script_id = (ushort)script_id;
								(script->custom_script).DLLID = module_id2;
								(script->custom_script).script_instance = script_instance;
							}
						}
						idx = idx + 1;
					} while (idx < (int)(uint)tOSIRISCurrentLevel.obj_script_count);
				}
			}
			mission_mod_id = tOSIRISCurrentMission.module_id;
			if ((((not_camera) && (tOSIRISCurrentMission.loaded)) &&
					(module_id = (uint)(ushort)tOSIRISCurrentMission.module_id,
					(OSIRIS_loaded_modules[module_id].flags & 1) != 0)) &&
				 (script_id = (*OSIRIS_loaded_modules[module_id].dGetGOScriptID)(script_name,is_door),
				 script_id != -1)) {
				if (obj->osiris_script == (tOSIRISScript *)0x0) {
					script = (tOSIRISScript *)malloc(sizeof(*script));
					obj->osiris_script = script;
					if (script == (tOSIRISScript *)0x0) {
						return false;
					}
					(script->default_script).script_instance = (void *)0x0;
					(obj->osiris_script->custom_script).script_instance = (void *)0x0;
					(obj->osiris_script->mission_script).script_instance = (void *)0x0;
					(obj->osiris_script->level_script).script_instance = (void *)0x0;
				}
				script = obj->osiris_script;
				script_instance = (void *)(*OSIRIS_loaded_modules[module_id].dCreateInstance)(script_id);
				if (script_instance != (void *)0x0) {
					(script->mission_script).DLLID = mission_mod_id;
					(script->mission_script).script_id = (ushort)script_id;
					(script->mission_script).script_instance = script_instance;
				}
			}
			return true;
		}
	}
	return false;
}

void osipf_MatcenValue(int matcen_handle,char op,char vtype,void *ptr,int prod_index)
{
	matcen *m;
	if (matcen_handle < 0 || matcen_handle >= Num_matcens)
		return;
	m = Matcen[matcen_handle];
	if (!m)
		return;
	switch(vtype) {
		case MTNV_C_ATTACH_TYPE:
			if (op == VF_GET)
				*(char *)ptr = m->GetAttachType();
			if (op == VF_SET)
				m->SetAttachType(*(char *)ptr);
			break;
		case MTNV_C_CONTROL_TYPE:
			if (op == VF_GET)
				*(char *)ptr = m->GetControlType();
			if (op == VF_SET)
				m->SetControlType(*(char *)ptr);
			break;
		case MTNV_I_ATTACH:
			if (op == VF_GET)
				*(int *)ptr = m->GetAttach();
			if (op == VF_SET)
				m->SetAttach(*(int *)ptr);
			break;
		case MTNV_V_CREATE_POINT:
			if (op == VF_GET)
				m->GetCreatePnt((vector *)ptr);
			if (op == VF_SET)
				m->SetCreatePnt((vector *)ptr);
			break;
		case MTNV_I_CREATE_ROOM:
			if (op == VF_GET)
				*(int *)ptr = m->GetCreateRoom();
			if (op == VF_SET)
				m->SetCreateRoom(*(int *)ptr);
			break;
		case MTNV_PC_NAME:
			if (op == VF_GET)
				m->GetName((char *)ptr);
			if (op == VF_SET)
				m->SetName((char *)ptr);
			break;
		case MTNV_I_MAX_PROD:
			if (op == VF_GET)
				*(int *)ptr = m->GetMaxProd();
			if (op == VF_SET)
				m->SetMaxProd(*(int *)ptr);
			break;
		case MTNV_F_PROD_MULTIPLIER:
			if (op == VF_GET)
				*(float *)ptr = m->GetProdMultiplier();
			if (op == VF_SET)
				m->SetProdMultiplier(*(float *)ptr);
			break;
		case MTNV_I_STATUS:
			if (op == VF_GET)
				*(int *)ptr = m->GetStatus();
			if (op == VF_SET_FLAGS)
				m->SetStatus(*(int *)ptr,true);
			if (op == VF_CLEAR_FLAGS)
				m->SetStatus(*(int *)ptr,false);
			if (op == VF_SET) {
				int cur = m->GetStatus();
				int to_clear = (*(int *)ptr ^ cur) & cur;
				int to_set = ~cur & (*(int *)ptr ^ cur);
				if (to_set)
					m->SetStatus(to_set,false);
				if (to_clear)
					m->SetStatus(to_clear,true);
			}
			break;
		case MTNV_C_CREATION_EFFECT:
			if (op == VF_GET)
				*(char *)ptr = m->GetCreationEffect();
			if (op == VF_SET)
				m->SetCreationEffect(*(char *)ptr);
			break;
		case MTNV_I_MAX_ALIVE_CHILDREN:
			if (op == VF_GET)
				*(int *)ptr = m->GetMaxAliveChildren();
			if (op == VF_SET)
				m->SetMaxAliveChildren(*(int *)ptr);
			break;
		case MTNV_F_PRE_PROD_TIME:
			if (op == VF_GET)
				*(float *)ptr = m->GetPreProdTime();
			if (op == VF_SET)
				m->SetPreProdTime(*(float *)ptr);
			break;
		case MTNV_F_POST_PROD_TIME:
			if (op == VF_GET)
				*(float *)ptr = m->GetPostProdTime();
			if (op == VF_SET)
				m->SetPostProdTime(*(float *)ptr);
			break;
		case MTNV_S_CREATION_TEXTURE:
			if (op == VF_GET)
				*(short *)ptr = m->GetCreationTexture();
			if (op == VF_SET)
				m->SetCreationTexture(*(short *)ptr);
			break;
		case MTNSV_I_SOUND:
			if (op == VF_GET)
				*(int *)ptr = m->GetSound(prod_index);
			if (op == VF_SET)
				m->SetSound(prod_index,*(int *)ptr);
			break;
		case MTNV_C_NUM_SPAWN_PTS:
			if (op == VF_GET)
				*(char *)ptr = m->GetNumSpawnPnts();
			if (op == VF_SET)
				m->SetNumSpawnPnts(*(char *)ptr);
			break;
		case MTNSV_I_SPAWN_POINT: // bug ?
			if (op == VF_GET)
				m->SetSpawnPnt(prod_index,*(int *)prod_index);
			if (op == VF_SET)
				*(int *)ptr = m->GetSpawnPnt(prod_index);
			break;
		case MTNV_C_NUM_PROD_TYPES:
			if (op == VF_GET)
				*(char *)ptr = m->GetNumProdTypes();
			if (op == VF_SET)
				m->SetNumSpawnPnts(*(char *)ptr);
			break;
		case MTNSV_I_PROD_ITEM_ID:
			if (op == VF_GET)
				m->GetProdInfo(prod_index,(int *)ptr,NULL,NULL,NULL);
			if (op == VF_SET)
				m->SetProdInfo(prod_index,(int *)ptr,NULL,NULL,NULL);
			break;
		case MTNSV_I_PROD_ITEM_PRIORITY:
			if (op == VF_GET)
				m->GetProdInfo(prod_index,NULL,(int *)ptr,NULL,NULL);
			if (op == VF_SET)
				m->SetProdInfo(prod_index,NULL,(int *)ptr,NULL,NULL);
			break;
		case MTNSV_F_PROD_ITEM_TIME:
			if (op == VF_GET)
				m->GetProdInfo(prod_index,NULL,NULL,(float *)ptr,NULL);
			if (op == VF_SET)
				m->SetProdInfo(prod_index,NULL,NULL,(float *)ptr,NULL);
			break;
		case MTNSV_I_PROD_ITEM_MAX_PROD:
			if (op == VF_GET)
				m->GetProdInfo(prod_index,NULL,NULL,NULL,(int *)ptr);
			if (op == VF_SET)
				m->SetProdInfo(prod_index,NULL,NULL,NULL,(int *)ptr);
	}
}

void osipf_ObjKill(int handle,int killer_handle,float damage,int flags,float min_time,float max_time)
{
	object *obj;
	object *killer;
	int objtype;
	
	obj = ObjGet(handle);
	killer = ObjGet(killer_handle);
	if (!obj)
		return;
	objtype = obj->type;
	if (((objtype == OBJ_PLAYER) || (objtype == OBJ_GHOST)) || (objtype == OBJ_OBSERVER)) {
		KillPlayer(obj,killer,damage,-1);
		return;
	}
	if (!IS_GENERIC(objtype) && objtype != OBJ_DOOR)
		return;
	if (IS_GUIDEBOT(obj))
		return;
	if (flags != -1)
		KillObject(obj,killer,damage,flags,(max_time - min_time) * ps_rand() / RAND_MAX + min_time);
	else
		KillObject(obj,killer,damage);
}

void Osiris_DetachScriptsFromObject(object *obj) {}
