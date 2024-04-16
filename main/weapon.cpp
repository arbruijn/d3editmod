#include "psrand.h"
#include "object.h"
#include "weapon.h"
#include "player.h"
#include "ship.h"
#include "polymodel.h"
#include "findintersection.h"
#include "stringtable.h"
#include "hud.h"
#include "hlsoundlib.h"
#include "viseffect.h"
#include "fireball.h"

extern int Netgame_flags, Demo_flags, Game_mode;
extern int DiffLevel_MP, ingame_difficulty;
extern float Diff_ai_weapon_speed[5];
extern float Gametime;

//struct hlsSystem { int x; } Sound_system;

void ReleaseGuidedMissile(int id) {
}

void ReleaseUserTimeoutMissile(int id) {
}

bool AutoSelectWeapon(int weapon_type, int wpn_idx) {
	return false;
}

void AddToShakeMagnitude(float amount) {
}

void StopSoundImmediate(void *,int hnd) {
}

int Play2dSound(void *, int sndnum, int unused, float volume, float pan) {
	return -1;
}

void DemoWriteObjWeapFireFlagChanged(int objnum)
{
}

void MultiSendRequestToFire(int dynamic_wb_index,int firing_mask) {
}

void MultiSendReleaseTimeoutMissile() {
}

void DoZoomEffect(player_weapon *pw, ubyte no_timeout) {
}

void DoFusionEffect(object *objp, int weapon_type) {
}

void StartOnOffWeapon(object *objp, ubyte wi)
{
	objp->weapon_fire_flags |= WFF_ON_OFF;
	objp->dynamic_wb[wi].cur_firing_mask = 0;
	if (Demo_flags == 1)
		DemoWriteObjWeapFireFlagChanged(objp - Objects);
}

void StopOnOffWeapon(object *objp)
{
	objp->weapon_fire_flags &= !WFF_ON_OFF;
	if (Demo_flags == 1)
		DemoWriteObjWeapFireFlagChanged(objp - Objects);
}

void StopWeapon(object *obj,player_weapon *plweap,otype_wb_info *wb)
{
	if (obj->weapon_fire_flags & WFF_ON_OFF)
		StopOnOffWeapon(obj);
	if (wb->flags & WBF_SPRAY)
		obj->weapon_fire_flags &= ~WBF_SPRAY;
	if (plweap) {
		if (plweap->sound_handle != -1) {
			StopSoundImmediate(&Sound_system,plweap->sound_handle);
			plweap->sound_handle = -1;
		}
		plweap->firing_time = 0;
	}
	if (Demo_flags == 1)
		DemoWriteObjWeapFireFlagChanged(obj - Objects);
}

bool WBIsBatteryReady(object *objp,otype_wb_info *wb, int wi)
{
	float recharge_scalar = objp->type == OBJ_PLAYER ? Players[objp->id].weapon_recharge_scalar : 1.0f;
	dynamic_wb_info *dyn = &objp->dynamic_wb[wi];
	float extra_wait = dyn->flags & DWBF_QUAD ? wb->gp_fire_wait[dyn->cur_firing_mask] * 0.25f : 0;
	return !(dyn->flags & DWBF_ANIMATING) &&
		(recharge_scalar * wb->gp_fire_wait[dyn->cur_firing_mask] + dyn->last_fire_time + extra_wait)
		< Gametime;
}

void WBFireBattery(object *obj, otype_wb_info *static_wb, int poly_wb_index, int dynamic_wb_index,float damage_scalar)
{
	poly_wb_info *pw = Poly_models[obj->rtype.pobj_info.model_num].poly_wb + poly_wb_index;
	dynamic_wb_info *dyn = obj->dynamic_wb + dynamic_wb_index;
	int mask = dyn->flags & DWBF_QUAD ? static_wb->gp_quad_fire_mask : static_wb->gp_fire_masks[dyn->cur_firing_mask];
	int num_shots = 0;
	for (int i = 0; i < MAX_WB_GUNPOINTS; i++)
		if (mask & (1 << i)) {
			int wobjnum = FireWeaponFromObject(obj, static_wb->gp_weapon_index[i],
				pw->gp_index[i],
				(static_wb->flags & WBF_FIRE_FVEC) != 0,
				(static_wb->flags & WBF_FIRE_TARGET) != 0);
			if (wobjnum != -1)
				num_shots++;
		}
	if (!(Game_mode & 0x24) || obj->type != OBJ_PLAYER ||
		obj->id != Player_num || !(Netgame_flags & NF_PERMISSABLE)) {
		dyn->last_fire_time = Gametime;
		if (num_shots > 0 && obj->type == OBJ_PLAYER)
			Players[obj->id].last_fire_weapon_time = Gametime;
		if (static_wb->flags & WBF_RANDOM_FIRE_ORDER)
			dyn->cur_firing_mask = (ps_rand() / 32767.0f) * static_wb->num_masks;
		else
			dyn->cur_firing_mask++;
		if (dyn->cur_firing_mask >= static_wb->num_masks)
			dyn->cur_firing_mask = 0;
	}
}

