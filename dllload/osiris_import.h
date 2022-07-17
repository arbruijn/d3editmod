/*
 THE COMPUTER CODE CONTAINED HEREIN IS THE SOLE PROPERTY OF OUTRAGE
 ENTERTAINMENT, INC. ("OUTRAGE").  OUTRAGE, IN DISTRIBUTING THE CODE TO
 END-USERS, AND SUBJECT TO ALL OF THE TERMS AND CONDITIONS HEREIN, GRANTS A
 ROYALTY-FREE, PERPETUAL LICENSE TO SUCH END-USERS FOR USE BY SUCH END-USERS
 IN USING, DISPLAYING,  AND CREATING DERIVATIVE WORKS THEREOF, SO LONG AS
 SUCH USE, DISPLAY OR CREATION IS FOR NON-COMMERCIAL, ROYALTY OR REVENUE
 FREE PURPOSES.  IN NO EVENT SHALL THE END-USER USE THE COMPUTER CODE
 CONTAINED HEREIN FOR REVENUE-BEARING PURPOSES.  THE END-USER UNDERSTANDS
 AND AGREES TO THE TERMS HEREIN AND ACCEPTS THE SAME BY USE OF THIS FILE.
 COPYRIGHT 1996-2000 OUTRAGE ENTERTAINMENT, INC.  ALL RIGHTS RESERVED.
 */
 #ifdef	__OSIRIS_IMPORT_H_
#define OSIRISEXTERN extern
#else
#define OSIRISEXTERN
#endif

#include "osiris_common.h"

// ===========================================================

// void mprintf(int n, char *format, ... );
// prints a debug message out to the monochrome monitor
typedef void( *mprintf_fp ) (int n, char *format, ... );
OSIRISEXTERN mprintf_fp mprintf;

// The main calling function for multisafe functions
typedef void( *MSafe_CallFunction_fp ) (int type,msafe_struct *mstruct);
OSIRISEXTERN MSafe_CallFunction_fp dMSafe_CallFunction;

// The main calling function for extracting multisafe data
typedef void( *MSafe_GetValue_fp ) (int type,msafe_struct *mstruct);
OSIRISEXTERN MSafe_GetValue_fp dMSafe_GetValue;

//	bool Obj_CallEvent(int objnum,int event,tOSIRISEventInfo *ei);
//	Sends an event to an object.  Returns true if the default action should
//	continue to process.
typedef bool( *Obj_CallEvent_fp )(int objnum,int event,tOSIRISEventInfo *ei);
OSIRISEXTERN Obj_CallEvent_fp dObj_CallEvent;

//	bool Trgr_CallEvent(int trignum,int event,tOSIRISEventInfo *ei);
//	Sends an event to a trigger.  Returns true if the default action should
//	continue to process.
typedef bool( *Trgr_CallEvent_fp )(int trignum,int event,tOSIRISEventInfo *ei);
OSIRISEXTERN Trgr_CallEvent_fp dTrgr_CallEvent;

//	void Sound_TouchFile(char *sound_name);
//	Touches a sound file so it loads into memory
typedef void( *Sound_TouchFile_fp )(char *sound_name);
OSIRISEXTERN Sound_TouchFile_fp dSound_TouchFile;

//	int Obj_FindID(char *object_name);
//	searches for an object id given it's name
typedef int( *Obj_FindID_fp )(char *object_name);
OSIRISEXTERN Obj_FindID_fp dObj_FindID;

//	int Obj_FindType(char *object_name);
//	searches for an object type given it's name
typedef int( *Obj_FindType_fp )(char *object_name);
OSIRISEXTERN Obj_FindType_fp dObj_FindType;

//	int Wpn_FindID(char *weapon_name);
//	searches through the weapons for a name and returns the id
typedef int( *Wpn_FindID_fp )(char *weapon_name);
OSIRISEXTERN Wpn_FindID_fp dWpn_FindID;

//	float Obj_GetTimeLived(int objhandle);
//	returns how long an object has lived
typedef float( *Obj_GetTimeLived_fp)(int objhandle);
OSIRISEXTERN Obj_GetTimeLived_fp dObj_GetTimeLived;

//	void Obj_GetGunPos(int objhandle,int gun_number,vector *gun_pnt,vector *gun_normal);
//	returns information about a gunpoint of an object
typedef void( *Obj_GetGunPos_fp)(int objhandle,int gun_number,vector *gun_pnt,vector *gun_normal
#ifdef def_arg
= NULL
#endif
);
OSIRISEXTERN Obj_GetGunPos_fp dObj_GetGunPos;

//	void Obj_GetGroundPos(int objhandle,int ground_number,vector *ground_pnt,vector *ground_normal);
//	returns information about a groundpoint of an object
typedef void( *Obj_GetGroundPos_fp)(int objhandle,int ground_number,vector *ground_pnt,vector *ground_normal
#ifdef def_arg
 = NULL
#endif 
);
OSIRISEXTERN Obj_GetGroundPos_fp dObj_GetGroundPos;

//	void Room_Value(int roomnum, char op, char vhandle, void *ptr);
typedef void( *Room_Value_fp)(int roomnum, char op, char vhandle, void *ptr, int index
#ifdef def_arg
 = 0
#endif
);
OSIRISEXTERN Room_Value_fp dRoom_Value;

//	ubyte Room_IsValid(int roomnum);
//	returns 0 if the room is not valid.  1 if the room is a terrain cell. 2 if the room is a room segment
typedef ubyte( *Room_IsValid_fp)(int roomnum);
OSIRISEXTERN Room_IsValid_fp dRoom_IsValid;

//	int Obj_GetAttachParent(int childhandle);
//	returns the handle to the parent of the given attached child
typedef int( *Obj_GetAttachParent_fp)(int childhandle);
OSIRISEXTERN Obj_GetAttachParent_fp dObj_GetAttachParent;

//	int Obj_GetNumAttachSlots(int objhandle);
//	returns the number of attach slots an object has.
typedef int( *Obj_GetNumAttachSlots_fp)(int objhandle);
OSIRISEXTERN Obj_GetNumAttachSlots_fp dObj_GetNumAttachSlots;

//	int Obj_GetAttachChildHandle(int objhandle,char attachpoint);
//	returns the handle to the child attached on the specified attach point of an object
typedef int( *Obj_GetAttachChildHandle_fp)(int objhandle,char attachpoint);
OSIRISEXTERN Obj_GetAttachChildHandle_fp dObj_GetAttachChildHandle;

//	int Obj_AttachObjectAP(int parenthandle,char parent_ap,int childhandle,char child_ap,ubyte f_use_aligned);
//	Attaches one object to another object based on their attach points
typedef int( *Obj_AttachObjectAP_fp)(int parenthandle,char parent_ap,int childhandle,char child_ap,ubyte f_use_aligned);
OSIRISEXTERN Obj_AttachObjectAP_fp dObj_AttachObjectAP;

//	int Obj_AttachObjectRad(int parenthandle,char parent_ap,int childhandle,float percent_rad);
//	Attached one object to another based on radius
typedef int( *Obj_AttachObjectRad_fp)(int parenthandle,char parent_ap,int childhandle,float percent_rad);
OSIRISEXTERN Obj_AttachObjectRad_fp dObj_AttachObjectRad;

