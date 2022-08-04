#include "game.h"
#include "osiris_common.h"
#include "doorway.h"
#include "multi.h"


void msafe_GetValue(ubyte type,msafe_struct *mstruct)
{
	char cVar1;
	ubyte oVar2;
	ushort uVar3;
	int oVar4;
	ubyte uVar5;
	sbyte bVar6;
	physics_info *ppVar7;
	vector *pvVar8;
	object *obj;
	light_info *plVar9;
	uint uVar10;
	bool valid;
	object *poVar11;
	int iVar12;
	int iVar13;
	uint uVar14;
	//Inventory *pIVar15;
	matrix *pmVar16;
	vector *pvVar17;
	//undefined4 *puVar18;
	matrix *pmVar19;
	char *pcVar20;
	char *pcVar21;
	float fVar22;
	
	switch(type) {
#if 0
	case MSAFE_ROOM_PORTAL_RENDER:
		valid = room_and_portal_valid(mstruct->roomnum,(int)mstruct->portalnum);
		if (valid == false) {
			mstruct->state = '\0';
			return;
		}
		if ((*(byte *)&Rooms[mstruct->roomnum].portals[mstruct->portalnum].flags & 1) != 0) {
			mstruct->state = '\x01';
			return;
		}
		goto LAB_00490713;
	case MSAFE_ROOM_FOG_STATE:
		valid = room_valid(mstruct->roomnum);
		if (valid != false) {
			mstruct->state = (byte)((uint)Rooms[mstruct->roomnum].flags >> 9) & 1;
			return;
		}
		goto LAB_00490713;
	case MSAFE_ROOM_DAMAGE:
		valid = room_valid(mstruct->roomnum);
		if (valid != false) {
			mstruct->amount = Rooms[mstruct->roomnum].damage;
			mstruct->index = (uint)Rooms[mstruct->roomnum].damage_type;
			return;
		}
		mstruct->amount = 0.0;
		mstruct->index = 0;
		return;
	case MSAFE_ROOM_HAS_PLAYER:
		valid = room_valid(mstruct->roomnum);
		if (valid == false) {
			mstruct->state = '\0';
			return;
		}
		iVar12 = Rooms[mstruct->roomnum].objects;
		valid = iVar12 == -1;
		if (!valid) {
			do {
				if (Objects[iVar12].type == OBJ_PLAYER) break;
				iVar12 = (int)Objects[iVar12].next;
			} while (iVar12 != -1);
			valid = iVar12 == -1;
		}
		mstruct->state = !valid;
		return;
	case MSAFE_ROOM_PORTAL_BLOCK:
		valid = room_and_portal_valid(mstruct->roomnum,(int)mstruct->portalnum);
		if (valid == false) {
			mstruct->state = '\0';
			return;
		}
		if ((*(byte *)&Rooms[mstruct->roomnum].portals[mstruct->portalnum].flags & 0x20) != 0) {
			mstruct->state = '\x01';
			return;
		}
		goto LAB_00490713;
	case MSAFE_OBJECT_SHIELDS:
		obj = ObjGet(mstruct->objhandle);
		if (obj != (object *)0x0) {
			mstruct->shields = obj->shields;
			return;
		}
		goto LAB_0048fa6f;
	case MSAFE_OBJECT_ENERGY:
		obj = ObjGet(mstruct->objhandle);
		if ((obj != (object *)0x0) && (obj = ObjGet(mstruct->objhandle), obj->type == OBJ_PLAYER)) {
			obj = ObjGet(mstruct->objhandle);
			mstruct->energy = Players[obj->id].energy;
			return;
		}
		mstruct->energy = 0.0;
		return;
	case MSAFE_OBJECT_MOVEMENT_SCALAR:
		obj = ObjGet(mstruct->objhandle);
		if ((obj != (object *)0x0) && (obj = ObjGet(mstruct->objhandle), obj->type == OBJ_PLAYER)) {
			obj = ObjGet(mstruct->objhandle);
			mstruct->scalar = Players[obj->id].movement_scalar;
			return;
		}
		goto LAB_0048fb88;
	case MSAFE_OBJECT_RECHARGE_SCALAR:
		obj = ObjGet(mstruct->objhandle);
		if ((obj != (object *)0x0) && (obj = ObjGet(mstruct->objhandle), obj->type == OBJ_PLAYER)) {
			obj = ObjGet(mstruct->objhandle);
			mstruct->scalar = Players[obj->id].weapon_recharge_scalar;
			return;
		}
		goto LAB_0048fb88;
	case MSAFE_OBJECT_WSPEED_SCALAR:
		obj = ObjGet(mstruct->objhandle);
		if ((obj != (object *)0x0) && (obj = ObjGet(mstruct->objhandle), obj->type == OBJ_PLAYER)) {
			obj = ObjGet(mstruct->objhandle);
			mstruct->scalar = Players[obj->id].weapon_speed_scalar;
			return;
		}
		goto LAB_0048fb88;
	case MSAFE_OBJECT_DAMAGE_SCALAR:
		obj = ObjGet(mstruct->objhandle);
		if ((obj != (object *)0x0) && (obj = ObjGet(mstruct->objhandle), obj->type == OBJ_PLAYER)) {
			obj = ObjGet(mstruct->objhandle);
			mstruct->scalar = Players[obj->id].damage_scalar;
			return;
		}
		goto LAB_0048fb88;
	case MSAFE_OBJECT_ARMOR_SCALAR:
		obj = ObjGet(mstruct->objhandle);
		if ((obj != (object *)0x0) && (obj = ObjGet(mstruct->objhandle), obj->type == OBJ_PLAYER)) {
			obj = ObjGet(mstruct->objhandle);
			mstruct->scalar = Players[obj->id].armor_scalar;
			return;
		}
LAB_0048fb88:
		mstruct->scalar = 0.0;
		return;
#endif
	case MSAFE_OBJECT_TYPE:
		obj = ObjGet(mstruct->objhandle);
		if (obj != (object *)0x0) {
			mstruct->type = (uint)obj->type;
			return;
		}
		mstruct->type = -1;
		return;
	case MSAFE_OBJECT_ID:
		obj = ObjGet(mstruct->objhandle);
		if (obj != (object *)0x0) {
			mstruct->id = (uint)obj->id;
			return;
		}
		mstruct->id = -1;
		return;
#if 0
	case MSAFE_OBJECT_POS:
		obj = ObjGet(mstruct->objhandle);
		if (obj == (object *)0x0) {
			(mstruct->pos).x = 0.0;
			(mstruct->pos).y = 0.0;
			(mstruct->pos).z = 0.0;
			return;
		}
		pvVar8 = &obj->pos;
		pvVar17 = &mstruct->pos;
		goto LAB_0048f8bc;
	case MSAFE_OBJECT_ORIENT:
		obj = ObjGet(mstruct->objhandle);
		if (obj != (object *)0x0) {
			pmVar16 = &obj->orient;
			pmVar19 = &mstruct->orient;
			for (iVar12 = 9; iVar12 != 0; iVar12 = iVar12 + -1) {
				(pmVar19->rvec).x = (pmVar16->rvec).x;
				pmVar16 = (matrix *)&(pmVar16->rvec).y;
				pmVar19 = (matrix *)&(pmVar19->rvec).y;
			}
			return;
		}
		pmVar16 = &unit_matrix;
		pmVar19 = &mstruct->orient;
		for (iVar12 = 9; iVar12 != 0; iVar12 = iVar12 + -1) {
			(pmVar19->rvec).x = (pmVar16->rvec).x;
			pmVar16 = (matrix *)&(pmVar16->rvec).y;
			pmVar19 = (matrix *)&(pmVar19->rvec).y;
		}
		return;
	case MSAFE_OBJECT_ROOMNUM:
		obj = ObjGet(mstruct->objhandle);
		if (obj != (object *)0x0) {
			mstruct->roomnum = obj->roomnum;
			return;
		}
		mstruct->roomnum = -1;
		return;
	case MSAFE_OBJECT_VELOCITY:
		obj = ObjGet(mstruct->objhandle);
		if ((obj == (object *)0x0) ||
			 ((obj->movement_type != MT_WALKING && (obj->movement_type != MT_PHYSICS)))) {
			pvVar8 = &mstruct->velocity;
LAB_0048f872:
			pvVar8->x = 0.0;
			pvVar8->y = 0.0;
			pvVar8->z = 0.0;
			return;
		}
		ppVar7 = &obj->mtype;
		pvVar8 = &mstruct->velocity;
		goto LAB_0048f857;
	case MSAFE_OBJECT_ROTVELOCITY:
		obj = ObjGet(mstruct->objhandle);
		if ((obj == (object *)0x0) ||
			 ((obj->movement_type != MT_WALKING && (obj->movement_type != MT_PHYSICS)))) {
			pvVar8 = &mstruct->rot_velocity;
LAB_0048f8d7:
			pvVar8->x = 0.0;
			pvVar8->y = 0.0;
			pvVar8->z = 0.0;
			return;
		}
		pvVar8 = &(obj->mtype).rotvel;
		pvVar17 = &mstruct->rot_velocity;
		goto LAB_0048f8bc;
	case MSAFE_OBJECT_THRUST:
		obj = ObjGet(mstruct->objhandle);
		if ((obj == (object *)0x0) ||
			 ((obj->movement_type != MT_WALKING && (obj->movement_type != MT_PHYSICS)))) {
			pvVar8 = &mstruct->thrust;
			goto LAB_0048f872;
		}
		ppVar7 = (physics_info *)&(obj->mtype).thrust;
		pvVar8 = &mstruct->thrust;
LAB_0048f857:
		pvVar8->x = (ppVar7->velocity).x;
		pvVar8->y = (ppVar7->velocity).y;
		pvVar8->z = (ppVar7->velocity).z;
		return;
	case MSAFE_OBJECT_ROTTHRUST:
		obj = ObjGet(mstruct->objhandle);
		if ((obj == (object *)0x0) ||
			 ((obj->movement_type != MT_WALKING && (obj->movement_type != MT_PHYSICS)))) {
			pvVar8 = &mstruct->rot_thrust;
			goto LAB_0048f8d7;
		}
		pvVar8 = &(obj->mtype).rotthrust;
		pvVar17 = &mstruct->rot_thrust;
LAB_0048f8bc:
		pvVar17->x = pvVar8->x;
		pvVar17->y = pvVar8->y;
		pvVar17->z = pvVar8->z;
		return;
	case MSAFE_OBJECT_FLAGS:
		obj = ObjGet(mstruct->objhandle);
		if (obj != (object *)0x0) {
			mstruct->flags = obj->flags;
			return;
		}
		mstruct->flags = 0;
		return;
	case MSAFE_OBJECT_SIZE:
		obj = ObjGet(mstruct->objhandle);
		if (obj != (object *)0x0) {
			mstruct->size = obj->size;
			return;
		}
		mstruct->size = 0.0;
		return;
	case MSAFE_OBJECT_CONTROL_TYPE:
		obj = ObjGet(mstruct->objhandle);
		if (obj != (object *)0x0) {
			mstruct->control_type = obj->control_type;
			return;
		}
		mstruct->control_type = '\0';
		return;
	case MSAFE_OBJECT_MOVEMENT_TYPE:
		obj = ObjGet(mstruct->objhandle);
		if (obj != (object *)0x0) {
			mstruct->movement_type = obj->movement_type;
			return;
		}
		mstruct->movement_type = '\0';
		return;
	case MSAFE_OBJECT_CREATION_TIME:
		obj = ObjGet(mstruct->objhandle);
		if (obj != (object *)0x0) {
			mstruct->creation_time = obj->creation_time;
			return;
		}
		mstruct->creation_time = 0.0;
		return;
	case MSAFE_OBJECT_PHYSICS_FLAGS:
		obj = ObjGet(mstruct->objhandle);
		if ((obj != (object *)0x0) &&
			 ((obj->movement_type == MT_WALKING || (obj->movement_type == MT_PHYSICS)))) {
			mstruct->physics_flags = (obj->mtype).flags;
			return;
		}
		mstruct->physics_flags = 0;
		return;
#endif
	case MSAFE_OBJECT_PARENT:
		obj = ObjGet(mstruct->objhandle);
		if (obj != (object *)0x0) {
			obj = ObjGetUltimateParent(obj);
			mstruct->objhandle = obj->handle;
			return;
		}
		break;
#if 0
	case MSAFE_OBJECT_ROTDRAG:
		obj = ObjGet(mstruct->objhandle);
		if ((obj != (object *)0x0) &&
			 ((obj->movement_type == MT_WALKING || (obj->movement_type == MT_PHYSICS)))) {
			mstruct->rot_drag = (obj->mtype).rotdrag;
			return;
		}
		mstruct->rot_drag = 1.0;
		return;
	case MSAFE_OBJECT_INVULNERABLE:
		obj = ObjGet(mstruct->objhandle);
		if (obj != (object *)0x0) {
			obj = ObjGet(mstruct->objhandle);
			if (obj->type != OBJ_PLAYER) {
				obj = ObjGet(mstruct->objhandle);
				oVar4 = obj->flags;
				mstruct->lifetime = 0.0;
				mstruct->state = ~(byte)((uint)oVar4 >> 10) & 1;
				return;
			}
			obj = ObjGet(mstruct->objhandle);
			bVar6 = *(byte *)&Players[obj->id].flags & 1;
			mstruct->state = bVar6;
			if (bVar6 != 0) {
				obj = ObjGet(mstruct->objhandle);
				mstruct->lifetime = Players[obj->id].invulnerable_time;
				return;
			}
			goto LAB_0048ff2b;
		}
		goto LAB_0048ff24;
	case MSAFE_OBJECT_CLOAK:
		obj = ObjGet(mstruct->objhandle);
		if (obj != (object *)0x0) {
			obj = ObjGet(mstruct->objhandle);
			if (obj->effect_info == (effect_info_s *)0x0) {
				mstruct->state = '\0';
			}
			else {
				obj = ObjGet(mstruct->objhandle);
				bVar6 = *(byte *)&obj->effect_info->type_flags & 1;
				mstruct->state = bVar6;
				if (bVar6 != 0) {
					obj = ObjGet(mstruct->objhandle);
					mstruct->lifetime = obj->effect_info->cloak_time;
					return;
				}
			}
			mstruct->lifetime = 0.0;
			return;
		}
LAB_0048ff24:
		mstruct->state = '\0';
LAB_0048ff2b:
		mstruct->lifetime = 0.0;
		return;
	case MSAFE_OBJECT_PLAYER_HANDLE:
		cVar1 = mstruct->slot;
		mstruct->objhandle = 0xffffffff;
		if ((-1 < cVar1) && (cVar1 < ' ')) {
			iVar12 = (int)cVar1;
			if (Players[iVar12].objnum != -1) {
				if ((Game_mode & 0x24) == 0) {
					valid = cVar1 == '\0';
				}
				else {
					if ((*(byte *)(&NetPlayers_flags + iVar12 * 0x15e) & 1) == 0) {
						return;
					}
					valid = (&NetPlayers_sequence)[iVar12 * 0x578] == '\r';
				}
				if (valid) {
					mstruct->objhandle = Objects[Players[iVar12].objnum].handle;
					return;
				}
			}
		}
		break;
	case MSAFE_OBJECT_PLAYER_CMASK:
		obj = ObjGet(mstruct->objhandle);
		if ((obj != (object *)0x0) && (obj->type == OBJ_PLAYER)) {
			mstruct->control_mask = Players[obj->id].controller_bitflags;
			return;
		}
		mstruct->control_mask = 0;
		return;
	case MSAFE_OBJECT_LIGHT_DIST:
		mstruct->light_distance = 0.0;
		obj = ObjGet(mstruct->objhandle);
		if ((obj != (object *)0x0) && (plVar9 = ObjGetLightInfo(obj), plVar9 != (light_info *)0x0)) {
			mstruct->light_distance = plVar9->light_distance;
			return;
		}
		break;
	case MSAFE_OBJECT_PLAYER_CONTROLAI:
		cVar1 = mstruct->slot;
		mstruct->state = '\0';
		if ((-1 < cVar1) && (cVar1 < ' ')) {
			iVar12 = (int)cVar1;
			if (Players[iVar12].objnum != -1) {
				if ((Game_mode & 0x24) == 0) {
					if (cVar1 == '\0') goto LAB_004900f8;
				}
				else if (((*(byte *)(&NetPlayers_flags + iVar12 * 0x15e) & 1) != 0) &&
								((&NetPlayers_sequence)[iVar12 * 0x578] == '\r')) {
LAB_004900f8:
					mstruct->state = Objects[Players[iVar12].objnum].control_type == CT_AI;
					return;
				}
			}
		}
		break;
	case MSAFE_OBJECT_WORLD_POSITION:
		obj = ObjGet(mstruct->objhandle);
		if (obj != (object *)0x0) {
			pmVar16 = &obj->orient;
			pmVar19 = &mstruct->orient;
			for (iVar12 = 9; iVar12 != 0; iVar12 = iVar12 + -1) {
				(pmVar19->rvec).x = (pmVar16->rvec).x;
				pmVar16 = (matrix *)&(pmVar16->rvec).y;
				pmVar19 = (matrix *)&(pmVar19->rvec).y;
			}
			mstruct->roomnum = obj->roomnum;
			(mstruct->pos).x = (obj->pos).x;
			(mstruct->pos).y = (obj->pos).y;
			(mstruct->pos).z = (obj->pos).z;
			return;
		}
		(mstruct->pos).x = 0.0;
		(mstruct->pos).y = 0.0;
		(mstruct->pos).z = 0.0;
		pmVar16 = &unit_matrix;
		pmVar19 = &mstruct->orient;
		for (iVar12 = 9; iVar12 != 0; iVar12 = iVar12 + -1) {
			(pmVar19->rvec).x = (pmVar16->rvec).x;
			pmVar16 = (matrix *)&(pmVar16->rvec).y;
			pmVar19 = (matrix *)&(pmVar19->rvec).y;
		}
		mstruct->roomnum = -1;
		return;
	case MSAFE_OBJECT_COUNT_TYPE:
		iVar12 = 0;
		obj = Objects;
		if (-1 < Highest_object_index) {
			iVar13 = Highest_object_index + 1;
			do {
				if (((uint)obj->type == mstruct->type) && ((uint)obj->id == mstruct->id)) {
					iVar12 = iVar12 + 1;
				}
				obj = obj + 1;
				iVar13 = iVar13 + -1;
			} while (iVar13 != 0);
		}
		mstruct->count = iVar12;
		return;
	case MSAFE_OBJECT_ENERGY_WEAPON:
		obj = ObjGet(mstruct->objhandle);
		if (((obj == (object *)0x0) || (obj->type != OBJ_WEAPON)) ||
			 ((*(byte *)&Weapons[obj->id].flags & 8) != 0)) {
			uVar5 = '\0';
		}
		else {
			uVar5 = '\x01';
		}
		mstruct->state = uVar5;
	case MSAFE_OBJECT_DAMAGE_AMOUNT:
		mstruct->amount = 0.0;
		obj = ObjGet(mstruct->objhandle);
		if ((obj != (object *)0x0) && (obj->type == OBJ_WEAPON)) {
			mstruct->amount = Weapons[obj->id].generic_damage * *(float *)&(obj->ctype).field_0x44;
			return;
		}
		break;
	case MSAFE_OBJECT_SHIELDS_ORIGINAL:
		obj = ObjGet(mstruct->objhandle);
		if ((obj != (object *)0x0) &&
			 (((oVar2 = obj->type, oVar2 == OBJ_CLUTTER || (oVar2 == OBJ_BUILDING)) ||
				((oVar2 == OBJ_ROBOT || (oVar2 == OBJ_POWERUP)))))) {
			mstruct->shields = (float)Object_info[obj->id].hit_points;
			return;
		}
LAB_0048fa6f:
		mstruct->shields = -1.0;
		return;
	case MSAFE_SHOW_ENABLED_CONTROLS:
		obj = ObjGet(mstruct->objhandle);
		if ((obj != (object *)0x0) && (obj->type == OBJ_PLAYER)) {
			mstruct->state = Hud_show_controls;
			return;
		}
		goto LAB_00490713;
	case MSAFE_MISC_WAYPOINT:
		mstruct->index = Current_waypoint;
		return;
	case MSAFE_MISC_ENABLE_SHIP:
		valid = PlayerIsShipAllowed(-1,mstruct->name);
		mstruct->state = valid;
		return;
	case MSAFE_MISC_GUIDEBOT_NAME:
		obj = ObjGet(mstruct->objhandle);
		if (((obj != (object *)0x0) && (obj->type == OBJ_PLAYER)) && (obj->id == Player_num)) {
			pilot_class::get_guidebot_name((pilot_class *)&Current_pilot,(undefined4 *)mstruct->name);
			return;
		}
		uVar10 = 0xffffffff;
		pcVar21 = s_GB_005a7c18;
		do {
			pcVar20 = pcVar21;
			if (uVar10 == 0) break;
			uVar10 = uVar10 - 1;
			pcVar20 = pcVar21 + 1;
			cVar1 = *pcVar21;
			pcVar21 = pcVar20;
		} while (cVar1 != '\0');
		uVar10 = ~uVar10;
		puVar18 = (undefined4 *)(pcVar20 + -uVar10);
		pcVar21 = mstruct->name;
		for (uVar14 = uVar10 >> 2; uVar14 != 0; uVar14 = uVar14 - 1) {
			*(undefined4 *)pcVar21 = *puVar18;
			puVar18 = puVar18 + 1;
			pcVar21 = pcVar21 + 4;
		}
		for (uVar10 = uVar10 & 3; uVar10 != 0; uVar10 = uVar10 - 1) {
			*pcVar21 = *(char *)puVar18;
			puVar18 = (undefined4 *)((int)puVar18 + 1);
			pcVar21 = pcVar21 + 1;
		}
		return;
#endif
	case MSAFE_DOOR_LOCK_STATE:
		valid = DoorwayLocked(mstruct->objhandle);
		mstruct->state = valid;
		return;
	case MSAFE_DOOR_POSITION:
		fVar22 = DoorwayGetPosition(mstruct->objhandle);
		mstruct->scalar = fVar22;
		return;
	case MSAFE_DOOR_OPENABLE:
		valid = DoorwayOpenable(mstruct->objhandle,mstruct->ithandle);
		mstruct->state = valid;
		return;
#if 0
	case MSAFE_TRIGGER_SET:
		uVar10 = TriggerGetState((uint)mstruct->trigger_num);
		mstruct->state = (ubyte)uVar10;
		return;
	case MSAFE_INVEN_CHECK:
	case MSAFE_COUNTERMEASURE_CHECK:
		obj = ObjGet(mstruct->objhandle);
		if (obj == (object *)0x0) {
			return;
		}
		if (obj->type != OBJ_PLAYER) {
			return;
		}
		uVar3 = obj->id;
		if (type == MSAFE_COUNTERMEASURE_CHECK) {
			mstruct->type = 5;
		}
		iVar12 = mstruct->type;
		if (iVar12 == 5) {
			iVar13 = mstruct->id;
			iVar12 = 5;
			pIVar15 = &Players[uVar3].counter_measures;
		}
		else {
			iVar13 = mstruct->id;
			pIVar15 = &Players[uVar3].inventory;
		}
		valid = Inventory::CheckItem(pIVar15,iVar12,iVar13);
		if (valid != false) {
			mstruct->state = '\x01';
			return;
		}
LAB_00490713:
		mstruct->state = '\0';
		return;
	case MSAFE_INVEN_COUNT:
		obj = ObjGet(mstruct->objhandle);
		if ((obj != (object *)0x0) && (obj->type == OBJ_PLAYER)) {
			iVar12 = Inventory::GetTypeIDCount(&Players[obj->id].inventory,mstruct->type,mstruct->id);
			mstruct->count = iVar12;
			return;
		}
		break;
	case MSAFE_INVEN_SIZE:
		obj = ObjGet(mstruct->objhandle);
		if ((obj != (object *)0x0) && (obj->type == OBJ_PLAYER)) {
			iVar12 = Inventory::Size(&Players[obj->id].inventory);
			mstruct->size = (float)iVar12;
			return;
		}
		break;
	case MSAFE_INVEN_GET_TYPE_ID:
		obj = ObjGet(mstruct->objhandle);
		if ((obj != (object *)0x0) && (obj->type == OBJ_PLAYER)) {
			pIVar15 = &Players[obj->id].inventory;
			iVar12 = Inventory::Size(pIVar15);
			if ((mstruct->index < 0) || (iVar12 < mstruct->index)) {
LAB_004906eb:
				mstruct->count = 0;
				mstruct->type = 0xff;
				mstruct->id = 0;
				return;
			}
			iVar12 = Inventory::GetPos(pIVar15);
			Inventory::GotoPosIdx(pIVar15,mstruct->index);
			iVar13 = Inventory::GetPosCount(pIVar15);
			mstruct->count = iVar13;
LAB_004906d3:
			Inventory::GetPosTypeID(pIVar15,&mstruct->type,&mstruct->id);
			Inventory::GotoPosIdx(pIVar15,iVar12);
			return;
		}
		break;
	case MSAFE_INVEN_CHECK_OBJECT:
		obj = ObjGet(mstruct->objhandle);
		poVar11 = ObjGet(mstruct->ithandle);
		mstruct->state = '\0';
		if (((obj != (object *)0x0) && (poVar11 != (object *)0x0)) && (obj->type == OBJ_PLAYER)) {
			valid = Inventory::CheckItem(&Players[obj->id].inventory,mstruct->ithandle,-1);
			mstruct->state = valid;
			return;
		}
		break;
	case MSAFE_COUNTERMEASURE_COUNT:
		obj = ObjGet(mstruct->objhandle);
		if ((obj != (object *)0x0) && (obj->type == OBJ_PLAYER)) {
			uVar3 = obj->id;
			iVar12 = FindWeaponName(mstruct->name);
			if (iVar12 != -1) {
				iVar12 = Inventory::GetTypeIDCount(&Players[uVar3].counter_measures,5,iVar12);
				mstruct->count = iVar12;
				return;
			}
			mstruct->count = 0;
			return;
		}
		break;
	case MSAFE_COUNTERMEASURE_SIZE:
		obj = ObjGet(mstruct->objhandle);
		if ((obj != (object *)0x0) && (obj->type == OBJ_PLAYER)) {
			iVar12 = Inventory::Size(&Players[obj->id].counter_measures);
			mstruct->size = (float)iVar12;
			return;
		}
		break;
	case MSAFE_COUNTERMEASURE_GET:
		obj = ObjGet(mstruct->objhandle);
		if ((obj != (object *)0x0) && (obj->type == OBJ_PLAYER)) {
			pIVar15 = &Players[obj->id].counter_measures;
			iVar12 = Inventory::Size(pIVar15);
			if ((mstruct->index < 0) || (iVar12 < mstruct->index)) goto LAB_004906eb;
			iVar12 = Inventory::GetPos(pIVar15);
			Inventory::GotoPosIdx(pIVar15,mstruct->index);
			iVar13 = Inventory::GetPosCount(pIVar15);
			mstruct->count = iVar13;
			goto LAB_004906d3;
		}
		break;
	case MSAFE_WEAPON_CHECK:
		obj = ObjGet(mstruct->objhandle);
		mstruct->state = '\0';
		if ((obj != (object *)0x0) && (obj->type == OBJ_PLAYER)) {
			iVar12 = mstruct->index;
			uVar3 = obj->id;
			if ((Players[uVar3].weapon_flags & 1 << ((byte)iVar12 & 0x1f)) != 0) {
				mstruct->state = '\x01';
				if (9 < iVar12) {
					mstruct->count = (uint)Players[uVar3].weapon_ammo[iVar12];
					return;
				}
				if (*(float *)(iVar12 * 0xfc + 0xa20964 + Players[uVar3].ship_index * 0x1724) != 0.0) {
					mstruct->count = (uint)Players[uVar3].weapon_ammo[iVar12];
					return;
				}
				mstruct->count = -1;
			}
		}
#endif
	}
}
