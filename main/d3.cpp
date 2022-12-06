// To compile with gcc:	(tested on Ubuntu 14.04 64bit):
//	 g++ sdl2_opengl.cpp -lSDL2 -lGL
// To compile with msvc: (tested on Windows 7 64bit)
//	 cl sdl2_opengl.cpp /I C:\sdl2path\include /link C:\path\SDL2.lib C:\path\SDL2main.lib /SUBSYSTEM:CONSOLE /NODEFAULTLIB:libcmtd.lib opengl32.lib

#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <GL/gl.h>

typedef int32_t i32;
typedef uint32_t u32;
typedef int32_t b32;

int WinWidth = 640;
int WinHeight = 480;

#include "ddio.h"
#include "bitmap.h"
#include "gametexture.h"
#include "texture.h"
#include "gr.h"
#include "lightmap_info.h"
#include "special_face.h"
#include "render.h"
#include "object.h"
#include "gametexture.h"
#include "vclip.h"
#include "findintersection.h"
#include "trigger.h"
#include "door.h"
#include "soundload.h"
#include "ship.h"
#include "ambient.h"
#include "matcen.h"

#include "terrain.h"
#include "boa.h"

#include "renderer/rend_opengl.h"

#include "loadlevel.h"

#include "stringtable.h"
#include "gameloop.h"
#include "postrender.h"
#include "globals.h"
#include "game.h"
#include "object.h"
#include "config.h"
#include "weapon.h"
#include "physics.h"
#include "pagein.h"
#include "collide.h"
#include "scorch.h"
#include "doorway.h"
#include "aimain.h"
#include "osiris_dll.h"
#include "levelgoal.h"
#include "multi.h"
#include "gamefont.h"
#include "grtext.h"
#include "hud.h"
#include "uidraw.h"
#include "damage.h"
#include "gamedll.h"
#include "attach.h"
#include "gameevent.h"
#include "fireball.h"
#include "cockpit.h"

#include "dll.h"
#define INCLUDED_FROM_D3
#include "osiris_common.h"
bool Osiris_CallEvent(object *obj, int evt, tOSIRISEventInfo *data );

tDetailSettings DetailPresetLow =
	{ 1280, 25,
		false, false, true, false, false,
		false, false, false, false, false,
		true,
		0, 0};
tDetailSettings DetailPresetVHi =
	{ 1920, 10,
		true, true, true, true, true,
		true, true, true, true, true,
		true,
		2, 0};

void ConfigSetDetailLevel(int level) {
	Detail_settings = DetailPresetVHi;
}

void LoadGameSettings() {
	ConfigSetDetailLevel(3);
}

void InitGraphics() {
	// Init our bitmaps (and lightmaps required for LoadLevel). Must be called before InitTextures and InitTerrain
	bm_InitBitmaps(); // calls lm_InitLightmaps() for us

	// Initializes the Texture system
	//InitTextures ();

	LoadAllFonts();
}

void InitIOSystems() {
	LoadGameSettings();
}

extern void InitStringTable();

void InitD3Systems1() {
	InitIOSystems();
	InitStringTable();

	InitGraphics();

	// Initialize the Object_info system
	InitObjectInfo();

	// initialize lighting systems
	InitLightmapInfo();
	InitSpecialFaces();
	//InitDynamicLighting();

	InitShips();

	// Initializes the fvi system
	InitFVI();

	InitMatcens();

	InitMathTables();

	// Initialize the terrain
	InitTerrain();

	InitSounds();

	InitDoors();
}

void InitD3Systems2() {
	// Systems2

	InitObjects();
	InitFireballs();
	InitTriggers();
}

#if 0
bool load_and_chunk_bitmap_compr(char *param_1,int *param_2)
{
  int bm_handle;
  chunked_bitmap local_14;
  
  bm_handle = bm_AllocLoadFileBitmap(param_1,0,0);
  if (bm_handle == -1) {
    param_2[0] = 0;
    param_2[1] = 0;
    param_2[2] = 0;
    return false;
  }
  bm_CreateChunkedBitmap(bm_handle, &local_14);
  if (bm_handle > 0) {
    bm_FreeBitmap(bm_handle);
  }
  param_2[0] = local_14.w;
  param_2[1] = local_14.h;
  param_2[2] = local_14.bm_array;
  return true;
}
#endif


void rend_DrawSimpleBitmap(int param_1,int param_2,int param_3)
{
  int y2;
  int iVar1;
  int bm;
  float u0;
  float v0;
  float u1;
  float v1;
  float zval;
  int color;
  float *alphas;
  
  rend_SetAlphaType(AT_CONSTANT_TEXTURE);
  rend_SetAlphaValue(0xff);
  rend_SetLighting(LS_NONE);
  rend_SetColorModel(CM_MONO);
  rend_SetOverlayType(OT_NONE);
  rend_SetFiltering(0);
  alphas = (float *)0x0;
  color = -1;
  zval = 1.00000000;
  v1 = 1.00000000;
  u1 = 1.00000000;
  v0 = 0.00000000;
  u0 = 0.00000000;
  bm = param_1;
  y2 = bm_h(param_1,0);
  y2 = y2 + param_3;
  iVar1 = bm_w(param_1,0);
  rend_DrawScaledBitmap(param_2,param_3,iVar1 + param_2,y2,bm,u0,v0,u1,v1,zval,color,alphas);
  rend_SetFiltering(1);
}

void draw_chbm(chunked_bitmap *param_1,int param_2,int param_3)

{
  int iVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  int *piVar6;
  int *piVar7;
  int iVar8;
  int local_8;
  int local_4;
  
  piVar6 = param_1->bm_array;
  iVar1 = param_1->w;
  iVar4 = param_1->h;
  iVar2 = bm_w(*piVar6,0);
  iVar3 = bm_h(*piVar6,0);
  rend_SetZBufferState('\0');
  rend_GetProjectionParameters(&local_4,&local_8);
  iVar5 = param_3;
  param_3 = iVar4;
  if (0 < iVar4) {
    do {
      iVar4 = iVar1;
      piVar7 = piVar6;
      iVar8 = param_2;
      if (0 < iVar1) {
        do {
          rend_DrawSimpleBitmap(*piVar7,iVar8,iVar5);
          iVar4 = iVar4 + -1;
          piVar7 = piVar7 + 1;
          iVar8 = iVar8 + iVar2;
        } while (iVar4 != 0);
      }
      piVar6 = piVar6 + iVar1;
      param_3 = param_3 + -1;
      iVar5 = iVar5 + iVar3;
    } while (param_3 != 0);
  }
  rend_SetZBufferState('\x01');
}

SDL_Window *Window;

chunked_bitmap load_bm;
void setup_load_screen() {
	int bm = bm_AllocLoadFileBitmap("oemmenu.ogf",0,0);
	if (bm == -1)
		return;
	if (!bm_CreateChunkedBitmap(bm, &load_bm))
		Error("Failed to slice up d3.ogf!");
	//bm_FreeBitmap(bm);

	rend_DrawSimpleBitmap(bm, 0, 0);
	SDL_GL_SwapWindow(Window);
}

extern void write_init_string(const char *msg, float progress);

void load_game_data(bool no_progress) {
	write_init_string(TXT_INITDATA, -1.0f);
	if (!mng_LoadNetPages(!no_progress))
		Error("Cannot load table file.");
}

