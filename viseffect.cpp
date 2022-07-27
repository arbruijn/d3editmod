#include <string.h>
#include "viseffect.h"
#include "fireball.h"
#include "game.h"
#include "globals.h"
#include "terrain.h"
#include "renderer.h"
#include "vclip.h"
#include "findintersection.h"
#include "physics.h"

void CreateRandomSparks (int num_sparks,vector *pos,int roomnum,int which_index,float force_scalar) {
}

int Num_vis_effects;
ushort max_vis_effects;
ushort *VisDeadList, *Vis_free_list;
vis_effect *VisEffects;
int Highest_vis_effect_index = -1;
int NumVisDead;
int Physics_vis_counter;

void ShutdownVisEffects(void)
{
	if (VisEffects)
		free(VisEffects);
	if (VisDeadList)
		free(VisDeadList);
	if (Vis_free_list)
		free(Vis_free_list);
}

void InitVisEffects(void)
{
	static int old_max_vis;
	
	max_vis_effects = MAX_VIS_EFFECTS;
	if (old_max_vis != MAX_VIS_EFFECTS) {
		if (!VisEffects) {
			VisEffects = (vis_effect *)malloc(MAX_VIS_EFFECTS * sizeof(vis_effect));
			VisDeadList = (ushort *)malloc(max_vis_effects * 2);
			Vis_free_list = (ushort *)malloc(max_vis_effects * 2);
		} else {
			VisEffects = (vis_effect *)realloc(VisEffects,MAX_VIS_EFFECTS * sizeof(vis_effect));
			VisDeadList = (ushort *)realloc(VisDeadList,max_vis_effects * 2);
			Vis_free_list = (ushort *)realloc(Vis_free_list,max_vis_effects * 2);
		}
		for (int i = 0; i < max_vis_effects; i++) {
			VisEffects[i].type = 0;
			VisEffects[i].roomnum = -1;
			VisEffects[i].prev = -1;
			VisEffects[i].next = -1;
			Vis_free_list[i] = i;
		}
		old_max_vis = max_vis_effects;
		Num_vis_effects = 0;
		Highest_vis_effect_index = 0;
		atexit(ShutdownVisEffects);
	}
}

int VisEffectAllocate(void)
{
	int iVar1;

	if (Num_vis_effects == max_vis_effects)
		return -1;
	int num = Vis_free_list[Num_vis_effects++];
	if (num > Highest_vis_effect_index)
		Highest_vis_effect_index = num;
	return num;
}

int VisEffectInitType(vis_effect *vis)
{
	ubyte id;

	id = vis->id;
	vis->size = Fireballs[id].size;
	vis->flags |= VF_USES_LIFELEFT;
	vis->lifeleft = vis->lifetime = Fireballs[id].total_life;
	if (((Fireballs[id].type == 0) && (id != CUSTOM_EXPLOSION_INDEX)) && (id != NAPALM_BALL_INDEX))
		vis->lighting_color = 0xfec2;
	return 1;
}


void VisEffectLink(int visnum,int roomnum)
{
	vis_effect *vis;

	vis = VisEffects + visnum;
	vis->roomnum = roomnum;
	if (roomnum & ROOMNUM_CELLNUM_FLAG)
		return;
	vis->next = Rooms[roomnum].vis_effects;
	Rooms[roomnum].vis_effects = visnum;
	vis->prev = -1;
	if (vis->next != -1)
		VisEffects[vis->next].prev = visnum;
}

int VisEffectCreate(ubyte type,ubyte id,int roomnum,vector *pos)
{
	vis_effect *vis;
	int num;
	vis_effect *pvVar4;

	if ((Game_mode & 0x24) && Dedicated_server)
		return -1;

	if (!(roomnum & ROOMNUM_CELLNUM_FLAG)) {
		if (Gametime - Rooms[roomnum].last_render_time > 5)
			return -1;
	} else {
		if ((roomnum & ROOMNUM_CELLNUM_MASK) > 0x10000)
			return -1;
		if (Gametime - Last_terrain_render_time > 5)
			return -1;
	}
	num = VisEffectAllocate();
	if (num == -1)
		return -1;
	vis = VisEffects + num;
	memset(vis, 0, sizeof(*vis));
	vis->id = id;
	vis->type = 1;
	vis->pos = *pos;
	vis->flags = 0;
	vis->phys_flags = 0;
	vis->roomnum = roomnum;
	vis->movement_type = 0;
	vis->attach_info.end_vertnum = 0xffff;
	vis->next = -1;
	vis->prev = -1;
	vis->lighting_color = 0;
	vis->creation_time = Gametime;
	VisEffectInitType(vis);
	VisEffectLink(num,roomnum);
	return num;
}


