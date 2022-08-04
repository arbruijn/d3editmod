#include "object.h"
#include "demofile.h"
#include "multi.h"
#include "game.h"
#include "polymodel.h"
#include "objinfo.h"

void AttachPropagateMass(object *obj,bool is_add)
{
	object *parent;
	float mass;
	
	if (!obj)
		return;
	while ((parent = ObjGet(obj->attach_parent_handle))) {
		mass = Object_info[obj->id].phys_info.mass;
		if (mass < 0)
			mass = 0;
		parent->mtype.phys_info.mass += is_add ? mass : -mass;
		if (parent->mtype.phys_info.mass < 0)
			parent->mtype.phys_info.mass = 0;
		if (!(parent->flags & OF_ATTACHED))
			break;
		obj = parent;
	}
	return;
}

void ProprogateUltimateAttachParent(object *obj,int ultimate_handle)
{
	int modelnum = obj->rtype.pobj_info.model_num; if
	(!obj->attach_children)
		return;
	for (int i = 0; i < Poly_models[modelnum].n_attach; i++) {
		object *child = ObjGet(obj->attach_children[i]);
		if (child) {
			child->attach_ultimate_handle = ultimate_handle;
			ProprogateUltimateAttachParent(child,ultimate_handle);
		}
	}
}

bool UnattachFromParent(object *obj)
{
	object *parent;
	int iVar2;
	int *piVar3;
	
	if (!(obj->flags & OF_ATTACHED))
		return false;
	AttachPropagateMass(obj,false);
	parent = ObjGet(obj->attach_parent_handle);
	for (int i = 0; i < Poly_models[parent->rtype.pobj_info.model_num].n_attach; i++)
		if (parent->attach_children[i] == obj->handle) {
			parent->attach_children[i] = -1;
			break;
		}
	obj->flags &= ~OF_ATTACHED;
	obj->attach_parent_handle = -1;
	obj->attach_ultimate_handle = -1;
	ProprogateUltimateAttachParent(obj,obj->handle);
	if (((Game_mode & 0x24) != 0) && (Netgame.local_role == 1))
		MultiSendUnattach(obj);
	if (Demo_flags == 1)
		DemoWriteUnattachObj(obj);
	return true;
}

bool UnattachChild(object *obj,char childnum)
{
	object *child = ObjGet(obj->attach_children[(int)childnum]);
	return child && UnattachFromParent(child);
}

bool UnattachChildren(object *obj)
{
	int modelnum = obj->rtype.pobj_info.model_num;
	if (!obj->attach_children)
		return false;
	for (int i = 0; i < Poly_models[modelnum].n_attach; i++) {
		object *child = ObjGet(obj->attach_children[i]);
		if (child)
			UnattachFromParent(child);
	}
	return true;
}