void DoPermissableWeaponMask(int wb_idx)
{
	int shipnum = Players[Player_num].ship_index;
	dynamic_wb_info *dyn = Objects[Players[Player_num].objnum].dynamic_wb + wb_idx;
	Players[Player_num].last_fire_weapon_time = Gametime;
	if (++dyn->cur_firing_mask >= Ships[shipnum].static_wb[wb_idx].num_masks)
		dyn->cur_firing_mask = 0;
	dyn->last_fire_time = Gametime;
}

int CreateAndFireWeapon(vector *pos,vector *dir,object *parent,int weapon_num)
{
	fvi_query fq;
	fvi_info hit_info;
	
	if (parent->flags & OF_DEAD)
		return -1;
	fq.startroom = parent->roomnum;
	fq.p0 = &parent->pos;
	fq.thisobjnum = parent - Objects;
	fq.p1 = pos;
	fq.rad = 0;
	fq.ignore_obj_list = NULL;
	fq.flags = 0x100209;
	if (parent->type == OBJ_BUILDING)
		fq.flags = 0x100208;
	if (fvi_FindIntersection(&fq,&hit_info,false))
		return -1;

	int objnum = ObjCreate(OBJ_WEAPON,weapon_num,hit_info.hit_room,pos,NULL,parent->handle);
	if (objnum < 0) {
		return -1;
	}
	Objects[objnum].ctype.laser_info.multiplier = 1.0f;
	Objects[objnum].ctype.laser_info.parent_type = parent->type;
	Objects[objnum].parent_handle = parent->flags & OF_ATTACHED ? parent->attach_ultimate_handle : parent->handle;
	if (parent) {
		object *p = parent;
		while (p->type == OBJ_WEAPON) {
			if (!(p = ObjGet(p->parent_handle)))
				break;
			Objects[objnum].ctype.laser_info.parent_type = p->type;
			Objects[objnum].parent_handle = p->handle;
		}
	}
	vm_VectorToMatrix(&Objects[objnum].orient,dir,&parent->orient.uvec,NULL);
	float speed;
	if (ObjGet(Objects[objnum].parent_handle) &&
		ObjGet(Objects[objnum].parent_handle)->type == OBJ_PLAYER)
		speed = 1.0f;
	else if (ObjGet(Objects[objnum].parent_handle)->control_type == CT_AI &&
		(ObjGet(Objects[objnum].parent_handle)->ai_info->flags & AIF_TEAM_MASK) == AIF_TEAM_REBEL)
		speed = 1.0f;
	else
		speed = Diff_ai_weapon_speed[Game_mode & 0x24 ? DiffLevel_MP : ingame_difficulty];
	if (!(Objects[objnum].mtype.phys_info.flags & PF_USES_THRUST)) {
		Objects[objnum].mtype.phys_info.thrust.z = 0.00000000;
		Objects[objnum].mtype.phys_info.thrust.y = 0.00000000;
		Objects[objnum].mtype.phys_info.thrust.x = 0.00000000;
		Objects[objnum].mtype.phys_info.rotthrust.z = 0.00000000;
		Objects[objnum].mtype.phys_info.rotthrust.y = 0.00000000;
		Objects[objnum].mtype.phys_info.rotthrust.x = 0.00000000;
	} else {
		vm_ScaleVector(&Objects[objnum].mtype.phys_info.thrust,dir,speed * Objects[objnum].mtype.phys_info.full_thrust);
		if (parent->type == OBJ_PLAYER)
			Objects[objnum].mtype.phys_info.thrust *= Players[parent->id].weapon_speed_scalar;
	}
	vm_ScaleVector(&Objects[objnum].mtype.phys_info.velocity,dir,speed * Weapons[weapon_num].phys_info.velocity.z);
	if (parent->type == OBJ_PLAYER)
		Objects[objnum].mtype.phys_info.velocity *= Players[parent->id].weapon_speed_scalar;
	#if 0
	if ((Objects[objnum].mtype.phys_info.flags & PF_USES_PARENT_VELOCITY) && parent->type != OBJ_WEAPON) {
		d = &(parent->orient).fvec;
		ppVar1 = &parent->mtype;
		local_578 = d->x * (ppVar1->velocity).x +
								(parent->orient).fvec.y * (parent->mtype).velocity.y +
								(parent->orient).fvec.z * (parent->mtype).velocity.z;
		if ((ushort)((ushort)(local_578 < 0.00000000) << 8 | (ushort)(local_578 == 0.00000000) << 0xe)
				== 0) {
			local_580 = d->x * local_578;
			local_57c = (parent->orient).fvec.y * local_578;
			local_578 = (parent->orient).fvec.z * local_578;
		}
		else {
			local_580 = 0.00000000;
			local_57c = 0.00000000;
			local_578 = 0.00000000;
		}
		fVar2 = (parent->orient).rvec.y;
		fVar3 = (parent->orient).rvec.z;
		d = &(parent->orient).uvec;
		fVar9 = (parent->orient).rvec.x * (ppVar1->velocity).x +
						(parent->orient).rvec.y * (parent->mtype).velocity.y +
						(parent->orient).rvec.z * (parent->mtype).velocity.z;
		fVar4 = (parent->orient).uvec.y;
		fVar5 = (parent->orient).uvec.z;
		local_558.x = (ppVar1->velocity).x;
		local_558.y = (parent->mtype).velocity.y;
		local_558.z = (parent->mtype).velocity.z;
		fVar10 = d->x * local_558.x +
						 (parent->orient).uvec.y * local_558.y + (parent->orient).uvec.z * local_558.z;
		fVar6 = Objects[objnum].mtype.phys_info.velocity.y;
		fVar7 = Objects[objnum].mtype.phys_info.velocity.z;
		(d_00->velocity).x =
				 (d_00->velocity).x +
				 local_580 + fVar9 * (parent->orient).rvec.x * 0.10000000 + fVar10 * d->x * 0.10000000;
		Objects[objnum].mtype.phys_info.velocity.y =
				 fVar6 + local_57c + fVar2 * 0.10000000 * fVar9 + fVar4 * 0.10000000 * fVar10;
		Objects[objnum].mtype.phys_info.velocity.z =
				 fVar7 + local_578 + fVar9 * fVar3 * 0.10000000 + fVar5 * 0.10000000 * fVar10;
	}
	#endif
	if (!(Weapons[Objects[objnum].id].flags & WF_ELECTRICAL)) {
		int flags = Objects[objnum].mtype.phys_info.flags;
		if (Objects[objnum].movement_type == MT_PHYSICS &&
			 (!(flags & PF_USES_PARENT_VELOCITY) ||
			 	(!(flags & PF_USES_THRUST) && (flags & PF_FIXED_VELOCITY) &&
			 		!(flags & (PF_WIGGLE | PF_GRAVITY | PF_REVERSE_GRAVITY | PF_HOMING | PF_GUIDED))))) {
			vector vp1 = Objects[objnum].pos + Objects[objnum].mtype.phys_info.velocity;
			fq.ignore_obj_list = (int *)0x0;
			fq.flags = 4;
			fq.startroom = Objects[objnum].roomnum;
			fq.p1 = &vp1;
			fq.rad = Objects[objnum].size;
			fq.thisobjnum = objnum;
			fq.p0 = &Objects[objnum].pos;
			int hit = fvi_FindIntersection(&fq,&hit_info,false);
			if (!hit || hit == 5) {
				Objects[objnum].ctype.laser_info.hit_status = 1;
			} else {
				//vm_VectorDistance(d,&local_558); // ???
				//vm_VectorDistance(d,&hit_info->hit_pnt); // ???
				Objects[objnum].ctype.laser_info.hit_pnt = hit_info.hit_pnt;
				Objects[objnum].ctype.laser_info.hit_status = 2;
				Objects[objnum].ctype.laser_info.hit_wall_pnt = hit_info.hit_face_pnt[0];
				Objects[objnum].ctype.laser_info.hit_room = hit_info.hit_room;
				Objects[objnum].ctype.laser_info.hit_pnt_room = hit_info.hit_face_room[0];
				Objects[objnum].ctype.laser_info.hit_wall_normal = hit_info.hit_wallnorm[0];
				Objects[objnum].ctype.laser_info.hit_face = hit_info.hit_face[0];
			}
		}
	} else {
		//AquireElectricalTarget(Objects + objnum);
	}
	//if (parent && (parent->type == OBJ_PLAYER || parent->type == OBJ_GHOST) && parent->id == Player_num)
	//	DoForceForRecoil(parent, Objects + objnum);
	Objects[objnum].ctype.laser_info.casts_light = true;
	Objects[objnum].ctype.laser_info.last_smoke_pos = Objects[objnum].pos - 
			 Objects[objnum].orient.fvec * (Objects[objnum].size / 2);
	return objnum;
}