void DrawVisLightningBolt(vis_effect *vis) {}
void DrawVisMassDriverEffect(vis_effect *vis,bool is_mercboss) {}
void DrawVisBillboardSmoketrail(vis_effect *vis) {}
void DrawVisThickLightning(vis_effect *vis) {}
void DrawVisSineWave(vis_effect *vis) {}
void DrawVisBlastRing(vis_effect *vis) {}
void DrawVisFadingLine(vis_effect *vis) {}
void DrawVisSnowflake(vis_effect *vis) {}
void DrawVisRainDrop(vis_effect *vis) {}
void DrawVisAxisBillboard(vis_effect *vis) {}

void DrawVisEffect(vis_effect *vis)
{
	ubyte alpha;
	int width;
	int bmheight;
	angle rot_angle;
	vclip *vclip;
	int bmidx;
	float fVar4;
	float visfrac;
	float run_time;
	ubyte alphatype;
	short tex;
	float height;
	int id;
	int visnum;
	float size;
	
	id = vis->id;
	if ((id == LIGHTNING_BOLT_INDEX) || (id == GRAY_LIGHTNING_BOLT_INDEX)) {
		DrawVisLightningBolt(vis);
		return;
	}
	if (id == MASSDRIVER_EFFECT_INDEX) {
		DrawVisMassDriverEffect(vis,false);
		return;
	}
	if (id == MERCBOSS_MASSDRIVER_EFFECT_INDEX) {
		DrawVisMassDriverEffect(vis,true);
		return;
	}
	if (id == BILLBOARD_SMOKETRAIL_INDEX) {
		DrawVisBillboardSmoketrail(vis);
		return;
	}
	if (id == THICK_LIGHTNING_INDEX) {
		DrawVisThickLightning(vis);
		return;
	}
	if (id == SINE_WAVE_INDEX) {
		DrawVisSineWave(vis);
		return;
	}
	if (id == BLAST_RING_INDEX) {
		DrawVisBlastRing(vis);
		return;
	}
	if (id == FADING_LINE_INDEX) {
		DrawVisFadingLine(vis);
		return;
	}
	if (id == SNOWFLAKE_INDEX) {
		DrawVisSnowflake(vis);
		return;
	}
	if ((id == RAINDROP_INDEX) || (id == PUDDLEDROP_INDEX)) {
		DrawVisRainDrop(vis);
		return;
	}
	if (id == AXIS_BILLBOARD_INDEX) {
		DrawVisAxisBillboard(vis);
		return;
	}
	run_time = Gametime - vis->creation_time;
	if (run_time < 0.0) {
		run_time = 0.0;
	}
	size = vis->size;
	if (Katmai &&
		 ((((id == BIG_EXPLOSION_INDEX || (id == MED_EXPLOSION_INDEX)) || (id == MED_EXPLOSION_INDEX2))
			|| (id == MED_EXPLOSION_INDEX3))))
		size = size * 1.8f;
	visnum = vis - VisEffects;
	if (Fireballs[id].type == FT_BILLOW) {
		rot_angle = FrameCount * 160 + visnum * 5000;
	} else if (!(vis->flags & VF_ATTACHED)) {
		if ((id == RUBBLE1_INDEX) || (id == RUBBLE2_INDEX)) {
			rot_angle = FrameCount * 860 + visnum * 5000;
		} else if (id == SUN_CORONA_INDEX) {
			rot_angle = (FrameCount + visnum * 10) * 500;
			bmidx = rand();
			size *= (rand() % 10) / 100.0f + 1;
		} else
			rot_angle = (visnum * 5000) % 65536;
	} else {
		rot_angle = 0;
	}
	visfrac = run_time / vis->lifetime;
	if ((vis->flags & VF_ATTACHED) != 0)
		visfrac = run_time - (int)run_time;
	if (visfrac >= 1)
		visfrac = 0.99999;
	if (vis->flags & VF_EXPAND) {
		size = vis->size * 0.5 + visfrac * vis->size * 0.5;
	}
	if (id == SMOKE_TRAIL_INDEX) {
		tex = vis->custom_handle;
		if ((GameTextures[tex].flags & TF_ANIMATED) == 0) {
			bmidx = GameTextures[tex].bm_handle;
		} else {
			vclip = GameVClips + GameTextures[tex].bm_handle;
			bmidx = vclip->frames[(int)(vclip->num_frames * visfrac)];
		}
	} else if (id == SPRAY_INDEX) {
		vclip = GameVClips + vis->custom_handle;
		bmidx = vclip->frames[(int)(vclip->num_frames * visfrac)];
	} else if (id == CUSTOM_EXPLOSION_INDEX || id == PARTICLE_INDEX) {
		visnum = vis->custom_handle;
		if (GameTextures[visnum].flags & TF_ANIMATED) {
			vclip = GameVClips + GameTextures[visnum].bm_handle;
			bmidx = vclip->frames[(int)(vclip->num_frames * visfrac)];
		} else
			bmidx = GetTextureBitmap(visnum,0,false);
	} else if (Fireballs[id].type == FT_SPARK) {
		size *= 1.0 - visfrac;
		bmidx = Fireballs[id].bm_handle;
	} else if (id == SUN_CORONA_INDEX || id == MUZZLE_FLASH_INDEX ||
		id == RUBBLE1_INDEX || id == RUBBLE2_INDEX) {
		bmidx = Fireballs[id].bm_handle;
	} else {
		vclip = GameVClips + Fireballs[id].bm_handle;
		bmidx = vclip->frames[(int)(vclip->num_frames * visfrac)];
	}
	if ((Fireballs[id].type == FT_SMOKE) && visfrac > 0.3) {
		height = (visfrac - 0.3) / 0.7f;
		if (!(vis->flags & VF_REVERSE))
			size *= height * 2.3 + 1.0;
		else
			size /= height * 2.3 + 1.0;
	}
	if (((id == SMOKE_TRAIL_INDEX) || (id == CUSTOM_EXPLOSION_INDEX)) ||
		 (id == PARTICLE_INDEX)) {
		if (GameTextures[vis->custom_handle].flags & TF_SATURATE) {
			alphatype = AT_SATURATE_TEXTURE;
		}
		else {
			alphatype = AT_CONSTANT_TEXTURE;
		}
	} else if (id == BLACK_SMOKE_INDEX) {
		alphatype = AT_LIGHTMAP_BLEND;
	} else {
		if ((((Fireballs[id].type == FT_SMOKE) && (id != MED_SMOKE_INDEX)) ||
				(id == RUBBLE1_INDEX)) || (id == RUBBLE2_INDEX))
		alphatype = AT_CONSTANT_TEXTURE;
	else
			alphatype = AT_SATURATE_TEXTURE;
	}
	rend_SetAlphaType(alphatype);
	float decay = visfrac > 0.5 ? 1 - (visfrac - 0.5) * 2 : 1.0;
	if (size > 80)
		size = 80;
	id = vis->id;
	if (((id != BIG_EXPLOSION_INDEX) && (id != BLUE_EXPLOSION_INDEX)) && size > 40)
		size = 40;
	if (((id == SMOKE_TRAIL_INDEX) || (id == CUSTOM_EXPLOSION_INDEX)) ||
		 (id == PARTICLE_INDEX)) {
		alpha = decay * GameTextures[vis->custom_handle].alpha * 255.0;
	}
	else {
		if (Fireballs[id].type == FT_SMOKE) {
			alpha = decay * 76.5;
		} else if ((id == RUBBLE1_INDEX) || (id == RUBBLE2_INDEX)) {
			alpha = 255;
		} else if (id == MUZZLE_FLASH_INDEX) {
			alpha = 128;
		} else if ((vis->flags & VF_ATTACHED) == 0) {
			alpha = decay * 229.5;
		} else {
			alpha = 229;
		}
	}
	rend_SetAlphaValue(alpha);
	rend_SetOverlayType(OT_NONE);
	if ((vis->flags & VF_NO_Z_ADJUST) == 0)
		rend_SetZBias(-size);
	rend_SetZBufferWriteMask(0);
	rend_SetWrapType(WT_CLAMP);
	rend_SetLighting(LS_NONE);
	if (((id == RUBBLE1_INDEX) || (id == RUBBLE2_INDEX)) || (id == GRAY_SPARK_INDEX)) {
		bmheight = bm_h(bmidx,0);
		width = bm_w(bmidx,0);
		height = (bmheight * size) / width;
		g3_DrawRotatedBitmap(&vis->pos,rot_angle,size,height,bmidx,GR_16_TO_COLOR(vis->lighting_color));
	}
	else {
		if ((vis->flags & VF_PLANAR) != 0) {
			bmheight = bm_h(bmidx,0);
			width = bm_w(bmidx,0);
			g3_DrawPlanarRotatedBitmap(&vis->pos,&vis->end_pos,rot_angle,size,(bmheight * size) / width,bmidx);
		} else {
		bmheight = bm_h(bmidx,0);
		width = bm_w(bmidx,0);
		height = (bmheight * size) / width;
				g3_DrawRotatedBitmap(&vis->pos,rot_angle,size,height,bmidx,-1);
	}
	}
	rend_SetZBias(0.0);
	rend_SetZBufferWriteMask(1);
	rend_SetWrapType(WT_WRAP);
}

