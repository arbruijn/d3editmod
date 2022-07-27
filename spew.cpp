#include "game.h"
#include "weapon.h"
#include "spew.h"
#include "viseffect.h"
#include "psrand.h"
#include "fireball.h"
#include "terrain.h"
#include "polymodel.h"

spew_info SpewEffects[MAX_SPEW];
ushort Server_spew_list[MAX_SPEW];
int spew_count;

int VisEffectCreateControlled
					(ubyte type,object *parent,ubyte id,int roomnum,vector *pos,float lifetime,
					vector *velocity,int phys_info,float size,float drag,float mass,bool isfire)
{
	static int weapon_napalm;
	
	if (weapon_napalm == -1)
		weapon_napalm = FindWeaponName("Napalm");
	if (!isfire) {
		int visnum = VisEffectCreate(type,id,roomnum,pos);
		if (visnum < 0)
			return -1;
		vis_effect *vis = VisEffects + visnum;
		vis->lifeleft = lifetime;
		vis->lifetime = lifetime;
		vis->movement_type = 1;
		vis->mass = mass;
		vis->drag = drag;
		if (size > 0)
			vis->size = size;
		vis->velocity = *velocity;
		if (phys_info)
			vis->phys_flags = phys_info;
		if (roomnum & ROOMNUM_CELLNUM_FLAG)
			vis->phys_flags |= PF_NO_COLLIDE;
		vis->flags |= VF_NO_Z_ADJUST;
		vis->lighting_color &= ~OPAQUE_FLAG16;
		return visnum;
	}
	if (roomnum & ROOMNUM_CELLNUM_FLAG) {
		if ((roomnum & ROOMNUM_CELLNUM_MASK) >= 0x10000)
			return -1;
		if (Gametime - Last_terrain_render_time > 5)
			return -1;
	} else if (Gametime - Rooms[roomnum].last_render_time > 5)
		return -1;
	int objnum = CreateAndFireWeapon(pos,velocity,parent,weapon_napalm);
	if (objnum < 0)
		return objnum;
	object *obj = Objects + objnum;
	obj->lifeleft = lifetime;
	obj->lifetime = lifetime;
	obj->movement_type = MT_PHYSICS;
	if (mass > 0 || drag > 0) {
		obj->mtype.phys_info.mass = mass;
		obj->mtype.phys_info.drag = drag;
	}
	if (size > 0)
		obj->size = size;
	obj->mtype.phys_info.velocity = *velocity;
	if (phys_info)
		obj->mtype.phys_info.flags = phys_info;
	obj->ctype.laser_info.casts_light = false;
	return objnum;
}

void SpewClearEvent(int handle,bool is_idx)
{
	int spewnum;
	spew_info *spew;
	
	spewnum = handle & 0xff;
	if (spewnum >= MAX_SPEW || (!is_idx && handle != SpewEffects[spewnum].handle))
		return;
	spew = SpewEffects + spewnum;
	spew->is_real = false;
	spew->is_obj = false;
	spew->random = 0;
	spew->inuse = false;
	spew->pos.z = 0.0;
	spew->pos.y = 0.0;
	spew->pos.x = 0.0;
	spew->roomnum = 0;
	spew->dir.z = 0.0;
	spew->dir.y = 0.0;
	spew->dir.x = 0.0;
	spew->effect_type = MED_SMOKE_INDEX;
	spew->phys_info = PF_FIXED_VELOCITY;
	spew->speed = 0.0;
	spew->size = 0.0;
	spew->lifetime = 0.0;
	spew->longevity = 0.0;
	spew->interval = 0.0;
	spew->mass = 0.0;
	spew->drag = 0.0;
	spew->create_time = 0;
	spew->interval_left = 0.0;
	spew->handle = 0;
	spew->state = 0;
}

