#define DF_RECORDING 1
#define DF_PLAYBACK 2

extern int Demo_flags;

void DemoWritePersistantHUDMessage(ddgr_color color,int x, int y, float time, int flags, int sound_index, const char *fmt, ...);