void VisEffectDelete(int visnum)
{
	VisEffectUnlink(visnum);
	VisEffects[visnum].type = 0;
	VisEffects[visnum].roomnum = -1;
	VisEffectFree(visnum);
}

void VisEffectDeleteDead(void)
{
	for (int i = 0; i < NumVisDead; i++)
		VisEffectDelete(VisDeadList[i]);
	NumVisDead = 0;
}


void VisEffectSetDeadFlag(vis_effect *vis)
{
	if (!(vis->flags & VF_DEAD) && vis->type) {
		vis->flags |= VF_DEAD;
		VisDeadList[NumVisDead] = vis - VisEffects;
		NumVisDead++;
	}
}

void VisEffectUnlink(int visnum)
{
	vis_effect *vis = VisEffects + visnum;
	
	if (vis->roomnum & ROOMNUM_CELLNUM_FLAG)
		return;
	if (vis->prev == -1)
		Rooms[vis->roomnum].vis_effects = vis->next;
	else
		VisEffects[vis->prev].next = vis->next;
	if (vis->next != -1)
		VisEffects[vis->next].prev = vis->prev;
	vis->roomnum = -1;
}

void VisEffectRelink(int visnum,int roomnum)
{
	VisEffectUnlink(visnum);
	VisEffectLink(visnum,roomnum);
}