void SpewEmitAll()
{
	for (int i = 0; i < MAX_SPEW; i++) {
		spew_info *spew = SpewEffects + i;
		if (!spew->inuse)
			continue;
		spew->interval_left -= Frametime;
		if (spew->is_real)
			spew->random = 0;
		if (spew->interval_left > 0 && spew->is_obj) {
			object *obj = ObjGet(spew->obj.handle);
			if (!obj) {
				SpewClearEvent(spew->handle,true);
			} else {
				if (!(obj->flags & OF_MOVED_THIS_FRAME) && (obj->flags & OF_ATTACHED)) {
					object *parent = ObjGet(obj->attach_ultimate_handle);
					if (parent && parent->movement_type == MT_PHYSICS && parent->flags & OF_MOVED_THIS_FRAME)
						spew->state |= SF_GUN_NEED_UPDATE;
				} else
					spew->state |= 1;
				if ((obj->flags & OF_POLYGON_OBJECT) &&
					obj->rtype.pobj_info.anim_end_frame != obj->rtype.pobj_info.anim_start_frame)
					spew->state |= SF_GUN_NEED_UPDATE;
			}
		}
		if (spew->state & SF_GUN_ALWAYS_UPDATE)
			spew->state |= SF_GUN_NEED_UPDATE;
		spew->state &= ~SF_GUN_UPDATED;
		int count = 5;
		while (spew->interval_left < 0 && count) {
			count--;
			float size = spew->random & 4 ? (((RAND_MAX / 2) - ps_rand()) / (float)RAND_MAX / 2 + 1) * spew->size : spew->size;
			float speed = spew->random & 2 ? (((RAND_MAX / 2) - ps_rand()) / (float)RAND_MAX / 2 + 1) * spew->speed : spew->speed;
			float lifetime = spew->random & 0x10 ? (((RAND_MAX / 2) - ps_rand()) / (float)RAND_MAX / 2 + 1) * spew->lifetime : spew->lifetime;
			if (spew->is_obj == false) {
				vector vel = spew->dir * speed;
				VisEffectCreateControlled(1,NULL,spew->effect_type,spew->roomnum,&spew->pos,
						lifetime,&vel,spew->phys_info,size,
						spew->drag,spew->mass,spew->is_real);
			} else {
				object *obj = ObjGet(spew->obj.handle);
				if (obj) {
					if (!(spew->state & SF_GUN_UPDATED)) {
						if (!(obj->flags & OF_MOVED_THIS_FRAME) && (obj->flags & OF_ATTACHED)) {
							object *parent = ObjGet(obj->attach_ultimate_handle);
							if (parent && parent->movement_type == MT_PHYSICS && parent->flags & OF_MOVED_THIS_FRAME)
								spew->state |= SF_GUN_NEED_UPDATE;
						} else
							spew->state |= SF_GUN_NEED_UPDATE;
						if ((obj->flags & OF_POLYGON_OBJECT) &&
							obj->rtype.pobj_info.anim_end_frame != obj->rtype.pobj_info.anim_start_frame)
							spew->state |= SF_GUN_NEED_UPDATE;
						if (spew->state & SF_GUN_ALWAYS_UPDATE)
							spew->state |= SF_GUN_NEED_UPDATE;
					}
					if ((spew->state & SF_GUN_NEED_UPDATE) && !(spew->state & SF_GUN_UPDATED)) {
						WeaponCalcGun(&spew->gunpointpos,&spew->gunpointdir,obj,spew->obj.gun);
						spew->state = (spew->state & ~SF_GUN_NEED_UPDATE) | SF_GUN_UPDATED;
					}
					vector vel = spew->gunpointdir * speed;
					VisEffectCreateControlled(1,NULL,spew->effect_type,spew->roomnum,&spew->gunpointpos,
						lifetime,&vel,spew->phys_info,size,
						spew->drag,spew->mass,spew->is_real);
				}
				SpewClearEvent(spew->handle,true);
			}
			spew->interval_left += spew->interval;
		}
		if (!count)
			spew->interval_left = spew->interval;
		if (spew->longevity > 0 && spew->longevity <= Gametime - spew->create_time)
			SpewClearEvent(spew->handle,true);
	}
}


