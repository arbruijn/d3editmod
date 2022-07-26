#include "inventory.h"
#include "object.h"
#include "levelgoal.h"
#include "objinfo.h"
#include "multi.h"
#include "game.h"
#include "weapon.h"
#include "osiris_dll.h"
#include "objscript.h"

Inventory::Inventory()
{
	this->root = NULL;
	this->count = 0;
	this->pos = NULL;
}

Inventory::~Inventory()
{
	Reset(false,2);
}

void Inventory::Reset(bool in_game,int reset_stage)
{
	inven_item *item;
	object *obj;
	int count;
	bool do_remove;
	int iflags;
	inven_item *next;
	bool remove_mission;
	bool remove_notspew;
	
	if ((reset_stage < 0) || (2 < reset_stage)) {
		reset_stage = 2;
	}
	item = this->root;
	count = this->count;
	remove_notspew = false;
	remove_mission = false;
	if (reset_stage == 0) {
		remove_notspew = true;
	}
	else {
		if (reset_stage == 1) {
			remove_notspew = true;
			remove_mission = false;
			goto joined_r0x004563b3;
		}
		if (reset_stage != 2) goto joined_r0x004563b3;
		remove_notspew = false;
	}
	remove_mission = true;
joined_r0x004563b3:
	do {
		if (count == 0) {
			ValidatePos(true);
			return;
		}
		iflags = item->iflags;
		next = item->next;
		do_remove = true;
		if ((((iflags & INVF_MISSIONITEM) != 0) && (!remove_mission)) &&
			 ((iflags & INVF_OBJECT) == 0)) {
			do_remove = false;
		}
		if (((iflags & INVF_NOTSPEWABLE) != 0) && (!remove_notspew)) {
			do_remove = false;
		}
		if ((in_game == false) || ((iflags & INVF_OBJECT) == 0)) {
			if (do_remove) {
				RemoveNode(item);
			}
		}
		else if (do_remove) {
			obj = ObjGet(item->type);
			if (obj)
				obj->flags = obj->flags & ~OF_INPLAYERINVENTORY;
			RemoveNode(item);
		}
		count = count + -1;
		item = next;
	} while( true );
}


bool Inventory::AddObject(int object_handle,int flags,char *description)
{
	ushort uVar1;
	inven_item *piVar2;
	object *obj;
	inven_item *item;
	char *str;
	bool bVar5;
	
	if (this->count >= 10)
		return false;
	obj = ObjGet(object_handle);
	if (obj) {
		if (obj->flags & OF_INFORM_DESTROY_TO_LG)
			Level_goals.Inform('\x02',0x400,obj->handle);
		if (obj->type == OBJ_DUMMY)
			ObjUnGhostObject(obj - Objects);
		if (!this->count) {
			item = new inven_item();
			this->root = item;
			item->next = item;
			this->root->prev = this->root;
		} else {
			item = new inven_item();
			piVar2 = this->root->prev;
			item->prev = piVar2;
			piVar2->next = item;
			item->next = this->root;
			this->root->prev = item;
		}
		item->iflags = INVF_OBJECT;
		item->count = 1;
		item->type = object_handle;
		item->id = -1;
		item->flags = 0;
		item->otype = (uint)obj->type;
		uVar1 = obj->id;
		item->oid = (uint)uVar1;
		if (Object_info[uVar1].description)
			item->description = strdup(Object_info[item->oid].description);
		else
			item->description = strdup("");
		if (Object_info[item->oid].flags & OIF_INVEN_SELECTABLE)
			item->iflags |= INVF_SELECTABLE;
		if (Object_info[item->oid].flags & OIF_INVEN_TYPE_MISSION)
			item->iflags |= INVF_MISSIONITEM;
		if (Object_info[item->oid].flags & OIF_INVEN_NOREMOVE)
			item->iflags |= INVF_NOREMOVEONUSE;
		if (Object_info[item->oid].flags & OIF_INVEN_VISWHENUSED)
			item->iflags |= INVF_VISWHENUSED;
		if (flags & 1)
			item->iflags |= INVF_NOTSPEWABLE;
		if (flags & 2)
			item->iflags |= INVF_TIMEOUTONSPEW;
		if (flags & 4)
			item->iflags |= INVF_MISSIONITEM;
		obj->flags = obj->flags | OF_INPLAYERINVENTORY;
		if (bVar5 || (Game_mode & 0x24) == 0 || Netgame.local_role == 1) {
			ObjGhostObject(Objects - obj);
			if ((Game_mode & 0x24) != 0 && (Netgame.local_role == 1))
				MultiSendGhostObject(obj,true);
		}
		item->icon_name = strdup(Object_info[item->oid].icon_name);
		if (!description)
			description = Object_info[item->oid].name;
		item->name = strdup(description);
		this->count++;
		if (item->iflags & INVF_SELECTABLE)
			this->pos = item;
		return true;
	}
	return false;
}