//	void Obj_UnattachFromParent(int objhandle);
//	Detaches an object from it's parent
typedef void( *Obj_UnattachFromParent_fp)(int objhandle);
OSIRISEXTERN Obj_UnattachFromParent_fp dObj_UnattachFromParent;

//	void Obj_UnattachChild(int objhandle,char parent_ap);
//	Detaches a child object from it's parent
typedef void( *Obj_UnattachChild_fp)(int objhandle,char parent_ap);
OSIRISEXTERN Obj_UnattachChild_fp dObj_UnattachChild;

//	void Obj_UnattachChildren(int objhandle);
//	Detaches all children from a parent
typedef void( *Obj_UnattachChildren_fp)(int objhandle);
OSIRISEXTERN Obj_UnattachChildren_fp dObj_UnattachChildren;

//	int FVI_RayCast(int objhandle,vector *p0,vector *p1,int start_roomnum,float rad,int flags,ray_info *ri);
//	Shoots out a ray, returns it's fate
typedef int( *FVI_RayCast_fp)(int objhandle,vector *p0,vector *p1,int start_roomnum,float rad,int flags,ray_info *ri);
OSIRISEXTERN FVI_RayCast_fp dFVI_RayCast;

//	int AI_GetPathID(char *string);
//	searches through GamePath index and returns index of path matching name returns -1 if not found
typedef int( *AI_GetPathID_fp)(char *string);
OSIRISEXTERN AI_GetPathID_fp dAI_GetPathID;

//	int AI_GoalFollowPathSimple(int objhandle,int path_id,int guid,int flags);
typedef int( *AI_GoalFollowPathSimple_fp)(int objhandle,int path_id,int guid,int flags,int slot
#ifdef def_arg
 = 3
#endif 
);
OSIRISEXTERN AI_GoalFollowPathSimple_fp dAI_GoalFollowPathSimple;

//	int AI_PowerSwitch(int objhandle,ubyte f_power_on);
typedef int( *AI_PowerSwitch_fp) (int objhandle,ubyte f_power_on);
OSIRISEXTERN AI_PowerSwitch_fp dAI_PowerSwitch;

//	void AI_Value(int objhandle, char op, char vtype, void *ptr);
typedef void( *AI_Value_fp)(int objhandle, char op, char vtype, void *ptr);
OSIRISEXTERN AI_Value_fp dAI_Value;

//	void Obj_Value(int objhandle, char op, char vtype, void *ptr);
typedef void( *Obj_Value_fp)(int objhandle, char op, char vtype, void *ptr, int index
#ifdef def_arg
 = 0
#endif 
);
OSIRISEXTERN Obj_Value_fp dObj_Value;

//	void Matcen_Value(int matcen_handle, char op, char vtype, void *ptr, int prod_index = 0);
typedef void( *Matcen_Value_fp)(int matcen_handle, char op, char vtype, void *ptr, int prod_index
#ifdef def_arg
 = 0
#endif
);
OSIRISEXTERN Matcen_Value_fp dMatcen_Value;

//	ubyte AI_TurnTowardsVectors(int objhandle,vector *fvec,vector *uvec);
typedef ubyte( *AI_TurnTowardsVectors_fp)(int objhandle,vector *fvec,vector *uvec);
OSIRISEXTERN AI_TurnTowardsVectors_fp dAI_TurnTowardsVectors;

//	void AI_SetType(int objhandle,int type);
typedef void( *AI_SetType_fp)(int objhandle,int type);
OSIRISEXTERN AI_SetType_fp dAI_SetType;

//	vector AI_FindHidePos(int hideobjhandle,int viewobjhandle,float time,int *hide_room);
typedef vector( *AI_FindHidePos_fp)(int hideobjhandle,int viewobjhandle,float time,int *hide_room);
OSIRISEXTERN AI_FindHidePos_fp dAI_FindHidePos;

//	int AI_GoalAddEnabler(int objhandle,int goal_index,int enabler_type,float percent,float interval,void *ptr);
typedef int( *AI_GoalAddEnabler_fp)(int objhandle,int goal_index,int enabler_type,float percent,float interval,void *ptr);
OSIRISEXTERN AI_GoalAddEnabler_fp dAI_GoalAddEnabler;

//	int AI_AddGoal(int objhandle,int goal_type,int level,float influence, int guid, int flags, ... );
typedef int( *AI_AddGoal_fp)(int objhandle,int goal_type,int level,float influence, int guid, int flags, ... );
OSIRISEXTERN AI_AddGoal_fp dAI_AddGoal;

//	void AI_ClearGoal(int objhandle,int goal_index);
typedef void( *AI_ClearGoal_fp)(int objhandle,int goal_index);
OSIRISEXTERN AI_ClearGoal_fp dAI_ClearGoal;

//	int AI_FindObjOfType(int objhandle,int type, bool f_ignore_init_room, int parent_handle = OBJECT_HANDLE_NONE);
typedef int( *AI_FindObjOfType_fp)(int objhandle, int type, int id, bool f_ignore_init_room, int parent_handle
#ifdef def_arg
 = OBJECT_HANDLE_NONE
#endif
);
OSIRISEXTERN AI_FindObjOfType_fp dAI_FindObjOfType;

//	vector AI_GetRoomPathPoint(int roomnum);
typedef vector( *AI_GetRoomPathPoint_fp)(int roomnum);
OSIRISEXTERN AI_GetRoomPathPoint_fp dAI_GetRoomPathPoint;

//	int AI_FindEnergyCenter(int objhandle);
typedef int( *AI_FindEnergyCenter_fp)(int objhandle);
OSIRISEXTERN AI_FindEnergyCenter_fp dAI_FindEnergyCenter;

//	float AI_GetDistToObj(int objhandle,int otherobjhandle);
typedef float( *AI_GetDistToObj_fp)(int objhandle,int otherobjhandle);
OSIRISEXTERN AI_GetDistToObj_fp dAI_GetDistToObj;

//	int AI_SetGoalFlags(int objhandle,int goal_handle,int flags,ubyte f_enable);
typedef int( *AI_SetGoalFlags_fp)(int objhandle,int goal_handle,int flags,ubyte f_enable);
OSIRISEXTERN AI_SetGoalFlags_fp dAI_SetGoalFlags;

//	void AI_SetGoalCircleDist(int objhandle,int goal_handle,float dist);
typedef void( *AI_SetGoalCircleDist_fp)(int objhandle,int goal_handle,float dist);
OSIRISEXTERN AI_SetGoalCircleDist_fp dAI_SetGoalCircleDist;

//	int File_ReadBytes(ubyte *buffer, int count, void *fileptr);
//Reads the specified number of bytes from a file into the buffer
//DO NOT USE THIS TO READ STRUCTURES.  This function is for byte
//data, such as a string or a bitmap of 8-bit pixels.
//Returns the number of bytes read.
//Throws an exception of type (cfile_error *) if the OS returns an error on read
typedef int( *File_ReadBytes_fp)(ubyte *buffer, int count, void *fileptr);
OSIRISEXTERN File_ReadBytes_fp dFile_ReadBytes;

