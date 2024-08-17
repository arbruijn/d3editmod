#include <string.h>
#include <stdarg.h>
#include "manage.h"
#include "gametexture.h"
#include "weapon.h"
#include "door.h"
#include "ssl_lib.h"
#include "ship.h"
#include "pserror.h"
#include "ddio.h"
#include "stringtable.h"
#include "args.h"
#include "globals.h"
#include "vclip.h"
#include "polymodel.h"

#ifndef WIN32
#define strnicmp strncasecmp
#define stricmp strcasecmp
#endif

void mng_FreePagetypePrimitives(int pagetype, char *name, int freetype) {}

int AllocDoor() {
	for (int i = 0; i < MAX_DOORS; i++)
		if (!Doors[i].used) {
			Doors[i].used = 1;
			Doors[i].flags = 0;
			Doors[i].hit_points = 0;
			Num_doors++;
			return i;
		}
	return -1;
}

int AllocWeapon() {
	for (int i = 0; i < MAX_WEAPONS; i++)
		if (!Weapons[i].used) {
			Weapons[i].used = 1;
			Num_weapons++;
			return i;
		}
	return -1;
}

void SetupStaticWB(otype_wb_info *static_wb) {
}

void SetupObjectID(object_info *obj_info, int type, bool f_anim, bool f_weapons, bool f_ai) {
	memset(obj_info, 0, sizeof(object_info));
	if (f_ai) {
		obj_info->ai_info = (t_ai_info *)malloc(sizeof(t_ai_info));
		memset(obj_info->ai_info, 0, sizeof(t_ai_info));
	}
	if (f_weapons) {
		obj_info->static_wb = (otype_wb_info *)malloc(sizeof(otype_wb_info) * MAX_WBS_PER_OBJ);
		memset(obj_info->static_wb, 0, sizeof(otype_wb_info) * MAX_WBS_PER_OBJ);
		SetupStaticWB(obj_info->static_wb);
	}
	if (f_anim) {
		anim_elem *anim = obj_info->anim = (anim_elem *)malloc(sizeof(anim_elem) * NUM_MOVEMENT_CLASSES);
		memset(anim, 0, sizeof(anim_elem) * NUM_MOVEMENT_CLASSES);
		for (int i = 0; i < NUM_MOVEMENT_CLASSES; i++)
			for (int j = 0; j < NUM_ANIMS_PER_CLASS; j++) {
				anim[i].elem[j].spc = 1.0f;
				anim[i].elem[j].anim_sound_index = 0;
			}
	}
	obj_info->type = type;
	obj_info->size = 4.0f;
	obj_info->phys_info.mass = 1.0f;
	obj_info->phys_info.drag = 0.1f;
	obj_info->phys_info.rotdrag = 0.01f;
	obj_info->phys_info.flags = OIF_DESTROYABLE;
	obj_info->phys_info.num_bounces = -1;
	obj_info->phys_info.coeff_restitution = 1.0f;
	obj_info->phys_info.hit_die_dot = -1.0f;
	obj_info->med_render_handle = -1;
	obj_info->lo_render_handle = -1;
	obj_info->med_lod_distance = 75.0f;
	obj_info->lo_lod_distance = 120.0f;
	obj_info->respawn_scalar = 1.0f;
	if (type == OBJ_ROBOT || type == OBJ_CLUTTER) {
		obj_info->med_lod_distance = obj_info->med_lod_distance * 10.0f;
		obj_info->lo_lod_distance = obj_info->lo_lod_distance * 10.0f;
	}
	obj_info->icon_name[0] = '\0';
	obj_info->description = NULL;
	obj_info->flags |= OIF_INVEN_SELECTABLE;
	for (int i = 0; i < MAX_DSPEW_TYPES; i++) {
		obj_info->dspew[i] = -1;
		obj_info->dspew_number[i] = 0;
	}
	obj_info->ammo_count = 0;
	obj_info->multi_allowed = true;
	Num_object_ids[type]++;
}

int AllocObjectID(int type, bool f_anim, bool f_weapons, bool f_ai)
{
	for (int i = 3; i < MAX_OBJECT_IDS; i++)
		if (Object_info[i].type == OBJ_NONE) {
			SetupObjectID(&Object_info[i], type, f_anim, f_weapons, f_ai);
			return i;
		}
	return -1;
}

int AllocateProceduralForTexture(int tex_num) {
	texture *tex = &GameTextures[tex_num];
	proc_struct *proc;
	int w, h;

	if (tex->flags & TF_TEXTURE_64)
		w = h = 64;
	else if (tex->flags & TF_TEXTURE_32)
		w = h = 32;
	else
		w = h = 128;
	proc = tex->procedural = (proc_struct *)malloc(sizeof(*tex->procedural));
	proc->proc1 = NULL;
	proc->proc2 = NULL;
	proc->last_evaluation_time = 0.00000000;
	proc->osc_time = 0.00000000;
	proc->osc_value = '\b';
	proc->procedural_bitmap = bm_AllocBitmap(w,h,0);
	proc->memory_type = '\0';
	proc->palette = (ushort *)malloc(sizeof(ushort) * 256);
	proc->last_procedural_frame = -1;
	proc->heat = -0x80;
	proc->dynamic_proc_elements = -1;
	proc->static_proc_elements = NULL;
	proc->num_static_elements = 0;
	proc->light = '\x01';
	proc->thickness = '\x04';
	memset(proc->palette, 0, sizeof(ushort) * 256);
	return 1;
}
void FreeProceduralForTexture(int tex_num) {
	if (GameTextures[tex_num].procedural)
		free(GameTextures[tex_num].procedural);
}
void AllocateStaticProceduralsForTexture(int handle, int num_elements) {
	GameTextures[handle].procedural->static_proc_elements = (static_proc_element *)
		malloc(num_elements * sizeof(static_proc_element));
}

int AllocTexture() {
	int idx;
	for (idx = 0; idx < MAX_TEXTURES; idx++)
		if (!GameTextures[idx].used)
			break;
	Num_textures++;
	memset(&GameTextures[idx], 0, sizeof(GameTextures[0]));
	GameTextures[idx].used = 1;
	GameTextures[idx].corona_type = 0;
	GameTextures[idx].bumpmap = -1;
	GameTextures[idx].procedural = NULL;
	GameTextures[idx].name[0] = 0;
	GameTextures[idx].flags = 0;
	GameTextures[idx].alpha = 1.0f;
	GameTextures[idx].speed = 1.0f;
	GameTextures[idx].reflectivity = 0.6f;
	GameTextures[idx].bm_handle = -1;
	GameTextures[idx].destroy_handle = -1;
	return idx;
}

#if 0
int AllocLoadVClip(char *name, int texture_size, int mipped, int pageable, int format) {
	return -1;
}
#endif

int LoadTextureImage(char *name, int *pis_ani, int texture_size, int mipped, int pageable, int format) {
	char ext[5];
	int len = strlen(name);
	if (len < 4)
		return -1;
	strncpy(ext, name + len - 3, sizeof(ext));
	int is_ani = strnicmp("oaf", ext, 3) == 0 || strnicmp("ifl", ext, 3)  == 0 || strnicmp("abm", ext, 3) == 0;
	if (pis_ani)
		*pis_ani = is_ani;
	if (is_ani)
		return AllocLoadVClip(name, texture_size, mipped, pageable, format);
	int handle = pageable ? bm_AllocLoadFileNoMemBitmap(name, mipped, format) :
		bm_AllocLoadFileBitmap(name, mipped, format);
	if (handle < 1)
		return -1;
	int w, h;
	if (texture_size == 1)
		w = h = 128;
	else if (texture_size == 2)
		w = h = 64;
	else if (texture_size == 3)
		w = h = 32;
	else if (texture_size == 4)
		w = h = 256;
	else
		return handle;
	if (pageable)
		return handle;
	if (bm_w(handle, 0) == w && bm_h(handle, 0) == h)
		return handle;
	int scaled = bm_AllocBitmap(w, h, (w * h * 2 / 3) * mipped);
	if (mipped)
		GameBitmaps[scaled].flags |= BF_MIPMAPPED;
	GameBitmaps[scaled].format = format;
	bm_ScaleBitmapToBitmap(scaled, handle);
	strcpy(GameBitmaps[scaled].name, GameBitmaps[handle].name);
	bm_FreeBitmap(handle);
	return scaled;
}

int mng_FindLoadTexture(char *name, CFILE *cf);
int mng_FindLoadSound(char *name, CFILE *cf);
int mng_FindLoadGeneric(char *name, CFILE *cf) { return -1; }
int mng_FindLoadWeapon(char *name, CFILE *cf);

typedef struct {
	char image_filename[PAGENAME_LEN];
	char destroy_name[PAGENAME_LEN];
	char sound_name[PAGENAME_LEN];
	texture item;
	int num_proc_elms;
	ubyte heat;
	ubyte light;
	ubyte thickness;
	ubyte osc_value;
	float eval_time;
	float osc_time;
	ubyte type[8000];
	ubyte frequency[8000];
	ubyte speed[8000];
	ubyte size[8000];
	ubyte x1[8000];
	ubyte y1[8000];
	ubyte x2[8000];
	ubyte y2[8000];
	short palette[256];
} texture_page;

