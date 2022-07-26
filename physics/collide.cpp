#include "game.h"
#include "hlsoundlib.h"
#include "sounds.h"
#include "weapon.h"
#include "globals.h"
#include "hud.h"
#include "aimain.h"
#include "psrand.h"
#include "fireball.h"
#include "terrain.h"
#include "scorch.h"
#include "trigger.h"
#include "damage.h"

//int sound_override_glass_breaking;
extern bool pressed[1024];
extern int Netgame_local_role;
bool Tracking_FVI;

bool collide_generic_and_wall(object * A, float hitspeed, int hitseg, int hitwall, vector * hitpt, vector *wall_normal, float hit_dot) {
	return false;
}

bool collide_player_and_wall(object * A, float hitspeed, int hitseg, int hitwall, vector * hitpt, vector *wall_normal, float hit_dot) {
	return false;
}

bool collide_weapon_and_wall
					(object *weapon,int unused,int roomnum,int facenum,vector *pos,vector *wall_norm,
					float hit_dot)
{
	notify not_info;

	if (stricmp(Weapons[weapon->id].name,"Yellow flare") == 0 &&
		weapon->parent_handle == Player_object->handle && pressed['\'']) {
		if ((roomnum & 0x80000000) == 0)
			AddHUDMessage("Room %d face %d",roomnum,facenum);
		else
			AddHUDMessage("Terrain cell %d",roomnum & 0x7fffffff);
	}
	int tex; 
	if (roomnum & 0x80000000)
		tex = Terrain_tex_seg[Terrain_seg[roomnum & 0x7fffffff].texseg_index].tex_index;
	else
		tex = Rooms[roomnum].faces[facenum].tmap;
	int tex_flags = GameTextures[tex].flags;
	if (tex_flags & TF_FORCEFIELD && !(Weapons[weapon->id].flags & WF_MATTER_WEAPON)) {
		int snd = sound_override_force_field != -1 ? sound_override_force_field : SOUND_FORCEFIELD_BOUNCE;
		notify not_info;
		not_info.snd_num = 0;
		not_info.pos.y = 0.9f;
		not_info.pos.z = 0.1f;
		not_info.pos.x = Sounds[snd].max_distance;
		AINotify(weapon,AIN_HEAR_NOISE,&not_info);
		Sound_system.Play3dSound(snd,3,weapon,1.0,0);
		return true;
	}
	if (!(roomnum & 0x80000000)) {
		face *fp = Rooms[roomnum].faces + facenum;
		CheckTrigger(roomnum,facenum,weapon,1);
		if ((GameTextures[fp->tmap].flags & TF_DESTROYABLE) && !(fp->flags & FF_DESTROYED)) {
			int eff = CreateFireball(pos,SHATTER_INDEX + (ps_rand() % 2),roomnum,0);
			if (eff >= 0) {
				int num_verts = fp->num_verts;
				vector verts[64], v;
				for (int i = 0; i < num_verts; i++)
					verts[i] = Rooms[roomnum].verts[fp->face_verts[i]];
				VisEffects[eff].size = sqrtf(vm_GetCentroid(&v,verts,num_verts)) / 2;
			}
			CreateRandomSparks(20,pos,roomnum,-1,1.0);
			int snd = sound_override_glass_breaking != -1 ? sound_override_glass_breaking : SOUND_BREAKING_GLASS;
			Sound_system.Play3dSound(snd,3,weapon,1.0,0);
			fp->flags |= FF_DESTROYED;
		}
		if ((fp->portal_num != -1) && (GameTextures[fp->tmap].flags & TF_BREAKABLE) &&
			 (Weapons[weapon->id].flags & WF_MATTER_WEAPON)) {
			if (((Game_mode & 0x24) == 0) || (Netgame_local_role != 0)) {
				BreakGlassFace(Rooms + roomnum,facenum,pos,&weapon->mtype.phys_info.velocity);
			}
			int snd = sound_override_glass_breaking != -1 ? sound_override_glass_breaking : SOUND_BREAKING_GLASS;
			notify not_info;
			not_info.snd_num = 0;
			not_info.pos.y = 0.9;
			not_info.pos.z = 1.0;
			not_info.pos.x = Sounds[snd].max_distance;
			AINotify(weapon,AIN_HEAR_NOISE,&not_info);
			return false;
		}
	}
	if ((Weapons[weapon->id].scorch_handle != -1) && (!(tex_flags & (TF_WATER | TF_VOLATILE | TF_LAVA))))
		AddScorch(roomnum,facenum,pos,Weapons[weapon->id].scorch_handle,Weapons[weapon->id].scorch_size);
	#if 0
	if (uVar8 != 0) {
		size = Weapons[weapon->id].terrain_damage_size;
		if (((ushort)((ushort)(size < 0.0) << 8 | (ushort)(size == 0.0) << 0xe) == 0) &&
			 (Weapons[weapon->id].terrain_damage_depth != 0)) {
			DeformTerrain(&weapon->pos,(uint)Weapons[weapon->id].terrain_damage_depth,size);
		}
	}
	DoWallEffects(weapon,tex);
	check_for_special_surface((char *)weapon,(int)fVar14,&param_6->x);
	#endif
	if ((weapon->flags & WF_IMAGE_BITMAP) == 0) {
		if (weapon->mtype.phys_info.num_bounces < 1 &&
			!(weapon->mtype.phys_info.flags & PF_STICK) &&
			hit_dot > weapon->mtype.phys_info.hit_die_dot) {
			not_info.snd_num = 0;
			not_info.pos.y = 0.9;
			not_info.pos.z = 0.1;
			int snd = Weapons[weapon->id].sounds[WSI_IMPACT_WALL];
			if (snd) {
				not_info.pos.x = Sounds[snd].max_distance;
				Sound_system.Play3dSound(snd,2,weapon,1.0,0);
				AINotify(weapon,AIN_HEAR_NOISE,&not_info);
			}
			int uVar4 = weapon->id;
			#if 0
			if ((((Weapons[uVar4].flags & WF_SPAWNS_IMPACT) != WF_NONE) &&
					(Weapons[uVar4].spawn_count != '\0')) && (-1 < Weapons[uVar4].spawn_handle)) {
				CreateImpactSpawnFromWeapon(weapon,param_6);
			}
			if ((((Weapons[weapon->id].flags & WF_SPAWNS_ROBOT) != WF_NONE) &&
					((Weapons[weapon->id].flags & WF_COUNTERMEASURE) != WF_NONE)) &&
				 (-1 < Weapons[weapon->id].robot_spawn_handle)) {
				CreateRobotSpewFromWeapon(weapon);
			}
			#endif
			DoWeaponExploded(weapon,wall_norm,NULL,0);
			weapon->flags |= OF_DEAD;
		} else if (Weapons[weapon->id].sounds[WSI_BOUNCE] != 0) {
			Sound_system.Play3dSound(Weapons[weapon->id].sounds[WSI_BOUNCE],3,weapon,1.0,0);
			if (-1 < Weapons[weapon->id].sounds[WSI_BOUNCE]) {
				not_info.snd_num = 0;
				not_info.pos.y = 0.2;
				not_info.pos.z = 0.6;
				not_info.pos.x = Sounds[Weapons[weapon->id].sounds[WSI_BOUNCE]].max_distance;
				AINotify(weapon,AIN_HEAR_NOISE,&not_info);
			}
		}
	}
	return true;
}

