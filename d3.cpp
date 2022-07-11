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

void initall() {
	bm_InitBitmaps();


	InitMathTables();
	InitMatcens();

	// Initializes the fvi system
	InitFVI();

	InitSounds();

	// Initialize the Object/Object_info system
	InitObjects();
	InitObjectInfo();

	InitDoors();

	InitShips();

	InitTriggers();

	// Init our bitmaps (and lightmaps required for LoadLevel). Must be called before InitTextures and InitTerrain
	bm_InitBitmaps(); // calls lm_InitLightmaps() for us

	// Initializes the Texture system
	//InitTextures ();

	// Initialize the terrain
	InitTerrain();

	// initialize lighting systems
	InitLightmapInfo();
	InitSpecialFaces();
	//InitDynamicLighting();

	// Set z-buffer state
	//tex_SetZBufferState (Use_software_zbuffer);
	//State_changed = 1;
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

int cur_time;
void UpdateFrametime() {
	int t = SDL_GetTicks();
	Frametime = (t - cur_time) / 1000.0f;
	cur_time = t;
}

void GameRenderFrame() {
	Viewer_object = &Objects[0];
	rend_StartFrame(0,0,WinWidth,WinHeight);
	g3_StartFrame(&Viewer_object->pos,&Viewer_object->orient,Render_zoom);
	Num_fogged_rooms_this_frame = 0;
	rend_SetZBufferState(1);
	rend_SetZBufferWriteMask(1);
	ResetPostrenderList();
	RenderMine(Viewer_object->roomnum);
	PostRender(Viewer_object->roomnum);
	g3_EndFrame();
	rend_EndFrame();
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

void physics_move(object *obj) {
	fvi_info hitres;
	Fvi_num_recorded_faces = 0;
	vector *pos = &obj->pos;
	int objnum = obj - Objects;
	float sim_time = Frametime;
	bool drag_modified = false;
	vector last_pos = *pos;
	fvi_query q;

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
	if (obj->mtype.phys_info.flags & PF_WIGGLE) {
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
		int hit = fvi_FindIntersection(&q, &hitres, false);
		if (!hit)
			ObjSetPos(obj, &hitres.hit_pnt, hitres.hit_room, NULL);
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

	for (;;) {
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
		hitres.hit_turnroll = new_turnroll;
		//hitres.hit_orient = new_orient;
		hitres.hit_rotvel = new_rotvel;
		hitres.hit_velocity = new_vel;

		int hit = fvi_FindIntersection(&q, &hitres, false);
		//if (obj->type == OBJ_WEAPON)
		//	printf("hit %d at %f %f %f %d vel %f %f %f\n", hit, XYZ(&hitres.hit_pnt), hitres.hit_room, XYZ(&hitres.hit_velocity));
		if (hit) {
			if (obj->type == OBJ_WEAPON)
				ObjDelete(obj - Objects);
			else
				obj->mtype.phys_info.velocity = {0, 0, 0};
			break;
		}
		if (hit) {
			if (obj->type == OBJ_PLAYER && hitres.num_hits > 1) {
				hitres.hit_wallnorm[0] = {0, 0, 0};
				for (int i = 0; i < hitres.num_hits; i++)
					hitres.hit_wallnorm[0] += hitres.hit_wallnorm[i];
			}
			vm_NormalizeVector(&hitres.hit_wallnorm[0]);
		}
		obj->mtype.phys_info.velocity = hitres.hit_velocity;
		if (hit == HIT_OUT_OF_TERRAIN_BOUNDS) {
			obj->flags |= OF_DEAD;
			break;
		}
		obj->last_pos = obj->pos;
		ObjSetPos(obj, &hitres.hit_pnt, hitres.hit_room, NULL);
		if (!hit) {
			sim_time = 0;
		} else {
			vm_NormalizeVector(&hitres.hit_wallnorm[0]);
			vector moved = obj->pos;
			moved -= frame_last_pos;
			vector moved_dir = moved;
			float moved_dist;
			if (VEC_ZERO(&moved))
				moved_dist = 0;
			else
				moved_dist = vm_NormalizeVector(&moved_dir);
			//if (hit != HIT_WALL || vm_DotProduct(&moved_dir, ) >= -0.00000100 || !moved_dist)
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

void ObjMoveOne(object *obj) {
	float saved_Frametime = Frametime;
	#if 0
	switch (obj->control_type) {
		case CT_NONE:
		case CT_POWERUP:
		case CT_SOAR:
		case CT_SOUNDSOURCE:
			break;
		case CT_AI:
			DoAIFrame(obj);
			break;
		case CT_EXPLOSION:
			DoExplosionFrame(obj);
			break;
		default:
			Error("Unknown control type %d in object %i, handle/type/id = %i/%i/%i", obj->control_type,
						obj-Objects, obj->handle, obj->type, obj->id);
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
			DoAIFrame(obj);
			DoExplosionFrame(obj);
	}
	#endif

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
			physics_move(obj);
			//obj_moved_trigger_check(obj);
			break;
		#if 0
		case MT_WALKING:
			walking_move(obj);
			obj_moved_trigger_check(obj);
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

void ObjDoFrameAll() {
	for (int i = 0; i <= Highest_object_index; i++)
		if (Objects[i].type != OBJ_NONE)
			ObjMoveOne(&Objects[i]);
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
	float full_thrust = obj->mtype.phys_info.full_thrust;
	obj->mtype.phys_info.rotthrust.x = -((pressed[maplk(SDLK_KP_2)] ? 1.0f : 0) - (pressed[maplk(SDLK_KP_8)] ? 1.0f : 0));
	obj->mtype.phys_info.rotthrust.y = (pressed[maplk(SDLK_KP_6)] ? 1.0f : 0) - (pressed[maplk(SDLK_KP_4)] ? 1.0f : 0);
	obj->mtype.phys_info.rotthrust.z = -((pressed[maplk(SDLK_KP_9)] ? 1.0f : 0) - (pressed[maplk(SDLK_KP_7)] ? 1.0f : 0));
	obj->mtype.phys_info.rotthrust.x += -((pressed[maplk(SDLK_DOWN)] ? 1.0f : 0) - (pressed[maplk(SDLK_UP)] ? 1.0f : 0));
	obj->mtype.phys_info.rotthrust.y += (pressed[maplk(SDLK_RIGHT)] ? 1.0f : 0) - (pressed[maplk(SDLK_LEFT)] ? 1.0f : 0);
	obj->mtype.phys_info.thrust = obj->orient.fvec * ((pressed[maplk(SDLK_a)] ? 1.0f : 0) - (pressed[maplk(SDLK_z)] ? 1.0f : 0)) * full_thrust;
	obj->mtype.phys_info.thrust += obj->orient.rvec * ((pressed[maplk(SDLK_KP_3)] ? 1.0f : 0) - (pressed[maplk(SDLK_KP_1)] ? 1.0f : 0)) * full_thrust;
	//obj->mtype.phys_info.thrust += obj->orient.uvec * ((pressed[maplk(SDLK_KP_ENTER)] ? 1.0f : 0) - (pressed[maplk(SDLK_DOWN)] ? 1.0f : 0));
	obj->mtype.phys_info.thrust += obj->orient.uvec * ((pressed[maplk(SDLK_KP_MINUS)] ? 1.0f : 0) - (pressed[maplk(SDLK_KP_PLUS)] ? 1.0f : 0)) * full_thrust;
	//memset(pressed, 0, sizeof(pressed));
	FireWeaponFromPlayer(obj, 0, pressed[maplk(SDLK_LCTRL)] || mbpressed[SDL_BUTTON_LEFT], 0, 0);
	FireWeaponFromPlayer(obj, 1, pressed[maplk(SDLK_SPACE)] || mbpressed[SDL_BUTTON_RIGHT], 0, 0);
}

void DoFrame() {
	ObjDoFrameAll();
	DoControls();

	GameRenderFrame();
	UpdateFrametime();
	Gametime += Frametime;
}

void start() {
	load_game_data(false);

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

	int level_lib_id = cf_OpenLibrary("/home/arne/pkg/descent3/missions/d3.mn3");
	printf("libid %d\n", level_lib_id);
	if (!cfexist("level1.d3l"))
		fprintf(stderr, "level missing\n");
	if (!LoadLevel("level1.d3l"))
		fprintf(stderr, "loadlevel failed\n");

	PageInAllData();

	InitPlayerNewShip(0, 1);
	InitPlayerNewLevel(0);

	#if 0
	//vector pos = {2047.683105, 276.381744, 2471.472900};int roomnum = 29;
	//vector pos = {2052.6709, 273.41095, 2126.76294};int roomnum = 34;
	vector pos = { 2052.465088, 269.267487, 2491.100342};int roomnum= 28;matrix orient = {{0.560977578, 0, -0.816692352}, {0, 1, 0}, {0.816692352, 0, 0.560977578}};
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
		DoFrame();
	return Running;
}
#endif


extern "C" void initialize_gl4es();
int main(int argc, char **argv) {
	char hogpath[PSPATHNAME_LEN];
	bool hogfile_opened = false;
	int hog_lib_id;

	#ifdef __EMSCRIPTEN__
	initialize_gl4es();
	ddio_MakePath(hogpath,".","d3demo.hog",NULL);
	#else
	ddio_MakePath(hogpath,"/home/arne/pkg/descent3","d3.hog",NULL);
	#endif
	if((hog_lib_id = cf_OpenLibrary(hogpath))!=0)
	{
		//RegisterHogFile(hogpath,hog_lib_id);
		mprintf((0,"Hog file %s opened\n",hogpath));
		hogfile_opened = true;
	} else {
		fprintf(stderr, "failed to open %s\n", hogpath);
		exit(1);
	}

	WindowFlags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE;
	Window = SDL_CreateWindow("OpenGL Test", 0, 0, WinWidth, WinHeight, WindowFlags);
	assert(Window);
	SDL_GLContext Context = SDL_GL_CreateContext(Window);
	
	Detail_settings.Fog_enabled = true;

	initall();

	rend_Init (RENDERER_OPENGL, NULL, NULL);

	setup_load_screen();

	
	#ifdef __EMSCRIPTEN__
	emscripten_request_animation_frame_loop(DoFrameEm, 0);
	#else
	start ();
	
	while (Running) {
		DoFrame();
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

#define INCLUDED_FROM_D3
#include "osiris_common.h"
bool Osiris_CallEvent(object *obj, int event, tOSIRISEventInfo *data ) { return true; }
