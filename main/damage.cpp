#include "game.h"
#include "objinfo.h"
#include "damage.h"
#include "osiris_dll.h"
#include "diff.h"
#include "gamedll.h"
#include "multi.h"
#include "weapon.h"
#include "doorway.h"
#include "player.h"
#include "ship.h"
#include "sounds.h"
#include "hlsoundlib.h"

float Diff_robot_damage[] = {2.75f, 1.5f, 1.0f, 0.8f, 0.6f};

int Game_music_info7;

float Multi_additional_damage[MAX_PLAYERS];
int Multi_additional_damage_type[MAX_PLAYERS];

void SetDeformDamageEffect(object *obj)
{
	effect_info_s *effect;

	effect = obj->effect_info;
	if (!effect)
		return;
	effect->type_flags = effect->type_flags | EF_DEFORM;
	effect->deform_time = 1.0;
	effect->deform_range = 0.3;
}

void ApplyFreezeDamageEffect(object *obj)
{
	float scalar;
	effect_info_s *eff;
	uint eff_flags;

	eff = obj->effect_info;
	if (!eff)
		return;
	eff_flags = eff->type_flags;
	if ((eff_flags & EF_FREEZE) == 0) {
		eff->freeze_scalar = 0.8;
		eff->type_flags = eff_flags | EF_FREEZE;
		return;
	}
	scalar = eff->freeze_scalar - 0.2;
	eff->type_flags = eff_flags | EF_FREEZE;
	eff->freeze_scalar = scalar;
	if (scalar < 0.3)
		eff->freeze_scalar = 0.3;
}

void SetNapalmDamageEffect(object *obj, object *killer, int weapon_id) {}

void KillObject(object *objp, object *killer, float damage, int death_flags, float delay) {
	if (objp->type == OBJ_DOOR)
		DoorwayDestroy(objp);
	objp->flags |= OF_DEAD;
}

void KillObject(object *objp, object *killer, float damage) {
	KillObject(objp,killer,damage,-1,0);
}

void KillPlayer(object *playerobj,object *killer,float damage_amount,int weapon_id) {
}