typedef struct {
	weapon item;
	char hud_image_name[PAGENAME_LEN];
	char fire_image_model_name[PAGENAME_LEN];
	char explode_image_name[PAGENAME_LEN];
	char smoke_image_name[PAGENAME_LEN];
	char scorch_image_name[PAGENAME_LEN];
	char icon_image_name[PAGENAME_LEN];
	char spawn_weapon_name[PAGENAME_LEN];
	char alt_spawn_weapon_name[PAGENAME_LEN];
	char particle_image_name[PAGENAME_LEN];
	char robot_spawn_name[PAGENAME_LEN];
	char sound_names[7][PAGENAME_LEN];
	char pad;
} weapon_page;

typedef struct  {
    door item;
    char model_name[PAGENAME_LEN];
    char open_sound[PAGENAME_LEN];
    char close_sound[PAGENAME_LEN];
} door_page;

typedef struct {
    struct sound_info item;
    char filename[PAGENAME_LEN];
} sound_page;

typedef struct {
    ship item;
    char model_name[PAGENAME_LEN];
    char dying_model_name[PAGENAME_LEN];
    char wb_weapon_names[MAX_PLAYER_WEAPONS][MAX_WB_GUNPOINTS][PAGENAME_LEN];
    char wb_sound_names[MAX_PLAYER_WEAPONS][MAX_WB_GUNPOINTS][PAGENAME_LEN];
    char med_model_name[PAGENAME_LEN];
    char lo_model_name[PAGENAME_LEN];
    char firing_sound_names[MAX_PLAYER_WEAPONS][PAGENAME_LEN];
    char firing_release_sound_names[MAX_PLAYER_WEAPONS][PAGENAME_LEN];
    char spew_powerup_names[MAX_PLAYER_WEAPONS][PAGENAME_LEN];
} ship_page;

typedef struct {
    object_info item;
    anim_elem anim[NUM_MOVEMENT_CLASSES];
    otype_wb_info static_wb[MAX_WBS_PER_OBJ];
    t_ai_info ai_info;
    char model_name[PAGENAME_LEN];
    char med_model_name[PAGENAME_LEN];
    char lo_model_name[PAGENAME_LEN];
    char sound_names[2][PAGENAME_LEN];
    char ai_sound_names[5][PAGENAME_LEN];
    char wb_weapon_names[MAX_WBS_PER_OBJ][MAX_WB_GUNPOINTS][PAGENAME_LEN];
    char wb_sound_names[MAX_WBS_PER_OBJ][MAX_WB_FIRING_MASKS][PAGENAME_LEN];
    char anim_sound_names[5][24][PAGENAME_LEN];
    char dspew_generic_names[2][PAGENAME_LEN];
} generic_page;

typedef struct {
	char name[PAGENAME_LEN];
	char name2[PAGENAME_LEN];
	ubyte used;
} gamefile_page;

typedef struct {
	char	name[PAGENAME_LEN];
	ubyte	b1, b2;
	char	unk[35+35+35+30];
	ubyte	used;
	ubyte	pad1, pad2, pad3;
	char name2[64][PAGENAME_LEN];
} megacell_page;

void InitTexturePage(texture_page *page) {
	memset(page, 0, sizeof(*page));
	page->item.alpha = 1.0f;
	page->item.speed = 1.0f;
	page->item.sound_volume = 1.0f;
	page->thickness = 4;
	page->heat = 200;
	page->light = 1;
	page->num_proc_elms = 0;
	page->eval_time = 0.0f;
	page->osc_time = 0.0f;
	page->osc_value = 8;
	page->item.reflectivity = 0.5f;
	page->item.corona_type = 0;
	page->item.slide_v = 0.0f;	
	page->item.slide_u = 0.0f;	
	page->item.bumpmap = -1;
	page->item.sound = -1;
	strcpy(page->image_filename, "");
	strcpy(page->destroy_name, "");
	strcpy(page->sound_name, "");
}

int TexturePageToTexture(texture_page *page, int idx, CFILE *cf) {
	texture *tex = &GameTextures[idx];
	*tex = page->item;
	int mipped, texture_size, bm, subidx;
	if (tex->flags & TF_TEXTURE_64) {
		mipped = 1;
		texture_size = 2;
	} else if (tex->flags & TF_TEXTURE_32) {
		mipped = 1;
		texture_size = 3;
	} else if (tex->flags & TF_TEXTURE_256) {
		mipped = 0;
		texture_size = 4;
	} else {
		mipped = 1;
		texture_size = 1;
	}
	bm = LoadTextureImage(page->image_filename, NULL, texture_size, mipped, 1, 0);
	if (bm < 0) {
		tex->bm_handle = 0;
		return 0;
	}
	tex->bm_handle = bm;
	if (tex->flags & TF_DESTROYABLE) {
		if (stricmp(page->destroy_name, tex->name) == 0) {
			tex->destroy_handle = -1;
			tex->flags &= ~TF_DESTROYABLE;
		} else {
			subidx = mng_FindLoadTexture(page->destroy_name, cf);
			if (subidx < 0)
				tex->destroy_handle = 0;
			else
				tex->destroy_handle = subidx;
		}
	}
	tex->sound = *page->sound_name ? mng_FindLoadSound(page->sound_name, cf) : -1;
	if (tex->r > 0 || tex->g > 0 || tex->b > 0)
		tex->flags |= TF_LIGHT;
	else
		tex->flags &= ~TF_LIGHT;
	if (tex->flags & TF_PROCEDURAL) {
		if (tex->procedural)
			FreeProceduralForTexture(idx);
		if (!page->num_proc_elms) {
			tex->flags &= ~TF_PROCEDURAL;
			return 1;
		}
		AllocateProceduralForTexture(idx);
		AllocateStaticProceduralsForTexture(idx, page->num_proc_elms);
		tex->procedural->num_static_elements = page->num_proc_elms;
		tex->procedural->heat = page->heat;
		tex->procedural->light = page->light;
		tex->procedural->thickness = page->thickness;
		tex->procedural->evaluation_time = page->eval_time;
		tex->procedural->osc_time = page->osc_time;
		tex->procedural->osc_value = page->osc_value;
		page->palette[255] = page->palette[254];
		memcpy(tex->procedural->palette, page->palette, sizeof(ushort) * 255);
		static_proc_element *elms = tex->procedural->static_proc_elements;
		for (int i = 0; i < page->num_proc_elms; i++) {
			elms[i].type = page->type[i];
			elms[i].frequency = page->frequency[i];
			elms[i].speed = page->speed[i];
			elms[i].size = page->size[i];
			elms[i].x1 = page->x1[i];
			elms[i].y1 = page->y1[i];
			elms[i].x2 = page->x2[i];
			elms[i].y2 = page->y2[i];
		}
	}
	return 1;
}

int mng_AddTexture(texture_page *page, CFILE *cf) {
	int idx = AllocTexture();
	if (idx < 0)
		return -1;
	if (!TexturePageToTexture(page, idx, cf))
		return -1;
	return idx;
}

int mng_ReadTexturePage(CFILE *cf, texture_page *page) {
	InitTexturePage(page);
	short version = cf_ReadShort(cf);
	cf_ReadString(page->item.name, sizeof(page->item.name), cf);
	cf_ReadString(page->image_filename, sizeof(page->image_filename), cf);
	cf_ReadString(page->destroy_name, sizeof(page->destroy_name), cf);
	page->item.r = cf_ReadFloat(cf);
	page->item.g = cf_ReadFloat(cf);
	page->item.b = cf_ReadFloat(cf);
	page->item.alpha = cf_ReadFloat(cf);
	page->item.speed = cf_ReadFloat(cf);
	page->item.slide_u = cf_ReadFloat(cf);
	page->item.slide_v = cf_ReadFloat(cf);
	page->item.reflectivity = cf_ReadFloat(cf);
	page->item.corona_type = cf_ReadByte(cf);
	page->item.damage = cf_ReadInt(cf);
	page->item.flags = cf_ReadInt(cf);
	if (page->item.flags & TF_PROCEDURAL) {
		for (int i = 0; i < 255; i++)
			page->palette[i] = cf_ReadShort(cf);
		page->heat = cf_ReadByte(cf);
		page->light = cf_ReadByte(cf);
		page->thickness = cf_ReadByte(cf);
		page->eval_time = cf_ReadFloat(cf);
		if (version >= 6) {
			page->osc_time = cf_ReadFloat(cf);
			page->osc_value = cf_ReadByte(cf);
		}
		page->num_proc_elms = cf_ReadShort(cf);
		for (int i = 0; i < page->num_proc_elms; i++) {
			page->type[i] = cf_ReadByte(cf);
			page->frequency[i] = cf_ReadByte(cf);
			page->speed[i] = cf_ReadByte(cf);
			page->size[i] = cf_ReadByte(cf);
			page->x1[i] = cf_ReadByte(cf);
			page->y1[i] = cf_ReadByte(cf);
			page->x2[i] = cf_ReadByte(cf);
			page->y2[i] = cf_ReadByte(cf);
		}
	}
	if ((page->item.flags & TF_PROCEDURAL) && !page->num_proc_elms)
		page->item.flags &= ~TF_PROCEDURAL;
	if (strnicmp(page->destroy_name, "INVALID", 7) == 0)
		strcpy(page->destroy_name, "");
	if (version < 5) {
		page->item.sound_volume = 1.0f;
		page->item.used = 1;
		return 1;
	}
	if (version >= 7) {
		cf_ReadString(page->sound_name, sizeof(page->sound_name), cf);
		page->item.sound_volume = cf_ReadFloat(cf);
		page->item.used = 1;
		return 1;
	}
	cf_ReadInt(cf);
	page->item.sound = -1;
	strcpy(page->sound_name, "");
	page->item.sound_volume = cf_ReadFloat(cf);
	page->item.used = 1;
	return 1;
}

