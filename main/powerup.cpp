#include "game.h"
#include "osiris_common.h"
#include "multi.h"
#include "sounds.h"
#include "hlsoundlib.h"
#include "objinfo.h"
#include "hud.h"
#include "stringtable.h"
#include "player.h"
#include "diff.h"

float Diff_shield_energy_scalar[5] = {2.25, 1.5, 1.0, 0.75, 0.5};

bool HandleWeaponPowerups(char *name,msafe_struct *mstruct,ubyte *pickedup) { return false; }
bool HandleCounterMeasurePowerups(char *name,msafe_struct *mstruct,ubyte *pickedup) { return false; }
bool HandleInventoryPowerups(char *name,msafe_struct *mstruct,ubyte *pickedup) { return false; }

bool HandleCommonPowerups(char *name,msafe_struct *mstruct,ubyte *pickedup)
{
	object *obj;
	bool handled;
	float amount;
	float amount_max;
	float new_val;
	int diff;
	int player;

	obj = ObjGet(mstruct->ithandle);
	player = (uint)obj->id;
	handled = false;
	if (stricmp("Shield",name) == 0) {
		handled = true;
		if (obj->shields < 200.0) {
			*pickedup = 1;
			diff = Game_mode & 0x24 ? DiffLevel_MP : ingame_difficulty;
			amount = Diff_shield_energy_scalar[diff] * 12.0;
			new_val = amount + obj->shields;
			if (new_val > 200)
				amount = 200 - obj->shields;
			obj->shields += amount;
			if (player == Player_num)
				AddFilteredHUDMessage(TXT_SHIELDBOOST, (int)obj->shields);
		} else if (player == Player_num)
			AddFilteredHUDMessage(TXT_MAXSHIELDS);
	} else if (stricmp("Energy",name) == 0) {
		handled = true;
		diff = Game_mode & 0x24 ? DiffLevel_MP : ingame_difficulty;
		amount_max = 200.0 - Players[player].energy;
		amount = Diff_shield_energy_scalar[diff] * 12.0;
		if (amount > amount_max)
			amount = amount_max;
		Players[player].energy += amount;
		if (Game_mode & 0x24 || amount > 0)
			*pickedup = 1;
		if (player == Player_num)
			AddFilteredHUDMessage(amount > 0 ? TXT_MSG_ENERGY : TXT_MAXENERGY);
#if 0 
	} else if (stricmp("QuadLaser",name) == 0) {
		handled = true;
		if (obj->dynamic_wb[0].flags & DWBF_QUAD) {
			if (player == Player_num)
				AddFilteredHUDMessage(TXT_QUADLASER);
			*pickedup = 1;
			obj->dynamic_wb[0].flags |= DWBF_QUAD;
			obj->dynamic_wb[5].flags |= DWBF_QUAD;
			if (quad_laser_id == -2)
				quad_laser_id = FindObjectIDName(name);
			if (quad_laser_id >= 0)
				Players[player].inventory.Add(THIEFITEM_QUADFIRE,quad_laser_id,NULL,-1,-1,6,NULL);
		} else if (player == Player_num) {
				AddFilteredHUDMessage(TXT_MSG_QUADHAVE);
				return true;
		}
	} else if (stricmp("FullMap",name) == 0) {
		handled = true;
		if (player == Player_num) {
			AddFilteredHUDMessage(TXT_FULLMAP);
			for (int i = 0; i < MAX_ROOMS; i++)
				if (!Rooms[i].flags & RF_SECRET)
					AutomapVisMap[i] = 1;
			*pickedup = 1;
			return true;
		}
	} else if (stricmp("ThiefAutoMap",name) == 0) {
		handled = true;
		if (player == Player_num)
			AddFilteredHUDMessage(TXT_RETURNEDAUTOMAP);
		ThiefPlayerReturnItem(obj->handle, THIEFITEM_AUTOMAP);
		*pickedup = 1;
	} else if (stricmp("HeadlightPowerup",name) == 0) {
		handled = true;
		if (!ThiefPlayerHasItem(obj->handle, THIEFITEM_HEADLIGHT)) {
			if (player == Player_num)
				AddFilteredHUDMessage(TXT_RETURNEDHEADLIGHT);
			ThiefPlayerReturnItem(obj->handle, THIEFITEM_HEADLIGHT);
			*pickedup = 1;
		} else if (player == Player_num)
			AddFilteredHUDMessage(TXT_HAVEHEADLIGHT);
	} else if (stricmp(name,"Invulnerability") == 0) {
		if (!(Players[player].flags & PLAYER_FLAG_INVULERABLE)) {
			MakePlayerInvulnerable(player,30.0,true);
			*pickedup = 1;
		} else if (player == Player_num)
			AddFilteredHUDMessage(TXT_INVULNALREADY);
		}
	} else if (stricmp(name,"Cloak") == 0) {
		handled = true;
		if (obj->effect_info && !(obj->effect_info->type_flags & EF_FADING_OUT) &&
			!(obj->effect_info->type_flags & EF_CLOAKED)) {
			MakeObjectInvisible(obj,30.0,1.0,false);
			*pickedup = 1;
		} else if (player == Player_num) {
			AddFilteredHUDMessage(TXT_CLOAKALREADY);
#endif
	}
	return handled;
}

void msafe_DoPowerup(msafe_struct *mstruct)
{
	bool handled;
	object *obj;
	ubyte pickedup;
	char *name;
	ubyte sndnum;
	float volume;
	
	obj = ObjGet(mstruct->objhandle);
	name = Object_info[obj->id].name;
	if (!HandleCommonPowerups(name,mstruct,&pickedup) &&
		!HandleWeaponPowerups(name,mstruct,&pickedup) &&
		!HandleCounterMeasurePowerups(name,mstruct,&pickedup) &&
		!HandleInventoryPowerups(name,mstruct,&pickedup))
		return;

	//MultiSendMSafePowerup(mstruct);
	if (pickedup && (!(Game_mode & 0x24) || (Netgame.local_role == 1))) {
		sndnum = mstruct->playsound;
		obj->flags |= OF_DEAD | (sndnum ? OF_SEND_MULTI_REMOVE_ON_DEATHWS : OF_SEND_MULTI_REMOVE_ON_DEATH);
		Sound_system.Play3dSound(SOUND_POWERUP_PICKUP,3,obj,1.0,0);
	}
}