bool ApplyDamageToGeneric
					(object *generic,object *weapon,int damage_type,float damage,int server_says,int weapon_id)
{
	ubyte diff;
	object *killer;
	uint multi;
	bool multi2;
	tOSIRISEventInfo evtdata;
	ai_frame *ai_info;
	int genflags;
	int gentype;
	
	gentype = generic->type;
	if (gentype == OBJ_DUMMY)
		return false;
	if (generic->flags & OF_AI_DEATH)
		return false;
	if ((damage_type != 0) && (!IS_GENERIC(gentype) || !(Object_info[generic->id].flags & OIF_NO_DIFF_SCALE_DAMAGE))) {
		diff = DiffLevel_MP;
		if ((Game_mode & 0x24) == 0) {
			diff = ingame_difficulty;
		}
		damage = damage * Diff_robot_damage[diff];
	}
	killer = weapon;
	if (((weapon != (object *)0x0) && (weapon->type == OBJ_WEAPON)) &&
		 ((killer = ObjGetUltimateParent(weapon), (Game_mode & 0x24) == 0 || (Netgame.local_role == 1))
		 )) {
		weapon_id = (int)weapon->id;
	}
	genflags = generic->flags;
	if (!(genflags & OF_DESTROYABLE)) {
		return false;
	}
	if (genflags & OF_USE_DESTROYED_POLYMODEL) {
		return false;
	}
	multi = Game_mode & 0x24;
	if ((multi != 0) && (genflags & OF_DESTROYED)) {
		return false;
	}
	if (generic->type == OBJ_POWERUP) {
		if (multi != 0) {
			return false;
		}
LAB_0042e2c0:
		evtdata.evt_damaged.it_handle = killer ? killer->handle : -1;
		evtdata.evt_damaged.weapon_handle = weapon ? weapon->handle : -1;
		evtdata.evt_damaged.damage_type = damage_type;
		evtdata.evt_damaged.damage = damage;
		Osiris_CallEvent(generic,EVT_DAMAGED,&evtdata);
		damage = evtdata.evt_damaged.damage;
		if (!damage)
			return false;
		generic->shields = generic->shields - damage;
		multi2 = (Game_mode & 0x24) == 0;
		if (!multi2) {
			if (killer == (object *)0x0) {
				DLLInfo.it_handle = -1;
			}
			else {
				DLLInfo.it_handle = killer->handle;
			}
			DLLInfo.me_handle = generic->handle;
			DLLInfo.fParam = damage;
			CallGameDLL(EVT_GAMEOBJECTSHIELDSCHANGED,&DLLInfo);
			goto LAB_0042e376;
		}
	}
	else {
		if (((multi == 0) || (server_says != 0)) || (Netgame.local_role == 1)) goto LAB_0042e2c0;
LAB_0042e376:
		multi2 = (Game_mode & 0x24) == 0;
	}
	if ((!multi2) && (Netgame.local_role == 1)) {
		MultiSendDamageObject(generic,killer,damage,weapon_id);
	}
	if ((generic->shields < 0.0) && ((generic->flags & OF_AI_DO_DEATH))) {
		generic->shields = 0.0;
		generic->flags |= OF_AI_DEATH;
	}
	if ((generic->shields < 0.0) && !(generic->flags & OF_DYING)) {
		if ((Game_mode & 0x24) == 0) {
			if ((((killer != (object *)0x0) && (killer->type == OBJ_PLAYER)) &&
					(ai_info = generic->ai_info, ai_info != (ai_frame *)0x0)) &&
				 (((ushort)((ushort)(ai_info->awareness < 15.0) << 8 |
									 (ushort)(ai_info->awareness == 15.0) << 0xe) == 0 &&
					(ai_info->target_handle == killer->handle)))) {
				Game_music_info7 = Game_music_info7 + 1;
			}
			if (Demo_flags == 2) goto LAB_0042e477;
		}
		else {
			DLLInfo.me_handle = generic->handle;
			if (killer == (object *)0x0) {
				DLLInfo.it_handle = -1;
			}
			else {
				DLLInfo.it_handle = killer->handle;
			}
			CallGameDLL(EVT_GAMEOBJKILLED,&DLLInfo);
			if (Netgame.local_role != 1) goto LAB_0042e477;
		}
		KillObject(generic,killer,damage);
	}
LAB_0042e477:
	if ((((Game_mode & 0x24) != 0) && (server_says == 0)) && (Netgame.local_role != 1)) {
		return false;
	}
	if (weapon_id != 0xff) {
		if ((Weapons[weapon_id].flags & WF_MICROWAVE)) {
			SetDeformDamageEffect(generic);
		}
		if ((Weapons[weapon_id].flags & WF_NAPALM)) {
			SetNapalmDamageEffect(generic,killer,weapon_id);
		}
		if ((Weapons[weapon_id].flags & WF_FREEZE)) {
			ApplyFreezeDamageEffect(generic);
		}
	}
	return true;
}


void PlayPlayerDamageSound(object *obj,int hit_type)
{
	int snd;

	switch(hit_type) {
		case 1:
			snd = SOUND_HIT_BY_ENERGY_WEAPON;
			break;
		case 2:
			snd = SOUND_HIT_BY_MATTER_WEAPON;
			break;
		case 4:
			snd = SOUND_HIT_BY_CONCUSSIVE_FORCE;
			break;
		case 5:
			snd = SOUND_PLAYER_HIT_WALL;
			break;
		case 6:
			snd = SOUND_VOLATILE_HISS;
			break;
		default:
			return;
	}
	Sound_system.Play3dSound(snd,4,obj,1.0,0);
}


int PlayPlayerInvulnerabilitySound(object *obj)
{
  return Sound_system.Play3dSound(SOUND_METALLIC_DOOR_HIT,4,obj,1.0,0);
}

#define EVENTID_DAMAGE 2

enum eventtype_enum {
	EVENTTYPE_RENDER=2
};

