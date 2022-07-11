#include <string.h>
#include "game.h"
#include "object.h"
#include "gametexture.h"
#include "terrain.h"
#include "sounds.h"
#include "fireball.h"
#include "player.h"
#include "soundload.h"
#include "objinfo.h"
#include "weapon.h"
#include "polymodel.h"
#include "ship.h"
#include "door.h"
#include "globals.h"
#include "vclip.h"

extern int paged_in_count, paged_in_num;

ubyte Textures_to_free[MAX_TEXTURES], Sounds_to_free[MAX_SOUNDS], Models_to_free[MAX_POLY_MODELS];

void PageInGeneric (int id);

void PageInTexture(int id, bool allow_compress)
{
}

void TouchTexture(int id)
{
	if (GameTextures[id].flags & TF_ANIMATED)
		PageInVClip(GameTextures[id].bm_handle);
	else
		PageInTexture(id, true);
}

void PageInLevelTexture (int id)
{
	if (id==-1 || id==0 || Dedicated_server)
		return;

	TouchTexture(id);

	if (GameTextures[id].flags & TF_DESTROYABLE && GameTextures[id].destroy_handle!=-1)
		PageInLevelTexture (GameTextures[id].destroy_handle);
}


void PageInSound (int id)
{
	if (id==-1 || Dedicated_server)
		return;

}

void PageInDoor (int id)
{
	//Set sounds
	door *doorpointer=&Doors[id];

	PageInPolymodel (doorpointer->model_handle);

	poly_model *pm=&Poly_models[doorpointer->model_handle];
	for (int t=0;t<pm->n_textures;t++)
		PageInLevelTexture (pm->textures[t]);
	

	if (doorpointer->open_sound!=-1 && doorpointer->open_sound!=SOUND_NONE_INDEX)
		PageInSound (doorpointer->open_sound);
	if (doorpointer->close_sound!=-1 && doorpointer->close_sound!=SOUND_NONE_INDEX)
		PageInSound (doorpointer->close_sound);
}

void PageInWeapon (int id)
{
	weapon *weaponpointer=&Weapons[id];
	
	if (id==-1)
		return;

	int i;
	
	if (!(weaponpointer->flags & (WF_IMAGE_BITMAP|WF_IMAGE_VCLIP)))
	{
		PageInPolymodel (weaponpointer->fire_image_handle);

		poly_model *pm=&Poly_models[weaponpointer->fire_image_handle];
		for (int t=0;t<pm->n_textures;t++)
			PageInLevelTexture (pm->textures[t]);
	}
	
	// Load the various textures associated with this weapon
	if (weaponpointer->explode_image_handle!=-1)
	{
		PageInLevelTexture (weaponpointer->explode_image_handle);
	}

	if (weaponpointer->particle_handle!=-1)
	{
		PageInLevelTexture (weaponpointer->particle_handle);
	}

	if (weaponpointer->smoke_handle!=-1)
	{
		PageInLevelTexture(weaponpointer->smoke_handle);
	}

	if (weaponpointer->scorch_handle!=-1)
	{
		PageInLevelTexture (weaponpointer->scorch_handle);
	}

	if (weaponpointer->icon_handle!=-1)
	{
		PageInLevelTexture (weaponpointer->icon_handle);
	}

	// Try to load spawn weapons
	if (weaponpointer->spawn_handle!=-1 && weaponpointer->spawn_count>0 && weaponpointer->spawn_handle!=id)
	{
		PageInWeapon (weaponpointer->spawn_handle);
	}
	
	if (weaponpointer->alternate_spawn_handle!=-1 && weaponpointer->spawn_count>0 && weaponpointer->alternate_spawn_handle!=id)
	{
		PageInWeapon (weaponpointer->alternate_spawn_handle);
	}

	if (weaponpointer->robot_spawn_handle!=-1)
	{
		PageInGeneric (weaponpointer->robot_spawn_handle);
	}

	// Try and load the various sounds
	for (i=0;i<MAX_WEAPON_SOUNDS;i++)
	{
		if (weaponpointer->sounds[i]!=SOUND_NONE_INDEX)
		{
			PageInSound (weaponpointer->sounds[i]);
		}
	}
}