//	int File_ReadInt(void *fileptr);
// The following functions read numeric vales from a CFILE.  All values are 
// stored in the file in Intel (little-endian) format.  These functions
// will convert to big-endian if required.
// These funtions will throw an exception of if the value cannot be read, 
// so do not call these if you don't require the data to be present.   

//Read and return an integer (32 bits)
//Throws an exception of type (cfile_error *) if the OS returns an error on read
typedef int( *File_ReadInt_fp)(void *fileptr);
OSIRISEXTERN File_ReadInt_fp dFile_ReadInt;

//	short File_ReadShort(void *fileptr);
//Read and return a short (16 bits)
//Throws an exception of type (cfile_error *) if the OS returns an error on read
typedef short( *File_ReadShort_fp)(void *fileptr);
OSIRISEXTERN File_ReadShort_fp dFile_ReadShort;

//	sbyte File_ReadByte(void *fileptr);
//Read and return a byte (8 bits)
//Throws an exception of type (cfile_error *) if the OS returns an error on read
typedef sbyte( *File_ReadByte_fp)(void *fileptr);
OSIRISEXTERN File_ReadByte_fp dFile_ReadByte;

//	float File_ReadFloat(void *fileptr);
//Read and return a float (32 bits)
//Throws an exception of type (cfile_error *) if the OS returns an error on read
typedef float( *File_ReadFloat_fp)(void *fileptr);
OSIRISEXTERN File_ReadFloat_fp dFile_ReadFloat;

//	double File_ReadDouble(void *fileptr);
//Read and return a double (64 bits)
//Throws an exception of type (cfile_error *) if the OS returns an error on read
typedef double( *File_ReadDouble_fp)(void *fileptr);
OSIRISEXTERN File_ReadDouble_fp dFile_ReadDouble;

//	int File_ReadString(char *buf,size_t n,void *fileptr);
//Reads a string from a CFILE.  If the file is type binary, this 
//function reads until a NULL or EOF is found.  If the file is text,
//the function reads until a newline or EOF is found.  The string is always
//written to the destination buffer null-terminated, without the newline.
//Parameters:  buf - where the string is written
//					n - the maximum string length, including the terminating 0
//					cfp - the CFILE pointer
//Returns the number of bytes in the string, before the terminator
//Does not generate an exception on EOF
typedef int( *File_ReadString_fp)(char *buf,int n,void *fileptr);
OSIRISEXTERN File_ReadString_fp dFile_ReadString;

//	int File_WriteBytes(const ubyte *buf,int count,void *fileptr);
//Writes the specified number of bytes from a file into the buffer
//DO NOT USE THIS TO WRITE STRUCTURES.  This function is for byte
//data, such as a string or a bitmap of 8-bit pixels.
//Returns the number of bytes written.
//Throws an exception of type (cfile_error *) if the OS returns an error on write
typedef int (*File_WriteBytes_fp)(const ubyte *buf,int count, void *fileptr);
OSIRISEXTERN File_WriteBytes_fp dFile_WriteBytes;

//	int File_WriteString(const char *buf,void *fileptr);
//Writes a null-terminated string to a file.  If the file is type binary, 
//the string is terminated in the file with a null.  If the file is type 
//text, the string is terminated with a newline.
//Parameters:	buf - pointer to the string
//					cfp = the CFILE pointer
//Returns the number of bytes written
//Throws an exception of type (cfile_error *) if the OS returns an error on write
typedef int( *File_WriteString_fp)(const char *buf,void *fileptr);
OSIRISEXTERN File_WriteString_fp dFile_WriteString;

//	void File_WriteInt(int i,void *fileptr);
// The following functions write numeric vales to a CFILE.  All values are 
// stored to the file in Intel (little-endian) format. 
// All these throw an exception if there's an error on write.


//Write an integer (32 bits)
//Throws an exception of type (cfile_error *) if the OS returns an error on write
typedef void( *File_WriteInt_fp)(int i,void *fileptr);
OSIRISEXTERN File_WriteInt_fp dFile_WriteInt;

//	void File_WriteShort(short s,void *fileptr);
//Write a short (16 bits)
//Throws an exception of type (cfile_error *) if the OS returns an error on write
typedef void( *File_WriteShort_fp)(short s,void *fileptr);
OSIRISEXTERN File_WriteShort_fp dFile_WriteShort;

//	void File_WriteByte(sbyte b,void *fileptr);
//Write a byte (8 bits).  If the byte is a newline & the file is a text file, writes a CR/LF pair.
//Throws an exception of type (cfile_error *) if the OS returns an error on write
typedef void( *File_WriteByte_fp)(sbyte b,void *fileptr);
OSIRISEXTERN File_WriteByte_fp dFile_WriteByte;

//	void File_WriteFloat(float f,void *fileptr);
//Write a float (32 bits)
//Throws an exception of type (cfile_error *) if the OS returns an error on write
typedef void( *File_WriteFloat_fp)(float f,void *fileptr);
OSIRISEXTERN File_WriteFloat_fp dFile_WriteFloat;

//	void File_WriteDouble(double d,void *fileptr);
//Write a double (64 bits)
//Throws an exception of type (cfile_error *) if the OS returns an error on write
typedef void( *File_WriteDouble_fp)(double d,void *fileptr);
OSIRISEXTERN File_WriteDouble_fp dFile_WriteDouble;

//	void *Scrpt_MemAlloc(tOSIRISMEMCHUNK *mem_info);
//		Allocates a chunk of memory to be associated with a script.  It will automatically
//	save this memory to disk on game save, and will pass the pointer to this memory on EVT_MEMRESTORE
typedef void*( *Scrpt_MemAlloc_fp)(tOSIRISMEMCHUNK *mem_info);
OSIRISEXTERN Scrpt_MemAlloc_fp dScrpt_MemAlloc;

//	void Scrpt_MemFree(void *memory_ptr);
//		Frees a chunk of memory that was allocated by Scrpt_MemAlloc().
typedef void( *Scrpt_MemFree_fp)(void *memory_ptr);
OSIRISEXTERN Scrpt_MemFree_fp dScrpt_MemFree;

//	void Scrpt_CancelTimer(int timer_id);
//		Cancels a timer thats in use, given it's ID
typedef void( *Scrpt_CancelTimer_fp)(int timer_id);
OSIRISEXTERN Scrpt_CancelTimer_fp dScrpt_CancelTimer;

//	int Scrpt_CreateTimer(tOSIRISTIMER *timer_info);
//		Adds a timer to the list to be processed.  You'll receive a EVT_TIMER when the timer is signaled.
//	Returns an id to the timer, which can be used to cancel a timer. -1 on error.
typedef int( *Scrpt_CreateTimer_fp)(tOSIRISTIMER *timer_info);
OSIRISEXTERN Scrpt_CreateTimer_fp dScrpt_CreateTimer;

//	void MSafe_DoPowerup (msafe_struct *mstruct);
//	Does whatever magic needs to be done to get the default powerups to work
typedef void( *MSafe_DoPowerup_fp)(msafe_struct *mstruct);
OSIRISEXTERN MSafe_DoPowerup_fp dMSafe_DoPowerup;