int FindEventID(int id) { return -1; }
void CreateNewEvent(int type, int id, float delay, void *data, int data_size, void (*func)(), int unk) {}
void VIBE_DoPlayerDamage(float amount) {}
ubyte Game_music_info_2;
void MultiSendRequestPeerDamage(object *weapon,int weapon_id,int hit_type,float damage) {}

void DoDamageEffect(void)
{
	float amount = Players[Player_num].damage_magnitude / 20;
	if (amount > 1)
		amount = 1;
	DrawAlphaBlendedScreen(1.0,0.0,0.0,amount * 0.4);
	if (Players[Player_num].damage_magnitude < 0.0001)
		Players[Player_num].damage_magnitude = 0.0;
	else
		CreateNewEvent(EVENTTYPE_RENDER,EVENTID_DAMAGE,0.0,NULL,0,DoDamageEffect,-1);
}

void DecreasePlayerShields(int player,float amount)
{
	Objects[Players[player].objnum].shields -= amount;
	Players[player].damage_magnitude += amount;
	if (player != Player_num)
		return;
	if (FindEventID(EVENTID_DAMAGE) == -1)
		CreateNewEvent(EVENTTYPE_RENDER,EVENTID_DAMAGE,0.0,NULL,0,DoDamageEffect,-1);
	if (Game_mode & 0x24)
		Multi_bail_ui_menu = 1;
	Game_music_info_2 = 1;
	if (amount > 0)
		VIBE_DoPlayerDamage(amount);
}

