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
void MultiSendObject(object *, int);
void MultiPlay3dSound(int,int, int);
extern int Demo_flags;
#define DF_RECORDING 1
void DemoWrite3DSound(int, int, int);
extern netplayer NetPlayers[MAX_NET_PLAYERS];

#endif