void mng_PushAddonPage(int pagetype, char *name, int overlay) {}

char *cur_table_file_name;
int cur_table_num;

int mng_FindReadTexturePage(char *name, texture_page *page, int pos) {
	int found = 0;
	CFILE *cf;

	if (cur_table_file_name)
		cf = cfopen(cur_table_file_name, "rb");
	else
		cf = NULL;
	if (!cf)
		return 0;
	if (pos)
		cfseek(cf, pos, SEEK_SET);
	while (!cfeof(cf)) {
		ubyte type = cf_ReadByte(cf);
		int len = cf_ReadInt(cf);
		if (type != PAGETYPE_TEXTURE) {
			cfseek(cf, len - 4, SEEK_CUR);
			continue;
		}
		mng_ReadTexturePage(cf, page);
		if (stricmp(name, page->item.name) == 0) {
			found = 1;
			break;
		}
	}
	cfclose(cf);
	return found;
}

int mng_FindLoadTexture(char *name, CFILE *cf) {
	int idx;
	texture_page *page;
	if ((idx = FindTextureName(name)) != -1)
		return idx;
	page = (texture_page *)malloc(sizeof(*page));
	if (!mng_FindReadTexturePage(name, page, cf ? cf->position : 0))
		return -1;
	idx = mng_AddTexture(page, cf);
	if (cur_table_num != -1)
		mng_PushAddonPage(PAGETYPE_TEXTURE, page->item.name, 0);
	return idx;
}

int mng_FindLoadSound(char *name, CFILE *cf) {
	return -1;
	#if 0
	int idx;
	texture_page *page;
	if ((idx = FindSoundName(name)) != -1)
		return idx;
	page = malloc(sizeof(*page));
	if (!mng_FindReadSoundPage(name, page, cf ? cf->position : 0))
		return -1;
	idx = mng_AddSound(page, cf);
	if (cur_table_num != -1)
		mng_PushAddonPage(PAGETYPE_SOUND, page->item.name, 0);
	return idx;
	#endif
}

void mng_ReadStoreTexture(CFILE *cf, bool replace) {
	texture_page page;
	memset(&page, 0, sizeof(page));
	if (!mng_ReadTexturePage(cf, &page))
		return;
	int idx = FindTextureName(page.item.name);
	if (idx == -1)
		mng_AddTexture(&page, cf);
	else if (replace) {
		mng_FreePagetypePrimitives(PAGETYPE_TEXTURE, page.item.name, 0);
		TexturePageToTexture(&page, idx, NULL);
	}
}

void WeaponPageInit(weapon_page *page) {
	memset(page, 0, sizeof(*page));
	strcpy(page->hud_image_name, "");
	strcpy(page->fire_image_model_name, "");
	strcpy(page->explode_image_name, "");
	strcpy(page->spawn_weapon_name, "");
	strcpy(page->alt_spawn_weapon_name, "");
	strcpy(page->robot_spawn_name, "");
	strcpy(page->smoke_image_name, "");
	strcpy(page->scorch_image_name, "");
	strcpy(page->icon_image_name, "");
	strcpy(page->particle_image_name, "");
	(page->item).alternate_chance = '\0';
	(page->item).alpha = 1.0f;
	(page->item).explode_time = 1.0f;
	(page->item).explode_size = 1.0f;
	(page->item).particle_count = 0;
	(page->item).scorch_size = 1.0f;
	(page->item).terrain_damage_size = 0.0f;
	(page->item).terrain_damage_depth = 0;
	(page->item).homing_fov = 0.4f;
	(page->item).custom_size = 0.0f;
	(page->item).recoil_force = 0.0f;
	(page->item).phys_info.hit_die_dot = 1.0f;
	for (int i = 0; i < 7; i++)
		strcpy(page->sound_names[i], "");
}

void mng_ReadPhysicsChunk(physics_info *phys_info,CFILE *cf) {
	phys_info->mass = cf_ReadFloat(cf);
	phys_info->drag = cf_ReadFloat(cf);
	phys_info->full_thrust = cf_ReadFloat(cf);
	phys_info->flags = cf_ReadInt(cf);
	phys_info->rotdrag = cf_ReadFloat(cf);
	phys_info->full_rotthrust = cf_ReadFloat(cf);
	phys_info->num_bounces = cf_ReadInt(cf);
	phys_info->velocity.z = cf_ReadFloat(cf);
	phys_info->rotvel.x = cf_ReadFloat(cf);
	phys_info->rotvel.y = cf_ReadFloat(cf);
	phys_info->rotvel.z = cf_ReadFloat(cf);
	phys_info->wiggle_amplitude = cf_ReadFloat(cf);
	phys_info->wiggles_per_sec = cf_ReadFloat(cf);
	phys_info->coeff_restitution = cf_ReadFloat(cf);
	phys_info->hit_die_dot = cf_ReadFloat(cf);
	phys_info->max_turnroll_rate = cf_ReadFloat(cf);
	phys_info->turnroll_ratio = cf_ReadFloat(cf);
}

void mng_ReadLightInfoChunk(light_info *light,CFILE *cf) {
  light->light_distance = cf_ReadFloat(cf);
  light->red_light1 = cf_ReadFloat(cf);
  light->green_light1 = cf_ReadFloat(cf);
  light->blue_light1 = cf_ReadFloat(cf);
  light->time_interval = cf_ReadFloat(cf);
  light->flicker_distance = cf_ReadFloat(cf);
  light->directional_dot = cf_ReadFloat(cf);
  light->red_light2 = cf_ReadFloat(cf);
  light->green_light2 = cf_ReadFloat(cf);
  light->blue_light2 = cf_ReadFloat(cf);
  light->flags = cf_ReadInt(cf);
  light->timebits = cf_ReadInt(cf);
  light->angle = cf_ReadByte(cf);
  light->lighting_render_type = cf_ReadByte(cf);
}

int WeaponPageToWeapon(weapon_page *page, int idx, CFILE *cf) {
	weapon *w = &Weapons[idx];
	int handle;
	char *name;
	*w = page->item;
	handle = w->flags & WF_HUD_ANIMATED ?
		AllocLoadVClip(page->hud_image_name, 0, 0, 0, 0) : bm_AllocLoadFileBitmap(page->hud_image_name, 0, 0);
	if (handle < 0) {
		w->hud_image_handle = -1;
		return 0;
	}
	w->hud_image_handle = handle;

	handle = w->flags & WF_IMAGE_BITMAP ? bm_AllocLoadFileBitmap(page->fire_image_model_name, 0, 0) :
		w->flags & WF_IMAGE_VCLIP ?	AllocLoadVClip(page->fire_image_model_name, 0, 0, 0, 0) :
		LoadPolyModel(page->fire_image_model_name, 1);
	if (handle < 0) {
		w->fire_image_handle = -1;
		return 0;
	}
	w->fire_image_handle = handle;

	w->explode_image_handle = stricmp(name = page->explode_image_name, "INVALID NAME") == 0 || !*name ||
		(handle = mng_FindLoadTexture(name, cf)) < 0 ? -1 : handle;
	w->particle_handle = stricmp(name = page->particle_image_name, "INVALID NAME") == 0 || !*name ||
		(handle = mng_FindLoadTexture(name, cf)) < 0 ? -1 : handle;
	w->spawn_handle = stricmp(name = page->spawn_weapon_name, "INVALID NAME") == 0 || !*name ||
		(handle = mng_FindLoadWeapon(name, cf)) < 0 ? -1 : handle;
	w->alternate_spawn_handle = stricmp(name = page->alt_spawn_weapon_name, "INVALID NAME") == 0 || !*name ||
		(handle = mng_FindLoadWeapon(name, cf)) < 0 ? -1 : handle;
	w->robot_spawn_handle = stricmp(name = page->robot_spawn_name, "INVALID NAME") == 0 || !*name ||
		(handle = mng_FindLoadGeneric(name, cf)) < 0 ? -1 : handle;
	w->smoke_handle = stricmp(name = page->smoke_image_name, "INVALID NAME") == 0 || !*name ||
		(handle = mng_FindLoadTexture(name, cf)) < 0 ? -1 : handle;
	w->scorch_handle = stricmp(name = page->scorch_image_name, "INVALID NAME") == 0 || !*name ||
		(handle = mng_FindLoadTexture(name, cf)) < 0 ? -1 : handle;
	w->icon_handle = stricmp(name = page->icon_image_name, "INVALID NAME") == 0 || !*name ||
		(handle = mng_FindLoadTexture(name, cf)) < 0 ? -1 : handle;
	for (int i = 0; i < 7; i++)
		w->sounds[i] = stricmp(name = page->sound_names[i], "INVALID NAME") == 0 || !*name ||
			(handle = mng_FindLoadSound(name, cf)) < 0 ? 0 : handle;
	return 1;
}