// int Obj_Create()(ubyte type,ushort id,int roomnum,vector *pos,const matrix *orient,int parent_handle)
typedef int( *Obj_Create_fp)(ubyte type,ushort id,int roomnum,vector *pos,const matrix *orient
#ifdef def_arg
 = NULL
#endif
,int parent_handle
#ifdef def_arg
 = OBJECT_HANDLE_NONE
#endif 
,vector *initial_velocity
#ifdef def_arg
=NULL
#endif
);
OSIRISEXTERN Obj_Create_fp dObj_Create;

// float Game_GetTime() (void)
typedef float(*Game_GetTime_fp)(void);
OSIRISEXTERN Game_GetTime_fp dGame_GetTime;

// float Game_GetFrameTime() (void)
typedef float(*Game_GetFrameTime_fp)(void);
OSIRISEXTERN Game_GetFrameTime_fp dGame_GetFrameTime;

// void Obj_WBValue() (int obj_handle, char wb_index, char op, char vtype, void *ptr, char g_index)
typedef void(*Obj_WBValue_fp)(int obj_handle, char wb_index, char op, char vtype, void *ptr, char g_index
#ifdef def_arg
 = 0
#endif 
);
OSIRISEXTERN Obj_WBValue_fp dObj_WBValue;

// ubyte Scrpt_TimerExists(int handle);
typedef ubyte( *Scrpt_TimerExists_fp)(int handle);
OSIRISEXTERN Scrpt_TimerExists_fp dScrpt_TimerExists;

typedef void(*Matcen_Reset_fp)(int handle);
OSIRISEXTERN Matcen_Reset_fp dMatcen_Reset;

typedef void(*Matcen_Copy_fp)(int dhandle, int shandle);
OSIRISEXTERN Matcen_Copy_fp dMatcen_Copy;

typedef int(*Matcen_Create_fp)(char *name);
OSIRISEXTERN Matcen_Create_fp dMatcen_Create;

typedef int(*Matcen_FindID_fp)(char *str);
OSIRISEXTERN Matcen_FindID_fp dMatcen_FindID;

//	void Msn_FlagSet(int flag,ubyte value);
//	Sets/Clears mission flags. flag is which mission flag to set/clear (1-32)
//	value is 0 to clear, or 1 to set
typedef void(*Msn_FlagSet_fp)(int flag,ubyte value);
OSIRISEXTERN Msn_FlagSet_fp dMsn_FlagSet;

//	int Msn_FlagGet(int flag);
//	Gets a mission flag. flag is what mission flag to get.  Returns 1 if set, 0 if not.
typedef int(*Msn_FlagGet_fp)(int flag);
OSIRISEXTERN Msn_FlagGet_fp dMsn_FlagGet;

typedef void(*Player_Value_fp)(int obj_handle, char op, char vhandle, void *ptr, int index
#ifdef def_arg
 = 0
#endif
);
OSIRISEXTERN Player_Value_fp dPlayer_Value;

typedef void(*Obj_SetCustomAnim_fp)(int handle, float start, float end, float time, char flags, int sound_handle
#ifdef def_arg
 = -1
#endif
, char next_anim_type
#ifdef def_arg
 = -1
#endif 
);
OSIRISEXTERN Obj_SetCustomAnim_fp dObj_SetCustomAnim;

typedef void(*Player_AddHudMessage_fp)(int handle, char *str);
OSIRISEXTERN Player_AddHudMessage_fp dPlayer_AddHudMessage;

typedef void(*Obj_Ghost_fp)(int handle, bool f_ghost);
OSIRISEXTERN Obj_Ghost_fp dObj_Ghost;

typedef void(*Obj_Burning_fp)(int handle, float time, float damage_per_second
#ifdef def_arg
 = 1.0f
#endif
);
OSIRISEXTERN Obj_Burning_fp dObj_Burning;

typedef bool(*Obj_IsEffect_fp)(int handle, int type_flag);
OSIRISEXTERN Obj_IsEffect_fp dObj_IsEffect;

//	opens a cfile for reading/writing (works just like fopen())
typedef void *(*File_Open_fp)(const char *filename,const char *mode);
OSIRISEXTERN File_Open_fp dFile_Open;

//	closes a cfile (like fclose())
typedef void (*File_Close_fp)(void *fileptr);
OSIRISEXTERN File_Close_fp dFile_Close;

//	returns the current position of the file (like ftell())
typedef int (*File_Tell_fp)(void *fileptr);
OSIRISEXTERN File_Tell_fp dFile_Tell;

//	returns 1 if the file is at the EOF, else 0 (like feof())
typedef ubyte (*File_eof_fp)(void *fileptr);
OSIRISEXTERN File_eof_fp dFile_eof;

typedef void (*Sound_Stop_fp)(int s_handle, bool f_immediately
#ifdef def_arg
 = false
#endif
);
OSIRISEXTERN Sound_Stop_fp dSound_Stop;

typedef int (*Sound_Play2d_fp)(int obj_handle, int s_id, float volume
#ifdef def_arg
 = 1.0f
#endif
);
OSIRISEXTERN Sound_Play2d_fp dSound_Play2d;

typedef int (*Sound_Play3d_fp)(int obj_handle, int s_id, float volume
#ifdef def_arg
 = 1.0f
#endif
);
OSIRISEXTERN Sound_Play3d_fp dSound_Play3d;

typedef int (*Sound_FindId_fp)(char *s_name);
OSIRISEXTERN Sound_FindId_fp dSound_FindId;

typedef bool (*AI_IsObjFriend_fp)(int obj_handle, int it_handle);
OSIRISEXTERN AI_IsObjFriend_fp dAI_IsObjFriend;

typedef bool (*AI_IsObjEnemy_fp)(int obj_handle, int it_handle);
OSIRISEXTERN AI_IsObjEnemy_fp dAI_IsObjEnemy;

typedef bool (*AI_GoalValue_fp)(int obj_handle, char g_index, char op, char vtype, void *ptr, char index
#ifdef def_arg
 = 0
#endif 
); 
OSIRISEXTERN AI_GoalValue_fp dAI_GoalValue;

typedef int (*AI_GetNearbyObjs_fp)(vector *pos, int init_roomnum, float rad, int *object_handle_list, int max_elements, bool f_lightmap_only, bool f_only_players_and_ais
#ifdef def_arg
 = true
#endif 
, bool f_include_non_collide_objects
#ifdef def_arg
 = false
#endif 
, bool f_stop_at_closed_doors
#ifdef def_arg
 = true
#endif
);
OSIRISEXTERN AI_GetNearbyObjs_fp dAI_GetNearbyObjs;

typedef char (*AI_GetCurGoalIndex_fp)(int obj_handle);
OSIRISEXTERN AI_GetCurGoalIndex_fp dAI_GetCurGoalIndex;

// Allocates a block of global memory for this module, of size amount_of_memory.
// unique_identfier is the script provided unique ID which is used throughout.
// script_identifier is the pointer of data provided to the script in it's InitializeDLL function (really the name of the script)
// Returns -1 if there isn't enough available memory
// Returns -2 if the unique identifier passed in is already used, but the requested amount_of_memory is different
// If the memory has already been allocated, it will return the handle.
typedef OMMSHANDLE (*OMMS_Malloc_fp)(int amount_of_memory,uint unique_identifier,char *script_identifier);
OSIRISEXTERN OMMS_Malloc_fp dOMMS_Malloc;