void PageInShip (int id)
{
	int i,t;
	
	ship *shippointer=&Ships[id];

	// Page in all textures for this object

	PageInPolymodel (shippointer->model_handle);


	poly_model *pm=&Poly_models[shippointer->model_handle];

	for (t=0;t<pm->n_textures;t++)
		PageInLevelTexture (pm->textures[t]);

	if (shippointer->med_render_handle!=-1)
	{
		PageInPolymodel (shippointer->med_render_handle);

		pm=&Poly_models[shippointer->med_render_handle];
		for (t=0;t<pm->n_textures;t++)
			PageInLevelTexture (pm->textures[t]);
	}

	if (shippointer->lo_render_handle!=-1)
	{
		PageInPolymodel (shippointer->lo_render_handle);
		
		pm=&Poly_models[shippointer->lo_render_handle];
		for (t=0;t<pm->n_textures;t++)
			PageInLevelTexture (pm->textures[t]);
	}

	if (shippointer->dying_model_handle!=-1)
	{
		PageInPolymodel (shippointer->dying_model_handle);
		

		pm=&Poly_models[shippointer->dying_model_handle];
		for (t=0;t<pm->n_textures;t++)
			PageInLevelTexture(pm->textures[t]);
	}

	// Try and load the various weapons
	int j;
	for(i = 0; i < MAX_PLAYER_WEAPONS; i++)
	{
		for(j = 0; j < MAX_WB_GUNPOINTS; j++)
		{
			if (shippointer->static_wb[i].gp_weapon_index[j] != LASER_INDEX)
			{	
				PageInWeapon (shippointer->static_wb[i].gp_weapon_index[j]);
			}
		}
	}

	// Try and load the various weapons
	for(i = 0; i < MAX_PLAYER_WEAPONS; i++)
	{
		for(j = 0; j < MAX_WB_FIRING_MASKS; j++)
		{
			if (shippointer->static_wb[i].fm_fire_sound_index[j] != SOUND_NONE_INDEX)
				PageInSound (shippointer->static_wb[i].fm_fire_sound_index[j]);
		}
	}

	for(i = 0; i < MAX_PLAYER_WEAPONS; i++)
	{
		if (shippointer->firing_sound[i]!=-1)
			PageInSound (shippointer->firing_sound[i]);
		
		if(shippointer->firing_release_sound[i] != -1)
			PageInSound (shippointer->firing_release_sound[i]);
		
		if (shippointer->spew_powerup[i]!=-1)
			PageInGeneric (shippointer->spew_powerup[i]);
		
	}
}

