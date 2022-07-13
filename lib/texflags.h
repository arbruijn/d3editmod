enum texflags {
	TF_VOLATILE				= 1,
	TF_WATER					= 1<<1,
	TF_METAL					= 1<<2,		// Shines like metal
	TF_MARBLE					= 1<<3,		// Shines like marble
	TF_PLASTIC				= 1<<4,		// Shines like plastic
	TF_FORCEFIELD			= 1<<5,
	TF_ANIMATED				= 1<<6,
	TF_DESTROYABLE			= 1<<7,
	TF_EFFECT					= 1<<8,
	TF_HUD_COCKPIT			= 1<<9,
	TF_MINE					= 1<<10,
	TF_TERRAIN				= 1<<11,
	TF_OBJECT					= 1<<12,
	TF_TEXTURE_64			= 1<<13,
	TF_TMAP2					= 1<<14,
	TF_TEXTURE_32			= 1<<15,
	TF_FLY_THRU				= 1<<16,
	TF_PASS_THRU				= 1<<17,
	TF_PING_PONG				= 1<<18,
	TF_LIGHT					= 1<<19,
	TF_BREAKABLE				= 1<<20,		// Breakable = as in glass,
	TF_SATURATE				= 1<<21,
	TF_ALPHA					= 1<<22,
	TF_DONTUSE				= 1<<23,		
	TF_PROCEDURAL			= 1<<24,		
	TF_WATER_PROCEDURAL	= 1<<25,
	TF_FORCE_LIGHTMAP		= 1<<26,
	TF_SATURATE_LIGHTMAP	= 1<<27,
	TF_TEXTURE_256			= 1<<28,
	TF_LAVA					= 1<<29,
	TF_RUBBLE					= 1<<30,
	TF_SMOOTH_SPECULAR		= 1<<31
};