//	Attaches to a block of global OMMS memory.  As long as at least one module (or script) is
//	attached to a module, the memory will not be deleted. (Increments the reference count)
//	Returns NULL if the memory couldn't be attached (it has been either free'd or never malloced)
typedef void *(*OMMS_Attach_fp)(OMMSHANDLE handle);
OSIRISEXTERN OMMS_Attach_fp dOMMS_Attach;

//	Detaches a block of global OMMS memory. (Reduces the reference count).
typedef void (*OMMS_Detach_fp)(OMMSHANDLE handle);
OSIRISEXTERN OMMS_Detach_fp dOMMS_Detach;

//	Frees a block of global memory
//	Only has affect if you are attached to the memory.  Memory will _ONLY_ be deleted when the
//	following conditions are meant for the shared memory block:
//	1) The reference count for the global memory is at 0 (OMMS_Attach() adds to reference count, OMMS_Detach() subtracts).
//	2) OMMS_Free() has been called for the block of global memory.
//	The conditions can happen in any order, but as soon as both are satisfied, the memory
//	becomes deleted and any pointer returned by OMMS_Attach() becomes invalid.
//	
//	handle : the value returned by OMMS_Malloc()
typedef void (*OMMS_Free_fp)(OMMSHANDLE handle);
OSIRISEXTERN OMMS_Free_fp dOMMS_Free;

//	Returns an OMMSHANDLE to a block of global memory allocated by a module/script.  Pass
//	in the unique_identifier and the script_identifier that was passed in the OMMS_Malloc().
//	Note: script_identifier is really the filename of the module that called the OMMS_Malloc().
//	Returns -1 if the module was never OMMS_Malloc()'d.
typedef OMMSHANDLE (*OMMS_Find_fp)(uint unique_identifier,char *script_identifier);
OSIRISEXTERN OMMS_Find_fp dOMMS_Find;

//	Returns information about the OMMS memory given it's handle returned from the OMMS_Find() or
//	OMMS_Malloc(). Returns 0 if the handle was invalid, 1 if the information has been filled in;
//	Pass NULL in for those parameters you don't need information about.
typedef char (*OMMS_GetInfo_fp)(OMMSHANDLE handle,uint *mem_size,uint *uid,ushort *reference_count,ubyte *has_free_been_called);
OSIRISEXTERN OMMS_GetInfo_fp dOMMS_GetInfo;

//	Starts an in-game cinematic sequence.  text_string is the text to be displayed
//	use pipes (|) to seperate lines.
typedef bool (*Cine_Start_fp)(tGameCinematic *info,char *text_string);
OSIRISEXTERN Cine_Start_fp dCine_Start;

//	Stops and clears up a in-game cinematic.
typedef void (*Cine_Stop_fp)(void);
OSIRISEXTERN Cine_Stop_fp dCine_Stop;

// Looks up the id's of the sound, room, trigger, object, ect. based on the name
typedef int (*Scrpt_FindSoundName_fp)(char *name);
OSIRISEXTERN Scrpt_FindSoundName_fp dScrpt_FindSoundName;

typedef int (*Scrpt_FindRoomName_fp)(char *name);
OSIRISEXTERN Scrpt_FindRoomName_fp dScrpt_FindRoomName;
	
typedef int (*Scrpt_FindTriggerName_fp)(char *name);
OSIRISEXTERN Scrpt_FindTriggerName_fp dScrpt_FindTriggerName;

typedef int (*Scrpt_FindObjectName_fp)(char *name);
OSIRISEXTERN Scrpt_FindObjectName_fp dScrpt_FindObjectName;

typedef int (*Scrpt_GetTriggerRoom_fp)(int trigger_id);
OSIRISEXTERN Scrpt_GetTriggerRoom_fp dScrpt_GetTriggerRoom;

typedef int (*Scrpt_GetTriggerFace_fp)(int trigger_id);
OSIRISEXTERN Scrpt_GetTriggerFace_fp dScrpt_GetTriggerFace;

typedef int (*Scrpt_FindDoorName_fp)(char *name);
OSIRISEXTERN Scrpt_FindDoorName_fp dScrpt_FindDoorName;

typedef int (*Scrpt_FindTextureName_fp)(char *name);
OSIRISEXTERN Scrpt_FindTextureName_fp dScrpt_FindTextureName;

typedef void (*Game_CreateRandomSparks_fp)(int num_sparks,vector *pos,int roomnum,int which_index
#ifdef def_arg
 = -1
#endif
,float force_scalar
#ifdef def_arg
 = 1.0f
#endif 
);
OSIRISEXTERN Game_CreateRandomSparks_fp dGame_CreateRandomSparks;

//		Cancels a timer given its ID
typedef void (*Scrpt_CancelTimerID_fp)(int id);
OSIRISEXTERN Scrpt_CancelTimerID_fp dScrpt_CancelTimerID;

//disable/enable ship
typedef void (*Game_EnableShip_fp)(char *ship_name,bool enable);
OSIRISEXTERN Game_EnableShip_fp dGame_EnableShip;

//is ship enabled
typedef bool (*Game_IsShipEnabled_fp)(char *ship_name);
OSIRISEXTERN Game_IsShipEnabled_fp dGame_IsShipEnabled;

//gets information about a path point
//pass NULL for parameters not needed.
//	pathid: path number
//	point: which path point
//returns true if operation was successful
typedef bool (*Path_GetInformation_fp)(int pathid,int point,vector *pos
#ifdef def_arg
=NULL
#endif
,int *room
#ifdef def_arg
=NULL
#endif
,matrix *orient
#ifdef def_arg
=NULL
#endif
);
OSIRISEXTERN Path_GetInformation_fp dPath_GetInformation;

//starts a canned cinematic sequence
typedef void (*Cine_StartCanned_fp)(tCannedCinematicInfo *info);
OSIRISEXTERN Cine_StartCanned_fp dCine_StartCanned;

typedef int (*Scrpt_FindMatcenName_fp)(char *name);
OSIRISEXTERN Scrpt_FindMatcenName_fp dScrpt_FindMatcenName;

typedef int (*Scrpt_FindPathName_fp)(char *name);
OSIRISEXTERN Scrpt_FindPathName_fp dScrpt_FindPathName;

typedef int (*Scrpt_FindLevelGoalName_fp)(char *name);
OSIRISEXTERN Scrpt_FindLevelGoalName_fp dScrpt_FindLevelGoalName;

typedef void (*LGoal_Value_fp)(char op, char vtype, void *ptr, int g_index
#ifdef def_arg
 = -1
#endif 
, int i_index
#ifdef def_arg
 = -1
#endif
);
OSIRISEXTERN LGoal_Value_fp dLGoal_Value;

typedef int (*Obj_MakeListOfType_fp)(int objhandle, int type, int id, bool f_ignore_init_room, int parent_handle, int max_recorded, int *handles);
OSIRISEXTERN Obj_MakeListOfType_fp dObj_MakeListOfType;

typedef void (*Obj_Kill_fp)(int handle, int killer_handle, float damage, int flags, float min_time, float max_time);
OSIRISEXTERN Obj_Kill_fp dObj_Kill;