bool collide_object_with_wall
					(object *A,float hitspeed,int hitseg,int hitwall,vector *hitpt,vector *wall_normal,
					float hit_dot)
{
	switch(A->type) {
	case OBJ_ROBOT:
	case OBJ_CLUTTER:
	case OBJ_BUILDING:
		collide_generic_and_wall(A,hitspeed,hitseg,hitwall,hitpt,wall_normal,hit_dot);
		if ((Game_mode & 0x24) != 0) {
			int DLLInfo[2], dll_room, dll_wall;
			vector dll_hitpt, dll_norm;
			float dll_dot, dll_speed;
			DLLInfo[0] = A->handle;
			DLLInfo[1] = -1;
			dll_hitpt = *hitpt;
			dll_norm = *wall_normal;
			dll_speed = hitspeed;
			dll_dot = hit_dot;
			dll_room = hitseg;
			dll_wall = hitwall;
			//CallGameDLL(0x513,(int)DLLInfo);
		}
		return true;
	case OBJ_SHARD:
		if (sound_override_glass_breaking == -1)
			Sound_system.Play3dSound(SOUND_BREAKING_GLASS,2,A,0.1,0);
		else
			Sound_system.Play3dSound(sound_override_glass_breaking,2,A,0.1,0);
		return true;
	case OBJ_PLAYER:
		collide_player_and_wall(A,hitspeed,hitseg,hitwall,hitpt,wall_normal,hit_dot);
		return true;
	case OBJ_WEAPON:
		return collide_weapon_and_wall(A,hitspeed,hitseg,hitwall,hitpt,wall_normal,hit_dot);
	case OBJ_NONE:
		Error("A object of type NONE hit a wall");
		return true;
	default:
		return true;
	}
}

void BreakGlassFace(room *rp, int facenum, vector *hitpnt, vector *hitvec) {
}
