#ifndef SPEW_H
#define SPEW_H

#define SF_GUN_NEED_UPDATE  1
#define SF_GUN_UPDATED 2
#define SF_GUN_ALWAYS_UPDATE 4

typedef struct spew_info {
	ubyte state;
	bool inuse;
	bool is_obj;
	bool is_real;
	union {
		vector pos;
		struct { int handle; int gun; } obj;
	};
	vector dir;
	int roomnum;
	int effect_type;
	int phys_info;
	int random;
	int handle;
	float drag;
	float mass;
	float interval;
	float longevity;
	float lifetime;
	float size;
	float speed;
	float interval_left;
	float create_time;
	vector gunpointdir;
	vector gunpointpos;
} spew_info;

#define MAX_SPEW 50
extern spew_info SpewEffects[MAX_SPEW];
extern ushort Server_spew_list[MAX_SPEW];

void SpewClearEvent(int handle,bool check_handle);
void SpewEmitAll();
int SpewCreate(spew_info *spw);
void SpewInit();

#endif