float timer_GetTime() {
	return SDL_GetTicks() / 1000.0f;
}

int last_timer;
int timer_paused;
void CalcFrameTime() {
	if (timer_paused)
		return;
	int t = SDL_GetTicks();
	Frametime = (t - last_timer) / 1000.0f;
	last_timer = t;
}
void InitFrameTime()
{
	last_timer = SDL_GetTicks();
	timer_paused = 0;
}

void GameRenderWorld(object *viewer,vector *viewer_eye,int viewer_roomnum,matrix *viewer_orient,
               float zoom,bool rear_view) {
	g3_StartFrame(viewer_eye,viewer_orient,zoom);
	Num_fogged_rooms_this_frame = 0;
	rend_SetZBufferState(1);
	rend_SetZBufferWriteMask(1);
	ResetPostrenderList();
	if (viewer_roomnum & ROOMNUM_CELLNUM_FLAG)
		RenderTerrain(0, -1, -1, -1, -1);
	else
		RenderMine(viewer_roomnum);
	PostRender(viewer_roomnum);
	g3_EndFrame();
}

void StartFrame(int x1, int y1, int x2, int y2, bool first, bool push)
{
	rend_StartFrame(x1, y1, x2, y2);
	grtext_SetParameters(0, 0, x2 - x1, y2 - y1);
}

void StartFrame(bool sw_clear)
{
	StartFrame(Game_window_x,Game_window_y,Game_window_w + Game_window_x,Game_window_h + Game_window_y,sw_clear,true);
}

void EndFrame()
{
	rend_EndFrame();
}

void GameDrawMainView()
{
	StartFrame(true);
	bool rear = Viewer_object == Player_object &&
		(Players[Player_num].flags & PLAYER_FLAGS_REARVIEW) != 0;
	GameRenderWorld(Viewer_object,&Viewer_object->pos,Viewer_object->roomnum,&Viewer_object->orient,
		Render_zoom, rear);
	EndFrame();
}

void GameDrawHud()
{
	StartFrame(false);
	g3_StartFrame(&Viewer_object->pos,&Viewer_object->orient,0.56);
	RenderHUDFrame();
	g3_EndFrame();
	EndFrame();
	StartFrame(0,0,Max_window_w,Max_window_h,false,true);
	g3_StartFrame(&Viewer_object->pos,&Viewer_object->orient,0.56);
	RenderAuxHUDFrame();
	g3_EndFrame();
	EndFrame();
}

void GameRenderFrame() {
	object *Player_object = &Objects[Players[Player_num].objnum];
	Viewer_object = &Objects[0];

	extern void SetScreenSize(int,int);
	SetScreenSize(WinWidth, WinHeight);

	GameDrawMainView();
	GameDrawHud();

	#if 0
	StartFrame(false);
	ui_DrawSetAlpha(32);
	ui_DrawRect(GR_WHITE, 0, 0, WinWidth, WinHeight);
	EndFrame();
	#endif


	SDL_GL_SwapWindow(Window);
	FrameCount++;
}

void d2_drag(object *obj, float sim_time) {
	float drag;

	if (!(drag = obj->mtype.phys_info.drag))
		return;

	int count;
	float r,k;

	float FT = 1.0f / 64.0f;
	count = (int)(sim_time / FT);
	r = sim_time - count * FT;
	k = r / FT;

	if (obj->mtype.phys_info.flags & PF_USES_THRUST) {
		vector accel = obj->mtype.phys_info.thrust;
		accel *= 1.0f / obj->mtype.phys_info.mass;

		while (count--) {
			obj->mtype.phys_info.velocity += accel;
			obj->mtype.phys_info.velocity *= 1.0f - drag;
		}

		//do linear scale on remaining bit of time

		obj->mtype.phys_info.velocity += accel * k;
		obj->mtype.phys_info.velocity *= 1.0f - k * drag;
	} else {
		float total_drag = 1.0f;

		while (count--)
			total_drag = total_drag * (1.0f - drag);

		//do linear scale on remaining bit of time
		total_drag = total_drag * (1.0f - k * drag);
		obj->mtype.phys_info.velocity *= total_drag;
	}
}

void collide_two_objects(object *A, object *B,vector *collision_point,vector *collision_normal,fvi_info *hit_info) {
	tOSIRISEventInfo info;
	info.evt_collide.it_handle = B->handle;
	Osiris_CallEvent(A,EVT_COLLIDE,&info);
	info.evt_collide.it_handle = A->handle;
	Osiris_CallEvent(B,EVT_COLLIDE,&info);
	if (A->type == OBJ_WEAPON && (IS_GENERIC(B->type) || B->type == OBJ_DOOR)) {
		weapon *w = Weapons + A->id;
		int wf = w->flags;
		int damage_type = wf & WF_NAPALM ? GD_FIRE : wf & WF_MATTER_WEAPON ? GD_MATTER :
			wf & WF_ELECTRICAL ? GD_ELECTRIC : GD_ENERGY;
		ApplyDamageToGeneric(B, A, damage_type, w->generic_damage * A->ctype.laser_info.multiplier,0,255);
		/*B->shields -= 100;
		if (B->shields <= 0)
			B->flags |= OF_DEAD;*/
	}
	if (A->type == OBJ_PLAYER && B->type == OBJ_POWERUP)
		B->flags |= OF_DEAD;
}