int mng_AddWeapon(weapon_page *page, CFILE *cf) {
	int idx = AllocWeapon();
	if (idx < 0)
		return -1;
	if (!WeaponPageToWeapon(page, idx, cf))
		return -1;
	return idx;
}

int mng_ReadWeaponPage(CFILE *cf, weapon_page *page) {
	WeaponPageInit(page);
	short version = cf_ReadShort(cf);
	cf_ReadString(page->item.name,PAGENAME_LEN,cf);
	cf_ReadString(page->hud_image_name,PAGENAME_LEN,cf);
	cf_ReadString(page->fire_image_model_name,PAGENAME_LEN,cf);
	cf_ReadString(page->particle_image_name,PAGENAME_LEN,cf);
	page->item.particle_count = cf_ReadByte(cf);
	page->item.particle_life = cf_ReadFloat(cf);
	page->item.particle_size = cf_ReadFloat(cf);
	page->item.flags = cf_ReadInt(cf);
	cf_ReadString(page->spawn_weapon_name,PAGENAME_LEN,cf);
	page->item.spawn_count = cf_ReadByte(cf);
	cf_ReadString(page->robot_spawn_name,PAGENAME_LEN,cf);
	cf_ReadString(page->alt_spawn_weapon_name,PAGENAME_LEN,cf);
	page->item.alternate_chance = cf_ReadByte(cf);
	page->item.gravity_time = cf_ReadFloat(cf);
	page->item.gravity_size = cf_ReadFloat(cf);
	page->item.homing_fov = cf_ReadFloat(cf);
	page->item.custom_size = cf_ReadFloat(cf);
	page->item.size = cf_ReadFloat(cf);
	page->item.thrust_time = cf_ReadFloat(cf);
	mng_ReadPhysicsChunk(&page->item.phys_info,cf);
	page->item.terrain_damage_size = cf_ReadFloat(cf);
	page->item.terrain_damage_depth = cf_ReadByte(cf);
	page->item.alpha = cf_ReadFloat(cf);
	cf_ReadString(page->explode_image_name,PAGENAME_LEN,cf);
	page->item.explode_time = cf_ReadFloat(cf);
	page->item.explode_size = cf_ReadFloat(cf);
	page->item.player_damage = cf_ReadFloat(cf);
	if (version < 7)
		page->item.generic_damage = page->item.player_damage;
	else
		page->item.generic_damage = cf_ReadFloat(cf);
	page->item.impact_size = cf_ReadFloat(cf);
	page->item.impact_time = cf_ReadFloat(cf);
	page->item.impact_player_damage = cf_ReadFloat(cf);
	if (version < 7)
		page->item.impact_generic_damage = page->item.impact_player_damage;
	else
		page->item.impact_generic_damage = cf_ReadFloat(cf);
	page->item.impact_force = cf_ReadFloat(cf);
	page->item.life_time = cf_ReadFloat(cf);
	mng_ReadLightInfoChunk(&page->item.lighting_info,cf);
	if (version < 8) {
		page->item.recoil_force = 0.0f;
	}
	else {
		page->item.recoil_force = cf_ReadFloat(cf);
	}
	for (int i = 0; i < 7; i++)
		cf_ReadString(page->sound_names[i],PAGENAME_LEN,cf);
	cf_ReadString(page->smoke_image_name,PAGENAME_LEN,cf);
	cf_ReadString(page->scorch_image_name,PAGENAME_LEN,cf);
	page->item.scorch_size = cf_ReadFloat(cf);
	cf_ReadString(page->icon_image_name,PAGENAME_LEN,cf);
	page->item.used = 1;
	if (stricmp(page->item.name, "EMDBlob") == 0)
		page->item.life_time = 1.7f;
	return 1;
}

int FindWeaponName(const char *name) {
	int seen_used = 0;
	for (int i = 0; i < MAX_WEAPONS; i++) {
		if (!Weapons[i].used)
			continue;
		if (stricmp(Weapons[i].name, name) == 0)
			return i;
		if (++seen_used == Num_weapons)
			break;
	}
	return -1;
}

void mng_ReadStoreWeapon(CFILE *cf, bool replace) {
	weapon_page page;
	memset(&page, 0, sizeof(page));
	if (!mng_ReadWeaponPage(cf, &page))
		return;
	int idx = FindWeaponName(page.item.name);
	if (idx == -1)
		mng_AddWeapon(&page, cf);
	else if (replace) {
		mng_FreePagetypePrimitives(PAGETYPE_WEAPON, page.item.name, 0);
		WeaponPageToWeapon(&page, idx, NULL);
	}
}

int mng_FindReadWeaponPage(char *name, weapon_page *page, int pos) {
	int found = 0;
	CFILE *cf;

	if (cur_table_file_name)
		cf = cfopen(cur_table_file_name, "rb");
	else
		cf = NULL;
	if (!cf)
		return 0;
	if (pos)
		cfseek(cf, pos, SEEK_SET);
	while (!cfeof(cf)) {
		ubyte type = cf_ReadByte(cf);
		int len = cf_ReadInt(cf);
		if (type != PAGETYPE_WEAPON) {
			cfseek(cf, len - 4, SEEK_CUR);
			continue;
		}
		mng_ReadWeaponPage(cf, page);
		if (stricmp(name, page->item.name) == 0) {
			found = 1;
			break;
		}
	}
	cfclose(cf);
	return found;
}

int mng_FindLoadWeapon(char *name, CFILE *cf) {
	int idx;
	weapon_page *page;
	if ((idx = FindWeaponName(name)) != -1)
		return idx;
	page = (weapon_page *)malloc(sizeof(*page));
	if (!mng_FindReadWeaponPage(name, page, cf ? cf->position : 0))
		return -1;
	idx = mng_AddWeapon(page, cf);
	if (cur_table_num != -1)
		mng_PushAddonPage(PAGETYPE_WEAPON, page->item.name, 0);
	return idx;
}

int DoorPageToDoor(door_page *page, int idx, CFILE *cf) {
	Doors[idx] = page->item;
	strcpy(Doors[idx].name, page->item.name);
	int model = LoadPolyModel(page->model_name, 1);
	Doors[idx].open_sound = mng_FindLoadSound(page->open_sound, NULL);
	Doors[idx].close_sound = mng_FindLoadSound(page->close_sound, NULL);
	if (model < 0) {
		Doors[idx].model_handle = -1;
		return 0;
	}
	Doors[idx].model_handle = model;
	return 1;
}

int mng_AddDoor(door_page *page, CFILE *cf) {
	int idx = AllocDoor();
	if (idx < 0)
		return -1;
	if (!DoorPageToDoor(page, idx, cf))
		return -1;
	return idx;
}


int mng_ReadDoorPage(CFILE *cf, door_page *page) {
  short version = cf_ReadShort(cf);
  cf_ReadString(page->item.name,PAGENAME_LEN,cf);
  cf_ReadString(page->model_name,PAGENAME_LEN,cf);
  page->item.total_open_time = cf_ReadFloat(cf);
  page->item.total_close_time = cf_ReadFloat(cf);
  page->item.total_time_open = cf_ReadFloat(cf);
  page->item.flags = cf_ReadByte(cf);
  if (version < 3)
    page->item.hit_points = 0;
  else
    page->item.hit_points = cf_ReadShort(cf);
  cf_ReadString(page->open_sound,PAGENAME_LEN,cf);
  cf_ReadString(page->close_sound,PAGENAME_LEN,cf);
  if (version >= 2) {
    cf_ReadString(page->item.module_name,MAX_MODULENAME_LEN,cf);
    page->item.used = 1;
    return 1;
  }
  page->item.module_name[0] = '\0';
  page->item.used = 1;
  return 1;
}

void mng_ReadStoreDoor(CFILE *cf, bool replace) {
	door_page page;
	memset(&page, 0, sizeof(page));
	if (!mng_ReadDoorPage(cf, &page))
		return;
	int idx = FindDoorName(page.item.name);
	if (idx == -1)
		mng_AddDoor(&page, cf);
	else if (replace) {
		mng_FreePagetypePrimitives(PAGETYPE_DOOR, page.item.name, 0);
		DoorPageToDoor(&page, idx, NULL);
	}
}

