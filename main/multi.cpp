#include "game.h"
#include "multi.h"

struct Netgame Netgame;
netplayer NetPlayers[MAX_NET_PLAYERS];
int Multi_bail_ui_menu;

void MultiSendGhostObject(object *obj,bool state) {}
void MultiSendClientInventoryUseItem(int type,int id) {}
void MultiSendRequestCountermeasure(int objnum,int weaponnum) {}
void MultiSendObject(object *obj,ubyte a,ubyte b) {}
void MultiSendDamageObject(object* obj, object* killer, float damage, int weapon_id) {}
void MultiPlay3dSound(int a, int b, int c) {}
void MultiSendDamagePlayer(int playernum,int weaponnum,int hit_type,float damage) {}
void MultiSendRemoveObject(object *obj, ubyte ws) {}
void MultiSendUnattach(object *obj) {}