//typedef bool (*AI_AreRoomsReachable_fp)(int room1, int room2);
//OSIRISEXTERN AI_AreRoomsReachable_fp dAI_AreRoomsReachable;

typedef bool (*AI_IsDestReachable_fp)(int handle, int room);
OSIRISEXTERN AI_IsDestReachable_fp dAI_IsDestReachable;

typedef bool (*AI_IsObjReachable_fp)(int handle, int target);
OSIRISEXTERN AI_IsObjReachable_fp dAI_IsObjReachable;

typedef char (*Game_GetDiffLevel_fp)(void);
OSIRISEXTERN Game_GetDiffLevel_fp dGame_GetDiffLevel;


typedef int (*Game_GetLanguage_fp)(void);
OSIRISEXTERN Game_GetLanguage_fp dGame_GetLanguage;


// Sets/Gets information about a path.
// If you change is PV_ALL (or any of it's individual components), pass in a pointer to an
// osiris_path_node_info struct.  For the others, you must pass in an appropriate pointer
// (i.e. an int* for PV_I_NUMNODES).  You can only set PV_ALL components.
//	for PV_I_NUMNODES, path_id MUST be specified, node_id is ignored
//	for PV_I_NUMPATHS, path_id and node_id are ignored
//	for PV_CS_NAME, path_id MUST be specified, node_id is ignored
//	for PV_ALL components, path_id and node_id MUST be valid.
typedef void (*Path_Value_fp)(int path_id, int node_id, char op, int changes, void *ptr);
OSIRISEXTERN Path_Value_fp dPath_Value;

// ===========================================================

typedef struct{
	mprintf_fp mprintf;
	MSafe_CallFunction_fp MSafe_CallFunction;
	MSafe_GetValue_fp MSafe_GetValue;
	Obj_CallEvent_fp Obj_CallEvent;
	Trgr_CallEvent_fp Trgr_CallEvent;
	Sound_TouchFile_fp Sound_TouchFile;
	Obj_FindID_fp Obj_FindID;
	Wpn_FindID_fp Wpn_FindID;
	Obj_GetTimeLived_fp Obj_GetTimeLived;
	Obj_GetGunPos_fp Obj_GetGunPos;
	Room_Value_fp Room_Value;
	Room_IsValid_fp Room_IsValid;
	Obj_GetAttachParent_fp Obj_GetAttachParent;
	Obj_GetNumAttachSlots_fp Obj_GetNumAttachSlots;
	Obj_GetAttachChildHandle_fp Obj_GetAttachChildHandle;
	Obj_AttachObjectAP_fp Obj_AttachObjectAP;
	Obj_AttachObjectRad_fp Obj_AttachObjectRad;
	Obj_UnattachFromParent_fp Obj_UnattachFromParent;
	Obj_UnattachChild_fp Obj_UnattachChild;
	Obj_UnattachChildren_fp Obj_UnattachChildren;
	FVI_RayCast_fp FVI_RayCast;
	AI_GetPathID_fp AI_GetPathID;
	AI_GoalFollowPathSimple_fp AI_GoalFollowPathSimple;
	AI_PowerSwitch_fp AI_PowerSwitch;
	AI_TurnTowardsVectors_fp AI_TurnTowardsVectors;
	AI_SetType_fp AI_SetType;
	AI_FindHidePos_fp AI_FindHidePos;
	AI_GoalAddEnabler_fp AI_GoalAddEnabler;
	AI_AddGoal_fp AI_AddGoal;
	AI_ClearGoal_fp AI_ClearGoal;
	AI_Value_fp AI_Value;
	AI_FindObjOfType_fp AI_FindObjOfType;
	AI_GetRoomPathPoint_fp AI_GetRoomPathPoint;
	AI_FindEnergyCenter_fp AI_FindEnergyCenter;
	AI_GetDistToObj_fp AI_GetDistToObj;
	AI_SetGoalFlags_fp AI_SetGoalFlags;
	AI_SetGoalCircleDist_fp AI_SetGoalCircleDist;
	File_ReadBytes_fp File_ReadBytes;
	File_ReadInt_fp File_ReadInt;
	File_ReadShort_fp File_ReadShort;
	File_ReadByte_fp File_ReadByte;
	File_ReadFloat_fp File_ReadFloat;
	File_ReadDouble_fp File_ReadDouble;
	File_ReadString_fp File_ReadString;
	File_WriteBytes_fp File_WriteBytes;
	File_WriteString_fp File_WriteString;
	File_WriteInt_fp File_WriteInt;
	File_WriteShort_fp File_WriteShort;
	File_WriteByte_fp File_WriteByte;
	File_WriteFloat_fp File_WriteFloat;
	File_WriteDouble_fp File_WriteDouble;
	Scrpt_MemAlloc_fp Scrpt_MemAlloc;
	Scrpt_MemFree_fp Scrpt_MemFree;
	Scrpt_CancelTimer_fp Scrpt_CancelTimer;
	Scrpt_CreateTimer_fp Scrpt_CreateTimer;
	MSafe_DoPowerup_fp MSafe_DoPowerup;
	Obj_Create_fp Obj_Create;
	Game_GetTime_fp Game_GetTime;
	Game_GetFrameTime_fp Game_GetFrameTime;
	Obj_WBValue_fp Obj_WBValue;
	Scrpt_TimerExists_fp Scrpt_TimerExists;
	Obj_Value_fp Obj_Value;
	Matcen_Value_fp Matcen_Value;
	Matcen_Reset_fp Matcen_Reset;
	Matcen_Copy_fp Matcen_Copy;
	Matcen_Create_fp Matcen_Create;
	Matcen_FindID_fp Matcen_FindID;
	Msn_FlagSet_fp Msn_FlagSet;
	Msn_FlagGet_fp Msn_FlagGet;
	Player_Value_fp Player_Value;
	Obj_SetCustomAnim_fp Obj_SetCustomAnim;
	Player_AddHudMessage_fp Player_AddHudMessage;
	Obj_Ghost_fp Obj_Ghost;
	Obj_Burning_fp Obj_Burning;
	Obj_IsEffect_fp Obj_IsEffect;
	File_Open_fp File_Open;
	File_Close_fp File_Close;
	File_Tell_fp File_Tell;
	File_eof_fp File_eof;
	Sound_Stop_fp Sound_Stop;
	Sound_Play2d_fp Sound_Play2d;
	Sound_Play3d_fp Sound_Play3d;
	Sound_FindId_fp Sound_FindId;
	AI_IsObjFriend_fp AI_IsObjFriend;
	AI_IsObjEnemy_fp AI_IsObjEnemy;
	AI_GoalValue_fp AI_GoalValue;
	AI_GetNearbyObjs_fp AI_GetNearbyObjs;
	AI_GetCurGoalIndex_fp AI_GetCurGoalIndex;
	OMMS_Malloc_fp OMMS_Malloc;
	OMMS_Attach_fp OMMS_Attach;
	OMMS_Detach_fp OMMS_Detach;
	OMMS_Free_fp OMMS_Free;
	OMMS_Find_fp OMMS_Find;
	OMMS_GetInfo_fp OMMS_GetInfo;
	Cine_Start_fp Cine_Start;
	Cine_Stop_fp Cine_Stop;
	Scrpt_FindSoundName_fp Scrpt_FindSoundName;
	Scrpt_FindRoomName_fp Scrpt_FindRoomName;
	Scrpt_FindTriggerName_fp Scrpt_FindTriggerName;
	Scrpt_FindObjectName_fp Scrpt_FindObjectName;
	Scrpt_GetTriggerRoom_fp Scrpt_GetTriggerRoom;
	Scrpt_GetTriggerFace_fp Scrpt_GetTriggerFace;
	Scrpt_FindDoorName_fp Scrpt_FindDoorName;
	Scrpt_FindTextureName_fp Scrpt_FindTextureName;
	Game_CreateRandomSparks_fp Game_CreateRandomSparks;
	Scrpt_CancelTimerID_fp Scrpt_CancelTimerID;
	Obj_GetGroundPos_fp Obj_GetGroundPos;
	Game_EnableShip_fp Game_EnableShip;
	Game_IsShipEnabled_fp Game_IsShipEnabled;
	Path_GetInformation_fp Path_GetInformation;
	Cine_StartCanned_fp Cine_StartCanned;
	Scrpt_FindMatcenName_fp Scrpt_FindMatcenName;
	Scrpt_FindPathName_fp Scrpt_FindPathName;
	Scrpt_FindLevelGoalName_fp Scrpt_FindLevelGoalName;
	Obj_FindType_fp Obj_FindType;
	LGoal_Value_fp LGoal_Value;
	Obj_MakeListOfType_fp Obj_MakeListOfType;
	Obj_Kill_fp Obj_Kill;
	AI_IsDestReachable_fp AI_IsDestReachable;
	AI_IsObjReachable_fp AI_IsObjReachable;
	Game_GetDiffLevel_fp Game_GetDiffLevel;
	Game_GetLanguage_fp Game_GetLanguage;
	Path_Value_fp Path_Value;
	int *restfp[MAX_MODULEFUNCS-123];
	char **string_table;
	int string_count;
	int module_identifier;
	
	bool module_is_static;							// if this is set to true after initialization
													// then the module will not unload if it's reference
													// count is 0....only when the level ends.
													// this is for Game modules ONLY.
	
	char *script_identifier;						// Contains a unique script identifier (string), which
													// can be used for the OMMS system. While this pointer
													// will always be valid during the lifetime of the module
													// DO NOT ALTER THE STRING IT POINTS TO.

	uint game_checksum;								// Checksum of game structures, if this doesn't match
													// the checksum at compile time, it is very likely that
													// bad things can happen, and this module shouldn't initialize.

}tOSIRISModuleInit;									// contains the necessary data to initialize