int ShipPageToShip(ship_page *page, int idx, CFILE *cf) {
	ship *ship = &Ships[idx];
	*ship = page->item;
	strcpy(ship->name, page->item.name);

	int handle = LoadPolyModel(page->model_name, 1);
	if (handle < 0) {
		ship->model_handle = -1;
		return 0;
	}
	ship->model_handle = handle;

	if (stricmp(page->dying_model_name, "INVALID NAME") == 0 || !*page->dying_model_name)
		ship->dying_model_handle = -1;
	else {
		handle = LoadPolyModel(page->dying_model_name, 1);
		if (handle < 0) {
			ship->dying_model_handle = -1;
			return 0;
		}
		ship->dying_model_handle = handle;
	}

	if (stricmp(page->med_model_name, "INVALID NAME") == 0 || !*page->med_model_name)
		ship->med_render_handle = -1;
	else {
		handle = LoadPolyModel(page->med_model_name, 1);
		if (handle < 0) {
			ship->med_render_handle = -1;
			return 0;
		}
		ship->med_render_handle = handle;
	}

	if (stricmp(page->lo_model_name, "INVALID NAME") == 0 || !*page->lo_model_name)
		ship->lo_render_handle = -1;
	else {
		handle = LoadPolyModel(page->lo_model_name, 1);
		if (handle < 0) {
			ship->lo_render_handle = -1;
			return 0;
		}
		ship->lo_render_handle = handle;
	}

	for (int i = 0; i < MAX_PLAYER_WEAPONS; i++)
		for (int j = 0; j < MAX_WB_GUNPOINTS; j++) {
			char *name = page->wb_weapon_names[i][j];
			ship->static_wb[i].gp_weapon_index[j] =
				!*name || (handle = mng_FindLoadWeapon(name, cf)) < 0 ? 0 : handle;
		}

	for (int i = 0; i < MAX_PLAYER_WEAPONS; i++)
		for (int j = 0; j < MAX_WB_GUNPOINTS; j++) {
			char *name = page->wb_sound_names[i][j];
			ship->static_wb[i].fm_fire_sound_index[j] =
				!*name || (handle = mng_FindLoadSound(name, cf)) < 0 ? 0 : handle;
		}

	for (int i = 0; i < MAX_PLAYER_WEAPONS; i++) {
		char *name = page->firing_sound_names[i];
		ship->firing_sound[i] =
			!*name ? -1 : (handle = mng_FindLoadSound(name, cf)) < 0 ? 0 : handle;
		name = page->firing_release_sound_names[i];
		ship->firing_release_sound[i] =
			!*name ? -1 : (handle = mng_FindLoadSound(name, cf)) < 0 ? 0 : handle;
		name = page->spew_powerup_names[i];
		ship->spew_powerup[i] =
			!*name || ((handle = mng_FindLoadSound(name, cf)) >= 0 && Object_info[handle].type != OBJ_POWERUP) ? -1 : handle;
	}

	return 1;
}

int mng_AddShip(ship_page *page, CFILE *cf) {
	int idx = AllocShip();
	if (idx < 0)
		return -1;
	if (!ShipPageToShip(page, idx, cf))
		return -1;
	return idx;
}

int mng_ReadShipPage(CFILE *cf, ship_page *page) {
	memset(page, 0, sizeof(*page));
	short version = cf_ReadShort(cf);
	cf_ReadString(page->item.name,PAGENAME_LEN,cf);
	cf_ReadString(page->item.cockpit_name,PAGENAME_LEN,cf);
	cf_ReadString(page->item.hud_config_name,PAGENAME_LEN,cf);
	cf_ReadString(page->model_name,PAGENAME_LEN,cf);
	cf_ReadString(page->dying_model_name,PAGENAME_LEN,cf);
	cf_ReadString(page->med_model_name,PAGENAME_LEN,cf);
	cf_ReadString(page->lo_model_name,PAGENAME_LEN,cf);
	page->item.med_lod_distance = cf_ReadFloat(cf);
	page->item.lo_lod_distance = cf_ReadFloat(cf);
	mng_ReadPhysicsChunk(&page->item.phys_info,cf);
	page->item.size = cf_ReadFloat(cf);
	page->item.armor_scalar = cf_ReadFloat(cf);
	page->item.flags = cf_ReadInt(cf);

	for (int i = 0; i < MAX_PLAYER_WEAPONS; i++) {
		page->item.fire_flags[i] = cf_ReadByte(cf);
		cf_ReadString(page->firing_sound_names[i],PAGENAME_LEN,cf);
		cf_ReadString(page->firing_release_sound_names[i],PAGENAME_LEN,cf);
		cf_ReadString(page->spew_powerup_names[i],PAGENAME_LEN,cf);
		page->item.max_ammo[i] = cf_ReadInt(cf);
		mng_ReadWeaponBatteryChunk(&page->item.static_wb[i],cf,version < 6 ? 1 : 2);
		for (int j = 0; j < MAX_WB_GUNPOINTS; j++)
			cf_ReadString(page->wb_sound_names[i][j],PAGENAME_LEN,cf);
		for (int j = 0; j < MAX_WB_GUNPOINTS; j++)
			cf_ReadString(page->wb_weapon_names[i][j],PAGENAME_LEN,cf);
	}
	page->item.used = 1;

	if (stricmp(page->item.name, "Pyro-GL") == 0)
		for (int i = 0; i < MAX_WB_FIRING_MASKS; i++)
			page->item.static_wb[4].gp_fire_wait[i] = 0.66f;
	else if (stricmp(page->item.name, "Phoenix") == 0)
		for (int i = 0; i < MAX_WB_FIRING_MASKS; i++)
			page->item.static_wb[4].gp_fire_wait[i] = 0.792f;
	else if (stricmp(page->item.name, "Magnum-AHT") == 0)
		for (int i = 0; i < MAX_WB_FIRING_MASKS; i++)
			page->item.static_wb[4].gp_fire_wait[i] = 1.122f;
	return 1;
}

void mng_ReadStoreShip(CFILE *cf, bool replace) {
	ship_page page;
	memset(&page, 0, sizeof(page));
	if (!mng_ReadShipPage(cf, &page))
		return;
	int idx = FindShipName(page.item.name);
	if (idx == -1)
		mng_AddShip(&page, cf);
	else if (replace) {
		mng_FreePagetypePrimitives(PAGETYPE_SHIP, page.item.name, 0);
		ShipPageToShip(&page, idx, NULL);
	}
}

#if 0
void mng_AddSound(sound_page *page, CFILE *cf) {}
void SoundPageToSound(sound_page *page, int idx, CFILE *cf) {}

void mng_ReadStoreSound(CFILE *cf, bool replace) {
	sound_page page;
	memset(page, 0, sizeof(page));
	if (!mng_ReadSoundPage(cf, &page))
		return;
	int idx = FindSoundName(page.item.name);
	if (idx == -1)
		mng_AddSound(&page, cf);
	else if (replace) {
		mng_FreePagetypePrimitives(PAGETYPE_SOUND, page.item.name, 0);
		SoundPageToSound(&page, idx, NULL);
	}
}
#endif

void mng_AddMegacell(megacell_page *page, CFILE *cf) {}
void MegacellPageToMegacell(megacell_page *page, int idx, CFILE *cf) {}

int mng_ReadMegacellPage(CFILE *cf, megacell_page *page) {
	memset(page, 0, sizeof(*page));
	short version = cf_ReadShort(cf);
	cf_ReadString(page->name, sizeof(page->name), cf);
	for (int i = 0; i < 64; i++)
		cf_ReadString(page->name2[i], sizeof(page->name2[0]), cf);
	page->b1 = cf_ReadByte(cf);
	page->b2 = cf_ReadByte(cf);
	page->used = 1;
	return 1;
}

int FindMegacellName(char *name) { return -1; }

void mng_ReadStoreMegacell(CFILE *cf, bool replace) {
	megacell_page page;
	memset(&page, 0, sizeof(page));
	if (!mng_ReadMegacellPage(cf, &page))
		return;
	int idx = FindMegacellName(page.name);
	if (idx == -1)
		mng_AddMegacell(&page, cf);
	else if (replace) {
		mng_FreePagetypePrimitives(PAGETYPE_MEGACELL, page.name, 0);
		MegacellPageToMegacell(&page, idx, NULL);
	}
}

int FindGamefileName(char *name) { return -1; }

void mng_AddGamefile(gamefile_page *page, CFILE *cf) {}
void GamefilePageToGamefile(gamefile_page *page, int idx, CFILE *cf) {}

int mng_ReadGamefilePage(CFILE *cf, gamefile_page *page) {
	memset(page, 0, sizeof(*page));
	short version = cf_ReadShort(cf);
	cf_ReadString(page->name, sizeof(page->name), cf);
	cf_ReadString(page->name2, sizeof(page->name2), cf);
	page->used = 1;
	return 1;
}

void mng_ReadStoreGamefile(CFILE *cf, bool replace) {
	gamefile_page page;
	memset(&page, 0, sizeof(page));
	if (!mng_ReadGamefilePage(cf, &page))
		return;
	int idx = FindGamefileName(page.name);
	if (idx == -1)
		mng_AddGamefile(&page, cf);
	else if (replace) {
		mng_FreePagetypePrimitives(PAGETYPE_GAMEFILE, page.name, 0);
		GamefilePageToGamefile(&page, idx, NULL);
	}
}

