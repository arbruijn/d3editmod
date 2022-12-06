#include "object.h"
#include "physics.h"
#include "game.h"

void PhysicsDoSimLinear(object *obj,vector *pos,vector *force,vector *velocity,vector *movement_vec,
									vector *movement_pos,float sim_time,int count)
{
	vector wind_force;
	bool have_wind;
	physics_info *obj_phys;
	vector *obj_pos;
	
	if (obj->mtype.phys_info.flags & PF_FIXED_VELOCITY) {
		*movement_vec = *velocity * sim_time;
		*movement_pos = *pos + *movement_vec;
		return;
	}
	float mass = obj->mtype.phys_info.mass, drag = obj->mtype.phys_info.drag;
	have_wind = false;
	int roomnum = obj->roomnum;
	if (((roomnum & 0x80000000) == 0) && ((obj->mtype.phys_info.flags & 0x1f800000) == 0)) {
		if ((Rooms[roomnum].wind.x || Rooms[roomnum].wind.y || Rooms[roomnum].wind.z) && !count &&
			drag > 0 && 
			(obj->type != OBJ_POWERUP || !Level_powerups_ignore_wind)) {
			have_wind = true;
			wind_force = Rooms[roomnum].wind * drag * 16.0f;
			*force += wind_force;
		}
	}
	obj_phys = &obj->mtype.phys_info;
	if (!mass || !drag) {
		*movement_vec = obj_phys->velocity * sim_time + *force * sim_time * 0.5 * sim_time;
		*movement_pos = obj->pos + *movement_vec;
		*velocity = obj_phys->velocity + *force * sim_time;
	} else {
		float eval = exp(-(drag / mass * sim_time));
		vector vel_step = (obj_phys->velocity - *force / drag) * (mass / drag) * (1 - eval);
		vector scaled_force = *force / drag * sim_time;
		*movement_pos = obj->pos + scaled_force + vel_step;
		*movement_vec = *movement_pos - obj->pos;
		*velocity = (obj_phys->velocity - *force / drag) * eval + *force / drag;
	}
	if (have_wind)
		*force -= wind_force;
}

void set_object_turnroll(object *obj,vector *rotvel,angle *turnroll)
{
	int maxrollrate;
	float newroll;
	angle oldroll;

	newroll = -(rotvel->y * obj->mtype.phys_info.turnroll_ratio);
	if (fabsf(newroll) > 32000)
		newroll = newroll >= 0 ? 32000 : -32000;
	float orgroll = newroll;
	if (newroll <= -1)
		newroll += 65535;
	angle newang = newroll;
	oldroll = *turnroll;
	if (0 && oldroll != newang) {
		int maxrollrate = Frametime * obj->mtype.phys_info.max_turnroll_rate;
		int i = oldroll;
		if (i > 32000)
			i -= 65535;
		int diff = orgroll - i;
		if (abs(diff) > maxrollrate) {
			*turnroll = diff < 1 ? oldroll - (short)maxrollrate : oldroll + (short)maxrollrate;
			return;
		}
	}
	*turnroll = newang;
}

bool PhysicsDoSimRot(object *obj,float frame_time,matrix *orient,vector *rotforce,vector *rotvel,
               angle *turn_roll)
{
	if (obj->mtype.phys_info.flags & PF_FIXED_ROT_VELOCITY) {
		angvec rot = {(angle)(frame_time * rotvel->x), (angle)(frame_time * rotvel->y), (angle)(frame_time * rotvel->z)};
		matrix rotmat;
		vm_AnglesToMatrix(&rotmat, rot.p, rot.h, rot.b);
		matrix result = rotmat * *orient;
		*orient = result;
		vm_Orthogonalize(orient);
		return true;
	}
	if (*turn_roll) {
		matrix rotmat;
		vm_AnglesToMatrix(&rotmat, 0, 0, -*turn_roll);
		matrix result = *orient * rotmat;
		*orient = result;
	}
	if (obj->mtype.phys_info.rotdrag > 0) {
		if (!(obj->mtype.phys_info.flags & PF_USES_THRUST))
			*rotforce = {0, 0, 0};
		float mass = obj->mtype.phys_info.mass, drag = obj->mtype.phys_info.rotdrag;
		if (!mass || !drag) {
			*rotvel += *rotforce * frame_time;
		} else {
			float eval = exp(-(drag / mass * frame_time));
			*rotvel = (*rotvel - *rotforce / drag) * eval + *rotforce / drag;
		}
	}
	angvec rot = {(angle)(frame_time * rotvel->x), (angle)(frame_time * rotvel->y), (angle)(frame_time * rotvel->z)};
	matrix rotmat;
	vm_AnglesToMatrix(&rotmat, rot.p, rot.h, rot.b);
	matrix result = *orient * rotmat;
	*orient = result;
	if (obj->mtype.phys_info.flags & PF_TURNROLL)
		set_object_turnroll(obj,rotvel,turn_roll);
	if (*turn_roll) {
		matrix rotmat;
		vm_AnglesToMatrix(&rotmat, 0, 0, *turn_roll);
		matrix result = *orient * rotmat;
		*orient = result;
	}
	vm_Orthogonalize(orient);
	return true;
}