bool Inventory::Add(int type,int id,object *parent,int aux_type,int aux_id,int flags,
							char *description)
{
	bool bVar1;
	
	if (((this->count < 10) && (-1 < type)) && (type != 0xff)) {
		if (type != OBJ_WEAPON) {
			if (aux_id == -1) {
				aux_id = id;
			}
			if (aux_type == -1) {
				aux_type = type;
			}
			return AddObjectItem(type,id,aux_type,aux_id,flags,description);
		}
		return AddCounterMeasure(id,aux_type,aux_id,flags,description);
	}
	return false;
}


bool Inventory::AddCounterMeasure
					(int id,int aux_type,int aux_id,int flag,char *description)

{
	inven_item *item, *last;
	char *name;
	
	if (this->count >= 10)
		return false;
	if (this->count == 0) {
		item = new inven_item();
		this->root = item;
		item->next = item;
		this->root->prev = this->root;
		item->count = 1;
	}
	else {
		item = FindItem(OBJ_WEAPON,id);
		if (item == NULL) {
			item = new inven_item();
			last = this->root->prev;
			item->prev = last;
			last->next = item;
			item->next = this->root;
			this->root->prev = item;
			item->count = 1;
		}
		else {
			item->count = item->count + 1;
		}
	}
	if (item->count == 1) {
		item->type = OBJ_WEAPON;
		item->id = id;
		item->flags = 0;
		item->iflags = 0;
		item->otype = aux_type;
		item->oid = aux_id;
		if (((aux_type == -1) || (aux_id == -1)) ||
			 (name = Object_info[aux_id].description, name == NULL)) {
			name = Weapons[id].name;
		}
		item->description = strdup(name);
		item->iflags |= 0x87;
		item->icon_name = strdup(GameBitmaps[GameTextures[Weapons[id].icon_handle].bm_handle].name);
		if (description)
			description = Weapons[id].name;
		item->name = strdup(description);
		this->count = this->count + 1;
	}
	this->pos = item;
	return true;
}


bool Inventory::AddObjectItem
					(int otype,int oid,int oauxt,int oauxi,int flags,char *description)
{
	char cVar1;
	inven_item *item;
	uint uVar3;
	uint uVar4;
	char *pcVar6;
	inven_item *last;
	
	if (9 < this->count) {
		return false;
	}
	if (this->count == 0) {
		item = new inven_item();;
		this->root = item;
		item->next = item;
		this->root->prev = this->root;
		item->count = 1;
	}
	else {
		item = FindItem(otype,oid);
		if (item == NULL) {
			item = new inven_item();;
			last = this->root->prev;
			item->prev = last;
			last->next = item;
			item->next = this->root;
			this->root->prev = item;
			item->count = 1;
		}
		else {
			item->count = item->count + 1;
		}
	}
	if (item->count == 1) {
		item->otype = oauxt;
		item->id = oid;
		item->type = otype;
		item->flags = 0;
		item->iflags = 0;
		item->oid = oauxi;
		if (Object_info[oid].description == (char *)0x0)
			item->description = strdup("");
		else
			item->description = strdup(Object_info[oid].description);
		if ((Object_info[oid].flags & OIF_INVEN_SELECTABLE) != 0) {
			item->iflags |= INVF_SELECTABLE;
		}
		if ((Object_info[oid].flags & OIF_INVEN_TYPE_MISSION) != 0) {
			item->iflags |= INVF_MISSIONITEM;
		}
		if ((Object_info[oid].flags & OIF_INVEN_NOREMOVE) != 0) {
			item->iflags |= INVF_NOREMOVEONUSE;
		}
		if ((Object_info[oid].flags & OIF_INVEN_VISWHENUSED) != 0) {
			item->iflags |= INVF_VISWHENUSED;
		}
		if ((flags & 1U) != 0) {
			item->iflags |= INVF_NOTSPEWABLE;
		}
		if ((flags & 2U) != 0) {
			item->iflags |= INVF_TIMEOUTONSPEW;
		}
		if ((flags & 4U) != 0) {
			item->iflags |= INVF_MISSIONITEM;
		}
		item->icon_name = strdup(Object_info[oid].icon_name);
		if (description == (char *)0x0)
			description = Object_info[oid].name;
		item->name =	strdup(description);
		this->count = this->count + 1;
	}
	if (item->iflags & INVF_SELECTABLE)
		this->pos = item;
	return true;
}