void do_physics_sim(object *obj) {
	fvi_info hit_info;
	Fvi_num_recorded_faces = 0;
	vector *pos = &obj->pos;
	int objnum = obj - Objects;
	float sim_time = Frametime;
	bool drag_modified = false;
	vector last_pos = *pos;
	fvi_query q;
	bool did_rot = false;

	if (obj->flags & (OF_DEAD | OF_ATTACHED))
		return;
	if (Frametime <= 0)
		return;
	if (obj->type == OBJ_DUMMY)
		return;
	if (obj->type == OBJ_PLAYER)
		if (obj->mtype.phys_info.thrust.x || obj->mtype.phys_info.thrust.y || obj->mtype.phys_info.thrust.z ||
			obj->mtype.phys_info.rotthrust.x || obj->mtype.phys_info.rotthrust.y || obj->mtype.phys_info.rotthrust.z)
			Players[obj->id].last_thrust_time = Gametime;
	if (0 && obj->mtype.phys_info.flags & PF_WIGGLE) {
		float thrust = vm_GetMagnitude(&obj->mtype.phys_info.thrust);
		if (thrust > 0.1f)
			obj->mtype.phys_info.last_still_time += Frametime * 0.5f;
		else
			obj->mtype.phys_info.last_still_time -= Frametime * 0.5f;
		if (obj->mtype.phys_info.last_still_time < 0.0f)
			obj->mtype.phys_info.last_still_time = 0.0f;
		else if (obj->mtype.phys_info.last_still_time > 1.0f)
			obj->mtype.phys_info.last_still_time = 1.0f;
		float still_fac = 1.0f - obj->mtype.phys_info.last_still_time;
		if (still_fac < 0.1f)
			still_fac = 0.1f;
		float cur_h = FixSin((uint)(Gametime * obj->mtype.phys_info.wiggles_per_sec * 65535.0f) & 0xffff);
		float last_h = FixSin((uint)((Gametime - Frametime) * obj->mtype.phys_info.wiggles_per_sec * 65535.0f) & 0xffff);
		float amp = (cur_h - last_h) * obj->mtype.phys_info.wiggle_amplitude * still_fac;
		q.ignore_obj_list = NULL;
		q.flags = FQ_CHECK_OBJS | FQ_IGNORE_POWERUPS | FQ_RECORD | FQ_NEW_RECORD_LIST | FQ_CHECK_CEILING;
		if (obj->type == OBJ_WEAPON)
			q.flags |= FQ_TRANSPOINT;
		if (obj->flags & OF_NO_OBJECT_COLLISIONS)
			q.flags &= ~FQ_CHECK_OBJS;
		q.startroom = obj->roomnum;
		q.thisobjnum = objnum;
		q.rad = obj->size;
		vector uvec_amp = obj->orient.uvec;
		uvec_amp *= amp;
		vector new_pos = *pos;
		new_pos += uvec_amp;
		q.p1 = &new_pos;
		q.p0 = pos;
		int hit = fvi_FindIntersection(&q, &hit_info, false);
		if (!hit)
			ObjSetPos(obj, &hit_info.hit_pnt, hit_info.hit_room, NULL);
	}
	#define VEC_ALMOST_ZERO(v) (fabsf((v)->x) <= 0.000001f && fabsf((v)->y) <= 0.000001f && fabsf((v)->z) <= 0.000001f)
	#define VEC_ZERO(v) (!(v)->x && !(v)->y && !(v)->z)
	if (VEC_ALMOST_ZERO(&obj->mtype.phys_info.velocity) &&
		VEC_ALMOST_ZERO(&obj->mtype.phys_info.thrust) &&
		VEC_ALMOST_ZERO(&obj->mtype.phys_info.rotvel) &&
		VEC_ALMOST_ZERO(&obj->mtype.phys_info.rotthrust) &&
		!(obj->mtype.phys_info.flags & PF_GRAVITY) &&
		(ROOMNUM_OUTSIDE(obj->roomnum) || VEC_ZERO(&Rooms[obj->roomnum].wind)) &&
		!(obj->mtype.phys_info.flags & PF_WIGGLE) &&
		(!obj->ai_info || !(obj->ai_info->flags & AIF_REPORT_NEW_ORIENT))) {
		if (obj->flags & OF_MOVED_THIS_FRAME)
			obj->flags = (obj->flags & ~OF_MOVED_THIS_FRAME) | OF_STOPPED_THIS_FRAME;
		return;
	}

	if (obj->ai_info)
		obj->ai_info->flags &= ~AIF_REPORT_NEW_ORIENT;
	obj->flags |= OF_MOVED_THIS_FRAME;

	// FIXME drag vectors

	int max_count = obj->type == OBJ_PLAYER ? 9 : 5;
	for (int count = 0; count < max_count; count++) {
		vector frame_last_pos = *pos;
		vector frame_last_vel = obj->mtype.phys_info.velocity;
		matrix frame_last_orient = obj->orient;
		vector frame_last_rotvel = obj->mtype.phys_info.rotvel;
		angle frame_last_turnroll = obj->mtype.phys_info.turnroll;

		matrix new_orient = obj->orient;
		vector new_rotvel = obj->mtype.phys_info.rotvel;
		vector new_vel = obj->mtype.phys_info.velocity;
		angle new_turnroll = obj->mtype.phys_info.turnroll;

		vector gravity_vec;
		float gravity_accel = 9.8f;
		if (obj->mtype.phys_info.flags & PF_GRAVITY)
			gravity_vec = {0.0f, gravity_accel * obj->mtype.phys_info.velocity.y, 0.0f};
		else if (obj->mtype.phys_info.flags & PF_REVERSE_GRAVITY)
			gravity_vec = {0.0f, -gravity_accel * obj->mtype.phys_info.velocity.y, 0.0f};
		else
			gravity_vec = {0.0f, 0.0f, 0.0f};

		//d2_drag(obj, sim_time);
		//obj->mtype.phys_info.rotvel /= obj->mtype.phys_info.drag * sim_time;
		bool just_rot = false;
		if ((!did_rot && !count) || obj->type == OBJ_CLUTTER) {
			did_rot = true;
			just_rot = true;
			PhysicsDoSimRot(obj,sim_time,&new_orient,&obj->mtype.phys_info.rotthrust,&new_rotvel,&new_turnroll);
			ObjSetOrient(obj,&new_orient);
		}
		#if 0
		obj->mtype.phys_info.rotvel += obj->mtype.phys_info.rotthrust * 1000;
		if (!VEC_ZERO(&obj->mtype.phys_info.rotvel)) {
			angvec angs;
			matrix rotmat, new_orient;
			//vm_ExtractAnglesFromMatrix(&angs, &obj->orient);
			angs.p = (int)(obj->mtype.phys_info.rotvel.x * sim_time) & 0xffff;
			angs.h = (int)(obj->mtype.phys_info.rotvel.y * sim_time) & 0xffff;
			angs.b = (int)(obj->mtype.phys_info.rotvel.z * sim_time) & 0xffff;
			vm_AnglesToMatrix(&rotmat, angs.p, angs.h, angs.b);
			vm_MatrixMul(&new_orient, &obj->orient, &rotmat);
			obj->orient = new_orient;
			obj->mtype.phys_info.rotvel *= powf(1/obj->mtype.phys_info.drag, sim_time);
		}
		#endif
		#if 0
		obj->mtype.phys_info.velocity += obj->mtype.phys_info.thrust * 200;
		vector vel = obj->mtype.phys_info.velocity;
		obj->mtype.phys_info.velocity *= powf(1/obj->mtype.phys_info.drag, sim_time);
		new_vel = obj->mtype.phys_info.velocity; // FIXME
		vector new_pos = *pos;
		new_pos += vel * sim_time;
		#else
		vector new_pos, force = obj->mtype.phys_info.thrust, moved;
		PhysicsDoSimLinear(obj, &obj->pos, &force, &new_vel, &moved, &new_pos, sim_time, 0);
		#endif
		if (!isfinite(new_pos.x) || !isfinite(new_pos.y) || !isfinite(new_pos.z)) {
			printf("thrust %f %f %f vel %f %f %f new_pos %f %f %f\n", XYZ(&obj->mtype.phys_info.thrust), XYZ(&new_vel), XYZ(&new_pos));
			new_pos = *pos;
			new_vel = obj->mtype.phys_info.velocity = {0, 0, 0};
		}
		//if (obj->type == OBJ_WEAPON)
		//	printf("thrust %f %f %f vel %f %f %f new_pos %f %f %f\n", XYZ(&obj->mtype.phys_info.thrust), XYZ(&new_vel), XYZ(&new_pos));
		obj->mtype.phys_info.rotthrust = {0, 0, 0};
		obj->mtype.phys_info.thrust = {0, 0, 0};

		if (obj->mtype.phys_info.flags & PF_NO_COLLIDE) {
			obj->mtype.phys_info.rotvel = new_rotvel;
			obj->mtype.phys_info.velocity = new_vel;
			obj->mtype.phys_info.turnroll = new_turnroll;
			ObjSetPos(obj,&new_pos,obj->roomnum,NULL);
			ObjSetAABB(obj);
			obj->last_pos = last_pos;
			//goto end;
			break;
		}

		q.ignore_obj_list = NULL;
		q.flags = FQ_RECORD | FQ_CHECK_OBJS;
		//FQ_CHECK_OBJS | FQ_IGNORE_POWERUPS | FQ_RECORD | FQ_NEW_RECORD_LIST | FQ_CHECK_CEILING;
		if (obj->type == OBJ_WEAPON)
			q.flags |= FQ_TRANSPOINT;
		if (obj->flags & OF_NO_OBJECT_COLLISIONS)
			q.flags &= ~FQ_CHECK_OBJS;
		q.startroom = obj->roomnum;
		q.thisobjnum = objnum;
		q.rad = obj->size;
		q.p1 = &new_pos;
		q.p0 = pos;
		q.o_orient = &frame_last_orient;
		q.o_rotvel = &frame_last_rotvel;
		q.o_rotthrust = &obj->mtype.phys_info.rotthrust;
		q.o_velocity = &frame_last_vel;
		q.o_turnroll = &frame_last_turnroll;
		q.o_thrust = &gravity_vec;
		hit_info.hit_turnroll = new_turnroll;
		//hit_info.hit_orient = new_orient;
		hit_info.hit_rotvel = new_rotvel;
		hit_info.hit_velocity = new_vel;

		int hit = fvi_FindIntersection(&q, &hit_info, false);
		//if (obj->type == OBJ_WEAPON)
		//	printf("hit %d at %f %f %f %d vel %f %f %f\n", hit, XYZ(&hit_info.hit_pnt), hit_info.hit_room, XYZ(&hit_info.hit_velocity));

		if (obj->type == OBJ_WEAPON && obj->ctype.laser_info.hit_status == 2) {
			laser_info_s *li = &obj->ctype.laser_info;
			vector *pnt = hit ? &li->hit_wall_pnt : &li->hit_pnt;
			vector v1 = *pnt - obj->pos;
			vector v2 = hit ? *pnt - hit_info.hit_face_pnt[0] : *pnt - hit_info.hit_pnt;
			if (v1 * v2 <= 0) {
				hit_info.hit_pnt = li->hit_pnt;
				hit_info.hit_face_pnt[0] = li->hit_wall_pnt;
				hit_info.hit_pnt = li->hit_pnt;
				hit_info.hit_room = li->hit_room;
				hit_info.hit_face_room[0] = li->hit_pnt_room;
				hit_info.hit_wallnorm[0] = li->hit_wall_normal;
				hit_info.hit_face[0] = li->hit_face;
			}
		}

		if (just_rot) {
			obj->mtype.phys_info.turnroll = hit_info.hit_turnroll;
			obj->mtype.phys_info.rotvel = hit_info.hit_rotvel;
		}
		obj->mtype.phys_info.velocity = hit_info.hit_velocity;
		if (hit) {
			switch (hit) {
				case HIT_WALL: {
					vector movedir = frame_last_pos - obj->pos;
					vm_NormalizeVector(&movedir);
					float dot = 0.1f; //movedir * hit_info.hit_wallnorm[0];  ???
					collide_object_with_wall(obj, 0, hit_info.hit_room,
						hit_info.hit_face[0], &hit_info.hit_pnt, &hit_info.hit_wallnorm[0], dot);
					break;
				}
				case HIT_OBJECT:
				case HIT_SPHERE_2_POLY_OBJECT:
					collide_two_objects(obj, Objects + hit_info.hit_object[0], hit_info.hit_face_pnt,hit_info.hit_wallnorm,&hit_info);
					break;
			}
			if (obj->flags & OF_DEAD)
				break;
		}

		if (hit) {
			if (obj->type == OBJ_WEAPON)
				ObjDelete(obj - Objects);
			else
				obj->mtype.phys_info.velocity = {0, 0, 0};
			break;
		}
		if (hit) {
			if (obj->type == OBJ_PLAYER && hit_info.num_hits > 1) {
				hit_info.hit_wallnorm[0] = {0, 0, 0};
				for (int i = 0; i < hit_info.num_hits; i++)
					hit_info.hit_wallnorm[0] += hit_info.hit_wallnorm[i];
			}
			vm_NormalizeVector(&hit_info.hit_wallnorm[0]);
		}
		if (hit == HIT_OUT_OF_TERRAIN_BOUNDS) {
			obj->flags |= OF_DEAD;
			break;
		}
		obj->last_pos = obj->pos;
		ObjSetPos(obj, &hit_info.hit_pnt, hit_info.hit_room, NULL);
		if (!hit) {
			sim_time = 0;
		} else {
			#if 0
			vm_NormalizeVector(&hit_info.hit_wallnorm[0]);
			vector moved = obj->pos;
			moved -= frame_last_pos;
			vector moved_dir = moved;
			float moved_dist;
			if (VEC_ZERO(&moved))
				moved_dist = 0;
			else
				moved_dist = vm_NormalizeVector(&moved_dir);
			//if (hit != HIT_WALL || vm_DotProduct(&moved_dir, ) >= -0.00000100 || !moved_dist)
			#endif
		}
		break;
	}

//end:
	if (drag_modified) {
		obj->mtype.phys_info.flags |= PF_GRAVITY | PF_BOUNCE;
		obj->mtype.phys_info.drag /= 300.0f;
	}
	// FIXME attach_update_pos
	obj->last_pos = last_pos;
	return;
}