int FireWeaponFromObject (object * obj, int weapon_num, int gun_num, bool f_force_forward, bool f_force_target) {
	vector gun_pos, dir;
	if (gun_num == -1) {
		gun_pos = obj->pos;
		dir = obj->orient.fvec;
	} else
		WeaponCalcGun(&gun_pos,&dir,obj,gun_num);
	return CreateAndFireWeapon(&gun_pos,&dir,obj,weapon_num);
}

void FireWeaponFromPlayer(object *objp,int weapon_type,int down_count,bool down_state,float down_time)
{
	player *pl = &Players[objp->id];
	player_weapon *pw = &pl->weapon[weapon_type];
	int wi = pw->index;
	ship *sh = &Ships[pl->ship_index];
	otype_wb_info *wb = &sh->static_wb[wi];
	bool is_ready = WBIsBatteryReady(objp,wb,wi);
	dynamic_wb_info *dyn = &objp->dynamic_wb[wi];
	float energy_mult = dyn->flags & DWBF_QUAD ? 2.0f : 1.0f;
	if (down_count == 0) {
		if (down_time == 0) {
			if (sh->fire_flags[wi] & SFF_ZOOM)
				DoZoomEffect(pw, false);
			if (!pw->firing_time)
				return;
			if ((sh->fire_flags[wi] & (SFF_FUSION | SFF_ZOOM)) && is_ready) {
				float damage_mult = 1.0f;
				if (sh->fire_flags[wi] & SFF_FUSION)
					damage_mult = pw->firing_time * 0.33333331f * 3.0f + 1.0f;
				if (!(Game_mode & 0x24) || !(Netgame_flags & NF_PERMISSABLE))
					WBFireBattery(objp,wb,0,wi,damage_mult);
				else {
					MultiSendRequestToFire(wi,dyn->cur_firing_mask);
					DoPermissableWeaponMask(wi);
				}
				pl->energy -= energy_mult * wb->energy_usage;
				if (pl->energy < 0)
					pl->energy = 0;
				float new_ammo = pl->weapon_ammo[wi] - energy_mult * wb->ammo_usage;
				pl->weapon_ammo[wi] = new_ammo < 0 ? 0 : new_ammo;
				pl->num_discharges_level++;
				if (sh->fire_flags[wi] & SFF_ZOOM)
					AddToShakeMagnitude(sh->phys_info.mass * 2);
			}
			int snd;
			if ((!(Game_mode & 0x24) || !(Netgame_flags & NF_PERMISSABLE)) &&
				(snd = sh->firing_release_sound[wi]) != -1)
				Play2dSound(&Sound_system,snd,4,0.5f,0);
			StopWeapon(objp,pw,wb);
			return;
		}
	}
	else {
		if (!down_time)
			down_time = 0.01f;
		if (down_time && pl->guided_obj && weapon_type == 1 && is_ready) {
			ReleaseGuidedMissile(objp->id);
			dyn->last_fire_time = Gametime;
			return;
		}
		if (down_time && pl->user_timeout_obj && weapon_type == 1 && is_ready) {
			if (Game_mode & 0x24)
				MultiSendReleaseTimeoutMissile();
			ReleaseUserTimeoutMissile(objp->id);
			dyn->last_fire_time = Gametime;
			return;
		}
	}
	if (energy_mult * wb->energy_usage != 0 && pl->energy <= 0) {
		AddHUDMessage(TXT_WPNNONRG);
		StopWeapon(objp,pw,wb);
		AutoSelectWeapon(weapon_type,-1);
		return;
	}
	if (energy_mult * wb->ammo_usage != 0 && !pl->weapon_ammo[wi]) {
		AddHUDMessage(weapon_type ? TXT_WPNNOPROJ : TXT_WPNNOAMMO);
		StopWeapon(objp,pw,wb);
		AutoSelectWeapon(weapon_type,-1);
		return;
	}
	if (wi >= 10 && pl->weapon_ammo[wi] <= 0) {
		AddHUDMessage(weapon_type ? TXT_WPNNOPROJ : TXT_WPNNOAMMO);
		StopWeapon(objp,pw,wb);
		AutoSelectWeapon(weapon_type,-1);
		return;
	}
	if (is_ready) {
		int snd;
		if ((snd = sh->firing_sound[wi]) != -1 && !pw->firing_time)
			pw->sound_handle = Play2dSound(&Sound_system,snd,4,0.5f,0);
		if (wb->flags & WBF_SPRAY)
			objp->weapon_fire_flags |= WFF_SPRAY;
		if (sh->fire_flags[wi] & SFF_ZOOM)
			DoZoomEffect(pw,0);
		if (sh->fire_flags[wi] & SFF_FUSION)
			DoFusionEffect(objp,weapon_type);
		if ((wb->flags & WBF_ON_OFF) && !(objp->weapon_fire_flags & WFF_ON_OFF))
			StartOnOffWeapon(objp,wi);
		if (!(sh->fire_flags[wi] & (SFF_ZOOM | SFF_FUSION))) {
			if (!(Game_mode & 0x24) || !(Netgame_flags & NF_PERMISSABLE)) {
				WBFireBattery(objp,wb,0,wi,1.0f);
			} else {
				MultiSendRequestToFire(wi,dyn->cur_firing_mask);
				DoPermissableWeaponMask(wi);
			}
			pl->energy -= energy_mult * wb->energy_usage;
			if (pl->energy < 0)
				pl->energy = 0;
			float new_ammo = pl->weapon_ammo[wi] - energy_mult * wb->ammo_usage;
			pl->weapon_ammo[wi] = new_ammo < 0 ? 0 : new_ammo;
			pl->num_discharges_level++;
		}
		pw->firing_time += down_time;
		if (Demo_flags == 1)
			DemoWriteObjWeapFireFlagChanged(objp - Objects);
		//VIBE_WeaponFire(weapon_type);
	}
}

