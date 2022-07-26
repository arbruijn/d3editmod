#ifndef _MULTI_H
#define _MULTI_H

#include "multi_external.h"
#include "object.h"

#define LR_SERVER 1
struct Netgame {
    int local_role;
    int flags;
};
extern struct Netgame Netgame;
void MultiSendObject(object *, ubyte opt1=0, ubyte opt2=0);
void MultiPlay3dSound(int,int, int);
extern int Demo_flags;
#define DF_RECORDING 1
void DemoWrite3DSound(int, int, int);
extern netplayer NetPlayers[MAX_NET_PLAYERS];
extern int Multi_bail_ui_menu;

void MultiSendGhostObject(object *,bool);
void MultiSendClientInventoryUseItem(int type,int id);
void MultiSendRequestCountermeasure(int objnum,int weaponnum);
void MultiSendDamageObject(object *obj,object *killer,float damage,int weapon_id);
void MultiSendDamagePlayer(int playernum,int weaponnum,int hit_type,float damage);

#endif