void DoCycledAnim(object *obj)
{
	float upd;
	float duration;
	float from;
	float to;
	anim_elem *anim;
	float frame;
	float spc;
	float time;
	
	time = Frametime;
	if (!(obj->flags & OF_POLYGON_OBJECT))
		return;
	anim = Object_info[obj->id].anim;
	spc = anim->elem[0].spc;
	from = anim->elem[0].from;
	to = anim->elem[0].to;
	if (spc < 0)
		return;
	frame = (obj->rtype).pobj_info.anim_frame;
	if (frame < from || frame > to)
		obj->rtype.pobj_info.anim_frame = frame = from;
	duration = to - from;
	if (duration <= 0.0) {
		obj->rtype.pobj_info.anim_frame = from;
	} else {
		for (upd = (1.0 / (spc / duration)) * time; upd > duration; upd -= duration)
			;
		frame += upd;
		obj->rtype.pobj_info.anim_frame = frame;
		if (frame >= to)
			obj->rtype.pobj_info.anim_frame = frame - duration;
	}
}

void ObjCheckTriggers(object *obj)
{
	for (int i = 0; i < Fvi_num_recorded_faces; i++)
		CheckTrigger(Fvi_recorded_faces[i].room_index, Fvi_recorded_faces[i].face_index, obj, TT_PASS_THROUGH);
}