bool SpewObjectNeedsEveryFrameUpdate(object *obj,int gun)
{
	int modelnum;
	float time[MAX_SUBOBJECTS];
	
	if (!(obj->flags & OF_POLYGON_OBJECT))
		return false;
	modelnum = obj->rtype.pobj_info.model_num;
	if (!Poly_models[modelnum].n_guns)
		return false;
	SetNormalizedTimeObj(obj,time);
	SetModelAnglesAndPos(Poly_models + modelnum,time, ~0);
	if (gun >= 0 && gun < Poly_models[modelnum].n_guns) {
		int sub = Poly_models[modelnum].gun_slots[gun].parent;
		while (sub != -1) {
			if (Poly_models[modelnum].submodel[sub].flags & (SOF_ROTATE | SOF_TURRET))
				return true;
			sub =  Poly_models[modelnum].submodel[sub].parent;
		}
	}
	return false;
}

int SpewCreate(spew_info *spw)
{
	spew_info *spew;
	object *obj;
	int handle;
	int i;
	
	if (spw->drag < 0.0)
		spw->drag = 0.0;
	if (spw->effect_type < 0 || spw->effect_type >= NUM_FIREBALLS)
		return -1;
	if (spw->lifetime < 0.0)
		spw->lifetime = 0.0;
	if (spw->longevity < 0.0)
		spw->longevity = 0.0;
	if (spw->mass < 0.0)
		spw->mass = 0.0;
	if (spw->size < 1.0)
		spw->size = 1.0;
	if (spw->speed < 0.0)
		spw->speed = 0.0;
	if (spw->interval <= 0)
		spw->interval = 0.001;
	if (spw->is_obj) {
		object *obj = ObjGet(spw->obj.handle);
		if (!obj || spw->obj.gun < 0 ||
			spw->obj.gun >= Poly_models[obj->rtype.pobj_info.model_num].n_guns)
			return -1;
	}
	for (i = 0; i < MAX_SPEW && SpewEffects[i].inuse; i++)
		;
	if (i == MAX_SPEW)
		return -1;
	spew = SpewEffects + i;
	spew_count++;
	spew->inuse = true;
	spew->random = spw->random;
	spew->is_obj = spw->is_obj;
	spew->is_real = spw->is_real;
	spew->state = 0;
	if (!spw->is_obj) {
		spew->pos = spw->pos;
		spew->dir = spw->dir;
		spew->roomnum = spw->roomnum;
	} else {
		spew->obj.handle = spw->obj.handle;
		if (spw->obj.handle == -1) {
			SpewClearEvent(i,false);
			return -1;
		}
		spew->obj.gun = spw->obj.gun;
		spew->state = SF_GUN_NEED_UPDATE;
		obj = ObjGet(spw->obj.handle);
		if (obj && SpewObjectNeedsEveryFrameUpdate(obj,spw->obj.gun))
			spew->state |= SF_GUN_ALWAYS_UPDATE;
	}
	spew->effect_type = spw->effect_type;
	spew->phys_info = spw->phys_info;
	spew->drag = spw->drag;
	spew->mass = spw->mass;
	spew->interval = spw->interval;
	spew->interval_left = spw->interval;
	handle = (spew_count & 0xff) * 0x10000 + (i & 0xff);
	spew->longevity = spw->longevity;
	spew->lifetime = spw->lifetime;
	spew->size = spw->size;
	spew->speed = spw->speed;
	spew->handle = handle;
	spew->create_time = Gametime;
	return handle;
}

void SpewInit()
{
	spew_count = 0;
	for (int i = 0; i < MAX_SPEW; i++)
		SpewClearEvent(i, true);
}