int GenericPageToGeneric(generic_page *page, int idx, CFILE *cf) {
	object_info *objinfo = &Object_info[idx];
	memcpy(objinfo, &page->item, sizeof(*objinfo) - 3 * sizeof(void *)); // skip ai_info, static_wb, anim ptrs
	strcpy(objinfo->name, page->item.name);
	if (objinfo->anim)
		memcpy(objinfo->anim, page->anim, sizeof(anim_elem) * NUM_MOVEMENT_CLASSES);
	if (objinfo->static_wb)
		memcpy(objinfo->static_wb, page->static_wb, sizeof(otype_wb_info) * MAX_WBS_PER_OBJ);
	if (objinfo->ai_info)
		*objinfo->ai_info = page->ai_info;
	objinfo->multi_allowed = true;
	page->item.description = NULL;
	strcpy(objinfo->icon_name, page->item.icon_name);
	int handle = LoadPolyModel(page->model_name, 1);
	if (handle < 0) {
		objinfo->render_handle = -1;
		return 0;
	}
	objinfo->render_handle = handle;

	if (stricmp(page->med_model_name, "INVALID NAME") == 0 || !*page->med_model_name)
		objinfo->med_render_handle = -1;
	else {
		handle = LoadPolyModel(page->med_model_name, 1);
		if (handle < 0) {
			objinfo->med_render_handle = -1;
			return 0;
		}
		objinfo->med_render_handle = handle;
	}

	if (stricmp(page->lo_model_name, "INVALID NAME") == 0 || !*page->lo_model_name)
		objinfo->lo_render_handle = -1;
	else {
		handle = LoadPolyModel(page->lo_model_name, 1);
		if (handle < 0) {
			objinfo->lo_render_handle = -1;
			return 0;
		}
		objinfo->lo_render_handle = handle;
	}

	for (int i = 0; i < MAX_OBJ_SOUNDS; i++) {
		char *name = page->sound_names[i];
		objinfo->sounds[i] = stricmp(name, "INVALID NAME") == 0 || !*name || (handle = mng_FindLoadSound(name, NULL)) < 0 ? 0 : handle;
	}

	for (int i = 0; i < MAX_DSPEW_TYPES; i++)
		if (!*page->dspew_generic_names[i]) {
			objinfo->dspew[i] = -1;
			objinfo->dspew_number[i] = 0;
			objinfo->dspew_percent[i] = 0.0f;
		} else
			objinfo->dspew[i] = (handle = mng_FindLoadGeneric(page->dspew_generic_names[i], cf)) < 0 ? 0 : handle;

	if (objinfo->ai_info)
		for (int i = 0; i < MAX_AI_SOUNDS; i++) {
			char *name = page->ai_sound_names[i];
			objinfo->ai_info->sound[i] = stricmp(name, "INVALID NAME") == 0 || !*name || (handle = mng_FindLoadSound(name, NULL)) < 0 ? 0 : handle;
		}
	if (objinfo->static_wb) {
		for (int i = 0; i < MAX_WBS_PER_OBJ; i++)
			for (int j = 0; j < MAX_WB_GUNPOINTS; j++) {
				char *name = page->wb_weapon_names[i][j];
				objinfo->static_wb[i].gp_weapon_index[j] = !*name || (handle = mng_FindLoadWeapon(name, NULL)) < 0 ? 0 : handle;
			}
		for (int i = 0; i < MAX_WBS_PER_OBJ; i++)
			for (int j = 0; j < MAX_WB_FIRING_MASKS; j++) {
				char *name = page->wb_sound_names[i][j];
				objinfo->static_wb[i].fm_fire_sound_index[j] = !*name || (handle = mng_FindLoadSound(name, NULL)) < 0 ? 0 : handle;
			}
	}

	if (objinfo->anim)
		for (int i = 0; i < NUM_MOVEMENT_CLASSES; i++)
			for (int j = 0; j < NUM_ANIMS_PER_CLASS; j++) {
				char *name = page->anim_sound_names[i][j];
				objinfo->anim[i].elem[j].anim_sound_index = stricmp(name, "INVALID NAME") == 0 || !*name || (handle = mng_FindLoadSound(name, NULL)) < 0 ? 0 : handle;
			}
	return 1;
}

bool Running_editor;

int mng_AddGeneric(generic_page *page, CFILE *cf) {
	bool f_anim = false, f_weapons = false, f_ai = false;
	if (page->item.flags & OIF_CONTROL_AI) {
		f_anim = f_weapons = f_ai = true;
	} else {
		int type = page->item.type;
		if (type == OBJ_ROBOT || type == OBJ_BUILDING)
			f_anim = true;
		else {
			if (type == OBJ_POWERUP || type == OBJ_CLUTTER)
				for (int i = 0; i < NUM_MOVEMENT_CLASSES; i++)
					for (int j = 0; j < NUM_ANIMS_PER_CLASS; j++)
						if (page->anim[i].elem[j].to || page->anim[i].elem[j].from) {
							f_anim = true;
							break;
						}
			for (int i = 0; i < MAX_WBS_PER_OBJ; i++)
				for (int j = 0; j < page->static_wb[i].num_masks; j++)
					if  (page->static_wb[i].gp_fire_masks[j]) {
						f_weapons = true;
						break;
					}
		}
	}
	if (Running_editor)
		f_anim = f_weapons = f_ai = true;	
	int idx = AllocObjectID(page->item.type, f_anim, f_weapons, f_ai);
	if (idx < 0)
		return -1;
	if (!GenericPageToGeneric(page, idx, cf))
		return -1;
	return idx;
}

void mng_GenericSetAmmo(object_info *objinfo)
{
  objinfo->ammo_count = 0;
  if (stricmp(objinfo->name,"Vauss") == 0)
    objinfo->ammo_count = 5000;
  if (stricmp(objinfo->name,"Napalm") == 0)
    objinfo->ammo_count = 500;
  if (stricmp(objinfo->name,"MassDriver") == 0)
    objinfo->ammo_count = 20;
  if (stricmp(objinfo->name,"Frag") == 0)
    objinfo->ammo_count = 1;
  if (stricmp(objinfo->name,"ImpactMortar") == 0)
    objinfo->ammo_count = 1;
  if (stricmp(objinfo->name,"NapalmRocket") == 0)
    objinfo->ammo_count = 1;
  if (stricmp(objinfo->name,"Cyclone") == 0)
    objinfo->ammo_count = 1;
  if (stricmp(objinfo->name,"BlackShark") == 0)
    objinfo->ammo_count = 1;
  if (stricmp(objinfo->name,"Concussion") == 0)
    objinfo->ammo_count = 1;
  if (stricmp(objinfo->name,"Homing") == 0)
    objinfo->ammo_count = 1;
  if (stricmp(objinfo->name,"Smart") == 0)
    objinfo->ammo_count = 1;
  if (stricmp(objinfo->name,"Mega") == 0)
    objinfo->ammo_count = 1;
  if (stricmp(objinfo->name,"Guided") == 0)
    objinfo->ammo_count = 1;
  if (stricmp(objinfo->name,"4PackHoming") == 0)
    objinfo->ammo_count = 4;
  if (stricmp(objinfo->name,"4PackConc") == 0)
    objinfo->ammo_count = 4;
  if (stricmp(objinfo->name,"4PackFrag") == 0)
    objinfo->ammo_count = 4;
  if (stricmp(objinfo->name,"4PackGuided") == 0)
    objinfo->ammo_count = 4;
  if (stricmp(objinfo->name,"Vauss clip") == 0)
    objinfo->ammo_count = 1250;
  if (stricmp(objinfo->name,"MassDriverAmmo") == 0)
    objinfo->ammo_count = 5;
  if (stricmp(objinfo->name,"NapalmTank") == 0)
    objinfo->ammo_count = 100;
  return;
}

void mng_InitGenericPage(generic_page *page) {
	memset(page, 0, sizeof(*page));
	strcpy(page->model_name, "");
	strcpy(page->med_model_name, "");
	strcpy(page->lo_model_name, "");
	for (int i = 0; i < MAX_OBJ_SOUNDS; i++)
		strcpy(page->sound_names[i], "");
	for (int i = 0; i < MAX_AI_SOUNDS; i++)
		strcpy(page->ai_sound_names[i], "");
	for (int i = 0; i < MAX_DSPEW_TYPES; i++)
		strcpy(page->dspew_generic_names[i], "");
	page->item.description = NULL;
	page->item.icon_name[0] = 0;
	for (int i = 0; i < NUM_MOVEMENT_CLASSES; i++)
		for (int j = 0; j < NUM_ANIMS_PER_CLASS; j++)
			strcpy(page->anim_sound_names[i][j], "");
	page->ai_info.night_vision = 0.7f;
	page->ai_info.fog_vision = 0.7f;
	page->item.phys_info.hit_die_dot = 1.0f;
	page->item.respawn_scalar = 1.0f;
	page->ai_info.curiousity = 0.5f;
	page->ai_info.lead_accuracy = 1.0f;
	page->ai_info.agression = 0.5f;
	page->ai_info.hearing = 1.0f;
	page->ai_info.frustration = 0.5f;
	page->ai_info.roaming = 0.5f;
	page->item.med_lod_distance = 75.0f;
	page->item.lo_lod_distance = 120.0f;
	page->ai_info.lead_varience = 0.0f;
	page->ai_info.fire_spread = 0.0f;
	page->ai_info.fight_team = 0.15f;
	page->ai_info.fight_same = 0.8f;
	page->ai_info.life_preservation = 0.0f;
	page->item.module_name[0] = 0;

	for (int i = 0; i < MAX_DEATH_TYPES; i++) {
		page->item.death_types[i].flags = 0;
		page->item.death_types[i].delay_min = 0.0f;
		page->item.death_types[i].delay_max = 0.0f;
		page->item.death_probabilities[i] = 0;
	}
}

