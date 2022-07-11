#include "object.h"
#include "physics.h"

extern int Level_powerups_ignore_wind;

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