bool ApplyDamageToPlayer(object *player_obj,object *weapon,int hit_type,float damage,int server_says,
	int weapon_num,bool play_sound)
{
	float fVar2;
	float fVar3;
	float fVar4;
	float fVar5;
	object *obj;
	object *killer;
	object *org_weapon;
	uint playernum;
	ushort playernum2;
	
	obj = player_obj;
	if (!player_obj || player_obj->type != OBJ_PLAYER)
		return false;
	org_weapon = (object *)0x0;
	killer = weapon;
	if ((weapon != (object *)0x0) && (weapon->type == OBJ_WEAPON)) {
		org_weapon = weapon;
		killer = ObjGetUltimateParent(weapon);
		if (((Game_mode & 0x24) == 0) || (Netgame.local_role == 1)) {
			weapon_num = (uint)weapon->id;
		}
		if (((((Game_mode & 0x24) != 0) && (Netgame.local_role == 0)) && (server_says == 0)) &&
			 ((Netgame.flags & NF_PEER_PEER) != 0)) {
			weapon_num = (uint)weapon->id;
		}
	}
	weapon = killer;
	playernum = (uint)obj->id;
	if ((Players[playernum].flags & (PLAYER_FLAGS_DYING || PLAYER_FLAGS_DEAD)) != 0) {
		PlayPlayerDamageSound(obj,hit_type);
		return false;
	}
	if (((Players[playernum].flags & PLAYER_FLAGS_INVULNERABLE) != 0) ||
		 !(Player_object->flags & OF_DESTROYABLE)) {
		if (hit_type != 0)
			PlayPlayerInvulnerabilitySound(obj);
		if (!org_weapon)
			return false;
		Players[obj->id].invul_magnitude = 1.0;
		Players[obj->id].invul_vector = org_weapon->pos - obj->pos;
		vm_NormalizeVectorFast(&Players[obj->id].invul_vector);
		return false;
	}
	if ((Game_mode & 0x24) == 0) {
		float scaled = damage * Players[playernum].armor_scalar *
						Ships[Players[playernum].ship_index].armor_scalar;
		DecreasePlayerShields(playernum,scaled);
		if (scaled > 0 && play_sound)
			PlayPlayerDamageSound(obj,hit_type);
		if (Players[obj->id].guided_obj)
			ReleaseGuidedMissile(obj->id);
		if (Players[obj->id].user_timeout_obj)
			ReleaseUserTimeoutMissile(obj->id);
		if (weapon_num != 0xff) {
			if ((Weapons[weapon_num].flags & WF_MICROWAVE) != 0)
				SetDeformDamageEffect(obj);
			if ((Weapons[weapon_num].flags & WF_FREEZE) != 0)
				ApplyFreezeDamageEffect(obj);
			if ((Weapons[weapon_num].flags & WF_NAPALM) != 0)
				SetNapalmDamageEffect(obj,weapon,weapon_num);
		}
		if (obj->shields < 0)
			KillPlayer(obj,weapon,fVar2,weapon_num);
		return true;
	}
	if ((((Netgame.local_role == 0) && ((Netgame.flags & NF_PEER_PEER) != 0)) && (server_says == 0)) &&
		 (playernum == Player_num)) {
		MultiSendRequestPeerDamage(weapon,weapon_num,hit_type,damage);
		return true;
	}
	if (Netgame.flags & NF_PEER_PEER) {
		if (Netgame.local_role == 1) {
			if ((server_says == 0) && (playernum != Player_num)) {
				return true;
			}
		}
	} else {
		if (Netgame.local_role != 1) {
			if (server_says) {
				return true;
			}
		}
	}
	if (Netgame.local_role == 1) {
		if ((weapon != (object *)0x0) && (weapon->type == OBJ_PLAYER)) {
			playernum2 = weapon->id;
			if ((playernum2 != obj->id) &&
				 ((Players[playernum2].team == Players[playernum].team &&
					((Netgame.flags & NF_DAMAGE_FRIENDLY) == 0)))) {
				return false;
			}
			damage = damage * Players[playernum2].damage_scalar;
		}
		damage = damage * Players[playernum].armor_scalar *
						 Ships[Players[playernum].ship_index].armor_scalar;
	}
	if (!(Netgame.flags & NF_PEER_PEER) || Netgame.local_role == 1) {
		DecreasePlayerShields(playernum,damage);
		if (((ushort)((ushort)(damage < 0.0) << 8 | (ushort)(damage == 0.0) << 0xe) == 0) &&
			 play_sound) {
			PlayPlayerDamageSound(obj,hit_type);
		}
		playernum = (uint)obj->id;
		if ((playernum == Player_num) && Players[playernum].guided_obj)
			ReleaseGuidedMissile(playernum);
		playernum = (uint)obj->id;
		if ((playernum == Player_num) && Players[playernum].user_timeout_obj)
			ReleaseUserTimeoutMissile(playernum);
		if (weapon_num != 0xff) {
			if ((Weapons[weapon_num].flags & WF_MICROWAVE) != 0) {
				SetDeformDamageEffect(obj);
			}
			if ((Weapons[weapon_num].flags & WF_NAPALM) != 0) {
				SetNapalmDamageEffect(obj,weapon,weapon_num);
			}
			if ((Weapons[weapon_num].flags & WF_FREEZE) != 0) {
				ApplyFreezeDamageEffect(obj);
			}
		}
		if (Netgame.local_role != 1)
			return true;
		if (weapon_num == 0xff && !org_weapon) {
			Multi_additional_damage[obj->id] += damage;
			Multi_additional_damage_type[obj->id] = hit_type;
		} else if (weapon_num != 0xff &&  Weapons[weapon_num].flags & WF_ELECTRICAL) {
			Multi_additional_damage[obj->id] += damage;
			Multi_additional_damage_type[obj->id] = 1;
		} else
			MultiSendDamagePlayer(obj->id,weapon_num,hit_type,damage);
	}
	if (Netgame.local_role != 1) {
		return true;
	}
	DLLInfo.me_handle = obj->handle;
	if (weapon == (object *)0x0) {
		DLLInfo.it_handle = -1;
	}
	else {
		DLLInfo.it_handle = weapon->handle;
	}
	DLLInfo.fParam = damage;
	CallGameDLL(EVT_GAMEOBJECTSHIELDSCHANGED,&DLLInfo);
	if (obj->shields < 0.0) {
		KillPlayer(obj,weapon,damage,weapon_num);
	}
	return true;
}