void ObjDoFrame(object *obj)
{
	float saved_Frametime = Frametime;
	switch (obj->control_type) {
		case CT_NONE:
		case CT_POWERUP:
		case CT_SOAR:
		case CT_SOUNDSOURCE:
			break;
		case CT_AI:
			AIDoFrame(obj);
			break;
		#if 0
		case CT_EXPLOSION:
			DoExplosionFrame(obj);
			break;
		case CT_FLYING:
			DoFlyingFrame(obj);
			break;
		case CT_WEAPON:
			DoWeaponFrame(obj);
			break;
		case CT_DEBRIS:
			DoDebrisFrame(obj);
			break;
		case CT_SPLINTER:
			DoSplinterFrame(obj);
			break;
		case CT_DYING:
			DoExplosionFrame(obj);
			break;
		case CT_DYING_AND_AI:
			AIDoFrame(obj);
			DoExplosionFrame(obj);
		default:
			Error("Unknown control type %d in object %i, handle/type/id = %i/%i/%i", obj->control_type,
						obj-Objects, obj->handle, obj->type, obj->id);
			break;
		#endif
	}
	int ct = obj->control_type, ot = obj->type;
	if ((ct != CT_AI) && (ct != CT_DYING_AND_AI) && (ct != CT_DEBRIS) &&
		(ot == OBJ_CLUTTER || ot == OBJ_BUILDING || ot == OBJ_ROBOT || ot == OBJ_POWERUP) &&
		(ot != OBJ_ROOM) && Object_info[obj->id].anim && Object_info[obj->id].anim->elem[0].to)
		DoCycledAnim(obj);

	int flags = obj->flags;
	if (flags & OF_DEAD) {
	    Frametime = saved_Frametime;
	    return;
	}

	switch(obj->movement_type) {
		case MT_NONE:
			if ((flags & OF_STUCK_ON_PORTAL) &&
				 !(Rooms[obj->mtype.phys_info.stuck_room].portals[obj->mtype.phys_info.stuck_portal].flags & RF_FUELCEN)) {
				ubyte oVar3 = obj->type;
				if ((oVar3 == OBJ_CLUTTER ||
					oVar3 == OBJ_BUILDING || oVar3 == OBJ_ROBOT || oVar3 == OBJ_POWERUP) &&
					(flags & OF_CLIENT_KNOWS)) {
					obj->flags = flags | OF_SEND_MULTI_REMOVE_ON_DEATH | OF_DEAD;
				}
				else {
					obj->flags = flags | OF_DEAD;
				}
			}
			break;
		case MT_PHYSICS:
			do_physics_sim(obj);
			ObjCheckTriggers(obj);
			break;
		#if 0
		case MT_WALKING:
			walking_move(obj);
			ObjCheckTriggers(obj);
			break;
		case MT_SHOCKWAVE:
			shockwave_move(obj,obj->parent_handle,1.00000000);
			break;
		case MT_OBJ_LINKED:
			PhysicsLinkList[Physics_NumLinked] = obj - Objects;
			Physics_NumLinked = Physics_NumLinked + 1;
			break;
		#endif
	}
}

void ObjDeleteDead()
{
	tOSIRISEventInfo info;
	for (int objnum = 0; objnum <= Highest_object_index; objnum++) {
		object *obj = Objects + objnum;
		if (obj->type == OBJ_NONE || !(obj->flags & OF_DEAD))
			continue;
		if (obj->flags & OF_INFORM_DESTROY_TO_LG)
			Level_goals.Inform(LIT_OBJECT,LGF_COMP_DESTROY,obj->handle);
		if (obj->flags & OF_INPLAYERINVENTORY) {
			InventoryRemoveObject(obj->handle);
		}
		if (obj->type == OBJ_DUMMY) {
			ObjUnGhostObject(objnum);
			if ((Game_mode & 0x24) && Netgame.local_role)
				MultiSendGhostObject(obj, false);
		}
		if (obj->flags & OF_POLYGON_OBJECT) {
			if (obj->flags & OF_ATTACHED) {
				info.evt_child_died.it_handle = obj->handle;
				Osiris_CallEvent(ObjGet(obj->attach_ultimate_handle),EVT_CHILD_DIED,&info);
			}
			UnattachFromParent(obj);
			UnattachChildren(obj);
		}
		info.evt_destroy.is_dying = 1;
		Osiris_CallEvent(obj,EVT_DESTROY,&info);
		if ((Game_mode & 0x24)) {
			DLLInfo.me_handle = obj->handle;
			DLLInfo.it_handle = DLLInfo.me_handle;
			CallGameDLL(EVT_GAMEOBJDESTROYED,&DLLInfo);
		}
		Osiris_DetachScriptsFromObject(obj);
		if ((Game_mode & 0x24) && Netgame.local_role) {
			if (obj->flags & OF_SEND_MULTI_REMOVE_ON_DEATH)
				MultiSendRemoveObject(obj,0);
			else if (obj->flags & OF_SEND_MULTI_REMOVE_ON_DEATHWS)
				MultiSendRemoveObject(obj,1);
		}
		#if 0
		oVar1 = obj->type;
		if ((oVar1 == OBJ_ROBOT) &&
			 ((*(ushort *)((int)poVar5 + -2) == 2 || (*(ushort *)((int)poVar5 + -2) == 0)))) {
			iVar4 = 0;
			piVar3 = Buddy_handle;
			do {
				if (poVar5[2] == *piVar3) {
					Buddy_handle[iVar4] = -1;
					break;
				}
				piVar3 = (int *)((uint *)piVar3 + 1);
				iVar4 = iVar4 + 1;
			} while ((int)piVar3 < 0x5b6ac8);
		}
		#endif
		if (obj->type != OBJ_PLAYER)
			ObjDelete(objnum);
	}
	VisEffectDeleteDead();
}

void ObjDoFrameAll() {
	for (int i = 0; i <= Highest_object_index; i++)
		if (Objects[i].type != OBJ_NONE)
			ObjDoFrame(&Objects[i]);
	VisEffectMoveAll();
	ObjDeleteDead();
}

void Jump(int num) {
	vector pos; int roomnum=-1; matrix orient;
	switch (num) {
		case 1:
			pos = {3705.27222, 201.327698, 2128.12769};roomnum=86;orient={{1,0,0},{0,1,0},{0,0,1}};//{{-0.26895839, -0.0622743145,  0.20449388}, {-0.120590039, 0.391920298, -0.0917164236}, {-0.150688574, -0.216117606, -0.211427435}};
			break;
		case 2:
			pos = {4040.13721, 239.690414, 2254.43262};roomnum=73;orient={{1,0,0},{0,1,0},{0,0,1}};//{{-0.0300339796, -0.00902694371,  0.181316838}, {0.0250956547, 0.310730785,  0.00479463348}, {-0.189535052,  0.0344414338, -0.00805729348}};
			break;
		case 3:
			pos = { 2052.465088, 269.267487, 2491.100342};roomnum= 28;orient = {{0.560977578, 0, -0.816692352}, {0, 1, 0}, {0.816692352, 0, 0.560977578}};
			break;
	}
	if (roomnum != -1)
		ObjSetPos(&Objects[0], &pos, roomnum, &orient);
}