bool Inventory::Use(int type,int id,object *parent)
{
	int iVar1;
	bool bVar2;
	inven_item *item;
	object *obj;
	int objnum;
	bool vis_when_used;
	tOSIRISEventInfo info;
	bool ret;
	
	item = FindItem(type,id);
	if (item == NULL)
		return false;
	iVar1 = item->iflags;
	if ((iVar1 & INVF_SELECTABLE) == 0)
		return false;
	ret = false;
	if ((Game_mode & 0x24) == 0) {
		bVar2 = false;
	}
	else {
		if ((Netgame.local_role & 1) == 0) {
			SendRequestToServerToUse(item->type,item->id);
			return false;
		}
		bVar2 = true;
	}
	if (parent == (object *)0x0) {
		return false;
	}
	if (type == OBJ_WEAPON) {
		CreateCountermeasureFromObject(parent,id);
		Remove(item->type,item->id);
		return true;
	}
	vis_when_used = (item->iflags & INVF_VISWHENUSED) != 0;
	if ((item->iflags & INVF_OBJECT) == 0) {
		objnum = ObjCreate(item->type,item->id,parent->roomnum,&parent->pos,NULL,parent->handle);
		if (objnum == -1) {
			return false;
		}
		if (vis_when_used == 0) {
			if (Objects[objnum].control_type != CT_AI) {
				SetObjectControlType(Objects + objnum,CT_NONE);
			}
			Objects[objnum].movement_type = MT_NONE;
			Objects[objnum].render_type = RT_NONE;
		}
		Objects[objnum].flags = item->flags;
		if (bVar2) {
			MultiSendObject(Objects + objnum,false,false);
		}
		InitObjectScripts(Objects + objnum,true);
	}
	else {
		obj = ObjGet(item->type);
		if (obj == (object *)0x0) {
			return false;
		}
		objnum = Objects - obj;
		obj->flags &= ~OF_INPLAYERINVENTORY;
		if (vis_when_used != 0) {
			ObjUnGhostObject(objnum);
			MultiSendGhostObject(obj,false);
		}
	}
	info.evt_use.it_handle = parent->handle;
	bVar2 = Osiris_CallEvent(Objects + objnum,EVT_USE,&info);
	if (bVar2 == false) {
		if ((item->iflags & 0x20) != 0) {
			Objects[objnum].flags = Objects[objnum].flags | OF_INPLAYERINVENTORY;
		}
	}
	else {
		Remove(item->type,item->id);
		ret = true;
	}
	if (!(item->iflags & INVF_NOREMOVEONUSE))
		Objects[objnum].flags = Objects[objnum].flags | (OF_DEAD|OF_SEND_MULTI_REMOVE_ON_DEATH);
	return ret;
}


void Inventory::SendRequestToServerToUse(int type,int id)
{
	inven_item *item;
	
	item = FindItem(type,id);
	if (item)
		MultiSendClientInventoryUseItem(type,id);
}

#if 0
bool Inventory::CheckItem(int type,int id)
{
	int got_type;
	int got_id;
		int save_id;
	int save_type;
	
	GetPosTypeID(save_type,save_id);
	GotoPos(type,id);
	GetPosTypeID(got_type,got_id);
	if ((got_type == type) && (got_id == id)) {
		return true;
	}
	GotoPos(save_type,save_id);
	return false;
}
#endif

bool Inventory::UsePos(object *param_1)
{
	inven_item *piVar1;
	bool bVar2;
	
	piVar1 = this->pos;
	if (piVar1) {
		bVar2 = Use(piVar1->type,piVar1->id,param_1);
		return bVar2;
	}
	return false;
}


bool Inventory::Use(int objhandle,object *parent)
{
	return Use(objhandle,-1,parent);
}

bool Inventory::Remove(int type,int id)
{
	inven_item *item;
	object *obj;
	int iVar2;
	
	item = FindItem(type,id);
	if (item == NULL) {
		return false;
	}
	if ((item->iflags & INVF_OBJECT) == 0) {
		iVar2 = item->count + -1;
		item->count = iVar2;
		if (0 < iVar2) {
			return true;
		}
	}
	else {
		obj = ObjGet(type);
		if (obj) {
			obj->flags &= ~OF_INPLAYERINVENTORY;
			RemoveNode(item);
			return true;
		}
	}
	RemoveNode(item);
	return true;
}


