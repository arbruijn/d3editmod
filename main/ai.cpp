#include "game.h"
#include "aimain.h"
#include "objinfo.h"
#include "diff.h"

int Buddy_handle[MAX_PLAYERS];

float Diff_ai_dodge_percent[5] = {0.04, 0.1, 1.0, 1.0, 1.5};
float Diff_ai_dodge_speed[5] = {0.2, 0.3, 1.0, 1.25, 1.5};
float Diff_ai_speed[5] = {0.7, 0.8, 1.0, 1.1, 1.2};
float Diff_ai_rotspeed[5] = {0.7, 0.8, 1.0, 1.1, 1.2};
float Diff_ai_circle_dist[5] = {1.1, 1, 1, 1, 1};


void AIUpdateAnim(object *obj) {
	ai_frame *info = obj->ai_info;
	int next_anim_type = info->next_animation_type;

	if (next_anim_type == AS_CUSTOM) {
		obj->rtype.pobj_info.anim_time = obj->rtype.pobj_info.custom_anim_info.anim_time;
		obj->rtype.pobj_info.anim_start_frame = obj->rtype.pobj_info.custom_anim_info.anim_start_frame;
		obj->rtype.pobj_info.anim_flags = obj->rtype.pobj_info.custom_anim_info.flags;
		obj->rtype.pobj_info.anim_end_frame = obj->rtype.pobj_info.custom_anim_info.anim_end_frame;
		info->next_animation_type = obj->rtype.pobj_info.custom_anim_info.next_anim_type;
	} else {
		int mtype = info->movement_type;
		obj->rtype.pobj_info.anim_start_frame =
			Object_info[obj->id].anim[mtype].elem[next_anim_type].from;
		obj->rtype.pobj_info.anim_end_frame =
			Object_info[obj->id].anim[mtype].elem[next_anim_type].to;
		obj->rtype.pobj_info.anim_time =
			Object_info[obj->id].anim[mtype].elem[next_anim_type].spc;
	}
	obj->rtype.pobj_info.anim_frame = obj->rtype.pobj_info.anim_start_frame;

	#if 0
	if ((Game_mode & 0x24) && (Netgame_local_role == 1))
		MultiAddObjAnimUpdate(obj - Objects);
	if (Demo_flags == 1)
		DemoWriteObjAnimChanged(obj - Objects);
	#endif
}

bool AINotify(object *obj, ubyte type, void *info) {
	if (type == AIN_ANIM_COMPLETE)
		AIUpdateAnim(obj);
	return false;
}

void ObjSetAIInfo(object *obj)
{
	t_ai_info *gen_info;
	ai_frame *obj_info;

	obj_info = obj->ai_info;
	gen_info = Object_info[obj->id].ai_info;
	if (!gen_info)
		return;
	obj_info->ai_class = gen_info->ai_class;
	obj_info->ai_type = gen_info->ai_type;
	obj_info->max_velocity = gen_info->max_velocity;
	obj_info->max_delta_velocity = gen_info->max_delta_velocity;
	obj_info->max_turn_rate = gen_info->max_turn_rate;
	obj_info->max_delta_turn_rate = gen_info->max_delta_turn_rate;
	obj_info->attack_vel_percent = gen_info->attack_vel_percent;
	obj_info->flee_vel_percent = gen_info->flee_vel_percent;
	obj_info->dodge_vel_percent = gen_info->dodge_vel_percent;
	obj_info->circle_distance = gen_info->circle_distance;
	obj_info->dodge_percent = gen_info->dodge_percent;
	for (int i = 0; i < 2; i++) {
		obj_info->melee_damage[i] = gen_info->melee_damage[i];
		obj_info->melee_latency[i] = gen_info->melee_latency[i];
	}
	for (int i = 0; i < MAX_AI_SOUNDS; i++)
		obj_info->sound[i] = gen_info->sound[i];
	obj_info->movement_type = gen_info->movement_type;
	obj_info->movement_subtype = gen_info->movement_subtype;
	obj_info->flags = gen_info->flags;
	obj_info->fov = gen_info->fov;
	obj_info->avoid_friends_distance = gen_info->avoid_friends_distance;
	obj_info->notify_flags = gen_info->notify_flags;
	obj_info->frustration = gen_info->frustration;
	obj_info->curiousity = gen_info->curiousity;
	obj_info->life_preservation = gen_info->life_preservation;
	obj_info->agression = gen_info->agression;
	obj_info->fire_spread = gen_info->fire_spread;
	obj_info->night_vision = gen_info->night_vision;
	obj_info->fog_vision = gen_info->fog_vision;
	obj_info->lead_accuracy = gen_info->lead_accuracy;
	obj_info->lead_varience = gen_info->lead_varience;
	obj_info->fight_team = gen_info->fight_team;
	obj_info->fight_same = gen_info->fight_same;
	obj_info->hearing = gen_info->hearing;
	obj_info->roaming = gen_info->roaming;
	obj_info->biased_flight_importance = gen_info->biased_flight_importance;
	obj_info->biased_flight_min = gen_info->biased_flight_min;
	obj_info->biased_flight_max = gen_info->biased_flight_max;
}