u32 WindowFlags;
b32 Running = 1;
b32 FullScreen = 0;
bool pressed[1024];
bool mbpressed[10];
inline int maplk(int lk) { return (lk & 0x1ff) | (lk & (1<<30) ? 0x200 : 0); }
void DoControls() {
	SDL_Event Event;
	bool justpressed[1024], justmbpressed[10];
	memset(justpressed, 0, sizeof(justpressed));
	memset(justmbpressed, 0, sizeof(justmbpressed));
	while (SDL_PollEvent(&Event)) {
		if (Event.type == SDL_KEYDOWN || Event.type == SDL_KEYUP) {
			pressed[maplk(Event.key.keysym.sym)] = Event.type == SDL_KEYDOWN;
			if (Event.type == SDL_KEYDOWN)
				justpressed[maplk(Event.key.keysym.sym)] = 1;
			//printf("pressed[%d] = %d\n", maplk(Event.key.keysym.sym), Event.type == SDL_KEYDOWN);
		}
		if (Event.type == SDL_KEYDOWN) {
			switch (Event.key.keysym.sym) {
				case SDLK_ESCAPE:
					Running = 0;
					break;
				case 'f':
					FullScreen = !FullScreen;
					if (FullScreen)
						SDL_SetWindowFullscreen(Window, WindowFlags | SDL_WINDOW_FULLSCREEN_DESKTOP);
					else
						SDL_SetWindowFullscreen(Window, WindowFlags);
					break;
				default:
					break;
			}
		} else if (Event.type == SDL_MOUSEBUTTONDOWN || Event.type == SDL_MOUSEBUTTONUP) {
			mbpressed[Event.button.button] = Event.type == SDL_MOUSEBUTTONDOWN;
			if (Event.type == SDL_MOUSEBUTTONDOWN)
				justmbpressed[Event.button.button] = 1;
		} else if (Event.type == SDL_QUIT) {
			Running = 0;
		} else if (Event.type == SDL_WINDOWEVENT) {
			if (Event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
			{
				WinWidth = Event.window.data1;
				WinHeight = Event.window.data2;
				printf("size %d %d\n", WinWidth, WinHeight);

				//glMatrixMode(GL_PROJECTION);
				//glLoadIdentity();
				//glOrtho(0.f, 640.f, 480.f, 0.f, 0.f, 1.f);
				//glViewport(0, 0, WinWidth, WinHeight);
				//glMatrixMode(GL_MODELVIEW);
				//glLoadIdentity();		
			}
		}
	}

	object *obj = &Objects[0];
	float full_rot = obj->mtype.phys_info.full_rotthrust;
	float full_thrust = obj->mtype.phys_info.full_thrust;
	obj->mtype.phys_info.rotthrust.x = -((pressed[maplk(SDLK_KP_2)] ? 1.0f : 0) - (pressed[maplk(SDLK_KP_8)] ? 1.0f : 0)) * full_rot;
	obj->mtype.phys_info.rotthrust.y = ((pressed[maplk(SDLK_KP_6)] ? 1.0f : 0) - (pressed[maplk(SDLK_KP_4)] ? 1.0f : 0)) * full_rot;
	obj->mtype.phys_info.rotthrust.z = -((pressed[maplk(SDLK_KP_9)] ? 1.0f : 0) - (pressed[maplk(SDLK_KP_7)] ? 1.0f : 0)) * full_rot;
	obj->mtype.phys_info.rotthrust.x += -((pressed[maplk(SDLK_DOWN)] ? 1.0f : 0) - (pressed[maplk(SDLK_UP)] ? 1.0f : 0)) * full_rot;
	obj->mtype.phys_info.rotthrust.y += (pressed[maplk(SDLK_RIGHT)] ? 1.0f : 0) - (pressed[maplk(SDLK_LEFT)] ? 1.0f : 0) * full_rot;
	obj->mtype.phys_info.thrust = obj->orient.fvec * ((pressed[maplk(SDLK_a)] ? 1.0f : 0) - (pressed[maplk(SDLK_z)] ? 1.0f : 0)) * full_thrust;
	obj->mtype.phys_info.thrust += obj->orient.rvec * ((pressed[maplk(SDLK_KP_3)] ? 1.0f : 0) - (pressed[maplk(SDLK_KP_1)] ? 1.0f : 0)) * full_thrust;
	//obj->mtype.phys_info.thrust += obj->orient.uvec * ((pressed[maplk(SDLK_KP_ENTER)] ? 1.0f : 0) - (pressed[maplk(SDLK_DOWN)] ? 1.0f : 0));
	obj->mtype.phys_info.thrust += obj->orient.uvec * ((pressed[maplk(SDLK_KP_MINUS)] ? 1.0f : 0) - (pressed[maplk(SDLK_KP_PLUS)] ? 1.0f : 0)) * full_thrust;
	//memset(pressed, 0, sizeof(pressed));
	FireWeaponFromPlayer(obj, 0, pressed[maplk(SDLK_LCTRL)] || mbpressed[SDL_BUTTON_LEFT], 0, 0);
	FireWeaponFromPlayer(obj, 1, pressed[maplk(SDLK_SPACE)] || mbpressed[SDL_BUTTON_RIGHT], 0, 0);
	if (justpressed['1']) Jump(1);
	if (justpressed['2']) Jump(2);
	if (justpressed['3']) Jump(3);
}

void GameFrame() {
	ObjDoFrameAll();
	DoMatcensFrame();
	Level_goals.DoFrame();
	DoorwayDoFrame();



	DoControls();

	GameRenderFrame();
	ProcessNormalEvents();
	CalcFrameTime();
	Gametime += Frametime;
}


unsigned door_id, door_inst, mod_generic_call;
unsigned pow_id, pow_inst;

struct dll *mod_level;
unsigned level_id, level_inst, mod_level_call;
int *lvl_obj_handles, *lvl_obj_ids, lvl_obj_count;
unsigned lvl_obj_insts[500];
int *lvl_trigger_handles, *lvl_trigger_ids, lvl_trigger_count;
unsigned lvl_trigger_insts[500];
int is_demo;

void setup_dll() {
	dll_init();
	struct dll *mod_generic = dll_load("generic.dll");
	assert(mod_generic);
	tOSIRISModuleInit init;
	extern void osiris_setup(tOSIRISModuleInit *init);
	osiris_setup(&init);
	if (is_demo)
		init.game_checksum = 0x87888c3b;
	dllfun_call(dll_find(mod_generic, "_InitializeDLL@4"), 1, (unsigned)&init);
	door_id = dllfun_call(dll_find(mod_generic, "_GetGOScriptID@8"), 2, (unsigned)"", (unsigned)1);
	door_inst = dllfun_call(dll_find(mod_generic, "_CreateInstance@4"), 1, (unsigned)door_id);
	pow_id = dllfun_call(dll_find(mod_generic, "_GetGOScriptID@8"), 2, (unsigned)"", (unsigned)0);
	pow_inst = dllfun_call(dll_find(mod_generic, "_CreateInstance@4"), 1, (unsigned)pow_id);
	assert(door_inst);
	mod_generic_call = dll_find(mod_generic, "_CallInstanceEvent@16");
	assert(mod_generic_call);

	mod_level = dll_load("level1.dll");
	assert(mod_level);
	dllfun_call(dll_find(mod_level, "_InitializeDLL@4"), 1, (unsigned)&init);
	level_id = 0;
	level_inst = dllfun_call(dll_find(mod_level, "_CreateInstance@4"), 1, (unsigned)level_id);
	assert(level_inst);
	lvl_obj_count = dllfun_call(dll_find(mod_level, "_GetCOScriptList@8"), 2, (unsigned)&lvl_obj_handles, (unsigned)&lvl_obj_ids);
	//switch_id = dllfun_call(dll_find(mod_generic, "_GetGOScriptID@8"), 2, (unsigned)"FirstForcefieldSwi", (unsigned)0);;
	assert(lvl_obj_count < (int)(sizeof(lvl_obj_insts) / sizeof(lvl_obj_insts[0])));
	for (int i = 0; i < lvl_obj_count; i++)
		lvl_obj_insts[i] = dllfun_call(dll_find(mod_level, "_CreateInstance@4"), 1, (unsigned)lvl_obj_ids[i]);
	mod_level_call = dll_find(mod_level, "_CallInstanceEvent@16");
	assert(mod_level_call);

	int mod_level_get_trigger = dll_find(mod_level, "_GetTriggerScriptID@8");
	assert(mod_level_get_trigger);
	for (int i = 0; i < Num_triggers; i++) {
		trigger *tp = Triggers + i;
		tp->osiris_script.script_id = dllfun_call(mod_level_get_trigger, 2, tp->roomnum, tp->facenum);
		if (tp->osiris_script.script_id >= 0)
			Triggers[i].osiris_script.script_instance = (void *)dllfun_call(dll_find(mod_level, "_CreateInstance@4"), 1, tp->osiris_script.script_id);
	}

	tOSIRISEventInfo info;
	Osiris_CallLevelEvent(EVT_LEVELSTART, &info);
}

int Osiris_create_events_disabled, Osiris_event_mask;

bool Osiris_IsEventEnabled(int evt)
{
	if (evt == EVT_CREATED || evt == EVT_AI_INIT)
		return !Osiris_create_events_disabled;
	return true;
}

void Osiris_EnableCreateEvents()
{
	Osiris_create_events_disabled = false;
}

void Osiris_DisableCreateEvents()
{
	Osiris_create_events_disabled = true;
}

bool Osiris_CallLevelEvent(int evt, tOSIRISEventInfo *data) {
	bool ok;
	int extra_evt = -1;
	if (evt == EVT_AI_NOTIFY)
		switch (((tOSIRISEVTAINOTIFY *)data)->notify_type) {
			case AIN_GOAL_COMPLETE:
				extra_evt = EVT_AIN_GOALCOMPLETE;
				break;
			case AIN_GOAL_FAIL:
			case AIN_GOAL_ERROR:
			case AIN_GOAL_INVALID:
				extra_evt = EVT_AIN_GOALFAIL;
		}
	ok  = dllfun_call(mod_level_call, 4, level_id, level_inst, evt, (unsigned)data) & 1;
	if (extra_evt != -1)
		ok  = dllfun_call(mod_level_call, 4, level_id, level_inst, extra_evt, (unsigned)data) & 1;
	return ok;
}

bool Osiris_CallTriggerEvent(int handle, int evt, tOSIRISEventInfo *data) {
	bool ok;

	if ((Game_mode & 0x24) && (Netgame.local_role != 1))
		return true;
	if (Osiris_event_mask & 2)
		return true;
	if (!Osiris_IsEventEnabled(evt))
		return true;
	if (handle < 0 || handle >= Num_triggers)
		return true;
	//if (!tOSIRISCurrentLevel.loaded)
	//	return true;

	int extra_evt = -1;
	if (evt == EVT_AI_NOTIFY)
		switch (((tOSIRISEVTAINOTIFY *)data)->notify_type) {
			case AIN_GOAL_COMPLETE:
				extra_evt = EVT_AIN_GOALCOMPLETE;
				break;
			case AIN_GOAL_FAIL:
			case AIN_GOAL_ERROR:
			case AIN_GOAL_INVALID:
				extra_evt = EVT_AIN_GOALFAIL;
		}
	int id = Triggers[handle].osiris_script.script_id;
	void *instance = Triggers[handle].osiris_script.script_instance;
	if (!instance)
		return true;
	ok  = dllfun_call(mod_level_call, 4, id, (unsigned)instance, evt, (unsigned)data) & 1;
	if (extra_evt != -1)
		ok  = dllfun_call(mod_level_call, 4, id, (unsigned)instance, extra_evt, (unsigned)data) & 1;
	return ok;
}

bool Osiris_CallEvent(object *obj, int evt, tOSIRISEventInfo *data) {
	int handle = obj->handle;
	if (!handle)
		return false;
	data->me_handle = obj->handle;
	if (obj->type == OBJ_DOOR) {
		printf("call door %d %x %x %x\n", door_id, door_inst, evt, (unsigned)data);
		dllfun_call(mod_generic_call, 4, door_id, door_inst, evt, (unsigned)data);
	}
	if (obj->type == OBJ_POWERUP) {
		printf("call powerup %d %x %x %x\n", pow_id, pow_inst, evt, (unsigned)data);
		dllfun_call(mod_generic_call, 4, pow_id, pow_inst, evt, (unsigned)data);
	}
	for (int i = 0; i < lvl_obj_count; i++)
		if (handle == lvl_obj_handles[i]) {
			printf("call level object %d %x %x %x\n", lvl_obj_ids[i], lvl_obj_insts[i], evt, (unsigned)data);
			dllfun_call(mod_level_call, 4, lvl_obj_ids[i], lvl_obj_insts[i], evt, (unsigned)data);
		}
	return true;
}

void DeleteMultiplayerObjects()
{
	for (int objnum = 0; objnum <= Highest_object_index; objnum++) {
		object *obj = Objects + objnum;
		if (obj->type == OBJ_PLAYER && obj->id != Player_num)
			ObjDelete(objnum);
	}
}

void StartLevel()
{
	PageInAllData();
	//Detail_settings.Bumpmapping_enabled = rend_SupportsBumpmapping() != 0;
	MakeBOA();
	ComputeAABB(true);
	ClearAllEvents();
	ClearRoomChanges();
	//ResetMarkers();
	ResetScorches();
	//ResetWaypoint();
	//ResetLightGlows();
	DoorwayDeactivateAll();
	for (int i = 0; i <= Highest_object_index; i++)
		if (Objects[i].type == OBJ_VIEWER)
			ObjDelete(i);
	//DeleteAmbientObjects();	
	AIInitAll();
	
	InitMatcensForLevel();

	Players[Player_num].ship_index = 0;
	InitCockpit(Players[Player_num].ship_index);

	ResetReticle();

	DeleteMultiplayerObjects();
	setup_dll();

	InitPlayerNewShip(0, 1);
	InitPlayerNewLevel(0);

	Gametime = 0;
	InitFrameTime();
}

void start() {
	load_game_data(false);

	extern ushort Hud_stat_mask;
	Hud_stat_mask = 0x3ffe;

	#if 0
	//int bm[3];
	//load_and_chunk_bitmap_compr("mainmenu.ogf", &bm);
	int bm = bm_AllocLoadFileBitmap("mainmenu.ogf",0,0);
	if (bm == -1)
		fprintf(stderr, "failed to load mainmenu.ogf\n");
	chunked_bitmap ch_bm;
	bm_CreateChunkedBitmap(bm,&ch_bm);
	//draw_chbm(&ch_bm, 0, 0);
	//rend_DrawSimpleBitmap(bm, 0, 0);
	#endif

	int level_lib_id = !is_demo && cf_OpenLibrary("missions/d3.mn3");
	if (!cfexist("level1.d3l"))
		fprintf(stderr, "level missing\n");
	if (!LoadLevel("level1.d3l"))
		fprintf(stderr, "loadlevel failed\n");

	StartLevel();

	#if 0 && !defined(__EMSCRIPTEN__)
	//vector pos = {2047.683105, 276.381744, 2471.472900};int roomnum = 29;
	//vector pos = {2052.6709, 273.41095, 2126.76294};int roomnum = 34;
	//vector pos = { 2052.465088, 269.267487, 2491.100342};int roomnum= 28;matrix orient = {{0.560977578, 0, -0.816692352}, {0, 1, 0}, {0.816692352, 0, 0.560977578}};
	//vector pos = { 2222.71216, 256.480469, 2636.76367}; int roomnum = 25; matrix orient = {{0.938324869, -0.0623264313, -0.0935816839}, { 0.0657258108, 0.964604557, 0.0356049426}, {0.0954767764, -0.0448376946, 0.925018847}};
	//vector pos = {2651.24951, 294.042664,  2886.84741};int roomnum=22;matrix orient={{-0.86755532, -0.0819036365, 0.0643768609}, {-0.0717293248,0.903370023,0.084609963}, {-0.0491046086,0.0747872889,0.851385415}};
	//vector pos = {2649.94922, 290.307281, 2880.80542};int roomnum=22;matrix orient={{0.857007921, 0.0189150218, -0.0619820394}, {-0.0209699292, 0.898529649, 0.219531447}, {0.0542396083, 0.0787136108, -0.810573339}};
	//vector pos = {2851.79688, 198.613342, 2700.8562};int roomnum=0x8000a8b2;matrix orient={{0.0815096274, -0.00359472446, -0.996666074}, {0.995789409, -0.0417942591, 0.0815886706}, {-0.0419482104, -0.999119759, 0.000172953864}};
	//vector pos = {2693.27612,263.714294,2850.17114};int roomnum=13;matrix orient={{0.643244863, 0.0142674344, 0.446664929}, {0.068410866, 0.887645841, 0.0422371626}, {0.55027771, -0.12580511, -0.7013008}};
	//vector pos = {2698.7666, 262.459106, 2843.17456};int roomnum=0x8000b1a8;matrix orient={{0.643244863, 0.0142674344, 0.446664929}, {0.068410866, 0.887645841, 0.0422371626}, {0.55027771, -0.12580511, -0.7013008}};
	//orient={{0.643244863, 0.0142674344, 0.446664929}, {0.068410866, 0.887645841, 0.0422371626}, {0.55027771, -0.12580511, -0.7013008}}; //fwd
	//orient={{0.0815096274, -0.00359472446, -0.996666074}, {0.995789409, -0.0417942591, 0.0815886706}, {-0.0419482104, -0.999119759, 0.000172953864}}; //down
	//vector pos {2683.43311, 263.104706, 2856.13916};int roomnum=13;matrix orient={{0.0815096274, -0.00359472446, -0.996666074}, {0.995789409, -0.0417942591, 0.0815886706}, {-0.0419482104, -0.999119759, 0.000172953864}};
	//vector pos={2679.35181, 264.636871, 2854.21509};int roomnum=13;matrix orient={{0.755932629, -0.00473004999, -0.521401942}, {0.178152472, 0.840885103, 0.251659811}, {0.446617872, -0.291483104, 0.697822511}};
	//orient={{0.643244863, 0.0142674344, 0.446664929}, {0.068410866, 0.887645841, 0.0422371626}, {0.55027771, -0.12580511, -0.7013008}}; //fwd
	//vector pos={2647.48535, 291.724976,  2880.74878};int roomnum=22;matrix orient={{ -0.784129441, -0.0412591547, -0.107602321}, {-0.0563592128, 0.895038843, 0.103231899}, {0.133064136, 0.129844069, -0.783052742}};
	//vector pos = {3679.74658, 178.975937, 2400.09326};int roomnum=88;matrix orient={{ 0.469908446, 0.0153542738,  0.155961797}, {-0.0350852311, 0.630550563, 0.0626305044}, {-0.178150684, -0.0946229398, 0.464835405}};
	vector pos = {3705.27222, 201.327698, 2128.12769};int roomnum=86; matrix orient={{1,0,0},{0,1,0},{0,0,1}}; //{{-0.26895839, -0.0622743145,  0.20449388}, {-0.120590039, 0.391920298, -0.0917164236}, {-0.150688574, -0.216117606, -0.211427435}};
	//vector pos = {4040.13721, 239.690414, 2254.43262};int roomnum=73;matrix orient={{-0.0300339796, -0.00902694371,  0.181316838}, {0.0250956547, 0.310730785,  0.00479463348}, {-0.189535052,  0.0344414338, -0.00805729348}};



	//extern void msafe_CallFunction(ubyte,msafe_struct*);msafe_struct mstruct; mstruct.roomnum=osipf_FindRoomName("TerrainAccessRoom");mstruct.state=0;mstruct.portalnum=1;mstruct.flags=1;msafe_CallFunction(MSAFE_ROOM_PORTAL_RENDER,&mstruct);

	ObjSetPos(&Objects[0], &pos, roomnum, &orient);
	#endif
}

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <emscripten/html5.h>
int started = 0;
EM_BOOL DoFrameEm(double time, void* userData) {
	if (!started)
		start(), started = 1;
	else
		GameFrame();
	return Running;
}
#endif


extern "C" void initialize_gl4es();
int main(int argc, char **argv) {
	char hogpath[PSPATHNAME_LEN];
	//bool hogfile_opened = false;
	int hog_lib_id;

	#ifdef __EMSCRIPTEN__
	initialize_gl4es();
	#endif
	#ifdef __EMSCRIPTEN__
	ddio_MakePath(hogpath,".","d3demo.hog",NULL);
	is_demo = 1;
	#else
	ddio_MakePath(hogpath,".","d3.hog",NULL);
	#endif
	if((hog_lib_id = cf_OpenLibrary(hogpath))!=0)
	{
		//RegisterHogFile(hogpath,hog_lib_id);
		mprintf((0,"Hog file %s opened\n",hogpath));
		//hogfile_opened = true;
	} else {
		fprintf(stderr, "failed to open %s\n", hogpath);
		exit(1);
	}

	WindowFlags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE;
	Window = SDL_CreateWindow("OpenGL Test", 0, 0, WinWidth, WinHeight, WindowFlags);
	assert(Window);
	SDL_GLContext Context = SDL_GL_CreateContext(Window);
	
	Detail_settings.Fog_enabled = true;
	Detail_settings.Scorches_enabled = true;

	InitD3Systems1();
	InitD3Systems2();

	rend_Init (RENDERER_OPENGL, NULL, NULL);

	extern void SetScreenSize(int,int);
	SetScreenSize(WinWidth, WinHeight);

	setup_load_screen();

	
	#ifdef __EMSCRIPTEN__
	emscripten_request_animation_frame_loop(DoFrameEm, 0);
	#else
	start ();
	
	while (Running) {
		GameFrame();
	}
	#endif
	return 0;
}

void Debug_ConsolePrintf( int n, const char * format, ... ) {
	va_list vp;
	va_start(vp, format);
	vprintf(format, vp);
	va_end(vp);
}
void Debug_ConsolePrintf( int n, int, int, const char * format, ... ) {
	va_list vp;
	va_start(vp, format);
	vprintf(format, vp);
	va_end(vp);
}

#if 0
#ifdef __EMSCRIPTEN__
extern "C" void *emscripten_GetProcAddress(const char *name);
extern "C" void *eglGetProcAddress(const char *name) {
	return emscripten_GetProcAddress(name);
}
#endif
#endif