#include "player.h"
#include "object.h"
#include "game.h"

void InitPlayerNewLevel(int playernum)
{
	object *obj = Objects + Players[playernum].objnum;
	if (playernum == Player_num) {
		if (obj->shields < 100)
			obj->shields = 100;
		if (Players[playernum].energy < 100)
			Players[playernum].energy = 100;
	}
	Players[playernum].num_kills_level = 0;
	Players[playernum].friendly_kills_level = 0;
	Players[playernum].num_markers = 0;
	Players[playernum].num_hits_level = 0;
	Players[playernum].num_discharges_level = 0;
	Players[playernum].keys = 0;
	Players[playernum].num_deaths_level = 0;
}

void InitPlayerNewShip(int playernum, int invel_reset) {
	Players[playernum].energy = 100.0f;
	Players[playernum].num_balls = 0;
	Players[playernum].laser_level = 0;
	Players[playernum].killer_objnum = -1;
	Players[playernum].light_dist = 0;
	Players[playernum].oldroom = -1;
	Players[playernum].guided_obj = NULL;
	Players[playernum].user_timeout_obj = NULL;
	Players[playernum].afterburn_time_left = 5.0f;
	Players[playernum].last_thrust_time = 0;
	Players[playernum].last_afterburner_time = 0;

	memset(&Players[playernum].weapon_ammo, 0, sizeof(Players[0].weapon_ammo));

	Players[playernum].weapon_flags = 0x100401;
	Players[playernum].weapon_ammo[10] = 7 - Difficulty_level;
	Players[playernum].weapon[0].index = 0;
	Players[playernum].weapon[0].firing_time = 0;
	Players[playernum].weapon[0].sound_handle = -1;
	Players[playernum].weapon[1].index = 10;
	Players[playernum].weapon[1].firing_time = 0;
	Players[playernum].weapon[1].sound_handle = -1;
	Players[playernum].small_left_obj = -1;
	Players[playernum].small_right_obj = -1;
	Players[playernum].small_dll_obj = -1;
	Players[playernum].flags = 0;
	Players[playernum].invulnerable_time = 0;
	Players[playernum].damage_magnitude = 0;
	Players[playernum].edrain_magnitude = 0;

	Players[playernum].last_homing_warning_sound_time = 0;
	Players[playernum].last_hit_wall_sound_time = 0;
	Players[playernum].multiplayer_flags = 0;
	Players[playernum].last_multiplayer_flags = 0;
	Players[playernum].afterburner_mag = 0;
	Players[playernum].thrust_mag = 0;
	Players[playernum].last_guided_time = 0;
	Players[playernum].afterburner_sound_handle = -1;
	Players[playernum].thruster_sound_state = 0;
	Players[playernum].thruster_sound_handle = -1;
	Players[playernum].movement_scalar = 1.0f;
	Players[playernum].armor_scalar = 1.0f;
	Players[playernum].damage_scalar = 1.0f;
	Players[playernum].turn_scalar = 1.0f;
	Players[playernum].weapon_recharge_scalar = 1.0f;
	Players[playernum].weapon_speed_scalar = 1.0f;
	Players[playernum].controller_bitflags = ~0;
}

