#ifndef _MULTI_H
#define _MULTI_H

#include "multi_external.h"
#include "object.h"

#define LR_SERVER 1
typedef struct Netgame {
    int local_role;
} Netgame;
void MultiSendObject(object *, int);
void MultiPlay3dSound(int,int, int);
extern int Demo_flags;
#define DF_RECORDING 1
void DemoWrite3DSound(int, int, int);
extern netplayer NetPlayers[32];
extern int Netgame_flags;

#endif