void mng_ReadWeaponBatteryChunk(otype_wb_info *static_wb,CFILE *cf,int version) {

	static_wb->energy_usage = cf_ReadFloat(cf);
	static_wb->ammo_usage = cf_ReadFloat(cf);
	for (int i = 0; i < MAX_WB_GUNPOINTS; i++)
		static_wb->gp_weapon_index[i] = cf_ReadShort(cf);

	for (int i = 0; i < MAX_WB_FIRING_MASKS; i++) {
		static_wb->gp_fire_masks[i] = cf_ReadByte(cf);
		static_wb->gp_fire_wait[i] = cf_ReadFloat(cf);
		static_wb->anim_time[i] = cf_ReadFloat(cf);
		static_wb->anim_start_frame[i] = cf_ReadFloat(cf);
		static_wb->anim_fire_frame[i] = cf_ReadFloat(cf);
		static_wb->anim_end_frame[i] = cf_ReadFloat(cf);
	}
	static_wb->num_masks = cf_ReadByte(cf);
	static_wb->aiming_gp_index = cf_ReadShort(cf);
	static_wb->aiming_flags = cf_ReadByte(cf);
	static_wb->aiming_3d_dot = cf_ReadFloat(cf);
	static_wb->aiming_3d_dist = cf_ReadFloat(cf);
	static_wb->aiming_XZ_dot = cf_ReadFloat(cf);
	static_wb->flags = version < 2 ? cf_ReadByte(cf) : cf_ReadShort(cf);
	static_wb->gp_quad_fire_mask = cf_ReadByte(cf);
}

int mng_ReadGenericPage(CFILE *cf, generic_page *page) {
	mng_InitGenericPage(page);

	short version = cf_ReadShort(cf);
	page->item.type = cf_ReadByte(cf);;
	cf_ReadString(page->item.name,PAGENAME_LEN,cf);
	cf_ReadString(page->model_name,PAGENAME_LEN,cf);
	cf_ReadString(page->med_model_name,PAGENAME_LEN,cf);
	cf_ReadString(page->lo_model_name,PAGENAME_LEN,cf);
	page->item.impact_size = cf_ReadFloat(cf);
	page->item.impact_time = cf_ReadFloat(cf);
	page->item.damage = cf_ReadFloat(cf);
	page->item.score = version < 24 ? cf_ReadByte(cf) : cf_ReadShort(cf);
	if (page->item.type == OBJ_POWERUP) {
		if (version < 25)
			mng_GenericSetAmmo(&page->item);
		else
			page->item.ammo_count = cf_ReadShort(cf);
	} else
		page->item.ammo_count = 0;
	char local_500[256];
	cf_ReadString(local_500,PAGENAME_LEN,cf);
	if (version < 18)
		page->item.module_name[0] = '\0';
	else
		cf_ReadString(page->item.module_name,MAX_MODULENAME_LEN,cf);
	if (version < 19)
		page->item.script_name_override[0] = '\0';
	else
		cf_ReadString(page->item.script_name_override,PAGENAME_LEN,cf);
	if (!cf_ReadByte(cf))
		page->item.description = NULL;
	else {
		char desc[1024];
		cf_ReadString(desc,sizeof(desc),cf);
		page->item.description = (char *)malloc(strlen(desc) + 1);
		strcpy(page->item.description, desc);
	}
	cf_ReadString(page->item.icon_name,PAGENAME_LEN,cf);
	page->item.med_lod_distance = cf_ReadFloat(cf);
	page->item.lo_lod_distance = cf_ReadFloat(cf);
	mng_ReadPhysicsChunk(&page->item.phys_info,cf);
	page->item.size = cf_ReadFloat(cf);
	mng_ReadLightInfoChunk(&page->item.lighting_info,cf);
	page->item.hit_points = cf_ReadInt(cf);
	page->item.flags = cf_ReadInt(cf);
	page->ai_info.flags = cf_ReadInt(cf);
	page->ai_info.ai_class = cf_ReadByte(cf);
	page->ai_info.ai_type = cf_ReadByte(cf);
	page->ai_info.movement_type = cf_ReadByte(cf);
	page->ai_info.movement_subtype = cf_ReadByte(cf);
	page->ai_info.fov = cf_ReadFloat(cf);
	page->ai_info.max_velocity = cf_ReadFloat(cf);
	page->ai_info.max_delta_velocity = cf_ReadFloat(cf);
	page->ai_info.max_turn_rate = cf_ReadFloat(cf);
	page->ai_info.notify_flags &= ~AI_NOTIFIES_ALWAYS_ON;
	page->ai_info.notify_flags = cf_ReadInt(cf) | AI_NOTIFIES_ALWAYS_ON;
	page->ai_info.max_delta_turn_rate = cf_ReadFloat(cf);
	page->ai_info.circle_distance = cf_ReadFloat(cf);
	page->ai_info.attack_vel_percent = cf_ReadFloat(cf);
	page->ai_info.dodge_percent = cf_ReadFloat(cf);
	page->ai_info.dodge_vel_percent = cf_ReadFloat(cf);
	page->ai_info.flee_vel_percent = cf_ReadFloat(cf);
	page->ai_info.melee_damage[0] = cf_ReadFloat(cf);
	page->ai_info.melee_damage[1] = cf_ReadFloat(cf);
	page->ai_info.melee_latency[0] = cf_ReadFloat(cf);
	page->ai_info.melee_latency[1] = cf_ReadFloat(cf);
	page->ai_info.curiousity = cf_ReadFloat(cf);
	page->ai_info.night_vision = cf_ReadFloat(cf);
	page->ai_info.fog_vision = cf_ReadFloat(cf);
	page->ai_info.lead_accuracy = cf_ReadFloat(cf);
	page->ai_info.lead_varience = cf_ReadFloat(cf);
	page->ai_info.fire_spread = cf_ReadFloat(cf);
	page->ai_info.fight_team = cf_ReadFloat(cf);
	page->ai_info.fight_same = cf_ReadFloat(cf);
	page->ai_info.agression = cf_ReadFloat(cf);
	page->ai_info.hearing = cf_ReadFloat(cf);
	page->ai_info.frustration = cf_ReadFloat(cf);
	page->ai_info.roaming = cf_ReadFloat(cf);
	page->ai_info.life_preservation = cf_ReadFloat(cf);
	if (version < 16) {
		if ((page->item.flags | OIF_USES_PHYSICS) && page->ai_info.max_velocity > 0) {
			page->ai_info.flags |= AIF_AUTO_AVOID_FRIENDS;
			page->ai_info.avoid_friends_distance = page->ai_info.circle_distance * 0.1f;
			if (page->ai_info.avoid_friends_distance > 4.0f)
				page->ai_info.avoid_friends_distance = 4.0f;
		} else
			page->ai_info.avoid_friends_distance = 4.0f;
	} else
		page->ai_info.avoid_friends_distance = cf_ReadFloat(cf);
	if (version < 17) {
		page->ai_info.biased_flight_importance = 0.5f;
		page->ai_info.biased_flight_min = 10.0f;
		page->ai_info.biased_flight_max = 50.0f;
	} else {
		page->ai_info.biased_flight_importance = cf_ReadFloat(cf);
		page->ai_info.biased_flight_min = cf_ReadFloat(cf);
		page->ai_info.biased_flight_max = cf_ReadFloat(cf);
	}
	for (int i = 0; i < MAX_DSPEW_TYPES; i++) {
		page->item.f_dspew = cf_ReadByte(cf);
		page->item.dspew_percent[i] = cf_ReadFloat(cf);
		page->item.dspew_number[i] = cf_ReadShort(cf);
		cf_ReadString(page->dspew_generic_names[i],PAGENAME_LEN,cf);
	}

	for (int i = 0; i < NUM_MOVEMENT_CLASSES; i++)
		for (int j = 0; j < NUM_ANIMS_PER_CLASS; j++) {
			if (version < 20) {
				page->anim[i].elem[j].from = cf_ReadByte(cf);
				page->anim[i].elem[j].to = cf_ReadByte(cf);
			} else {
				page->anim[i].elem[j].from = cf_ReadShort(cf);
				page->anim[i].elem[j].to = cf_ReadShort(cf);
			}
			page->anim[i].elem[j].spc = cf_ReadFloat(cf);
		}

	for (int i = 0; i < MAX_WBS_PER_OBJ; i++)
		mng_ReadWeaponBatteryChunk(&page->static_wb[i],cf,version < 15 ? 1 : 2);


	for (int i = 0; i < MAX_WBS_PER_OBJ; i++)
		for (int j = 0; j < MAX_WB_GUNPOINTS; j++)
			cf_ReadString(page->wb_weapon_names[i][j],PAGENAME_LEN,cf);

	for (int i = 0; i < MAX_OBJ_SOUNDS; i++)
		cf_ReadString(page->sound_names[i], PAGENAME_LEN,cf);

	if (version < 26) {
		char buf[PAGENAME_LEN + 1];
		cf_ReadString(buf,PAGENAME_LEN,cf);
	}

	for (int i = 0; i < MAX_AI_SOUNDS; i++)
		cf_ReadString(page->ai_sound_names[i], PAGENAME_LEN,cf);

	for (int i = 0; i < MAX_WBS_PER_OBJ; i++)
		for (int j = 0; j < MAX_WB_FIRING_MASKS; j++)
			cf_ReadString(page->wb_sound_names[i][j],PAGENAME_LEN,cf);

	for (int i = 0; i < NUM_MOVEMENT_CLASSES; i++)
		for (int j = 0; j < NUM_ANIMS_PER_CLASS; j++)
			cf_ReadString (page->anim_sound_names[i][j],PAGENAME_LEN,cf);
	if (version < 21)
		page->item.respawn_scalar = 1.0f;
	else
		page->item.respawn_scalar = cf_ReadFloat(cf);
	if (version >= 22) {
		int n_death_types = cf_ReadShort(cf);
		if (n_death_types > MAX_DEATH_TYPES)
			return 0;
		for (int i = 0; i < n_death_types; i++) {
			page->item.death_types[i].flags = cf_ReadInt(cf);
			page->item.death_types[i].delay_min = cf_ReadFloat(cf);
			page->item.death_types[i].delay_max = cf_ReadFloat(cf);
			page->item.death_probabilities[i] = cf_ReadByte(cf);
			if (version < 27 && (page->item.death_types[i].flags & 3) != 1)
				page->item.death_types[i].delay_min = 
					page->item.death_types[i].delay_max = 0;
		}
	}
	if (version < 20 &&
		(page->item.type == OBJ_ROBOT || page->item.type == OBJ_BUILDING) &&
		(page->item.flags & OIF_CONTROL_AI) &&
		(page->item.flags & OIF_DESTROYABLE))
		page->item.score = page->item.hit_points * 3;
	return 1;
}

