#include "game.h"
#define INCLUDED_FROM_D3
#include "osiris_common.h"
#include "doorway.h"
#include "multi.h"
#include "lib/multi_world_state.h"
#include "inventory.h"
#include "player.h"
#include "guidebot.h"
#include "demofile.h"
#include "hud.h"
#include "hlsoundlib.h"
#include "sounds.h"
#include "levelgoal.h"
#include "spew.h"

int GetPlayerSlot(int objnum);
void D3MusicSetRegion(short index,short unk) {}


bool Demo_call_ok;

bool VALIDATE_ROOM_FACE(int roomnum,int facenum)
{
	if ((roomnum >= 0) && (roomnum <= Highest_room_index) && (Rooms[roomnum].used) &&
		(facenum >= 0) && (facenum < Rooms[roomnum].num_faces))
		return true;
	printf("invalid room %d face %d\n", roomnum, facenum);
	Int3();
	return false;
}

bool VALIDATE_ROOM_PORTAL(int roomnum,int portalnum)
{
	if ((roomnum >= 0) && (roomnum <= Highest_room_index) && (Rooms[roomnum].used) &&
		(portalnum >= 0) && (portalnum < Rooms[roomnum].num_portals))
		return true;
	Int3();
	return false;
}

void msafe_CallFunction(ubyte type,msafe_struct *mstruct)
{
	vector *b;
	//vis_effect *a;
	ushort uVar1;
	effect_info_s *peVar2;
	bool bVar3;
	ubyte uVar4;
	char cVar5;
	ushort uVar6;
	bool valid;
	bool ok;
	int playernum;
	object *obj;
	object *obj2;
	int spew_id;
	uint *puVar7;
	ubyte bVar8;
	//obj_flags_enum *poVar9;
	//obj_flags_enum oVar10;
	uint uVar11;
	int iVar12;
	short *player;
	char *netplayer;
	int iVar13;
	//spew_t *psVar14;
	//undefined4 weapon_num;
	float fade_time;
	//undefined4 uVar15;
	//undefined uVar16;
	void *local_184;
	char local_17d;
	float local_17c;
	//spew_t spew;
	vector color;
	char local_100 [256];
	uint killer;
	float amount;
	portal *cportal;
	uint flags;
	portal *portal;
	int roomnum;
	ubyte soundnum;
	int gunpoint;
	spew_info spew;
	
	if ((Demo_flags == 2) && !Demo_call_ok)
		return;
	ok = true;
	switch (type) {
	case MSAFE_ROOM_TEXTURE:
		valid = VALIDATE_ROOM_FACE(mstruct->roomnum,mstruct->facenum);
		if (valid == false) {
			return;
		}
		ChangeRoomFaceTexture(mstruct->roomnum,mstruct->facenum,mstruct->index);
		break;
#if 0
	case MSAFE_ROOM_WIND:
		valid = room_valid(mstruct->roomnum);
		if (valid == false) {
			return;
		}
		roomnum = mstruct->roomnum;
		Rooms[roomnum].wind.x = (mstruct->wind).x;
		Rooms[roomnum].wind.y = (mstruct->wind).y;
		Rooms[roomnum].wind.z = (mstruct->wind).z;
		Rooms[mstruct->roomnum].room_change_flags = Rooms[mstruct->roomnum].room_change_flags | 1;
		break;
	case MSAFE_ROOM_FOG:
		valid = room_valid(mstruct->roomnum);
		if (valid == false) {
			return;
		}
		Rooms[mstruct->roomnum].fog_r = mstruct->fog_r;
		Rooms[mstruct->roomnum].fog_g = mstruct->fog_g;
		Rooms[mstruct->roomnum].fog_b = mstruct->fog_b;
		Rooms[mstruct->roomnum].fog_depth = mstruct->fog_depth;
		Rooms[mstruct->roomnum].room_change_flags = Rooms[mstruct->roomnum].room_change_flags | 2;
		Rooms[mstruct->roomnum].flags = Rooms[mstruct->roomnum].flags | 0x200;
		break;
	case MSAFE_ROOM_LIGHT_PULSE:
		valid = room_valid(mstruct->roomnum);
		if (valid == false) {
			return;
		}
		Rooms[mstruct->roomnum].pulse_time = mstruct->pulse_time;
		Rooms[mstruct->roomnum].pulse_offset = mstruct->pulse_offset;
		goto LAB_00490fb0;
#endif
	case MSAFE_ROOM_PORTAL_RENDER:
		if (!VALIDATE_ROOM_PORTAL(mstruct->roomnum,mstruct->portalnum))
			return;
		portal = Rooms[mstruct->roomnum].portals + mstruct->portalnum;
		if (!mstruct->state)
			portal->flags &= ~PF_RENDER_FACES;
		else
			portal->flags |= PF_RENDER_FACES;
		Rooms[mstruct->roomnum].room_change_flags |= RCF_PORTAL_RENDER;
		portal->flags |= PF_CHANGED;
		if (mstruct->flags) {
			cportal = Rooms[portal->croom].portals + portal->cportal;
			if (!mstruct->state)
				cportal->flags &= ~PF_RENDER_FACES;
			else
				cportal->flags |= PF_RENDER_FACES;
			Rooms[portal->croom].room_change_flags |= RCF_PORTAL_RENDER;
			cportal->flags |= PF_CHANGED;
		}
		break;
#if 0
	case MSAFE_ROOM_FOG_STATE:
		valid = room_valid(mstruct->roomnum);
		if (valid == false) {
			return;
		}
		roomnum = mstruct->roomnum;
		if (mstruct->state == '\0') {
			flags = Rooms[roomnum].flags & 0xfffffdff;
		}
		else {
			flags = Rooms[roomnum].flags | 0x200;
		}
		Rooms[roomnum].flags = flags;
		Rooms[mstruct->roomnum].room_change_flags = Rooms[mstruct->roomnum].room_change_flags | 2;
		break;
	case MSAFE_ROOM_CHANGING_FOG:
		valid = room_valid(mstruct->roomnum);
		if (valid == false) {
			return;
		}
		color.x = mstruct->fog_r;
		color.y = mstruct->fog_g;
		color.z = mstruct->fog_b;
		SetRoomChangeOverTime(mstruct->roomnum,true,&color,mstruct->fog_depth,mstruct->interval);
		break;
	case MSAFE_ROOM_CHANGING_WIND:
		valid = room_valid(mstruct->roomnum);
		if (valid == false) {
			return;
		}
		SetRoomChangeOverTime(mstruct->roomnum,false,&mstruct->wind,0.0,mstruct->interval);
		break;
	case MSAFE_ROOM_DAMAGE:
		valid = room_valid(mstruct->roomnum);
		if (valid == false) {
			return;
		}
		Rooms[mstruct->roomnum].damage = mstruct->amount;
		Rooms[mstruct->roomnum].damage_type = *(ubyte *)&mstruct->index;
		Rooms[mstruct->roomnum].room_change_flags = Rooms[mstruct->roomnum].room_change_flags | 0x80;
		break;
	case MSAFE_ROOM_LIGHT_FLICKER:
		valid = room_valid(mstruct->roomnum);
		if (valid == false) {
			return;
		}
		roomnum = mstruct->roomnum;
		if (mstruct->state == '\0') {
			Rooms[roomnum].flags = Rooms[roomnum].flags & 0xfff7ffff;
		}
		else {
			Rooms[roomnum].flags = Rooms[roomnum].flags | RF_FLICKER;
		}
		goto LAB_00490fb0;
	case MSAFE_ROOM_LIGHT_STROBE:
		valid = room_valid(mstruct->roomnum);
		if (valid == false) {
			return;
		}
		roomnum = mstruct->roomnum;
		if (mstruct->state == '\0') {
			Rooms[roomnum].flags = Rooms[roomnum].flags & 0xfffbffff;
		}
		else {
			Rooms[roomnum].flags = Rooms[roomnum].flags | RF_STROBE;
		}
LAB_00490fb0:
		Rooms[mstruct->roomnum].room_change_flags = Rooms[mstruct->roomnum].room_change_flags | 8;
		break;
	case MSAFE_ROOM_REFUEL:
		valid = room_valid(mstruct->roomnum);
		if (valid == false) {
			return;
		}
		roomnum = mstruct->roomnum;
		if (mstruct->state == '\0') {
			Rooms[roomnum].flags = Rooms[roomnum].flags & 0xfffffffe;
		}
		else {
			Rooms[roomnum].flags = Rooms[roomnum].flags | RF_FUELCEN;
		}
		break;
	case MSAFE_ROOM_BREAK_GLASS:
		ok = room_and_portal_valid(mstruct->roomnum,(int)mstruct->portalnum);
		if (ok == false) {
			return;
		}
		ok = false;
		BreakGlassFace(Rooms + mstruct->roomnum,
									 (int)Rooms[mstruct->roomnum].portals[mstruct->portalnum].portal_face,
									 (vector *)0x0,(float *)0x0);
		break;
	case MSAFE_ROOM_PORTAL_BLOCK:
		valid = room_and_portal_valid(mstruct->roomnum,(int)mstruct->portalnum);
		if (valid == false) {
			return;
		}
		portal = Rooms[mstruct->roomnum].portals + mstruct->portalnum;
		flags = Rooms[mstruct->roomnum].portals[mstruct->portalnum].flags;
		if ((flags & 0x60) == 0) {
			return;
		}
		if (mstruct->state == '\0') {
			flags = flags & 0xffffffdf;
		}
		else {
			flags = flags | PF_BLOCK;
		}
		portal->flags = flags;
		portal->flags = flags | 0x10;
		portal = Rooms[portal->croom].portals + portal->cportal;
		flags = portal->flags;
		if (mstruct->state == '\0') {
			portal->flags = flags & 0xffffffdf;
			portal->flags = flags & 0xffffffdf | PF_CHANGED;
		}
		else {
			portal->flags = flags | PF_BLOCK;
			portal->flags = flags | 0x30;
		}
		break;
	case MSAFE_ROOM_REMOVE_ALL_POWERUPS:
		valid = room_valid(mstruct->roomnum);
		if (valid != false) {
			flags = FindObjectIDName(s_InvisiblePowerup_005a7850);
			roomnum = Highest_object_index + 1;
			if (0 < roomnum) {
				poVar9 = &Objects[0].flags;
				do {
					if (((((object *)(poVar9 + -1))->type == OBJ_POWERUP) && (poVar9[5] == mstruct->roomnum))
						 && (*(ushort *)((int)poVar9 + -2) != flags)) {
						*poVar9 = *poVar9 | (OF_DEAD|OF_SEND_MULTI_REMOVE_ON_DEATH);
					}
					poVar9 = poVar9 + 0xca;
					roomnum = roomnum + -1;
				} while (roomnum != 0);
			}
		}
		break;
	case MSAFE_OBJECT_SHIELDS:
		obj = ObjGet(mstruct->objhandle);
		if (obj == NULL) {
			return;
		}
		obj = ObjGet(mstruct->objhandle);
		obj->shields = mstruct->shields;
		obj = ObjGet(mstruct->objhandle);
		obj->flags = obj->flags &
								 (OF_FORCE_CEILING_CHECK|OF_DEAD|OF_DESTROYED|OF_STOPPED_THIS_FRAME|OF_ATTACHED|
									OF_MOVED_THIS_FRAME|OF_AI_DO_DEATH|OF_USES_LIFELEFT|OF_SAFE_TO_RENDER|
									OF_OUTSIDE_MINE|OF_DESTROYABLE|OF_BIG_OBJECT|OF_POLYGON_OBJECT|OF_DYING|
									OF_USE_DESTROYED_POLYMODEL|OF_RENDERED|OF_NO_OBJECT_COLLISIONS|OF_STUCK_ON_PORTAL|
									OF_TEMP_GRAVITY|OF_CLIENT_KNOWS|OF_SERVER_SAYS_DELETE|OF_SERVER_OBJECT|
									OF_PING_ACCELERATE|OF_SEND_MULTI_REMOVE_ON_DEATH|OF_SEND_MULTI_REMOVE_ON_DEATHWS|
									OF_PREDICTED|OF_INPLAYERINVENTORY|OF_INFORM_PLAYER_COLLIDE_TO_LG|
									OF_INFORM_PLAYER_WEAPON_COLLIDE_TO_LG|OF_INFORM_DESTROY_TO_LG|OF_CLIENTDEMOOBJECT)
		;
		break;
	case MSAFE_OBJECT_ENERGY:
		obj = ObjGet(mstruct->objhandle);
		if (obj == NULL) {
			return;
		}
		obj = ObjGet(mstruct->objhandle);
		Players[obj->id].energy = mstruct->energy;
		break;
	case MSAFE_OBJECT_LIGHT_COLOR:
		obj = ObjGet(mstruct->objhandle);
		if (obj != NULL) {
			ObjSetLocalLighting(obj);
			obj->lighting_info->red_light1 = mstruct->r1;
			obj->lighting_info->green_light1 = mstruct->g1;
			obj->lighting_info->blue_light1 = mstruct->b1;
			obj->change_flags = obj->change_flags | 4;
		}
		break;
	case MSAFE_OBJECT_MOVEMENT_SCALAR:
		obj = ObjGet(mstruct->objhandle);
		if (obj == NULL) {
			return;
		}
		obj = ObjGet(mstruct->objhandle);
		Players[obj->id].movement_scalar = mstruct->scalar;
		break;
	case MSAFE_OBJECT_RECHARGE_SCALAR:
		obj = ObjGet(mstruct->objhandle);
		if (obj == NULL) {
			return;
		}
		obj = ObjGet(mstruct->objhandle);
		Players[obj->id].weapon_recharge_scalar = mstruct->scalar;
		break;
	case MSAFE_OBJECT_WSPEED_SCALAR:
		obj = ObjGet(mstruct->objhandle);
		if (obj == NULL) {
			return;
		}
		obj = ObjGet(mstruct->objhandle);
		Players[obj->id].weapon_speed_scalar = mstruct->scalar;
		break;
	case MSAFE_OBJECT_DAMAGE_SCALAR:
		obj = ObjGet(mstruct->objhandle);
		if (obj == NULL) {
			return;
		}
		obj = ObjGet(mstruct->objhandle);
		Players[obj->id].damage_scalar = mstruct->scalar;
		break;
	case MSAFE_OBJECT_ARMOR_SCALAR:
		obj = ObjGet(mstruct->objhandle);
		if (obj == NULL) {
			return;
		}
		obj = ObjGet(mstruct->objhandle);
		Players[obj->id].armor_scalar = mstruct->scalar;
		break;
	case MSAFE_OBJECT_DAMAGE_OBJECT:
		obj = ObjGet(mstruct->objhandle);
		if (obj == NULL) {
			return;
		}
		ok = false;
		obj = ObjGet(mstruct->objhandle);
		killer = mstruct->killer_handle;
		if (obj->type == OBJ_PLAYER) {
			amount = mstruct->amount;
			uVar16 = 1;
			weapon_num = 0xff;
			uVar15 = 0;
			if (killer == 0xffffffff) {
				flags = (uint)mstruct->damage_type;
				obj = NULL;
			}
			else {
				flags = (uint)mstruct->damage_type;
				uVar16 = 1;
				obj = ObjGet(killer);
			}
			obj2 = ObjGet(mstruct->objhandle);
			ApplyDamageToPlayer(obj2,obj,flags,amount,uVar15,weapon_num,uVar16);
		}
		else {
			amount = mstruct->amount;
			flags = 0xff;
			roomnum = 0;
			if (killer == 0xffffffff) {
				uVar11 = (uint)mstruct->damage_type;
				obj = NULL;
			}
			else {
				uVar11 = (uint)mstruct->damage_type;
				obj = ObjGet(killer);
			}
			obj2 = ObjGet(mstruct->objhandle);
			ApplyDamageToGeneric(obj2,obj,uVar11,amount,roomnum,flags);
		}
		break;
	case MSAFE_OBJECT_TYPE:
		obj = ObjGet(mstruct->objhandle);
		if (obj == NULL) {
			return;
		}
		obj->type = *(obj_type_enum *)&mstruct->type;
		break;
	case MSAFE_OBJECT_ID:
		obj = ObjGet(mstruct->objhandle);
		if (obj == NULL) {
			return;
		}
		obj->id = *(ushort *)&mstruct->id;
		break;
	case MSAFE_OBJECT_ADD_WEAPON:
		obj = ObjGet(mstruct->objhandle);
		if (obj == NULL) {
			return;
		}
		roomnum = __ftol((float10)mstruct->ammo);
		flags = mstruct->index;
		obj = ObjGet(mstruct->objhandle);
		AddWeaponToPlayer((uint)obj->id,flags,roomnum);
		obj = ObjGet(mstruct->objhandle);
		if (obj->id == Player_num) {
			ok = false;
		}
		break;
#endif
	case MSAFE_OBJECT_START_SPEW:
		obj = ObjGet(mstruct->objhandle);
		if (obj == NULL) {
			return;
		}
		gunpoint = mstruct->gunpoint;
		if (gunpoint == -2) {
			obj = ObjGet(mstruct->objhandle);
			if (obj == NULL) break;
			spew.is_obj = false;
			spew.pos = obj->pos;
			spew.dir = obj->orient.uvec;
			spew.roomnum = obj->roomnum;
		} else if (gunpoint == -1) {
			obj = ObjGet(mstruct->objhandle);
			if (obj == NULL) break;
			spew.is_obj = false;
			spew.pos = obj->pos;
			spew.dir = obj->orient.fvec;
			spew.roomnum = obj->roomnum;
		} else {
			spew.obj.handle = mstruct->objhandle;
			spew.is_obj = true;
			spew.obj.gun = gunpoint;
		}
		spew.random = mstruct->random;
		spew.phys_info = mstruct->phys_info;
		spew.effect_type = mstruct->effect_type;
		spew.interval = mstruct->interval;
		spew.is_real = mstruct->is_real != 0;
		spew.drag = mstruct->drag;
		spew.size = mstruct->size;
		spew.mass = mstruct->mass;
		spew.longevity = mstruct->longevity;
		spew.lifetime = mstruct->lifetime;
		spew.speed = mstruct->speed;
		if (((Game_mode & 0x24) == 0) || (Netgame.local_role != 0)) {
			mstruct->id = SpewCreate(&spew);
		} else {
			Server_spew_list[mstruct->id] = SpewCreate(&spew) & 0xff;
		}
		break;
	case MSAFE_OBJECT_STOP_SPEW:
		if (((Game_mode & 0x24) == 0) || (Netgame.local_role != 0)) {
			SpewClearEvent(mstruct->id,true);
		}
		else {
			SpewClearEvent(SpewEffects[mstruct->id].handle,true);
			Server_spew_list[mstruct->id] = 0xffff;
		}
		break;
#if 0
	case MSAFE_OBJECT_GHOST:
		obj = ObjGet(mstruct->objhandle);
		if (obj == NULL) {
			return;
		}
		obj = ObjGet(mstruct->objhandle);
		if (obj->type == OBJ_NONE) {
			return;
		}
		obj = ObjGet(mstruct->objhandle);
		if (obj->type == OBJ_DUMMY) {
			return;
		}
		obj = ObjGet(mstruct->objhandle);
		if (obj->type == OBJ_PLAYER) {
			return;
		}
		obj = ObjGet(mstruct->objhandle);
		if (obj->type == OBJ_GHOST) {
			return;
		}
		ObjGhostObject(mstruct->objhandle & 0x7ff);
		break;
	case MSAFE_OBJECT_UNGHOST:
		obj = ObjGet(mstruct->objhandle);
		if (obj == NULL) {
			return;
		}
		obj = ObjGet(mstruct->objhandle);
		if (obj->type != OBJ_DUMMY) {
			return;
		}
		ObjUnGhostObject(mstruct->objhandle & 0x7ff);
		break;
	case MSAFE_OBJECT_REMOVE:
		ok = false;
		obj = ObjGet(mstruct->objhandle);
		if (obj == NULL) {
			return;
		}
		soundnum = mstruct->playsound;
		obj = ObjGet(mstruct->objhandle);
		oVar10 = obj->flags;
		obj->flags = oVar10 | OF_DEAD;
		if (soundnum == '\0') {
			oVar10 = oVar10 | (OF_DEAD|OF_SEND_MULTI_REMOVE_ON_DEATH);
		}
		else {
			oVar10 = oVar10 | (OF_DEAD|OF_SEND_MULTI_REMOVE_ON_DEATHWS);
		}
		obj->flags = oVar10;
		if (mstruct->playsound != '\0') {
			flags = 0;
			amount = 1.0;
			obj = ObjGet(mstruct->objhandle);
			hlsSystem::Play3dSound((hlsSystem *)&Sound_system,SOUND_POWERUP_PICKUP,3,obj,amount,flags);
		}
		break;
	case MSAFE_OBJECT_FLAGS:
		obj = ObjGet(mstruct->objhandle);
		if (obj == NULL) {
			return;
		}
		ok = obj->flags != mstruct->flags;
		if (ok) {
			obj->flags = mstruct->flags;
		}
		if (((Game_mode & 0x24) != 0) && (Netgame.local_role == 0)) {
			obj->flags = obj->flags | OF_SERVER_OBJECT;
		}
		break;
	case MSAFE_OBJECT_CONTROL_TYPE:
		obj = ObjGet(mstruct->objhandle);
		if (obj == NULL) {
			return;
		}
		if ((obj->flags & OF_DYING) != OF_NONE) {
			return;
		}
		obj->control_type = mstruct->control_type;
		break;
	case MSAFE_OBJECT_MOVEMENT_TYPE:
		obj = ObjGet(mstruct->objhandle);
		if (obj == NULL) {
			return;
		}
		obj->movement_type = mstruct->movement_type;
		break;
	case MSAFE_OBJECT_CREATION_TIME:
		obj = ObjGet(mstruct->objhandle);
		if (obj == NULL) {
			return;
		}
		obj->creation_time = mstruct->creation_time;
		break;
	case MSAFE_OBJECT_CLOAKALLPLAYERS:
		if (mstruct->state == '\0') {
			if ((Game_mode & 0x24) == 0) {
				MakeObjectVisible(Player_object);
				ok = true;
			}
			else {
				player = &Players[0].objnum;
				netplayer = &NetPlayers_sequence;
				do {
					if (((netplayer[-0x54b] & 1U) != 0) && (*netplayer == '\r')) {
						MakeObjectVisible(Objects + *player);
					}
					netplayer = netplayer + 0x578;
					player = player + 0xf8;
				} while ((int)netplayer < 0x6f31c3);
				ok = true;
			}
		}
		else if ((Game_mode & 0x24) == 0) {
			obj_cloak(Player_object,mstruct->lifetime,1.0,true);
			ok = true;
		}
		else {
			player = &Players[0].objnum;
			netplayer = &NetPlayers_sequence;
			do {
				if (((netplayer[-0x54b] & 1U) != 0) && (*netplayer == '\r')) {
					obj_cloak(Objects + *player,mstruct->lifetime,1.0,true);
				}
				netplayer = netplayer + 0x578;
				player = player + 0xf8;
			} while ((int)netplayer < 0x6f31c3);
			ok = true;
		}
		break;
	case MSAFE_OBJECT_PHYSICS_FLAGS:
		obj = ObjGet(mstruct->objhandle);
		if (obj == NULL) {
			return;
		}
		if ((obj->mtype).flags != mstruct->physics_flags) {
			(obj->mtype).flags = mstruct->physics_flags;
			obj->change_flags = obj->change_flags | 8;
			break;
		}
	case MSAFE_SOUND_STOP:
switchD_00490b92_caseD_71:
		ok = false;
		break;
	case MSAFE_OBJECT_PARENT:
		obj = ObjGet(mstruct->objhandle);
		if (obj == NULL) {
			return;
		}
		obj->parent_handle = mstruct->ithandle;
		break;
	case MSAFE_OBJECT_ROTDRAG:
		obj = ObjGet(mstruct->objhandle);
		if (obj == NULL) {
			return;
		}
		ok = false;
		(obj->mtype).rotdrag = mstruct->rot_drag;
		break;
	case MSAFE_OBJECT_INVULNERABLE:
		obj = ObjGet(mstruct->objhandle);
		if (obj == NULL) {
			return;
		}
		obj = ObjGet(mstruct->objhandle);
		if (obj->type == OBJ_PLAYER) {
			if (mstruct->state == '\0') {
				obj = ObjGet(mstruct->objhandle);
				MakePlayerVulnerable((uint)obj->id);
				ok = true;
			}
			else {
				amount = mstruct->lifetime;
				ok = false;
				obj = ObjGet(mstruct->objhandle);
				MakePlayerInvulnerable((uint)obj->id,amount,ok);
				ok = true;
			}
		}
		else if (mstruct->state == '\0') {
			obj = ObjGet(mstruct->objhandle);
			obj->flags = obj->flags | OF_DESTROYABLE;
			ok = true;
		}
		else {
			obj = ObjGet(mstruct->objhandle);
			obj->flags = obj->flags &
									 (OF_FORCE_CEILING_CHECK|OF_DEAD|OF_DESTROYED|OF_STOPPED_THIS_FRAME|OF_ATTACHED|
										OF_MOVED_THIS_FRAME|OF_AI_DO_DEATH|OF_USES_LIFELEFT|OF_SAFE_TO_RENDER|
										OF_OUTSIDE_MINE|OF_BIG_OBJECT|OF_POLYGON_OBJECT|OF_DYING|
										OF_USE_DESTROYED_POLYMODEL|OF_RENDERED|OF_NO_OBJECT_COLLISIONS|
										OF_STUCK_ON_PORTAL|OF_TEMP_GRAVITY|OF_CLIENT_KNOWS|OF_SERVER_SAYS_DELETE|
										OF_SERVER_OBJECT|OF_PING_ACCELERATE|OF_AI_DEATH|OF_SEND_MULTI_REMOVE_ON_DEATH|
										OF_SEND_MULTI_REMOVE_ON_DEATHWS|OF_PREDICTED|OF_INPLAYERINVENTORY|
										OF_INFORM_PLAYER_COLLIDE_TO_LG|OF_INFORM_PLAYER_WEAPON_COLLIDE_TO_LG|
										OF_INFORM_DESTROY_TO_LG|OF_CLIENTDEMOOBJECT);
			ok = true;
		}
		break;
	case MSAFE_OBJECT_CLOAK:
		obj = ObjGet(mstruct->objhandle);
		if (obj == NULL) {
			return;
		}
		if (mstruct->state == '\0') {
			obj = ObjGet(mstruct->objhandle);
			MakeObjectVisible(obj);
			ok = true;
		}
		else {
			amount = mstruct->lifetime;
			ok = false;
			fade_time = 1.0;
			obj = ObjGet(mstruct->objhandle);
			obj_cloak(obj,amount,fade_time,ok);
			ok = true;
		}
		break;
	case MSAFE_OBJECT_PLAYER_CMASK:
		obj = ObjGet(mstruct->objhandle);
		if ((obj != NULL) && (obj->type == OBJ_PLAYER)) {
			if (mstruct->control_val == '\0') {
				Players[obj->id].controller_bitflags =
						 Players[obj->id].controller_bitflags & ~mstruct->control_mask;
			}
			else {
				Players[obj->id].controller_bitflags =
						 Players[obj->id].controller_bitflags | mstruct->control_mask;
			}
		}
		break;
	case MSAFE_OBJECT_NO_RENDER:
		obj = ObjGet(mstruct->objhandle);
		if (obj == NULL) {
			return;
		}
		obj = ObjGet(mstruct->objhandle);
		obj->render_type = RT_NONE;
		obj = ObjGet(mstruct->objhandle);
		obj->change_flags = obj->change_flags | 1;
		break;
	case MSAFE_OBJECT_LIGHT_DIST:
		obj = ObjGet(mstruct->objhandle);
		if (obj != NULL) {
			ObjSetLocalLighting(obj);
			if (mstruct->light_distance < 0.0) {
				mstruct->light_distance = 0.0;
			}
			obj->lighting_info->light_distance = mstruct->light_distance;
			obj->change_flags = obj->change_flags | 2;
		}
		break;
	case MSAFE_OBJECT_DEFORM:
		obj = ObjGet(mstruct->objhandle);
		if ((obj != NULL) && (peVar2 = obj->effect_info, peVar2 != (effect_info_s *)0x0)) {
			peVar2->type_flags = peVar2->type_flags | 2;
			obj->effect_info->deform_range = mstruct->amount;
			obj->effect_info->deform_time = mstruct->lifetime;
		}
		break;
	case MSAFE_OBJECT_VIEWER_SHAKE:
		AddToShakeMagnitude(mstruct->amount);
		break;
#endif
	case MSAFE_OBJECT_PLAYER_KEY:
		playernum = GetPlayerSlot(mstruct->ithandle);
		if (playernum != -1) {
			bVar8 = mstruct->index - 1;
			if (Players[playernum].keys & (1 << bVar8))
				return;
			Players[playernum].keys |= 1 << bVar8;
			Global_keys |= 1 << bVar8;
			if ((mstruct->objhandle != 0xffffffff) &&
				 (obj = ObjGet(mstruct->objhandle), obj)) {
				Sound_system.Play3dSound(SOUND_POWERUP_PICKUP,3,obj,1.0,0);
				Players[playernum].inventory.Add(obj->type,obj->id,NULL,-1,-1,1,mstruct->message);
				if ((Game_mode & 0x24) == 0) {
					ok = false;
					obj->flags |= OF_DEAD;
					break;
				}
				if (obj->flags & OF_INFORM_DESTROY_TO_LG)
					Level_goals.Inform(LIT_OBJECT,LGF_COMP_DESTROY,obj->handle);
			}
			ok = false;
		}
		break;
#if 0
	case MSAFE_OBJECT_PLAYER_CONTROLAI:
		cVar5 = mstruct->slot;
		ok = false;
		if (((-1 < cVar5) && (cVar5 < ' ')) && (roomnum = (int)cVar5, Players[roomnum].objnum != -1)) {
			if ((Game_mode & 0x24) == 0) {
				if (cVar5 != '\0') break;
			}
			else if (((*(byte *)(&NetPlayers_flags + roomnum * 0x15e) & 1) == 0) ||
							((&NetPlayers_sequence)[roomnum * 0x578] != '\r')) break;
			osipf_SetPlayerControlMode(roomnum,mstruct->state != '\0');
		}
		break;
	case MSAFE_OBJECT_WORLD_POSITION:
		obj = ObjGet(mstruct->objhandle);
		ok = false;
		if ((obj != NULL) && (mstruct->roomnum != 0xffffffff)) {
			ObjSetPos(obj,&mstruct->pos,mstruct->roomnum,&mstruct->orient);
			ok = true;
		}
		break;
	case MSAFE_OBJECT_SPARKS:
		obj = ObjGet(mstruct->objhandle);
		if ((obj != NULL) && (peVar2 = obj->effect_info, peVar2 != (effect_info_s *)0x0)) {
			peVar2->type_flags = peVar2->type_flags | 0x2000;
			obj->effect_info->spark_delay = 1.0 / mstruct->amount;
			obj->effect_info->spark_timer = 0.0;
			obj->effect_info->spark_time_left = mstruct->lifetime;
		}
		break;
	case MSAFE_OBJECT_SETONFIRE:
		obj = ObjGet(mstruct->objhandle);
		if (obj == NULL) {
			return;
		}
		obj = ObjGet(mstruct->objhandle);
		local_17c = mstruct->longevity;
		amount = mstruct->interval;
		peVar2 = obj->effect_info;
		if (peVar2 != (effect_info_s *)0x0) {
			if ((ushort)((ushort)(local_17c < 0.0) << 8 | (ushort)(local_17c == 0.0) << 0xe) == 0) {
				peVar2->type_flags = peVar2->type_flags | 8;
				obj->effect_info->damage_time = local_17c + obj->effect_info->damage_time;
				obj->effect_info->damage_per_second = amount;
				amount = Gametime - obj->effect_info->last_damage_time;
				if ((ushort)((ushort)(amount < 1.0) << 8 | (ushort)(amount == 1.0) << 0xe) == 0) {
					obj->effect_info->last_damage_time = 0.0;
				}
				obj->effect_info->damage_handle = mstruct->ithandle;
				if (obj->effect_info->sound_handle == 0) {
					roomnum = hlsSystem::Play3dSound
															((hlsSystem *)&Sound_system,SOUND_PLAYER_BURNING,4,obj,1.0,0);
					obj->effect_info->sound_handle = roomnum;
				}
			}
			else {
				peVar2->type_flags = peVar2->type_flags & 0xfffffff7;
				obj->effect_info->damage_time = 0.0;
			}
		}
		break;
	case MSAFE_OBJECT_SHAKE_AREA:
		obj = ObjGet(mstruct->objhandle);
		if ((obj != NULL) &&
			 (amount = vm_VectorDistanceQuick(&Viewer_object->pos,&obj->pos), amount < mstruct->scalar)) {
			AddToShakeMagnitude((1.0 - amount / mstruct->scalar) * mstruct->amount);
		}
		break;
	case MSAFE_SHOW_ENABLED_CONTROLS:
		obj = ObjGet(mstruct->objhandle);
		if ((obj != NULL) && (obj->type == OBJ_PLAYER)) {
			Hud_show_controls = mstruct->state != '\0';
		}
		break;
	case MSAFE_OBJECT_FIRE_WEAPON:
		obj = ObjGet(mstruct->objhandle);
		if ((((obj != NULL) && (roomnum = mstruct->index, -1 < roomnum)) && (roomnum < 200)) &&
			 (Weapons[roomnum].used != 0)) {
			FireWeaponFromObject(obj,roomnum,(int)mstruct->gunpoint,false,false);
		}
		goto switchD_00490b92_caseD_71;
	case MSAFE_OBJECT_DESTROY_ROBOTS_EXCEPT:
		local_184 = mstruct->list;
		roomnum = mstruct->count;
		ok = false;
		if (-1 < Highest_object_index) {
			poVar9 = &Objects[0].flags;
			iVar12 = Highest_object_index + 1;
			do {
				if (((object *)(poVar9 + -1))->type == OBJ_ROBOT) {
					if ((*(ushort *)((int)poVar9 + -2) != 2) && (*(ushort *)((int)poVar9 + -2) != 0))
					goto LAB_004922a3;
				}
				else if ((((object *)(poVar9 + -1))->type == OBJ_BUILDING) &&
								((ai_frame *)poVar9[0xad] != (ai_frame *)0x0)) {
LAB_004922a3:
					iVar13 = 0;
					if (0 < roomnum) {
						puVar7 = (uint *)((int)local_184 + 4);
						do {
							if (*(char *)(puVar7 + -1) == '\0') {
								if ((uint)*(ushort *)((int)poVar9 + -2) == *puVar7) goto LAB_004922df;
							}
							else if ((*(char *)(puVar7 + -1) == '\x01') && (poVar9[2] == puVar7[1]))
							goto LAB_004922df;
							iVar13 = iVar13 + 1;
							puVar7 = puVar7 + 3;
						} while (iVar13 < roomnum);
					}
					*poVar9 = *poVar9 | (OF_DEAD|OF_SEND_MULTI_REMOVE_ON_DEATH);
				}
LAB_004922df:
				poVar9 = poVar9 + 0xca;
				iVar12 = iVar12 + -1;
			} while (iVar12 != 0);
		}
		break;
	case MSAFE_WEATHER_RAIN:
		SetRainState((uint)mstruct->state,mstruct->scalar);
		break;
	case MSAFE_WEATHER_SNOW:
		SetSnowState((uint)mstruct->state,mstruct->scalar);
		break;
	case MSAFE_WEATHER_LIGHTNING:
		SetLightningState((uint)mstruct->state,mstruct->scalar,mstruct->randval);
		break;
	case MSAFE_WEATHER_LIGHTNING_BOLT:
		if (mstruct->texnum == -1) {
			return;
		}
		if (mstruct->flags == 0) {
			obj = ObjGet(mstruct->objhandle);
			if (obj == NULL) {
				return;
			}
			obj = ObjGet(mstruct->ithandle);
		}
		else {
			obj = ObjGet(mstruct->objhandle);
		}
		if (obj == NULL) {
			return;
		}
		roomnum = VisEffectCreate(1,THICK_LIGHTNING_INDEX,mstruct->roomnum,&mstruct->pos);
		if (-1 < roomnum) {
			a = VisEffects + roomnum;
			a->lifeleft = mstruct->lifetime;
			b = &a->end_pos;
			a->lifetime = mstruct->lifetime;
			b->x = (mstruct->pos2).x;
			(a->end_pos).y = (mstruct->pos2).y;
			(a->end_pos).z = (mstruct->pos2).z;
			a->custom_handle = mstruct->texnum;
			a->lighting_color = *(ushort *)&mstruct->color;
			uVar4 = __ftol((float10)mstruct->size);
			(a->billboard_info).width = uVar4;
			(a->billboard_info).texture = mstruct->state;
			(a->velocity).x = (float)mstruct->count;
			(a->velocity).y = mstruct->interval;
			roomnum = mstruct->index;
			a->flags = 0x103;
			(a->velocity).z = (float)roomnum;
			amount = vm_VectorDistanceQuick(&a->pos,b);
			a->size = amount;
			if (mstruct->flags == 0) {
				*(byte *)&a->flags = *(byte *)&a->flags | 0x48;
				obj = ObjGet(mstruct->objhandle);
				(a->attach_info).obj_handle = obj->handle;
				obj = ObjGet(mstruct->ithandle);
				(a->attach_info).dest_objhandle = obj->handle;
				obj = ObjGet(mstruct->ithandle);
				b->x = (obj->pos).x;
				(a->end_pos).y = (obj->pos).y;
				(a->end_pos).z = (obj->pos).z;
			}
			else {
				*(byte *)&a->flags = *(byte *)&a->flags | VF_ATTACHED;
				obj = ObjGet(mstruct->objhandle);
				(a->attach_info).obj_handle = obj->handle;
				obj = ObjGet(mstruct->objhandle);
				(a->attach_info).modelnum = (obj->rtype).model_num;
				uVar6 = __ftol((float10)mstruct->g1);
				(a->attach_info).vertnum = uVar6;
				uVar6 = __ftol((float10)mstruct->g2);
				(a->attach_info).end_vertnum = uVar6;
				flags = (uint)(a->attach_info).vertnum;
				obj = ObjGet(mstruct->objhandle);
				WeaponCalcGun(&a->pos,(vector *)0x0,obj,flags);
				flags = (uint)(a->attach_info).end_vertnum;
				obj = ObjGet(mstruct->objhandle);
				WeaponCalcGun(b,(vector *)0x0,obj,flags);
			}
		}
		break;
	case MSAFE_SOUND_2D:
		if (mstruct->state == '\0') {
			flags = 0xffffffff;
		}
		else {
			flags = GetPlayerSlot(mstruct->objhandle);
			if (flags == 0xffffffff) break;
			mstruct->objhandle = Objects[Players[flags].objnum].handle;
			if (flags != Player_num) {
				mstruct->sound_handle = -1;
				goto LAB_00492360;
			}
		}
		roomnum = hlsSystem::Play2dSound((hlsSystem *)&Sound_system,mstruct->index,4,mstruct->volume,0);
		goto LAB_0049235a;
	case MSAFE_SOUND_OBJECT:
		obj = ObjGet(mstruct->objhandle);
		if (obj == NULL) {
			return;
		}
		flags = 0;
		amount = 1.0;
		obj = ObjGet(mstruct->objhandle);
		roomnum = hlsSystem::Play3dSound((hlsSystem *)&Sound_system,mstruct->index,4,obj,amount,flags);
		mstruct->sound_handle = roomnum;
		break;
	case MSAFE_SOUND_STREAMING:
		if (mstruct->state == '\0') {
			flags = 0xffffffff;
		}
		else {
			flags = GetPlayerSlot(mstruct->objhandle);
			if (flags == 0xffffffff) break;
			mstruct->objhandle = Objects[Players[flags].objnum].handle;
			if (flags != Player_num) goto LAB_00492360;
		}
		roomnum = StreamPlay(mstruct->name,mstruct->volume);
LAB_0049235a:
		mstruct->sound_handle = roomnum;
		goto LAB_00492360;
	case MSAFE_SOUND_STOP_OBJ:
		sound_stop_obj(&Sound_system,mstruct->objhandle);
		break;
	case MSAFE_SOUND_VOLUME_OBJ:
		sound_volume_obj(&Sound_system,mstruct->objhandle,(int)mstruct->volume);
		break;
#endif
	case MSAFE_MISC_HUD_MESSAGE:
	case MSAFE_MISC_FILTERED_HUD_MESSAGE:
		printf("hud message: %s\n", mstruct->message);
		break;
#if 0	
		if (mstruct->state == '\0') {
			flags = 0xffffffff;
		}
		else {
			flags = GetPlayerSlot(mstruct->objhandle);
			if (flags == 0xffffffff) break;
			mstruct->objhandle = Objects[Players[flags].objnum].handle;
			if (flags != Player_num) goto LAB_00492360;
		}
		local_17d = XlateGBMessage(local_100,mstruct->message);
		if (type == MSAFE_MISC_HUD_MESSAGE) {
			cVar5 = AddColoredHUDMessage(mstruct->color,s_%s,local_100);
		}
		else {
			cVar5 = AddFilteredColoredHUDMessage(mstruct->color,s_%s,local_100);
		}
		if ((local_17d != '\0') && (cVar5 != '\0')) {
			uVar15 = 0;
			amount = 1.0;
			iVar12 = 4;
			roomnum = FindSoundName(s_GBotGreetB1_005a7c38);
			hlsSystem::Play2dSound((hlsSystem *)&Sound_system,roomnum,iVar12,amount,uVar15);
		}
		goto LAB_00492360;
	case MSAFE_MISC_WAYPOINT:
		ResetWaypoint(mstruct->index);
		break;
	case MSAFE_MISC_END_LEVEL:
		if ((Game_mode & 0x24) == 0) {
			ok = false;
			Game_static = 7 - (uint)(mstruct->state != '\0');
		}
		else {
			MultiEndLevel();
			ok = false;
		}
		break;
	case MSAFE_MISC_POPUP_CAMERA:
		CreateSmallView(0,mstruct->objhandle,3,mstruct->interval,0.726 / mstruct->scalar,
										(int)mstruct->gunpoint,NULL);
		break;
	case MSAFE_MISC_CLOSE_POPUP:
		ClosePopupView(0);
		break;
#endif
	case MSAFE_MISC_GAME_MESSAGE:
		if (mstruct->state == '\0') {
			flags = 0xffffffff;
		}
		else {
			flags = GetPlayerSlot(mstruct->objhandle);
			if (flags == 0xffffffff) break;
			mstruct->objhandle = Objects[Players[flags].objnum].handle;
			if (flags != Player_num) goto set_slot;
		}
		AddGameMessage(mstruct->message);
		XlateGBMessage(local_100,mstruct->message2);
		roomnum = (Game_window_h / 4) - 15;
		if (Demo_flags == 1) {
			DemoWritePersistantHUDMessage(mstruct->color,-1,roomnum,10.0,3,0x2b,local_100);
		}
		if (Demo_flags != 2) {
			AddPersistentHUDMessage(mstruct->color,-1,roomnum,10.0,3,0x2b,local_100);
		}
		goto set_slot;
	case MSAFE_MUSIC_REGION:
		if (mstruct->state == '\0') {
			flags = 0xffffffff;
		}
		else {
			flags = GetPlayerSlot(mstruct->objhandle);
			if (flags == 0xffffffff) break;
			mstruct->objhandle = Objects[Players[flags].objnum].handle;
			if (flags != Player_num) goto set_slot;
		}
		D3MusicSetRegion(*(short *)&mstruct->index,0);
set_slot:
		if ((((Game_mode & 0x24) != 0) && (Netgame.local_role == 1)) && (flags == Player_num)) {
			ok = false;
		}
		mstruct->slot = (sbyte)flags;
		break;
#if 0
	case MSAFE_MISC_ENABLE_SHIP:
		PlayerSetShipPermission(-1,mstruct->name,mstruct->state != '\0');
		break;
	case MSAFE_MISC_LEVELGOAL:
		if (((Game_mode & 0x24) != 0) && (Netgame.local_role == 0)) {
			local_184 = (void *)0xffffffff;
			levelgoals::GoalSetName((levelgoals *)&Level_goals,mstruct->index,mstruct->message);
			levelgoals::GoalStatus
								((levelgoals *)&Level_goals,mstruct->index,'\x02',(int *)&local_184,true);
			levelgoals::GoalStatus((levelgoals *)&Level_goals,mstruct->index,'\0',&mstruct->type,true);
			levelgoals::GoalPriority((levelgoals *)&Level_goals,mstruct->index,'\0',&mstruct->count);
		}
		break;
	case MSAFE_MISC_GUIDEBOT_NAME:
		obj = ObjGet(mstruct->objhandle);
		if (((obj != NULL) && (obj->type == OBJ_PLAYER)) && (obj->id == Player_num)) {
			pilot_class::set_guidebot_name((pilot_class *)&Current_pilot,mstruct->name);
		}
		break;
	case MSAFE_MISC_START_TIMER:
		spew.drag = (float)osiris_timer_get_timer_handle(mstruct->index);
		if (spew.drag == 0.0) {
			return;
		}
		psVar14 = &spew;
		for (roomnum = 0x13; roomnum != 0; roomnum = roomnum + -1) {
			*(undefined4 *)psVar14 = 0;
			psVar14 = (spew_t *)&psVar14->pos;
		}
		spew.effect_type = mstruct->color;
		spew.roomnum._2_2_ = (ushort)((uint)spew.roomnum >> 0x10) & 0xff00 | 0xc0;
		spew.orient.z._0_2_ = CONCAT11(0x16,spew.orient.z._0_1_);
		spew._0_2_ = 0;
		spew._2_2_ = 0;
		spew.orient.z = (float)CONCAT22(0x1000,spew.orient.z._0_2_);
		spew.roomnum = CONCAT22(spew.roomnum._2_2_,4);
		spew.random = &LAB_00451170;
		AddHUDItem((tHUDItem *)&spew);
		ok = false;
		break;
	case MSAFE_MISC_UPDATE_HUD_ITEM:
		roomnum = GetCustomtext2HUDItemIndex();
		if (roomnum == -1) {
			psVar14 = &spew;
			for (roomnum = 0x13; roomnum != 0; roomnum = roomnum + -1) {
				*(undefined4 *)psVar14 = 0;
				psVar14 = (spew_t *)&psVar14->pos;
			}
			spew.effect_type = mstruct->color;
			spew.roomnum._2_2_ = (ushort)((uint)spew.roomnum >> 0x10) & 0xff00 | 0xc0;
			spew.orient.z._0_2_ = CONCAT11(0x17,spew.orient.z._0_1_);
			spew._0_2_ = 0;
			spew._2_2_ = 0;
			spew._44_4_ = 0xff;
			spew.orient.z = (float)CONCAT22(0x1000,spew.orient.z._0_2_);
			spew.roomnum = CONCAT22(spew.roomnum._2_2_,4);
			spew.random = (undefined *)0x0;
			AddHUDItem((tHUDItem *)&spew);
		}
		UpdateCustomtext2HUDItem(mstruct->message);
		break;
#endif
	case MSAFE_DOOR_LOCK_STATE:
		DoorwayLockUnlock(mstruct->objhandle,mstruct->state != 0);
		break;
	case MSAFE_DOOR_ACTIVATE:
		DoorwayActivate(mstruct->objhandle);
		break;
	case MSAFE_DOOR_POSITION:
		DoorwaySetPosition(mstruct->objhandle,mstruct->scalar);
		break;
	case MSAFE_DOOR_STOP:
		DoorwayStop(mstruct->objhandle);
		break;
#if 0
	case MSAFE_TRIGGER_SET:
		TriggerSetState(mstruct->trigger_num,mstruct->state != 0);
		ok = false;
		break;
#endif		
	case MSAFE_INVEN_ADD_TYPE_ID:
		obj = ObjGet(mstruct->objhandle);
		if ((obj != NULL) && (obj->type == OBJ_PLAYER)) {
			if (mstruct->type == OBJ_WEAPON) {
				Players[obj->id].counter_measures.Add(OBJ_WEAPON,mstruct->id,NULL,-1,-1,mstruct->flags,NULL);
			}
			else {
				Players[obj->id].inventory.Add(mstruct->type,mstruct->id,NULL,-1,-1,mstruct->flags,NULL);
			}
		}
		break;
	case MSAFE_INVEN_REMOVE:
	case MSAFE_COUNTERMEASURE_REMOVE:
		obj = ObjGet(mstruct->objhandle);
		if ((obj != NULL) && (obj->type == OBJ_PLAYER)) {
			uVar1 = obj->id;
			if (type == MSAFE_COUNTERMEASURE_REMOVE)
				mstruct->type = OBJ_WEAPON;
			if (mstruct->type == OBJ_WEAPON)
				Players[uVar1].counter_measures.Remove(OBJ_WEAPON,mstruct->id);
			else
				Players[uVar1].inventory.Remove(mstruct->type,mstruct->id);
		}
		break;
	case MSAFE_INVEN_ADD_OBJECT:
		obj = ObjGet(mstruct->objhandle);
		obj2 = ObjGet(mstruct->ithandle);
		ok = false;
		if (obj && obj2 && obj->type == OBJ_PLAYER)
			ok = Players[obj->id].inventory.AddObject(obj2->handle,mstruct->flags,mstruct->message[0] ? mstruct->message : NULL);
		break;
	case MSAFE_INVEN_REMOVE_OBJECT:
		obj = ObjGet(mstruct->objhandle);
		obj2 = ObjGet(mstruct->ithandle);
		mstruct->state = '\0';
		ok = false;
		if (((obj != NULL) && (obj2 != NULL)) && (obj->type == OBJ_PLAYER)) {
			valid = Players[obj->id].inventory.Remove(obj2->handle,-1);
			mstruct->state = valid;
			if (valid != false) {
				ok = true;
			}
		}
		break;
#if 0
	case MSAFE_COUNTERMEASURE_ADD:
		obj = ObjGet(mstruct->objhandle);
		if ((obj != NULL) && (obj->type == OBJ_PLAYER)) {
			uVar1 = obj->id;
			local_17c = 0.0;
			if ((0 < mstruct->count) &&
				 ((roomnum = FindWeaponName(mstruct->name), roomnum != -1 &&
					(local_184 = (void *)0x0, 0 < mstruct->count)))) {
				do {
					ok = Players[uVar1].counter_measures.Add(OBJ_WEAPON,roomnum,obj,mstruct->aux_type,mstruct->aux_id,0,NULL);
					if (ok != false) {
						local_17c = (float)((int)local_17c + 1);
					}
					local_184 = (void *)((int)local_184 + 1);
				} while ((int)local_184 < mstruct->count);
			}
			ok = local_17c != 0.0;
			mstruct->count = (int)local_17c;
		}
		break;
	case MSAFE_WEAPON_ADD:
		obj = ObjGet(mstruct->objhandle);
		if ((obj == NULL) || (obj->type != OBJ_PLAYER)) break;
		local_184 = (void *)(mstruct->index * 0xfc + 0xa2086c + Players[obj->id].ship_index * 0x1724);
		valid = PlayerHasWeapon((uint)obj->id,mstruct->index);
		if ((valid == false) && (mstruct->count < 1)) {
			return;
		}
		if (0 < mstruct->count) {
			flags = mstruct->index;
			bVar3 = false;
			if ((9 < (int)flags) || (*(float *)((int)local_184 + 0xf8) != 0.0)) {
				bVar3 = true;
			}
			if (valid == false) {
				AddWeaponToPlayer((uint)obj->id,flags,-(uint)bVar3 & mstruct->count);
				return;
			}
			uVar6 = Players[obj->id].weapon_ammo[flags];
			if ((((!bVar3) ||
					 (roomnum = player_wb_add_ammo((uint)obj->id,flags,mstruct->count), 0 < roomnum)) &&
					(obj->id == Player_num)) && (uVar6 == 0)) {
				AutoSelectWeapon((uint)obj->id,mstruct->index);
			}
			break;
		}
		roomnum = mstruct->index;
		if (roomnum < 10) {
			iVar12 = 0;
			if (mstruct->state != '\x01') break;
			flags = (uint)obj->id;
LAB_00492fb0:
			Players[flags].weapon_flags = Players[flags].weapon_flags & ~(1 << ((byte)roomnum & 0x1f));
		}
		else {
			iVar12 = 1;
			Players[obj->id].weapon_ammo[roomnum] =
					 Players[obj->id].weapon_ammo[roomnum] + *(short *)&mstruct->count;
			flags = (uint)obj->id;
			roomnum = mstruct->index;
			if (Players[flags].weapon_ammo[roomnum] != 0) break;
			if (mstruct->state == '\x01') goto LAB_00492fb0;
		}
		if ((*(int *)((uint)obj->id * 0x1f0 + 0x8f63b0 + iVar12 * 0xc) == mstruct->index) &&
			 (obj->id == Player_num)) {
			AutoSelectWeapon(iVar12,0xffffffff);
		}
#endif
	}
#if 0
	if ((Demo_flags == 1 && ok) ||
		(ok && (Game_mode & 0x24) && Netgame.local_role == 1))
		MultiSendMSafeFunction(type,mstruct);
#endif		
}