void Inventory::RemoveNode(inven_item *item)
{
	inven_item *piVar1;
	inven_item *next;
	inven_item *prev;
	bool was_pos;
	
	if (item) {
		this->count = this->count + -1;
		prev = item->prev;
		next = item->next;
		was_pos = false;
		if (item->description != (char *)0x0)
			free(item->description);
		if (item->icon_name != (char *)0x0)
			free(item->icon_name);
		if (item->name != (char *)0x0)
			free(item->name);
		if (this->pos == item) {
			piVar1 = this->pos->next;
			if (piVar1 == item) {
				this->pos = NULL;
			}
			else {
				was_pos = true;
				this->pos = piVar1;
			}
		}
		piVar1 = this->root;
		if (item == piVar1) {
			prev = piVar1->next;
			if (prev == piVar1) {
				delete item;
				this->root = NULL;
			}
			else {
				next = piVar1->prev;
				delete item;
				this->root = prev;
				prev->prev = next;
				next->next = this->root;
			}
		}
		else {
			delete item;
			prev->next = next;
			next->prev = prev;
		}
		if (was_pos) {
			ValidatePos(true);
		}
	}
}


inven_item * Inventory::FindItem(int type,int id)
{
	int count;
	inven_item *item;
	
	item = this->root;
	count = this->count;
	while( true ) {
		if (count == 0) {
			return NULL;
		}
		if ((item->type == type) && (item->id == id)) break;
		item = item->next;
		count = count + -1;
	}
	return item;
}


bool Inventory::CheckItem(int type,int id)

{
	inven_item *item;
	
	item = FindItem(type,id);
	return item;
}


int Inventory::SaveInventory(CFILE *file)

{
	int count;
	int startpos;
	object *obj;
	int endpos;
	inven_item *item;
	CFILE *file1;
	int posidx, idx;
	
	file1 = file;
	count = Size();
	startpos = cftell(file);
	idx = 0;
	posidx = 0;
	cf_WriteInt(file1,count);
	if (0 < count) {
		item = this->root;
		do {
			if (this->pos == item) {
				posidx = idx;
			}
			if (item->id == -1) {
				obj = ObjGet(item->type);
				if (obj) goto LAB_00457102;
			}
			else {
LAB_00457102:
				cf_WriteInt(file1,item->type);
				cf_WriteInt(file1,item->otype);
				cf_WriteInt(file1,item->id);
				cf_WriteInt(file1,item->oid);
				cf_WriteInt(file1,item->flags);
				cf_WriteInt(file1,item->count);
				if (item->description == (char *)0x0) {
					cf_WriteByte(file1,0);
				}
				else {
					cf_WriteString(file1,item->description);
				}
				if (item->icon_name == (char *)0x0) {
					cf_WriteByte(file1,0);
				}
				else {
					cf_WriteString(file1,item->icon_name);
				}
				if (item->name == (char *)0x0) {
					cf_WriteByte(file1,0);
				}
				else {
					cf_WriteString(file1,item->name);
				}
				cf_WriteInt(file1,item->iflags);
			}
			item = item->next;
			count = count + -1;
			idx++;
		} while (0 < count);
	}
	cf_WriteInt(file1,posidx);
	endpos = cftell(file1);
	return endpos - startpos;
}