void mng_ReadStoreGeneric(CFILE *cf, bool replace) {
	generic_page page;
	memset(&page, 0, sizeof(page));
	if (!mng_ReadGenericPage(cf, &page))
		return;
	int idx = FindObjectIDName(page.item.name);
	if (idx == -1)
		mng_AddGeneric(&page, cf);
	else if (replace) {
		mng_FreePagetypePrimitives(PAGETYPE_GENERIC, page.item.name, 0);
		GenericPageToGeneric(&page, idx, NULL);
	}
}

int tbl_old;
char tables_dir[PSPATHNAME_LEN];
char global_TableFile[PSPATHNAME_LEN];

//void ds_printf(const char *msg, ...) { va_list vp; va_start(vp, msg); vprintf(msg, vp); va_end(vp); }
//void write_init_string(const char *msg, float progress) { printf("%s", msg); }
void ds_printf(const char *msg, ...) { }
void write_init_string(const char *msg, float progress) { }

int mng_LoadTableFiles(int show_progress) {
	int last_pos = 0, start, size;
	CFILE *cf;
	char path[PSPATHNAME_LEN];

	if (Dedicated_server)
		show_progress = 0;
	int filt = FindArg("-filter");
	if (filt == 0)
		ddio_MakePath(path, tables_dir, "Table.gam", NULL);
	else
		strcpy(path, GameArgs[filt + 1]);
	if (!(cf = cfopen(path, "rb"))) {
		Error("Cannot open table file <%s>", global_TableFile);
		return 0;
	}
	cur_table_file_name = path;
	if (show_progress) {
		cfseek(cf, 0, SEEK_END);
		size = cftell(cf);
		cfseek(cf, 0, SEEK_SET);
	}
	//start = timer_GetTime();
	while (!cfeof(cf)) {
		if (show_progress) {
			int pos = ((float)cftell(cf) / size) * 20.0f;
			if (pos > last_pos) {
				write_init_string(TXT_INITDATA, pos / 20.0f);
				last_pos = pos;
			}
		}
		ubyte type = cf_ReadByte(cf);
		int start_pos = cftell(cf);
		int len = cf_ReadInt(cf);
		//printf("<%d,%x>", type, len);
		switch (type) {
			case PAGETYPE_UNKNOWN:
				break;
			case PAGETYPE_TEXTURE:
				ds_printf("T");
				mng_ReadStoreTexture(cf, false);
				break;
			case PAGETYPE_WEAPON:
				ds_printf("W");
				mng_ReadStoreWeapon(cf, false);
				break;
			#if 0
			case PAGETYPE_ROBOT:
			case PAGETYPE_POWERUP:
				Error("Your local table file is invalid");
				break;
			#endif
			case PAGETYPE_DOOR:
				ds_printf("D");
				mng_ReadStoreDoor(cf, false);
				break;
			case PAGETYPE_GENERIC:
				ds_printf("G");
				mng_ReadStoreGeneric(cf, false);
				break;
			case PAGETYPE_SHIP:
				ds_printf("P");
				mng_ReadStoreShip(cf, false);
				break;
			#if 0
			case PAGETYPE_SOUND:
				ds_printf("S");
				mng_ReadStoreSound(cf, false);
				break;
			case PAGETYPE_MEGACELL:
				ds_printf("M");
				mng_ReadStoreMegacell(cf, false);
				break;
			#else
			//case PAGETYPE_WEAPON:
			//case PAGETYPE_SHIP:
			case PAGETYPE_SOUND:
			case PAGETYPE_MEGACELL:
			case PAGETYPE_GAMEFILE:
			//case PAGETYPE_GENERIC:
				cfseek(cf, len - 4, SEEK_CUR);
				break;
			#endif
			default:
				Error("Unknown type in table %d", type);
				goto err;
		}
		if (cftell(cf) != start_pos + len)
			Error("Length mismatch for type %d cur %d exp %d", type, cftell(cf), start_pos + len);
	}
	ds_printf("Page reading completed.");
	cfclose(cf);
	cur_table_file_name = NULL;

	strcpy(path, "extra.gam");
	if (!(cf = cfopen(path, "rb")))
		return 1;
	cur_table_file_name = path;
 	while (!cfeof(cf)) {
		ubyte type = cf_ReadByte(cf);
		int start_pos = cftell(cf);
		int len = cf_ReadInt(cf);
		switch (type) {
			case PAGETYPE_UNKNOWN:
				break;
			case PAGETYPE_TEXTURE:
				ds_printf("T");
				mng_ReadStoreTexture(cf, true);
				break;
			case PAGETYPE_WEAPON:
				ds_printf("W");
				mng_ReadStoreWeapon(cf, true);
				break;
			case PAGETYPE_DOOR:
				ds_printf("D");
				mng_ReadStoreDoor(cf, true);
				break;
			case PAGETYPE_SHIP:
				ds_printf("P");
				mng_ReadStoreShip(cf, true);
				break;
			#if 0
			case PAGETYPE_SOUND:
				ds_printf("S");
				mng_ReadStoreSound(cf, true);
				break;
			case PAGETYPE_GAMEFILE:
				ds_printf("F");
				mng_ReadStoreGamefile(cf, true);
				break;
			case PAGETYPE_GENERIC:
				ds_printf("G");
				mng_ReadStoreGeneric(cf, true);
				break;
			#else
			//case PAGETYPE_SHIP:
			case PAGETYPE_SOUND:
			case PAGETYPE_MEGACELL:
			case PAGETYPE_GAMEFILE:
			case PAGETYPE_GENERIC:
				cfseek(cf, len - 4, SEEK_CUR);
				break;
			#endif
			default:
				goto err;
		}
		if (cftell(cf) != start_pos + len)
			Error("Length mismatch for type %d cur %d exp %d\n", type, cftell(cf), start_pos + len);
	}
	cur_table_file_name = NULL;
	cfclose(cf);
	return 1;
err:
	cfclose(cf);
	cur_table_file_name = NULL;
	return 0;
}

int mng_InitLocalTables() {
	return 1;
}

void RemapEverything() {}

int mng_LoadNetPages(int show_progress) {
	int ret1 = mng_LoadTableFiles(show_progress);
	int ret2 = mng_InitLocalTables();
	RemapEverything();
	return ret1 || ret2;
}
