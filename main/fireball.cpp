#include "fireball.h"
#include "psrand.h"
#include "viseffect.h"

int CreateFireball(vector *pos,int fireball_num,int roomnum,int realtype)
{
	int id;
	float time;

	if (realtype == 0)
		return VisEffectCreate(1,fireball_num,roomnum,pos);
	id = ObjCreate(OBJ_FIREBALL,fireball_num,roomnum,pos,NULL,-1);
	if (id < 0)
		return -1;
	time = Fireballs[fireball_num].total_life;
	Objects[id].size = Fireballs[fireball_num].size;
	Objects[id].flags |= OF_USES_LIFELEFT;
	Objects[id].lifeleft = time;
	Objects[id].lifetime = time;
	return id;
}

int GetRandomMediumExplosion()
{
	int explosions[] = { MED_EXPLOSION_INDEX, MED_EXPLOSION_INDEX2, MED_EXPLOSION_INDEX3 };
	return explosions[ps_rand() % 3];
}

int GetRandomSmallExplosion()
{
	int explosions[] = { SMALL_EXPLOSION_INDEX, SMALL_EXPLOSION_INDEX2 };
	return explosions[ps_rand() % 2];
}

int GetRandomBillowingExplosion()
{
	int explosions[] = { BILLOWING_INDEX, MED_EXPLOSION_INDEX2 };
	return explosions[ps_rand() % 2];
}

int GetRandomExplosion(float size)
{
	return size > 15 ? BIG_EXPLOSION_INDEX : GetRandomMediumExplosion();
}


int CreateObjectFireball(object *obj,float size)
{
	int visnum;
  
	size *= obj->size;
	visnum = CreateFireball(&obj->pos,GetRandomExplosion(size),obj->roomnum,0);
	if (visnum >= 0)
		VisEffects[visnum].size = size * 2;
	return visnum;
}

void InitFireballs()
{
	fireball *fb;
	int handle;

	for (int i = 0; i < NUM_FIREBALLS; i++) {
		fb = Fireballs + i;
		if (stricmp(fb->name, "NOIMAGE") == 0)
			continue;
		handle = LoadTextureImage(fb->name,NULL,fb->tex_size,0,0,0);
		if (handle < 0) {
			Error("Couldn't load fireball %s in InitFireballs.", fb->name);
			return;
		}
		fb->bm_handle = handle;
	}
	handle = Fireballs[GRAY_SPARK_INDEX].bm_handle;
	ushort *data = bm_data(handle,0);
	int size = bm_h(handle,0) * bm_w(handle,0);
	for (int i = 0; i < size; i++) {
		ushort pix = data[i];
		if (pix == 0x7e0) {
			data[i] = 0;
		} else {
			float light = ((pix & 0x1f) * 0.11 + ((pix >> 6) & 0x1f) * 0.59 + (pix >> 11) * 0.3) / 32;
			ubyte val = light > 1 ? 255 : light * 255;
			data[i] = GR_RGB16(val, val, val) | OPAQUE_FLAG16;
		}
	}
	GameBitmaps[handle].flags |= BF_CHANGED;
}

