#include "game.h"
#include "3d.h"
#include "config.h"
#include "psrand.h"

#define MAX_SCORCHES 500
int Scorch_texture_handles[10];
int Num_scorch_textures;
int Scorch_start, Scorch_end;

int Num_scorches_to_render;
ushort Scorches_to_render[MAX_FACES_PER_ROOM];

typedef struct scorchmark {
	int roomface;
	vector pos;
	ubyte bm_idx;
	sbyte orient_rx;
	sbyte orient_ry;
	sbyte orient_rz;
	sbyte orient_ux;
	sbyte orient_uy;
	sbyte orient_uz;
	ubyte size;
} scorchmark;

scorchmark Scorches[MAX_SCORCHES];

void ResetScorches()
{
	Scorch_end = -1;
	Scorch_start = -1;
}

void DeleteScorch(int id)
{
	int roomface = Scorches[id].roomface;
	for (int i = Scorch_start;;) {
		if (Scorches[i].roomface == roomface && i != id)
			return;
		if (i == Scorch_end)
			break;
		if (++i == MAX_SCORCHES)
			i = 0;
	}
	Rooms[roomface >> 16].faces[roomface & 0xffff].flags &= ~FF_SCORCHED;
}


void AddScorch(int roomnum,int facenum,vector *pos,int handle,float size)
{
	if (roomnum & 0x80000000)
		return;
	room *rp = &Rooms[roomnum];
	face *fp = &rp->faces[facenum];
	if (GameTextures[fp->tmap].flags & (TF_LIGHT | TF_PROCEDURAL))
		return;
	int roomface = roomnum * 0x10000 + facenum;
	int sizes = 0;
	for (int i = Scorch_start; ; i = i == MAX_SCORCHES ? 0 : i + 1) {
		scorchmark *s = Scorches + i;
		if (s->roomface == roomface) {
			int size = s->size / 16.0f;
			if (size < 1)
				size = 1;
			sizes += size;
			if (sizes >= 30)
				return;
		}
		if (i == Scorch_end)
			break;
	}

	vector *last = rp->verts + fp->face_verts[fp->num_verts - 1];
	for (int i = 0; i < fp->num_verts; i++) {
		vector dir, *cur = rp->verts + fp->face_verts[i];
		vm_GetNormalizedDir(&dir, cur, last);
		vector off = *pos - *last;
		float dot = dir * off;
		vector dirdot = *last + dir * dot;
		if (vm_VectorDistance(&dirdot, pos) < size)
			return;
		last = cur;
	}
		
	int id = Scorch_end + 1;
	if (id == MAX_SCORCHES)
		id = 0;
	if (id == Scorch_start) {
		DeleteScorch(Scorch_start);
		Scorch_start = Scorch_start + 1;
		if (Scorch_start == MAX_SCORCHES)
			Scorch_start = 0;
	}
	if (Scorch_start == -1)
		Scorch_start = 0;
	Scorch_end = id;
	Scorches[id].roomface = roomface;
	Scorches[id].pos = *pos;
	Scorches[id].size = size * 16.0f;
	int tex_idx;
	for (tex_idx = 0; tex_idx < Num_scorch_textures; tex_idx++)
		if (Scorch_texture_handles[tex_idx] == handle)
			break;
	if (tex_idx == Num_scorch_textures) {
		Num_scorch_textures = Num_scorch_textures + 1;
		Scorch_texture_handles[tex_idx] = handle;
	}
	Scorches[id].bm_idx = tex_idx;

	matrix orient;
	int angle = ps_rand() * 2; //60962; //ps_rand() * 2
	vm_VectorAngleToMatrix(&orient,&fp->normal,angle);
	Scorches[id].orient_rx = orient.rvec.x * -127.0f;
	Scorches[id].orient_ry = orient.rvec.y * -127.0f;
	Scorches[id].orient_rz = orient.rvec.z * -127.0f;
	Scorches[id].orient_ux = orient.uvec.x * 127.0f;
	Scorches[id].orient_uy = orient.uvec.y * 127.0f;
	Scorches[id].orient_uz = orient.uvec.z * 127.0f;
	fp->flags |= FF_SCORCHED;
}

float scorch_uvs[4][2] = {{0,0},{1,0},{1,1},{0,1}};

void DrawScorches(int roomnum, int facenum) {
	g3Point pnts[4];
	g3Point *pps[4];

	if (!Detail_settings.Scorches_enabled)
		return;

	int roomface = roomnum * 0x10000 + facenum;

	if (!StateLimited) {
		rend_SetAlphaType(AT_LIGHTMAP_BLEND);
		rend_SetAlphaValue(255);
		rend_SetLighting(LS_NONE);
		rend_SetColorModel(CM_MONO);
		rend_SetOverlayType(OT_NONE);
		rend_SetZBias(-0.5);
		rend_SetZBufferWriteMask(0);
		rend_SetTextureType(TT_LINEAR);
	}
	for (int id = Scorch_start; ; id = id == MAX_SCORCHES - 1 ? 0 : id + 1) {
		scorchmark *s = &Scorches[id];
		if (s->roomface == roomface) {
			float size = s->size / 16;
			float depth = g3_CalcPointDepth(&s->pos);
			if (depth <= 200.0) {
				if (depth > 170.0)
					size *= 1.0f - (depth - 170.0f) / 3;
				size /= 127;
				vector r = {s->orient_rx * size, s->orient_ry * size, s->orient_rz * size};
				vector u = {s->orient_ux * size, s->orient_uy * size, s->orient_uz * size};
				vector verts[4];
				verts[0] = s->pos - r + u;
				verts[1] = s->pos + r + u;
				verts[2] = s->pos + r - u;
				verts[3] = s->pos - r - u;
				for (int i = 0; i < 4; i++) {
					g3Point *pnt = pps[i] = &pnts[i];
					g3_RotatePoint(pnt, &verts[i]);
					pnt->p3_uvl.u = scorch_uvs[i][0];
					pnt->p3_uvl.v = scorch_uvs[i][1];
					pnt->p3_uvl.l = 1.0f;
					pnt->p3_flags |= PF_UV | PF_L;
				}
				g3_DrawPoly(4,pps,GetTextureBitmap(Scorch_texture_handles[s->bm_idx],0,false));
			}
		}
		if (id == Scorch_end)
			break;
	}

	if (!StateLimited) {
		rend_SetZBias(0.0);
		rend_SetZBufferWriteMask(1);
	}
}

void RenderScorchesForRoom(room *rp)
{
	if (!Detail_settings.Scorches_enabled)
		return;
	rend_SetAlphaType(AT_LIGHTMAP_BLEND);
	rend_SetAlphaValue(255);
	rend_SetLighting(LS_NONE);
	rend_SetColorModel(CM_MONO);
	rend_SetOverlayType(OT_NONE);
	rend_SetZBias(-0.5);
	rend_SetZBufferWriteMask(0);
	rend_SetTextureType(TT_LINEAR);
	for (int i = 0; i < Num_scorches_to_render; i++)
		DrawScorches(rp - Rooms, Scorches_to_render[i]);
	rend_SetZBias(0.0);
	rend_SetZBufferWriteMask(1);
}