void DoConcussiveForce(object *weapon,int parent_handle,float amount) {}

void MakeShockwave(object *weapon,int parent_handle)
{
	object *parent, *last;
	int objnum;
	
	if (weapon->impact_time <= 0) {
		DoConcussiveForce(weapon,parent_handle,1.0);
		return;
	}
	parent = ObjGet(parent_handle);
	if (parent) {
		do {
			last = parent;
			parent = ObjGet(last->parent_handle);
		} while (parent);
		parent_handle = last->handle;
	} else
		parent_handle = -1;
	objnum = ObjCreate(OBJ_SHOCKWAVE,0,weapon->roomnum,&weapon->pos,NULL,parent_handle);
	if (objnum < 0)
		return;
	object *obj = Objects + objnum;
	obj->control_type = CT_NONE;
	obj->movement_type = MT_SHOCKWAVE;
	obj->render_type = RT_NONE;
	obj->lifeleft = weapon->impact_time;
	obj->impact_size = weapon->impact_size;
	obj->impact_force = weapon->impact_force;
	obj->impact_player_damage = weapon->impact_player_damage;
	obj->impact_generic_damage = weapon->impact_generic_damage;
	obj->impact_time = weapon->impact_time;
	obj->flags |= OF_USES_LIFELEFT;
}