void PageInGeneric (int id)
{
	int i,t;
	
	if (id==-1)
		return;

	object_info *objinfopointer=&Object_info[id];

	// Page in all textures for this object

	PageInPolymodel (objinfopointer->render_handle);

	poly_model *pm=&Poly_models[objinfopointer->render_handle];

	for (t=0;t<pm->n_textures;t++)
		PageInLevelTexture(pm->textures[t]);


	if (objinfopointer->med_render_handle!=-1)
	{
		PageInPolymodel (objinfopointer->med_render_handle);
		


		pm=&Poly_models[objinfopointer->med_render_handle];
		for (t=0;t<pm->n_textures;t++)
			PageInLevelTexture (pm->textures[t]);
	}

	if (objinfopointer->lo_render_handle!=-1)
	{
		PageInPolymodel (objinfopointer->lo_render_handle);
		

		pm=&Poly_models[objinfopointer->lo_render_handle];
		for (t=0;t<pm->n_textures;t++)
			PageInLevelTexture (pm->textures[t]);
	}

	// Process all sounds for this object
	for (i=0;i<MAX_OBJ_SOUNDS;i++)
	{
		if (objinfopointer->sounds[i]!=SOUND_NONE_INDEX)
		{
			PageInSound (objinfopointer->sounds[i]);
		}
	}

	if (objinfopointer->ai_info) {
		for (i=0;i<MAX_AI_SOUNDS;i++)
		{
			if (objinfopointer->ai_info->sound[i]!=SOUND_NONE_INDEX)
			{
				PageInSound (objinfopointer->ai_info->sound[i]);
			}
		}
	}

	// Try and load the various wb sounds
	int j;
	if(objinfopointer->static_wb)
	{
		for(i = 0; i < MAX_WBS_PER_OBJ; i++)
		{
			for(j = 0; j < MAX_WB_FIRING_MASKS; j++)
			{
				if(objinfopointer->static_wb[i].fm_fire_sound_index[j]!=SOUND_NONE_INDEX)
				{
					PageInSound (objinfopointer->static_wb[i].fm_fire_sound_index[j]);
				}
			}
		}
	}

	// Try and load the various wb sounds
	if(objinfopointer->anim) {
		for(i = 0; i < NUM_MOVEMENT_CLASSES; i++)
		{
			for(j = 0; j < NUM_ANIMS_PER_CLASS; j++)
			{
				if(objinfopointer->anim[i].elem[j].anim_sound_index!=SOUND_NONE_INDEX)
				{
					PageInSound (objinfopointer->anim[i].elem[j].anim_sound_index);
				}
			}
		}
	}

	// Load the spew types
	for(i=0;i<MAX_DSPEW_TYPES;i++)
	{
		if (objinfopointer->dspew_number[i]>0 && objinfopointer->dspew[i]!=0 && objinfopointer->dspew[i]!=id)
		{
			PageInGeneric (objinfopointer->dspew[i]);
		}
	}

	// Try and load the various weapons

	if(objinfopointer->static_wb){
		// Automatically include laser
		PageInWeapon (LASER_INDEX);

		for(i = 0; i < MAX_WBS_PER_OBJ; i++)
		{
			for(j = 0; j < MAX_WB_GUNPOINTS; j++)
			{
				if (objinfopointer->static_wb[i].gp_weapon_index[j]!=LASER_INDEX)
				{
					PageInWeapon (objinfopointer->static_wb[i].gp_weapon_index[j]);
				}
			}
		}
	}
}

extern char *Static_sound_names[];
void PageInAllData ()
{
	int i;

	memset(Textures_to_free, 0, sizeof(Textures_to_free));
	memset(Sounds_to_free, 0, sizeof(Sounds_to_free));
	memset(Models_to_free, 0, sizeof(Models_to_free));

	paged_in_count = paged_in_num = 0;
	
	PageInShip (Players[Player_num].ship_index);
	PageInLevelTexture (FindTextureName("LightFlareStar"));
	PageInLevelTexture (FindTextureName("LightFlare"));

	// Get static fireballs
	for (i=0;i<NUM_FIREBALLS;i++)
	{
		char name[PAGENAME_LEN];
		strcpy (name,Fireballs[i].name);

		name[strlen(name)-4]=0;
		int id=IGNORE_TABLE(FindTextureName (name));
		if (id!=-1)
			PageInLevelTexture (id);
	}

	// Get static sounds
	for (i=0;i<NUM_STATIC_SOUNDS;i++)
	{
		int sid=FindSoundName(IGNORE_TABLE(Static_sound_names[i]));

		if (sid!=-1)
			PageInSound(sid);
	}

	// First get textures	
	for (i=0;i<=Highest_room_index;i++)
	{
		if (!Rooms[i].used)
			continue;

		room *rp=&Rooms[i];
		for (int t=0;t<rp->num_faces;t++)
		{
			PageInLevelTexture(rp->faces[t].tmap);
		}
	}

	// Touch all terrain textures
	for (i=0;i<TERRAIN_TEX_WIDTH*TERRAIN_TEX_DEPTH;i++)
	{
		PageInLevelTexture (Terrain_tex_seg[i].tex_index);
	}
	
	if (Terrain_sky.textured)
	{
		PageInLevelTexture (Terrain_sky.dome_texture);
	}
	
	for (i=0;i<Terrain_sky.num_satellites;i++)
		PageInLevelTexture (Terrain_sky.satellite_texture[i]);

	// Touch all objects
	for (i=0;i<=Highest_object_index;i++)
	{
		object *obj=&Objects[i];

		if (obj->type==OBJ_POWERUP || obj->type==OBJ_ROBOT || obj->type==OBJ_CLUTTER || obj->type==OBJ_BUILDING)
		{
			PageInGeneric (obj->id);
			continue;
		}

		if (obj->type==OBJ_DOOR)
		{
			PageInDoor (obj->id);
			continue;
		}
	}
}