//	osicommon_Initialize
//	Initializes module's functions
void osicommon_Initialize(tOSIRISModuleInit *mi);
#if 0
#ifdef __cplusplus
#ifndef __OSIRIS_IMPORT_H_
void osicommon_Initialize(tOSIRISModuleInit *mi)
{
	int i = 0;
	mprintf					= (mprintf_fp)mi->fp[i++];
	MSafe_CallFunction		= (MSafe_CallFunction_fp)mi->fp[i++];
	MSafe_GetValue			= (MSafe_GetValue_fp)mi->fp[i++];
	Obj_CallEvent			= (Obj_CallEvent_fp)mi->fp[i++];
	Trgr_CallEvent			= (Trgr_CallEvent_fp)mi->fp[i++];
	Sound_TouchFile			= (Sound_TouchFile_fp)mi->fp[i++];
	Obj_FindID				= (Obj_FindID_fp)mi->fp[i++];
	Wpn_FindID				= (Wpn_FindID_fp)mi->fp[i++];
	Obj_GetTimeLived		= (Obj_GetTimeLived_fp)mi->fp[i++];
	Obj_GetGunPos			= (Obj_GetGunPos_fp)mi->fp[i++];
	Room_Value      		= (Room_Value_fp)mi->fp[i++];
	Room_IsValid			= (Room_IsValid_fp)mi->fp[i++];
	Obj_GetAttachParent		= (Obj_GetAttachParent_fp)mi->fp[i++];
	Obj_GetNumAttachSlots	= (Obj_GetNumAttachSlots_fp)mi->fp[i++];
	Obj_GetAttachChildHandle= (Obj_GetAttachChildHandle_fp)mi->fp[i++];
	Obj_AttachObjectAP		= (Obj_AttachObjectAP_fp)mi->fp[i++];
	Obj_AttachObjectRad		= (Obj_AttachObjectRad_fp)mi->fp[i++];
	Obj_UnattachFromParent	= (Obj_UnattachFromParent_fp)mi->fp[i++];
	Obj_UnattachChild		= (Obj_UnattachChild_fp)mi->fp[i++];
	Obj_UnattachChildren	= (Obj_UnattachChildren_fp)mi->fp[i++];
	FVI_RayCast				= (FVI_RayCast_fp)mi->fp[i++];
	AI_GetPathID			= (AI_GetPathID_fp)mi->fp[i++];
	AI_GoalFollowPathSimple	= (AI_GoalFollowPathSimple_fp)mi->fp[i++];
	AI_PowerSwitch			= (AI_PowerSwitch_fp)mi->fp[i++];
	AI_TurnTowardsVectors	= (AI_TurnTowardsVectors_fp)mi->fp[i++];
	AI_SetType				= (AI_SetType_fp)mi->fp[i++];
	AI_FindHidePos			= (AI_FindHidePos_fp)mi->fp[i++];
	AI_GoalAddEnabler		= (AI_GoalAddEnabler_fp)mi->fp[i++];
	AI_AddGoal				= (AI_AddGoal_fp)mi->fp[i++];
	AI_ClearGoal			= (AI_ClearGoal_fp)mi->fp[i++];
	AI_Value             = (AI_Value_fp)mi->fp[i++];
	AI_FindObjOfType		= (AI_FindObjOfType_fp)mi->fp[i++];
	AI_GetRoomPathPoint		= (AI_GetRoomPathPoint_fp)mi->fp[i++];
	AI_FindEnergyCenter		= (AI_FindEnergyCenter_fp)mi->fp[i++];
	AI_GetDistToObj			= (AI_GetDistToObj_fp)mi->fp[i++];
	AI_SetGoalFlags			= (AI_SetGoalFlags_fp)mi->fp[i++];
	AI_SetGoalCircleDist	= (AI_SetGoalCircleDist_fp)mi->fp[i++];
	File_ReadBytes			= (File_ReadBytes_fp)mi->fp[i++];
	File_ReadInt			= (File_ReadInt_fp)mi->fp[i++];
	File_ReadShort			= (File_ReadShort_fp)mi->fp[i++];
	File_ReadByte			= (File_ReadByte_fp)mi->fp[i++];
	File_ReadFloat			= (File_ReadFloat_fp)mi->fp[i++];
	File_ReadDouble			= (File_ReadDouble_fp)mi->fp[i++];
	File_ReadString			= (File_ReadString_fp)mi->fp[i++];
	File_WriteBytes			= (File_WriteBytes_fp)mi->fp[i++];
	File_WriteString		= (File_WriteString_fp)mi->fp[i++];
	File_WriteInt			= (File_WriteInt_fp)mi->fp[i++];
	File_WriteShort			= (File_WriteShort_fp)mi->fp[i++];
	File_WriteByte			= (File_WriteByte_fp)mi->fp[i++];
	File_WriteFloat			= (File_WriteFloat_fp)mi->fp[i++];
	File_WriteDouble		= (File_WriteDouble_fp)mi->fp[i++];
	Scrpt_MemAlloc			= (Scrpt_MemAlloc_fp)mi->fp[i++];
	Scrpt_MemFree			= (Scrpt_MemFree_fp)mi->fp[i++];
	Scrpt_CancelTimer		= (Scrpt_CancelTimer_fp)mi->fp[i++];
	Scrpt_CreateTimer		= (Scrpt_CreateTimer_fp)mi->fp[i++];
	MSafe_DoPowerup			= (MSafe_DoPowerup_fp)mi->fp[i++];
	Obj_Create           = (Obj_Create_fp)mi->fp[i++];
	Game_GetTime         = (Game_GetTime_fp)mi->fp[i++];
	Game_GetFrameTime    = (Game_GetFrameTime_fp)mi->fp[i++];
	Obj_WBValue          = (Obj_WBValue_fp)mi->fp[i++];
	Scrpt_TimerExists	 = (Scrpt_TimerExists_fp)mi->fp[i++];
	Obj_Value          = (Obj_Value_fp)mi->fp[i++];
	Matcen_Value       = (Matcen_Value_fp)mi->fp[i++];
	Matcen_Reset       = (Matcen_Reset_fp)mi->fp[i++];
	Matcen_Copy        = (Matcen_Copy_fp)mi->fp[i++];
	Matcen_Create      = (Matcen_Create_fp)mi->fp[i++];
	Matcen_FindID      = (Matcen_FindID_fp)mi->fp[i++];
	Msn_FlagSet			= (Msn_FlagSet_fp)mi->fp[i++];
	Msn_FlagGet			= (Msn_FlagGet_fp)mi->fp[i++];
	Player_Value		= (Player_Value_fp)mi->fp[i++];
   Obj_SetCustomAnim = (Obj_SetCustomAnim_fp)mi->fp[i++];
	Player_AddHudMessage = (Player_AddHudMessage_fp)mi->fp[i++];
	Obj_Ghost            = (Obj_Ghost_fp)mi->fp[i++];
   Obj_Burning          = (Obj_Burning_fp)mi->fp[i++];
	Obj_IsEffect         = (Obj_IsEffect_fp)mi->fp[i++];
	File_Open			= (File_Open_fp)mi->fp[i++];
	File_Close			= (File_Close_fp)mi->fp[i++];
	File_Tell			= (File_Tell_fp)mi->fp[i++];
	File_eof			= (File_eof_fp)mi->fp[i++];
	Sound_Stop = (Sound_Stop_fp)mi->fp[i++];
	Sound_Play2d = (Sound_Play2d_fp)mi->fp[i++];
	Sound_Play3d = (Sound_Play3d_fp)mi->fp[i++];
	Sound_FindId = (Sound_FindId_fp)mi->fp[i++];
	AI_IsObjFriend = (AI_IsObjFriend_fp)mi->fp[i++];
	AI_IsObjEnemy = (AI_IsObjEnemy_fp)mi->fp[i++];
	AI_GoalValue = (AI_GoalValue_fp)mi->fp[i++];
	AI_GetNearbyObjs = (AI_GetNearbyObjs_fp)mi->fp[i++];
	AI_GetCurGoalIndex = (AI_GetCurGoalIndex_fp)mi->fp[i++];
	OMMS_Malloc = (OMMS_Malloc_fp)mi->fp[i++];
	OMMS_Attach = (OMMS_Attach_fp)mi->fp[i++];
	OMMS_Detach = (OMMS_Detach_fp)mi->fp[i++];
	OMMS_Free = (OMMS_Free_fp)mi->fp[i++];
	OMMS_Find = (OMMS_Find_fp)mi->fp[i++];
	OMMS_GetInfo = (OMMS_GetInfo_fp)mi->fp[i++];
	Cine_Start = (Cine_Start_fp)mi->fp[i++];
	Cine_Stop = (Cine_Stop_fp)mi->fp[i++];
	Scrpt_FindSoundName = (Scrpt_FindSoundName_fp)mi->fp[i++];
	Scrpt_FindRoomName = (Scrpt_FindRoomName_fp)mi->fp[i++];
	Scrpt_FindTriggerName = (Scrpt_FindTriggerName_fp)mi->fp[i++];
	Scrpt_FindObjectName = (Scrpt_FindObjectName_fp)mi->fp[i++];
	Scrpt_GetTriggerRoom = (Scrpt_GetTriggerRoom_fp)mi->fp[i++];
	Scrpt_GetTriggerFace = (Scrpt_GetTriggerFace_fp)mi->fp[i++];
	Scrpt_FindDoorName = (Scrpt_FindDoorName_fp)mi->fp[i++];
	Scrpt_FindTextureName = (Scrpt_FindTextureName_fp)mi->fp[i++];
	Game_CreateRandomSparks = (Game_CreateRandomSparks_fp)mi->fp[i++];
	Scrpt_CancelTimerID	= (Scrpt_CancelTimerID_fp)mi->fp[i++];
	Obj_GetGroundPos = (Obj_GetGroundPos_fp)mi->fp[i++];
	Game_EnableShip	= (Game_EnableShip_fp)mi->fp[i++];
	Game_IsShipEnabled = (Game_IsShipEnabled_fp)mi->fp[i++];
	Path_GetInformation = (Path_GetInformation_fp)mi->fp[i++];
	Cine_StartCanned = (Cine_StartCanned_fp)mi->fp[i++];
	Scrpt_FindMatcenName = (Scrpt_FindMatcenName_fp)mi->fp[i++];
	Scrpt_FindPathName = (Scrpt_FindPathName_fp)mi->fp[i++];
	Scrpt_FindLevelGoalName = (Scrpt_FindLevelGoalName_fp)mi->fp[i++];
	Obj_FindType = (Obj_FindType_fp)mi->fp[i++];
	LGoal_Value = (LGoal_Value_fp)mi->fp[i++];
	Obj_MakeListOfType = (Obj_MakeListOfType_fp)mi->fp[i++];
	Obj_Kill = (Obj_Kill_fp)mi->fp[i++];
//	AI_AreRoomsReachable = (AI_AreRoomsReachable_fp)mi->fp[i++];
	AI_IsDestReachable = (AI_IsDestReachable_fp)mi->fp[i++];
	AI_IsObjReachable = (AI_IsObjReachable_fp)mi->fp[i++];
	Game_GetDiffLevel = (Game_GetDiffLevel_fp )mi->fp[i++];
	Game_GetLanguage = (Game_GetLanguage_fp)mi->fp[i++];
	Path_Value = (Path_Value_fp)mi->fp[i++];
}
#endif
#endif
#endif

#ifndef __OSIRIS_IMPORT_H_
#define __OSIRIS_IMPORT_H_
#endif