int CreateGravityField(vector *pos,int roomnum,float size,float time,int parent_handle)
{
  int objnum;
  
  objnum = ObjCreate(OBJ_FIREBALL,GRAVITY_FIELD_INDEX,roomnum,pos,NULL,-1);
  if (objnum < 0)
	objnum = -1;
  else {
	object *obj = Objects + objnum;
    obj->flags |= OF_USES_LIFELEFT;
    obj->lifetime = time;
    obj->size = 1.0;
    obj->lifeleft = time;
    obj->ctype.blast_info.bm_handle = Fireballs[BLUE_BLAST_RING_INDEX].bm_handle;
    obj->parent_handle = parent_handle;
    obj->ctype.blast_info.max_size = size;
  }
  return objnum;
}

void DoWeaponExploded(object *wpnobj,vector *dir,vector *pos,object *target)
{
	int visnum;
	vis_effect *vis;
	int weapon_id;
	int wflags;
	weapon *w;

	weapon_id = wpnobj->id;
	w = Weapons + weapon_id;
	if (w->flags & WF_ELECTRICAL)
		return;
	MakeShockwave(wpnobj,wpnobj->parent_handle);
	if (!pos || !(w->flags & WF_PLANAR_BLAST))
		pos = &wpnobj->pos;
	if (!dir)
		dir = &wpnobj->orient.fvec;
	weapon_id = wpnobj->id;
	if (w->flags & WF_GRAVITY_FIELD)
		CreateGravityField(&wpnobj->pos,wpnobj->roomnum,Weapons[weapon_id].gravity_size,
								Weapons[weapon_id].gravity_time,wpnobj->parent_handle);
	if ((target == Viewer_object) && !(w->flags & WF_MATTER_WEAPON))
		return;
	wflags = w->flags;
	if ((wflags & WF_PLANAR_BLAST) == 0) {
		if ((wflags & WF_BLAST_RING) == 0) {
			if (w->explode_image_handle <= 0) {
				VisEffectCreate(1,GetRandomSmallExplosion(),wpnobj->roomnum,pos);
			} else {
				visnum = VisEffectCreate(1,CUSTOM_EXPLOSION_INDEX,wpnobj->roomnum,pos);
				if (visnum < 0)
					return;
				vis = VisEffects + visnum;
				vis->size = w->explode_size;
				vis->lifetime = w->explode_time;
				vis->lifeleft = w->explode_time;
				vis->custom_handle = w->explode_image_handle;
				vis->lighting_color = GR_RGB16(w->lighting_info.red_light2 * 255,
					w->lighting_info.green_light2 * 255, w->lighting_info.blue_light2 * 255) | OPAQUE_FLAG16;
				if ((w->flags & WF_EXPAND) != 0)
					vis->flags |= VF_EXPAND;
			}
		} else {
			visnum = VisEffectCreate(1,BLAST_RING_INDEX,wpnobj->roomnum,pos);
			if (visnum < 0)
				return;
			vis = VisEffects + visnum;
			vis->size = w->explode_size;
			vis->lifetime = w->explode_time;
			vis->lifeleft = w->explode_time;
			vis->custom_handle = GetTextureBitmap(w->explode_image_handle,0,false);
			vis->lighting_color = GR_RGB16(w->lighting_info.red_light2 * 255,
				w->lighting_info.green_light2 * 255, w->lighting_info.blue_light2 * 255) | OPAQUE_FLAG16;
		}
	}
	else if ((wflags & WF_BLAST_RING) == 0) {
		if (w->explode_image_handle < 1) {
			visnum = VisEffectCreate(1,GetRandomSmallExplosion(),wpnobj->roomnum,pos);
			if (visnum < 0)
				return;
		} else {
			visnum = VisEffectCreate(1,CUSTOM_EXPLOSION_INDEX,wpnobj->roomnum,pos);
			if (visnum < 0) {
				return;
			}
			vis = VisEffects + visnum;
			vis->size = w->explode_size;
			vis->lifetime = w->explode_time;
			vis->lifeleft = w->explode_time;
			vis->custom_handle = w->explode_image_handle;
			vis->lighting_color = GR_RGB16(w->lighting_info.red_light2 * 255,
				w->lighting_info.green_light2 * 255, w->lighting_info.blue_light2 * 255) | OPAQUE_FLAG16;
		}
		vis = VisEffects + visnum;
		vis->flags |= VF_PLANAR;
		vis->end_pos = *dir;
		if ((w->flags & WF_EXPAND) != 0)
			vis->flags |= VF_EXPAND;
	}
	else {
		visnum = VisEffectCreate(1,BLAST_RING_INDEX,wpnobj->roomnum,pos);
		if (visnum < 0)
			return;
		vis = VisEffects + visnum;
		vis->size = w->explode_size;
		vis->lifetime = w->explode_time;
		vis->lifeleft = w->explode_time;
		vis->custom_handle = GetTextureBitmap(w->explode_image_handle,0,false);
		vis->flags = vis->flags | VF_PLANAR;
		vis->end_pos = *dir;
		vis->lighting_color = GR_RGB16(w->lighting_info.red_light2 * 255,
			w->lighting_info.green_light2 * 255, w->lighting_info.blue_light2 * 255) | OPAQUE_FLAG16;
	}
}