int Inventory::ReadInventory(CFILE *file)
{
	inven_item *piVar1;
	int startpos;
	int count;
	uint type;
	int otype;
	int id;
	object *poVar2;
	inven_item *item;
	int oid;
	int flags;
	int item_count;
	char *str;
	int iflags;
	int posidx;
	int endpos;
	char text [512];
	
	startpos = cftell(file);
	count = cf_ReadInt(file);
	this->count = count;
	this->root = NULL;
	for (; 0 < count; count = count + -1) {
		type = cf_ReadInt(file);
		otype = cf_ReadInt(file);
		id = cf_ReadInt(file);
		if ((id == -1) && (poVar2 = ObjGet(type), poVar2 == (object *)0x0)) {
			cf_ReadInt(file);
			cf_ReadInt(file);
			cf_ReadInt(file);
			cf_ReadString(text,0x200,file);
			cf_ReadString(text,0x200,file);
			cf_ReadString(text,0x200,file);
			cf_ReadInt(file);
			this->count = this->count + -1;
		}
		else {
			if (this->root == NULL) {
				item = new inven_item();;
				this->root = item;
				item->next = item;
				this->root->prev = this->root;
			}
			else {
				item = new inven_item();;
				piVar1 = this->root->prev;
				item->prev = piVar1;
				piVar1->next = item;
				item->next = this->root;
				this->root->prev = item;
			}
			item->type = type;
			item->id = id;
			item->otype = otype;
			oid = cf_ReadInt(file);
			item->oid = oid;
			flags = cf_ReadInt(file);
			item->flags = flags;
			item_count = cf_ReadInt(file);
			item->count = item_count;
			cf_ReadString(text,0x200,file);
			str = strdup(text);
			item->description = str;
			cf_ReadString(text,0x200,file);
			str = strdup(text);
			item->icon_name = str;
			cf_ReadString(text,0x200,file);
			str = strdup(text);
			item->name = str;
			iflags = cf_ReadInt(file);
			item->iflags = iflags;
		}
	}
	posidx = cf_ReadInt(file);
	GotoPos(posidx);
	ValidatePos(true);
	endpos = cftell(file);
	return endpos - startpos;
}

void Inventory::ResetPos()
{
	this->pos = this->root;
}

void Inventory::NextPos(bool skip)
{
	inven_item *item;
	
	item = this->pos;
	if (item) {
		if (!item->next)
				this->pos = item->next;
	} else
		this->pos = this->root;
	if (!skip)
		ValidatePos(true);
}

void Inventory::PrevPos(bool skip)
{
	inven_item *item;
	
	item = this->pos;
	if (item) {
		if (!item->prev)
				this->pos = item->prev;
	} else
		this->pos = this->root;
	if (!skip)
		ValidatePos(false);
}


bool Inventory::AtEnd()

{
	if (this->pos == NULL) {
		return true;
	}
	return this->pos->next == this->root;
}


bool Inventory::GetPosTypeID(int &type,int &id)
{
	if (this->pos == NULL) {
		type = id = 0;
		return false;
	}
	type = this->pos->type;
	id = this->pos->id;
	return (this->pos->iflags & INVF_OBJECT) != 0;
}


bool Inventory::GetAuxPosTypeID(int &type, int &id)

{
	if (this->pos == NULL) {
		type = id = 0;
		return false;
	}
	type = this->pos->otype;
	id = this->pos->oid;
	return (this->pos->iflags & INVF_OBJECT) != 0;
}


char *Inventory::GetPosName()
{
	inven_item *item;
	
	item = this->pos;
	return item ? item->name : NULL;
}


int Inventory::GetPosCount()
{
	inven_item *item;
	
	item = this->pos;
	if (item == NULL) {
		return (int)item;
	}
	if ((item->iflags & 0x20) != 0) {
		return 1;
	}
	return item->count;
}


bool Inventory::GetPosInfo(ushort &iflags,int &flags)
{
	if (this->pos == NULL) {
		iflags = 0;
		flags = 0;
		return false;
	}
	iflags = this->pos->iflags;
	flags = this->pos->flags;
	return (this->pos->iflags & INVF_OBJECT) != 0;
}

void Inventory::GotoPos(int idx)
{
	inven_item *next;
	
	ResetPos();
	if (0 < idx) {
		do {
			if ((this->pos) && (next = this->pos->next)) {
				this->pos = next;
			}
			idx = idx + -1;
		} while (idx != 0);
	}
}

void Inventory::GotoPos(int type,int id)
{
	inven_item *item;
	
	item = FindItem(type,id);
	if (item)
		this->pos = item;
}


void Inventory::ValidatePos(bool forward)
{
	inven_item *item;
	inven_item *pos;
	
	pos = this->pos;
	if (pos && (pos->iflags & INVF_SELECTABLE)) {
		if (forward == false) {
			item = pos->prev;
		}
		else {
			item = pos->next;
		}
		while (item != pos) {
			if (item->iflags & INVF_SELECTABLE) {
				this->pos = item;
				return;
			}
			if (forward == false) {
				item = item->prev;
			}
			else {
				item = item->next;
			}
		}
		this->pos = NULL;
	}
}

int Inventory::Size()
{
	return count;
}

void CreateCountermeasureFromObject(object *obj,int weapon_id)
{
	if (Game_mode & 0x24)
		MultiSendRequestCountermeasure(obj - Objects, weapon_id);
	else						
		FireWeaponFromObject(obj,weapon_id,5,false,false);
}