int VisEffectFree(int visnum)
{
	Vis_free_list[--Num_vis_effects] = visnum;
	VisEffects[visnum].type = 0;
	if (visnum == Highest_vis_effect_index)
		while (Highest_vis_effect_index > 0 && VisEffects[Highest_vis_effect_index].type)
			Highest_vis_effect_index--;
	return 1;
}

void do_vis_physics_sim(vis_effect *vis)
{
	vector startpos;
	fvi_query fq;
	fvi_info hit_info;
	int phys_flags;
	int hit;
	
	startpos = vis->pos;
	if ((vis->flags & VF_DEAD) || Frametime <= 0)
		return;
	if (abs(vis->velocity.x) < 1e-6 && abs(vis->velocity.y) < 1e-6 && abs(vis->velocity.z) < 1e-6 &&
		!(vis->phys_flags & PF_GRAVITY))
		return;
	phys_flags = vis->phys_flags;
	if (phys_flags & PF_FIXED_VELOCITY) {
		vector move = vis->velocity * Frametime;
		vis->pos += move;
	} else {
		float gravity = phys_flags & PF_GRAVITY ? Gravity_strength :
			phys_flags & PF_REVERSE_GRAVITY ? -Gravity_strength : 0;
		if (vis->mass > 0 && vis->drag > 0) {
			float mass = vis->mass, drag = vis->drag;
			float eval = exp(-(drag / mass * Frametime));
			vector force_m = {0, gravity * mass, 0};
			vector velmove = (vis->velocity - force_m / drag) * (mass / drag) * (1 - eval);
			vis->pos += force_m / drag * Frametime + velmove;
			vis->velocity = (vis->velocity - force_m / drag) * eval + force_m / drag;
		} else {
			vector force_t = {0, gravity * Frametime, 0};
			vis->pos += vis->velocity * Frametime + force_t * 0.5 * Frametime;
			vis->velocity += force_t;
		}
	}
	if (!(vis->phys_flags & PF_NO_COLLIDE)) {
		Physics_vis_counter++;
		fq.startroom = vis->roomnum;
		fq.p0 = &startpos;
		fq.p1 = &vis->pos;
		fq.rad = 0.0;
		fq.thisobjnum = -1;
		fq.ignore_obj_list = NULL;
		fq.flags = FQ_ONLY_DOOR_OBJ | FQ_IGNORE_WALLS | FQ_CHECK_OBJS; 
		hit = fvi_FindIntersection(&fq,&hit_info,false);
		if ((hit == 0) && (hit_info.hit_room != -1)) {
			if (hit_info.hit_room != vis->roomnum)
				VisEffectRelink(vis - VisEffects,hit_info.hit_room);
		} else
			VisEffectDelete(vis - VisEffects);
	}
}

void VisEffectMoveOne(vis_effect *vis)
{
	if (vis->flags & VF_USES_LIFELEFT)
		vis->lifeleft -= Frametime;
	if (vis->movement_type == MT_PHYSICS)
		do_vis_physics_sim(vis);
	if ((vis->flags & VF_USES_LIFELEFT) && vis->lifeleft < 0)
		VisEffectSetDeadFlag(vis);
	//...
}

void VisEffectMoveAll()
{
	for (int i = 0; i <= Highest_vis_effect_index; i++)
		if (VisEffects[i].type)
			VisEffectMoveOne(VisEffects + i);
}