bool AIInit(object *obj, ubyte ai_class, ubyte ai_type, ubyte ai_movement) {
	ai_frame *info = obj->ai_info;
	bool no_diff_scale_move;

	ObjSetAIInfo(obj);

	no_diff_scale_move = (IS_GENERIC(obj->type) && Object_info[obj->id].flags & OIF_NO_DIFF_SCALE_MOVE) ||
		((info->flags & AIF_TEAM_MASK) == AIF_TEAM_REBEL);

	memset(info->memory, 0, sizeof(info->memory));

	info->ai_type = ai_type;
	info->movement_type = ai_movement;
	info->ai_class = ai_class;
	info->next_movement = -1;
	info->anim_sound_handle = 0;
	info->status_reg = 0;
	info->last_played_sound_index = -1;
	info->weapon_speed = 0.0;
	info->vec_to_target_perceived = obj->orient.fvec;
	info->last_dodge_dir = {0, 0, 0};
	info->dodge_till_time = Gametime - 1.0;
	for (int i = 0; i < MAX_AI_SOUNDS; i++)
		info->last_sound_time[i] = Gametime;

	info->next_melee_time = Gametime;
	info->next_flinch_time = Gametime;
	// ...
	info->last_see_target_time = Gametime - 14.0;
	info->last_render_time = -1.0;
	info->last_hear_target_time = Gametime - 14.0;
	info->next_target_update_time = Gametime;

	info->awareness = 0;
	info->notify_flags |= AI_NOTIFIES_ALWAYS_ON;

	if (!no_diff_scale_move) {
		int diff = Game_mode & 0x24 ? DiffLevel_MP : ingame_difficulty;
		info->dodge_percent *= Diff_ai_dodge_percent[diff];
		info->dodge_vel_percent *= Diff_ai_dodge_speed[diff];
		if (obj->movement_type != MT_WALKING)
			info->max_velocity *= Diff_ai_speed[diff];
		info->max_turn_rate *= Diff_ai_rotspeed[diff];
		info->circle_distance *= Diff_ai_circle_dist[diff];
	}
	info->last_see_target_pos = obj->pos;
	info->vec_to_target_actual = obj->orient.fvec;
	info->vec_to_target_perceived = obj->orient.fvec;
	info->dist_to_target_actual = vm_NormalizeVector(&info->vec_to_target_actual);
	info->dist_to_target_perceived = 1000;

	//AIAddMoveGoal(obj,info->ai_type);
	if ((obj->type == OBJ_ROBOT) && ((obj->id == 2) || (obj->id == 0)))
		info->flags = info->flags | AIF_DODGE;
	return true;
}

void AIInitAll() {
	for (int i = 0; i < MAX_PLAYERS; i++)
		Buddy_handle[i] = -1;
	for (int i = 0; i <= Highest_object_index; i++) {
		object *obj = Objects + i;
		if (obj->type != OBJ_NONE && obj->control_type == CT_AI)
			AIInit(Objects + i, obj->ai_info->ai_class, obj->ai_info->ai_type, obj->ai_info->movement_type);
	}
}

void ai_do_animation(object *obj, float step_time) {
	if (!Object_info[obj->id].anim)
		return;
	int flags;
	for (;;) {
		float start_frame = obj->rtype.pobj_info.anim_start_frame;
		float end_frame = obj->rtype.pobj_info.anim_end_frame;
		float time = obj->rtype.pobj_info.anim_time;
		if (((obj->movement_type == MT_WALKING) && (obj->ai_info->movement_type == MT_AT_REST)) &&
			 ((obj->ai_info->animation_type == AS_ALERT || (obj->ai_info->animation_type == AS_IDLE)))) {
			float speed = vm_GetMagnitude(&obj->mtype.phys_info.velocity) / obj->ai_info->max_velocity;
			if (speed < 0.7) {
				speed += vm_GetMagnitude(&obj->mtype.phys_info.rotvel) / 40000;
				if (speed > 1)
					speed = 1;
			}
			int next_type = obj->ai_info->next_animation_type;
			if (((next_type != AS_GOTO_ALERT_STANDING) && (next_type != AS_GOTO_IDLE_STANDING)) ||
				 (speed >= 0.15)) {
				if (speed <= 0)
					return;
			} else
				speed = 0.15;
			time /= speed;
		}
		flags = obj->rtype.pobj_info.anim_flags;
		if ((!(flags & AIAF_LOOPING) && (obj->rtype.pobj_info.anim_frame == end_frame)) ||
			(end_frame == start_frame))
			break;
		if (time <= 0) {
			obj->rtype.pobj_info.anim_frame = end_frame;
			break;
		}
		if (obj->rtype.pobj_info.anim_frame < start_frame || obj->rtype.pobj_info.anim_frame > end_frame)
			obj->rtype.pobj_info.anim_frame = start_frame;
		float duration = end_frame - start_frame;
		if (duration <= 0) {
			obj->rtype.pobj_info.anim_frame = start_frame;
			return;
		}
		float step = (duration / time) * step_time;
		if (obj->rtype.pobj_info.anim_frame + step < end_frame) {
			obj->rtype.pobj_info.anim_frame += step;
			return;
		}
		step_time -= (end_frame - obj->rtype.pobj_info.anim_frame) * step_time / step;
		obj->rtype.pobj_info.anim_frame = flags & AIAF_LOOPING ? start_frame : end_frame;
		if (flags & AIAF_NOTIFY)
			AINotify(obj,AIN_ANIM_COMPLETE,NULL);
	}
	if (flags & AIAF_NOTIFY)
		AINotify(obj,AIN_ANIM_COMPLETE,NULL);
}

void AIDoFrame(object *obj) {
	ai_frame *info;

	obj->mtype.phys_info.flags &= ~PF_USES_THRUST;
	obj->mtype.phys_info.rotthrust = {0, 0, 0};
	obj->mtype.phys_info.thrust = {0, 0, 0};
	if (obj->type == OBJ_DUMMY)
		return;
	info = obj->ai_info;
	if (info ->flags & AIF_DISABLED)
		return;
	ai_do_animation(obj, Frametime);
}
