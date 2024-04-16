#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdarg.h>
#include <malloc.h>
#ifdef WIN32
#include <windows.h>
#else
#include "dminwindef.h"
#include "dwinnt.h"
#include "dntimage.h"
#endif
#include "dll.h"
#include "msafenames.h"

typedef void x86emu_t;

//#undef WIN32
#include "CFILE.H"
#include "globals.h"
#define INCLUDED_FROM_D3
#include "osiris_import.h"
#include "osiris_dll.h"
#if 0
#define FILE CFILE
#define fopen cfopen
#define fclose cfclose
#define fread cfread
#define fseek cfseek
#define fgets cfgets
#endif

#if 0
struct {
	struct {
		unsigned segment;
		unsigned offset;
	} start;
	unsigned load;
	unsigned max_instructions;
	unsigned bits_32 : 1;
	char* file;
} opt = { .bits_32 = 1 };


x86emu_t* emu_new()
{
	x86emu_t* emu = x86emu_new(X86EMU_PERM_R | X86EMU_PERM_W | X86EMU_PERM_X, 0);
	return NULL;
}
#endif


unsigned tls_vars = 1;
unsigned tls_vals[20];
unsigned last_error;
#define ELMS(x) (sizeof(x)/sizeof((x)[0]))
#define TLS_OUT_OF_INDEXES 0xffffffff
#define ERROR_INVALID_PARAMETER 87

struct heap {
	int size;
	int limit;
	unsigned* allocs;
};
struct heap* heap_create() {
	struct heap* heap = (struct heap*)malloc(sizeof(*heap));
	heap->size = 256;
	heap->allocs = (unsigned*)malloc(heap->size * sizeof(unsigned));
	heap->limit = 0;
	return heap;
}
int heap_del(struct heap* heap, unsigned ptr) {
	for (int i = 0; i < heap->limit; i++)
		if (heap->allocs[i] == ptr) {
			if (i == heap->limit - 1)
				while (--heap->limit && !heap->allocs[heap->limit - 1])
					;
			else
				heap->allocs[i] = 0;
			return 1;
		}
	return 0;
}
int heap_add(struct heap* heap, unsigned ptr) {
	if (heap->limit < heap->size) {
		heap->allocs[heap->limit++] = ptr;
		return 1;
	}
	for (int i = 0; i < heap->size; i++)
		if (!heap->allocs[i]) {
			heap->allocs[i] = ptr;
			return 1;
		}
	int new_size = heap->size * 2;
	unsigned* new_allocs = (unsigned*)realloc(heap->allocs, new_size * sizeof(unsigned));
	if (!new_allocs)
		return 0;
	heap->size = new_size;
	heap->allocs = new_allocs;
	heap->allocs[heap->limit++] = ptr;
	return 1;
}
void heap_free_all(struct heap* heap) {
	for (int i = 0; i < heap->limit; i++)
		if (heap->allocs[i])
			free((void*)heap->allocs[i]);
	heap->limit = 0;
}
void heap_done(struct heap* heap) {
	free(heap->allocs);
	free(heap);
}

#if 0
static x86emu_t* emu;
static void* funcode;
static void* stack;
#endif

unsigned myGetStdHandle(unsigned type) { return 1; }
unsigned myWriteFile(unsigned hFile, unsigned buf, unsigned size, unsigned pret, unsigned ovl) {
#if 0
	int ret = write((int)hFile, (void*)buf, size);
	if (pret && ret >= 0)
		*(unsigned*)pret = ret;
	return ret >= 0;
#else
	return 0;
#endif
}
unsigned myVirtualAlloc(unsigned lpAddress, unsigned dwSize, unsigned flAllocationType, unsigned flProtect) {
	if (lpAddress)
		return flAllocationType == 0x1000 ? lpAddress : 0;
	return (unsigned)_aligned_malloc(dwSize, 4096);
}
unsigned __stdcall myIsBadWritePtr(unsigned a, unsigned b) { return 0; }
unsigned __stdcall myIsBadReadPtr(unsigned a, unsigned b) { return 0; }
unsigned __stdcall myHeapValidate(unsigned a, unsigned b, unsigned c) { return 1; }
unsigned __stdcall myGetCommandLineA() { return (unsigned)""; }
unsigned __stdcall myGetVersion() { return 0; }
unsigned __stdcall myGetProcAddress(unsigned lib, unsigned name) { printf("getprocaddr %x %s\n", lib, (char*)name); return 0; }
unsigned __stdcall myGetModuleHandleA(unsigned name) { static int mod = 0; printf("getmodhan %s\n", (char*)name); return (unsigned)&mod; }
unsigned __stdcall myGetCurrentThreadId() { return 0; }
unsigned __stdcall myTlsSetValue(unsigned n, unsigned val) { if (n >= tls_vars) { last_error = ERROR_INVALID_PARAMETER; return 0; } tls_vals[n] = val; return 1; }
unsigned __stdcall myTlsAlloc() { if (tls_vars == ELMS(tls_vals)) return TLS_OUT_OF_INDEXES; return tls_vars++; }
unsigned __stdcall myTlsFree(unsigned v) { if (v == tls_vars - 1) tls_vars--; return 1; }
unsigned __stdcall mySetLastError(unsigned err) { last_error = err; return 0; }
unsigned __stdcall myTlsGetValue(unsigned n) { if (n < tls_vars) { last_error = 0; return tls_vals[n]; } last_error = ERROR_INVALID_PARAMETER; return 0; }
unsigned __stdcall myGetLastError() { return last_error; }
unsigned __stdcall myDebugBreak() { return 0; }
unsigned __stdcall myInterlockedDecrement(unsigned a) { return 0; }
unsigned __stdcall myOutputDebugStringA(unsigned msg) { printf("OutputDebugString: %s\n", (char*)msg); return 0; }
unsigned __stdcall myLoadLibraryA(unsigned name) { printf("loadlib %s\n", (char*)name); return 0; }
unsigned __stdcall myInterlockedIncrement(unsigned a) { return 0; }
unsigned __stdcall myGetModuleFileNameA(unsigned a, unsigned b, unsigned c) { return 0; }
unsigned __stdcall myExitProcess(unsigned a) { /*x86emu_stop(emu);*/ return 0; }
unsigned __stdcall myTerminateProcess(unsigned a, unsigned b) { return 0; }
unsigned __stdcall myGetCurrentProcess() { return 0; }
unsigned __stdcall myInitializeCriticalSection(unsigned a) { return 0; }
unsigned __stdcall myDeleteCriticalSection(unsigned a) { return 0; }
unsigned __stdcall myEnterCriticalSection(unsigned a) { return 0; }
unsigned __stdcall myLeaveCriticalSection(unsigned a) { return 0; }
unsigned __stdcall myRtlUnwind(unsigned a, unsigned b, unsigned c, unsigned d) { return 0; }
unsigned __stdcall myHeapAlloc(unsigned heap, unsigned flags, unsigned size) { unsigned ptr = (unsigned)malloc(size); heap_add((struct heap*)heap, ptr); return ptr; }
unsigned __stdcall myHeapReAlloc(unsigned a, unsigned b, unsigned c, unsigned d) { return 0; }
unsigned __stdcall myHeapFree(unsigned heap, unsigned b, unsigned ptr) { if (!heap_del((struct heap*)heap, ptr)) return 0; free((void*)ptr); return 1; }
unsigned __stdcall myVirtualFree(unsigned ptr, unsigned b, unsigned free_type) { if (free_type == MEM_RELEASE) _aligned_free((void*)ptr); return 1; }
unsigned __stdcall myGetEnvironmentVariableA(unsigned a, unsigned b, unsigned c) { return 0; }
unsigned __stdcall myGetVersionExA(unsigned a) { return 0; }
unsigned __stdcall myHeapDestroy(unsigned heap) { heap_free_all((struct heap*)heap); heap_done((struct heap*)heap); return 1; }
unsigned __stdcall myHeapCreate(unsigned a, unsigned b, unsigned c) { return (unsigned)heap_create(); }
unsigned __stdcall myGetCPInfo(unsigned a, unsigned b) { return 0; }
unsigned __stdcall myGetACP() { return 1252; }
unsigned __stdcall myGetOEMCP() { return 437; }
unsigned __stdcall mySetHandleCount(unsigned a) { return 0; }
unsigned __stdcall myGetFileType(unsigned a) { return 0; }
unsigned __stdcall myGetStartupInfoA(unsigned info) { memset((void*)info, 0, 17 * 4); return 0; }
unsigned __stdcall myFreeEnvironmentStringsA(unsigned a) { return 0; }
unsigned __stdcall myFreeEnvironmentStringsW(unsigned a) { return 0; }
unsigned __stdcall myWideCharToMultiByte(unsigned a, unsigned b, unsigned srcn, unsigned srclen, unsigned destn, unsigned destlen, unsigned x, unsigned y) {
	uint16_t* src = (uint16_t*)srcn;
	uint8_t* dest = (uint8_t*)destn, * p = dest;
	if (srclen == 0xffffffff) { uint16_t* ps = src; while (*ps++) {} srclen = ps - src; }
	if (!dest)
		return srclen;
	while (srclen-- && destlen--) *p++ = *src++;
	return p - dest;
}
unsigned __stdcall myGetEnvironmentStrings() { static uint8_t strs[] = { 0, 0 }; return (unsigned)&strs; }
unsigned __stdcall myGetEnvironmentStringsW() { static uint16_t strs[] = { 0, 0 }; return (unsigned)&strs; }
unsigned __stdcall mySetFilePointer(unsigned a, unsigned b, unsigned c, unsigned d) { return 0; }
unsigned __stdcall myMultiByteToWideChar(unsigned a, unsigned b, unsigned c, unsigned d, unsigned e, unsigned f) { return 0; }
unsigned __stdcall myGetStringTypeA(unsigned a, unsigned b, unsigned c, unsigned d, unsigned e) { return 0; }
unsigned __stdcall myGetStringTypeW(unsigned a, unsigned b, unsigned c, unsigned d, unsigned e) { return 0; }
unsigned __stdcall myLCMapStringA(unsigned a, unsigned b, unsigned c, unsigned d, unsigned e, unsigned f) { return 0; }
unsigned __stdcall myLCMapStringW(unsigned a, unsigned b, unsigned c, unsigned d, unsigned e, unsigned f) { return 0; }
unsigned __stdcall myRaiseException(unsigned a, unsigned b, unsigned c, unsigned d) { return 0; }
unsigned __stdcall mySetStdHandle(unsigned a, unsigned b) { return 0; }
unsigned __stdcall myFlushFileBuffers(unsigned a) { return 0; }
unsigned __stdcall myCloseHandle(unsigned a) { return 0; }

unsigned File_Open(unsigned name, unsigned mode) {
	printf("File_Open %s %s\n", (char*)name, (char*)mode);
	return (unsigned)cfopen((char*)name, (char*)mode);
}

unsigned File_eof(unsigned f) {
	return cfeof((CFILE*)f);
}

unsigned File_ReadString(unsigned buf, unsigned size, unsigned f) {
	return (unsigned)cf_ReadString((char*)buf, size, (CFILE*)f);
}

unsigned File_Close(unsigned f) {
	cfclose((CFILE*)f);
	return 0;
}

unsigned FindName(unsigned name) {
	printf("FindName %s\n", (char*)name);
	return 0;
}
unsigned Scrpt_FindObjectName(unsigned name) {
	printf("FindObjectName %s\n", (char*)name);
	return osipf_FindObjectName((char*)name);
}
unsigned Scrpt_FindRoomName(unsigned name) {
	printf("FindRoomName %s\n", (char*)name);
	return osipf_FindRoomName((char*)name);
}
unsigned Scrpt_FindTriggerName(unsigned name) {
	printf("FindTriggerName %s\n", (char*)name);
	return osipf_FindTriggerName((char*)name);
}
unsigned Scrpt_FindLevelGoalName(unsigned name) {
	printf("FindLevelGoalName %s\n", (char*)name);
	return osipf_FindLevelGoalName((char*)name);
}
unsigned Scrpt_FindTextureName(unsigned name) {
	printf("FindTextureName %s\n", (char*)name);
	return osipf_FindTextureName((char*)name);
}
unsigned Scrpt_GetTriggerFace(unsigned trigger) { return osipf_GetTriggerFace(trigger); }
unsigned Scrpt_GetTriggerRoom(unsigned trigger) { return osipf_GetTriggerRoom(trigger); }
unsigned Scrpt_CreateTimer(unsigned timer) { return Osiris_CreateTimer((tOSIRISTIMER*)timer); }
unsigned Cine_StartCanned(unsigned info) { return 0; }
extern void msafe_CallFunction(ubyte type, msafe_struct* mstruct);
extern void msafe_GetValue(ubyte type, msafe_struct* mstruct);
extern void msafe_DoPowerup(msafe_struct* mstruct);
extern void osipf_LGoalValue(char action, char type, void* val, int goal, int item);
extern void osipf_MatcenValue(int h, char op, char type, void* val, int prod);
union bitfloat { unsigned bit; float f; };
extern void osipf_ObjectCustomAnim(int objnum, float start, float end, float time, char flags, int sound, char next_type);
unsigned MSafe_CallFunction(unsigned type, unsigned mstruct) { msafe_CallFunction(type, (msafe_struct*)mstruct); return 0; }
unsigned MSafe_GetValue(unsigned type, unsigned mstruct) { msafe_GetValue(type, (msafe_struct*)mstruct); return 0; }
unsigned MSafe_DoPowerup(unsigned mstruct) { printf("mpowerup\n"); msafe_DoPowerup((msafe_struct*)mstruct); return 0; }
unsigned LGoal_Value(unsigned act, unsigned type, unsigned val, unsigned goal, unsigned item) { osipf_LGoalValue(act, type, (void*)val, goal, item); return 0; }
unsigned Matcen_Value(unsigned h, unsigned op, unsigned type, unsigned val, unsigned prod) { osipf_MatcenValue(h, op, type, (void*)val, prod); return 0; }
unsigned Obj_SetCustomAnim(unsigned objnum, unsigned start, unsigned end, unsigned time, unsigned flags, unsigned sound, unsigned next_type) {
	union bitfloat start_float = { start }, end_float = { end }, time_float = { time };
	osipf_ObjectCustomAnim(objnum, start_float.f, end_float.f, time_float.f, flags, sound, next_type);
	return 0;
}
void osipf_ObjKill(int handle, int killer_handle, float damage, int flags, float min_time, float max_time);
unsigned Obj_Kill(unsigned handle, unsigned killer_handle, unsigned damage, unsigned flags, unsigned min_time, unsigned max_time) {
	union bitfloat damagef = { damage }, min_timef = { min_time }, max_timef = { max_time };
	osipf_ObjKill(handle, killer_handle, damagef.f, flags, min_timef.f, max_timef.f);
	return 0;
}

struct {
	const char* name;
	int args;
	union {
		void *p;
		unsigned (*fun0)();
		unsigned (*fun1)(unsigned a);
		unsigned (*fun2)(unsigned a, unsigned b);
		unsigned (*fun3)(unsigned a, unsigned b, unsigned c);
		unsigned (*fun4)(unsigned a, unsigned b, unsigned c, unsigned d);
		unsigned (*fun5)(unsigned a, unsigned b, unsigned c, unsigned d, unsigned e);
		unsigned (*fun6)(unsigned a, unsigned b, unsigned c, unsigned d, unsigned e, unsigned f);
		unsigned (*fun7)(unsigned a, unsigned b, unsigned c, unsigned d, unsigned e, unsigned f, unsigned g);
		unsigned (*fun8)(unsigned a, unsigned b, unsigned c, unsigned d, unsigned e, unsigned f, unsigned g, unsigned h);
	};
} funs[] = {
	{"GetStdHandle", 1, myGetStdHandle},
	{"WriteFile", 5, myWriteFile},
	{"VirtualAlloc", 4, myVirtualAlloc},
	{"IsBadWritePtr", 2, myIsBadWritePtr},
	{"IsBadReadPtr", 2, myIsBadReadPtr},
	{"HeapValidate", 3, myHeapValidate},
	{"GetCommandLineA", 0, myGetCommandLineA},
	{"GetVersion", 0, myGetVersion},
	{"GetProcAddress", 2, myGetProcAddress},
	{"GetModuleHandleA", 1, myGetModuleHandleA},
	{"GetCurrentThreadId", 0, myGetCurrentThreadId},
	{"TlsSetValue", 2, myTlsSetValue},
	{"TlsAlloc", 0, myTlsAlloc},
	{"TlsFree", 1, myTlsFree},
	{"SetLastError", 1, mySetLastError},
	{"TlsGetValue", 1, myTlsGetValue},
	{"GetLastError", 0, myGetLastError},
	{"DebugBreak", 0, myDebugBreak},
	{"InterlockedDecrement", 1, myInterlockedDecrement},
	{"OutputDebugStringA", 1, myOutputDebugStringA},
	{"LoadLibraryA", 1, myLoadLibraryA},
	{"InterlockedIncrement", 1, myInterlockedIncrement},
	{"GetModuleFileNameA", 3, myGetModuleFileNameA},
	{"ExitProcess", 1, myExitProcess},
	{"TerminateProcess", 2, myTerminateProcess},
	{"GetCurrentProcess", 0, myGetCurrentProcess},
	{"InitializeCriticalSection", 1, myInitializeCriticalSection},
	{"DeleteCriticalSection", 1, myDeleteCriticalSection},
	{"EnterCriticalSection", 1, myEnterCriticalSection},
	{"LeaveCriticalSection", 1, myLeaveCriticalSection},
	{"RtlUnwind", 4, myRtlUnwind},
	{"HeapAlloc", 3, myHeapAlloc},
	{"HeapReAlloc", 4, myHeapReAlloc},
	{"HeapFree", 3, myHeapFree},
	{"VirtualFree", 3, myVirtualFree},
	{"GetEnvironmentVariableA", 3, myGetEnvironmentVariableA},
	{"GetVersionExA", 1, myGetVersionExA},
	{"HeapDestroy", 1, myHeapDestroy},
	{"HeapCreate", 3, myHeapCreate},
	{"GetCPInfo", 2, myGetCPInfo},
	{"GetACP", 0, myGetACP},
	{"GetOEMCP", 0, myGetOEMCP},
	{"SetHandleCount", 1, mySetHandleCount},
	{"GetFileType", 1, myGetFileType},
	{"GetStartupInfoA", 1, myGetStartupInfoA},
	{"FreeEnvironmentStringsA", 1, myFreeEnvironmentStringsA},
	{"FreeEnvironmentStringsW", 1, myFreeEnvironmentStringsW},
	{"WideCharToMultiByte", 8, myWideCharToMultiByte},
	{"GetEnvironmentStrings", 0, myGetEnvironmentStrings},
	{"GetEnvironmentStringsW", 0, myGetEnvironmentStringsW},
	{"SetFilePointer", 4, mySetFilePointer},
	{"MultiByteToWideChar", 6, myMultiByteToWideChar},
	{"GetStringTypeA", 5, myGetStringTypeA},
	{"GetStringTypeW", 5, myGetStringTypeW},
	{"LCMapStringA", 6, myLCMapStringA},
	{"LCMapStringW", 6, myLCMapStringW},
	{"RaiseException", 4, myRaiseException},
	{"SetStdHandle", 2, mySetStdHandle},
	{"FlushFileBuffers", 1, myFlushFileBuffers},
	{"CloseHandle", 1, myCloseHandle},
	{"mprintf", -1},
	{"File_Open", 2 | 0x100, File_Open},
	{"File_eof", 1 | 0x100, File_eof},
	{"File_ReadString", 3 | 0x100, File_ReadString},
	{"File_Close", 1 | 0x100, File_Close},
	{"FindName", 1 | 0x100, FindName},
	{"Scrpt_FindObjectName", 1 | 0x100, Scrpt_FindObjectName},
	{"Scrpt_FindRoomName", 1 | 0x100, Scrpt_FindRoomName},
	{"Scrpt_FindTriggerName", 1 | 0x100, Scrpt_FindTriggerName},
	{"Scrpt_FindLevelGoalName", 1 | 0x100, Scrpt_FindLevelGoalName},
	{"Scrpt_FindTextureName", 1 | 0x100, Scrpt_FindTextureName},
	{"Scrpt_GetTriggerFace", 1 | 0x100, Scrpt_GetTriggerFace},
	{"Scrpt_GetTriggerRoom", 1 | 0x100, Scrpt_GetTriggerRoom},
	{"Scrpt_CreateTimer", 1 | 0x100, Scrpt_CreateTimer},
	{"Cine_StartCanned", 1 | 0x100, Cine_StartCanned},
	{"MSafe_CallFunction", 2 | 0x100, MSafe_CallFunction},
	{"MSafe_GetValue", 2 | 0x100, MSafe_GetValue},
	{"MSafe_DoPowerup", 1 | 0x100, MSafe_DoPowerup},
	{"Obj_SetCustomAnim", 7 | 0x100, Obj_SetCustomAnim},
	{"Obj_Kill", 6 | 0x100, Obj_Kill},
	{"LGoal_Value", 5 | 0x100, LGoal_Value},
	{"Matcen_Value", 5 | 0x100, Matcen_Value},
};
#define MAX_FUNS (sizeof(funs) / sizeof(funs[0]))

#if 0
int do_int(x86emu_t* emu, u8 num, unsigned type)
{
	//printf("int num %x\n", num);
	if (num == 0x10) {
		putchar(EMU_R_EAX & 0xff);
		fflush(stdout);
		return 1;
	}
	if (num == 0x80 && EMU_R_AL < MAX_FUNS) {
		int n = EMU_R_AL;
		unsigned* buf = (unsigned*)EMU_R_ESP;
		int c = funs[n].args == -1 ? -1 : funs[n].args & 0xff;
#ifndef __EMSCRIPTEN__
		printf("run fun %d %s", n, funs[n].name);
		bool msafe = strncmp(funs[n].name, "MSafe_", 6) == 0 && funs[n].name[6] != 'D';
		for (int i = 0; i < c; i++) {
			printf(" %x", buf[i + 1]);
			if (msafe && !i) printf(" (%s)", msafe_names[buf[i + 1]]);
		}
		printf("\n");
#endif
		switch (c) {
		case -1:
			vprintf((const char*)buf[2], (va_list)&buf[3]);
			break;
		case 0:
			EMU_R_EAX = funs[n].fun0();
			break;
		case 1:
			EMU_R_EAX = funs[n].fun1(buf[1]);
			break;
		case 2:
			EMU_R_EAX = funs[n].fun2(buf[1], buf[2]);
			break;
		case 3:
			EMU_R_EAX = funs[n].fun3(buf[1], buf[2], buf[3]);
			break;
		case 4:
			EMU_R_EAX = funs[n].fun4(buf[1], buf[2], buf[3], buf[4]);
			break;
		case 5:
			EMU_R_EAX = funs[n].fun5(buf[1], buf[2], buf[3], buf[4], buf[5]);
			break;
		case 6:
			EMU_R_EAX = funs[n].fun6(buf[1], buf[2], buf[3], buf[4], buf[5], buf[6]);
			break;
		case 7:
			EMU_R_EAX = funs[n].fun7(buf[1], buf[2], buf[3], buf[4], buf[5], buf[6], buf[7]);
			break;
		case 8:
			EMU_R_EAX = funs[n].fun8(buf[1], buf[2], buf[3], buf[4], buf[5], buf[6], buf[7], buf[8]);
			break;
		}
		EMU_R_EIP = buf[0];
		if (funs[n].args & 0x100)
			EMU_R_ESP += 4;
		else
			EMU_R_ESP += 4 + c * 4;
		return 1;
	}
	if (num == 0x81) {
		printf("return %x\n", EMU_R_EAX);
		x86emu_stop(emu);
		return 1;
	}
	if ((type & 0xff) == INTR_TYPE_FAULT || num == 0x20) {
		printf("fault %d at %x\n", num, EMU_R_EIP);
		x86emu_stop(emu);
	}

	return 0;
}
#endif

void error(const char* msg, ...) {
	va_list vp;
	va_start(vp, msg);
	vfprintf(stderr, msg, vp);
	va_end(vp);
}

#if 0
void* mapfile(const char* filename, intptr_t base, int* psize) {
	int fd;
	void* ptr;

	if ((fd = open(filename, O_RDONLY)) == -1) {
		perror(filename);
		return NULL;
	}

	int size = lseek(fd, 0, SEEK_END);
	lseek(fd, 0, SEEK_SET);

	if (!(ptr = mmap(NULL, size, PROT_READ, MAP_PRIVATE, fd, 0))) {
		perror("mmap");
		return NULL;
	}
	return ptr;
}
#endif

void* readfile(const char* filename, unsigned* psize) {
	CFILE* f;
	void* ptr = NULL;
	IMAGE_DOS_HEADER dos;
	IMAGE_NT_HEADERS nt;

	if (!(f = cfopen(filename, "rb"))) {
		perror(filename);
		return NULL;
	}

	int size = cfilelength(f);

	if (cf_ReadBytes((ubyte*)&dos, sizeof(dos), f) != sizeof(dos))
		goto read_err;
	if (dos.e_magic != 0x5a4d)
		goto read_err;
	cfseek(f, dos.e_lfanew, SEEK_SET);
	if (cf_ReadBytes((ubyte*)&nt, sizeof(nt), f) != sizeof(nt))
		goto read_err;
	if (nt.Signature != 0x4550)
		goto read_err;
	if ((unsigned)size > nt.OptionalHeader.SizeOfImage)
		size = nt.OptionalHeader.SizeOfImage;
	//if (!(ptr = memalign(0x1000, nt.OptionalHeader.SizeOfImage))) {
#if 0
	if (!(ptr = mmap((void*)0x10000000, nt.OptionalHeader.SizeOfImage, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0))) {
		perror("memalign");
		return NULL;
	}
#else
	//if (!(ptr = _aligned_malloc(nt.OptionalHeader.SizeOfImage, 0x1000))) {
	if (!(ptr = VirtualAlloc(NULL, nt.OptionalHeader.SizeOfImage,
		MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE))) {
		perror("VirtualAlloc");
		return NULL;
	}
#endif
	if (psize)
		*psize = nt.OptionalHeader.SizeOfImage;
	cfseek(f, 0, SEEK_SET);
	if (cf_ReadBytes((ubyte*)ptr, size, f) != size)
		goto read_err;
	cfclose(f);
	return ptr;
read_err:
	perror(filename);
	cfclose(f);
	if (ptr)
		free(ptr);
	return NULL;
}

void* mk_funcode() {
#if 0
	char* funcode = (char*)malloc(MAX_FUNS * 5);
	for (unsigned i = 0; i < MAX_FUNS; i++) {
		char* fun = funcode + i * 5;
		*fun++ = 0xb0;
		*fun++ = i;
		*fun++ = 0xcd;
		*fun++ = 0x80;
		*fun++ = 0xc3;
	}
	return funcode;
#else
	return NULL;
#endif
}


void image_setup_sections(IMAGE_NT_HEADERS* ntHdr, void* pFile, void* pBase) {
	PIMAGE_SECTION_HEADER firstSect = (IMAGE_SECTION_HEADER*)((char*)ntHdr + sizeof(IMAGE_NT_HEADERS));
	for (int sectIdx = ntHdr->FileHeader.NumberOfSections - 1; sectIdx >= 0; sectIdx--) {
		PIMAGE_SECTION_HEADER sect = firstSect + sectIdx;
#ifdef LOG
		printf("sect %x fileptr %x memptr %x filesize %x memsize %x\n",
			sectIdx, sect->PointerToRawData, sect->VirtualAddress,
			sect->SizeOfRawData, sect->Misc.VirtualSize);
#endif
		if (sect->VirtualAddress != sect->PointerToRawData)
			memmove((char*)pBase + sect->VirtualAddress,
				(char*)pFile + sect->PointerToRawData, sect->SizeOfRawData);
		if (sect->Misc.VirtualSize > sect->SizeOfRawData)
			memset((char*)pBase + sect->VirtualAddress + sect->SizeOfRawData, 0,
				sect->Misc.VirtualSize - sect->SizeOfRawData);
	}
}

void image_reloc(IMAGE_NT_HEADERS* ntHdr, void* pBase) {
	IMAGE_DATA_DIRECTORY* reloc_dir = &ntHdr->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC];
	IMAGE_BASE_RELOCATION* reloc = (IMAGE_BASE_RELOCATION*)((char*)pBase + reloc_dir->VirtualAddress);
	IMAGE_BASE_RELOCATION* reloc_end = (IMAGE_BASE_RELOCATION*)((char*)pBase + reloc_dir->VirtualAddress + reloc_dir->Size);

	DWORD delta = (DWORD)pBase - ntHdr->OptionalHeader.ImageBase;
	while (reloc < reloc_end) {
		DWORD va = reloc->VirtualAddress, size = reloc->SizeOfBlock;
		if (!size)
			abort();
		WORD* p = (WORD*)(reloc + 1), * pe = (WORD*)((char*)reloc + size);
		while (p < pe) {
			WORD val = *p++;
			void* dest = (void*)((char*)pBase + va + (val & 0xfff));
			val >>= 12;
			if (val == IMAGE_REL_BASED_HIGHLOW)
				*(DWORD*)dest += delta;
			else if (val == IMAGE_REL_BASED_LOW)
				*(WORD*)dest += delta & 0xffff;
			else if (val == IMAGE_REL_BASED_HIGH)
				*(WORD*)dest += delta >> 16;
		}
		reloc = (IMAGE_BASE_RELOCATION*)pe;
	}
}

unsigned find_fun(const char* name) {
	for (unsigned i = 0; i < MAX_FUNS; i++)
		if (strcmp(name, funs[i].name) == 0)
			return (unsigned)funs[i].p; // (DWORD)((char*)funcode + i * 5);
	printf("not found %s\n", name);
	return 0;
}

void image_import(IMAGE_NT_HEADERS* ntHdr, void* pBase) {
	IMAGE_DATA_DIRECTORY* dir = &ntHdr->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT];
	IMAGE_IMPORT_DESCRIPTOR* imp = (IMAGE_IMPORT_DESCRIPTOR*)((char*)pBase + dir->VirtualAddress);
	for (; imp->Name; imp++) {
		const char* libName = (char*)pBase + imp->Name;
		//printf("import %s\n", libName);
#if 0
		HMODULE lib = LoadLibrary(libName);
		if (!lib) {
			error(libName);
			return 0;
		}
#endif
		PIMAGE_THUNK_DATA src, dst;
		dst = (IMAGE_THUNK_DATA*)((char*)pBase + imp->FirstThunk);
		if (imp->OriginalFirstThunk)
			src = (IMAGE_THUNK_DATA*)((char*)pBase + imp->OriginalFirstThunk);
		else
			src = dst;
		for (; src->u1.AddressOfData; src++, dst++) {
			const char* name;
			if (IMAGE_SNAP_BY_ORDINAL(src->u1.Ordinal)) {
				name = (char*)IMAGE_ORDINAL(src->u1.Ordinal);
				printf("ordinal %d\n", src->u1.Ordinal);
			}
			else {
				PIMAGE_IMPORT_BY_NAME pImport = (IMAGE_IMPORT_BY_NAME*)((char*)pBase + src->u1.AddressOfData);
				name = (char*)pImport->Name;
				//printf("import function %s\n", name);
				//dst->u1.Function = 0; //NULL; //(DWORD)GetProcAddress(lib, name);
				dst->u1.Function = find_fun(name);
			}
			if (!dst->u1.Function) {
				printf("\t{\"%s\", my%s, 0},\n", name, name);
				//error(name);
				//return 0;
			}
		}
	}
}

struct dll {
	void* mem;
	unsigned memsize;
	unsigned entry;
	IMAGE_EXPORT_DIRECTORY* exports;
};


void dll_done() {
	#if 0
	if (!emu)
		return;
	free(stack);
	free(funcode);
	#if 0
	x86emu_done(emu);
	#endif
	stack = NULL;
	funcode = NULL;
	emu = NULL;
	#endif
}

void dll_init() {
#if 0
	if (emu)
		return;
	static uint8_t endcode[] = { 0xcc, 'S', 'C' };
	static unsigned unwind2[] = { (unsigned)-1, (unsigned)&endcode, (unsigned)&endcode };
	emu = emu_new();
	funcode = mk_funcode();
	int stack_size = 1048576;
	stack = malloc(stack_size);
	EMU_R_ESP = (unsigned)stack + stack_size;
	Push32(emu, (unsigned)-1);
	Push32(emu, (unsigned)&unwind2);
	Push32(emu, (unsigned)&endcode);
	Push32(emu, 0);
	tls_vals[0] = EMU_R_ESP;
	emu->segs_offs[_FS] = (uint32_t)&tls_vals;
	atexit(dll_done);
	msafenames_init();
#endif
}

unsigned emucall(x86emu_t* emu, unsigned ip) {
#if 0
	static uint8_t endcode[] = { 0xcc, 'S', 'C' };
	Push32(emu, (unsigned)&endcode);
	R_EIP = ip;
	Run(emu, 0);
	emu->quit = 0;
	return R_EAX;
#else
	return 0;
#endif
}

typedef unsigned(__stdcall std0fun)();
typedef unsigned(__stdcall std1fun)(unsigned);
typedef unsigned(__stdcall std2fun)(unsigned, unsigned);
typedef unsigned(__stdcall std3fun)(unsigned, unsigned, unsigned);
typedef unsigned(__stdcall std4fun)(unsigned, unsigned, unsigned, unsigned);
typedef unsigned(__stdcall std5fun)(unsigned, unsigned, unsigned, unsigned, unsigned);
typedef unsigned(__stdcall std6fun)(unsigned, unsigned, unsigned, unsigned, unsigned, unsigned);
typedef unsigned(__stdcall std7fun)(unsigned, unsigned, unsigned, unsigned, unsigned, unsigned, unsigned);
typedef unsigned(__stdcall std8fun)(unsigned, unsigned, unsigned, unsigned, unsigned, unsigned, unsigned, unsigned);

unsigned dllfun_call(unsigned fun, int argc, ...) {
	va_list vp;
	va_start(vp, argc);
#if 0
	R_ESP -= argc * 4;
	for (int i = 0; i < argc; i++)
		*(unsigned*)(R_ESP + i * 4) = va_arg(vp, unsigned);
	va_end(vp);
	return emucall(emu, fun);
#else
	unsigned args[8];
	if (argc > 8)
		abort();
	for (int i = 0; i < argc; i++)
		args[i] = va_arg(vp, unsigned);
	va_end(vp);
	switch (argc) {
	case 0:
		return ((std0fun*)fun)();
	case 1:
		return ((std1fun*)fun)(args[0]);
	case 2:
		return ((std2fun*)fun)(args[0], args[1]);
	case 3:
		return ((std3fun*)fun)(args[0], args[1], args[2]);
	case 4:
		return ((std4fun*)fun)(args[0], args[1], args[2], args[3]);
	case 5:
		return ((std5fun*)fun)(args[0], args[1], args[2], args[3], args[4]);
	case 6:
		return ((std6fun*)fun)(args[0], args[1], args[2], args[3], args[4], args[5]);
	case 7:
		return ((std7fun*)fun)(args[0], args[1], args[2], args[3], args[4], args[5], args[6]);
	case 8:
		return ((std8fun*)fun)(args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7]);
	}
#endif
}

struct dll* dll_load(const char* filename) {
	void* pFile, * pBase;
	struct dll* dll;

	if (!(dll = (struct dll*)malloc(sizeof(*dll))))
		return 0;

	if (!(pFile = readfile(filename, &dll->memsize)))
		return 0;

	IMAGE_DOS_HEADER* pDosHeader = (IMAGE_DOS_HEADER*)pFile;
	IMAGE_NT_HEADERS* ntHdr = (IMAGE_NT_HEADERS*)((char*)pFile + pDosHeader->e_lfanew);

	pBase = pFile;
	image_setup_sections(ntHdr, pFile, pBase);
	image_reloc(ntHdr, pBase);
	image_import(ntHdr, pBase);

	dll->mem = pBase;
	dll->entry = (unsigned)pBase + ntHdr->OptionalHeader.AddressOfEntryPoint;

	IMAGE_DATA_DIRECTORY* export_data = &ntHdr->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT];
	IMAGE_EXPORT_DIRECTORY* export_dir = (IMAGE_EXPORT_DIRECTORY*)((char*)pBase + export_data->VirtualAddress);
	dll->exports = export_dir;
	for (ULONG i = 0; i < export_dir->NumberOfNames; i++) {
		unsigned f = ((uint16_t*)((char*)pBase + export_dir->AddressOfNameOrdinals))[i];
		unsigned addr = ((unsigned*)((char*)pBase + export_dir->AddressOfFunctions))[f];
		printf("%s %d %x %x\n", (char*)pBase + ((unsigned*)((char*)pBase + export_dir->AddressOfNames))[i], f, addr, (unsigned)pBase + addr);
	}

	if (!dllfun_call(dll->entry, 3, (unsigned)dll->mem, DLL_PROCESS_ATTACH, 0)) {
		free(dll->mem);
		free(dll);
		return NULL;
	}

	return dll;
}

void dll_free(struct dll* dll) {
	if (!dll)
		return;
	dllfun_call(dll->entry, 3, (unsigned)dll->mem, DLL_PROCESS_DETACH, 0);
	//free(dll->mem);
#if 0
	munmap(dll->mem, dll->memsize);
#else
	_aligned_free(dll->mem);
#endif
	free(dll);
}

unsigned dll_find(struct dll* dll, const char* name) {
	IMAGE_EXPORT_DIRECTORY* export_dir = dll->exports;
	char* base = (char*)dll->mem;
	unsigned* names = (unsigned*)(base + export_dir->AddressOfNames);
	for (ULONG i = 0; i < export_dir->NumberOfNames; i++) {
		if (strcmp(base + names[i], name) == 0) {
			unsigned f = ((uint16_t*)(base + export_dir->AddressOfNameOrdinals))[i];
			unsigned addr = ((unsigned*)(base + export_dir->AddressOfFunctions))[f];
			return (unsigned)base + addr;
		}
	}
	return 0;
}

void osiris_setup(tOSIRISModuleInit* mod) {
	memset(mod, 0, sizeof(*mod));
	mod->game_checksum = 0x87888d9b;
	mod->mprintf = (mprintf_fp)find_fun("mprintf");
	mod->File_ReadString = (File_ReadString_fp)find_fun("File_ReadString");
	mod->File_Open = (File_Open_fp)find_fun("File_Open");
	mod->File_Close = (File_Close_fp)find_fun("File_Close");
	mod->File_eof = (File_eof_fp)find_fun("File_eof");
	mod->Scrpt_FindRoomName = (Scrpt_FindRoomName_fp)find_fun("Scrpt_FindRoomName");
	mod->Scrpt_FindSoundName = (Scrpt_FindRoomName_fp)find_fun("FindName");
	mod->Scrpt_FindLevelGoalName = (Scrpt_FindLevelGoalName_fp)find_fun("Scrpt_FindLevelGoalName");
	mod->Scrpt_FindTextureName = (Scrpt_FindTextureName_fp)find_fun("Scrpt_FindTextureName");
	mod->Scrpt_FindPathName = (Scrpt_FindRoomName_fp)find_fun("FindName");
	mod->Scrpt_FindTriggerName = (Scrpt_FindTriggerName_fp)find_fun("Scrpt_FindTriggerName");
	mod->Scrpt_FindDoorName = (Scrpt_FindRoomName_fp)find_fun("FindName");
	mod->Scrpt_FindObjectName = (Scrpt_FindObjectName_fp)find_fun("Scrpt_FindObjectName");
	mod->Scrpt_FindMatcenName = (Scrpt_FindRoomName_fp)find_fun("FindName");
	mod->Scrpt_GetTriggerFace = (Scrpt_GetTriggerFace_fp)find_fun("Scrpt_GetTriggerFace");
	mod->Scrpt_GetTriggerRoom = (Scrpt_GetTriggerRoom_fp)find_fun("Scrpt_GetTriggerRoom");
	mod->Scrpt_CreateTimer = (Scrpt_CreateTimer_fp)find_fun("Scrpt_CreateTimer");
	mod->Cine_StartCanned = (Cine_StartCanned_fp)find_fun("Cine_StartCanned");
	mod->MSafe_CallFunction = (MSafe_CallFunction_fp)find_fun("MSafe_CallFunction");
	mod->MSafe_GetValue = (MSafe_GetValue_fp)find_fun("MSafe_GetValue");
	mod->MSafe_DoPowerup = (MSafe_DoPowerup_fp)find_fun("MSafe_DoPowerup");
	mod->Obj_SetCustomAnim = (Obj_SetCustomAnim_fp)find_fun("Obj_SetCustomAnim");
	mod->Obj_Kill = (Obj_Kill_fp)find_fun("Obj_Kill");
	mod->LGoal_Value = (LGoal_Value_fp)find_fun("LGoal_Value");
	mod->Matcen_Value = (Matcen_Value_fp)find_fun("Matcen_Value");
}

#ifdef TEST
unsigned emucall4(x86emu_t* emu, unsigned ip, unsigned a, unsigned b, unsigned c, unsigned d) {
	Push32(emu, d);
	Push32(emu, c);
	Push32(emu, b);
	Push32(emu, a);
	return emucall(emu, ip);
}

unsigned emucall3(x86emu_t* emu, unsigned ip, unsigned a, unsigned b, unsigned c) {
	Push32(emu, c);
	Push32(emu, b);
	Push32(emu, a);
	return emucall(emu, ip);
}

unsigned emucall2(x86emu_t* emu, unsigned ip, unsigned a, unsigned b) {
	Push32(emu, b);
	Push32(emu, a);
	return emucall(emu, ip);
}

unsigned emucall1(x86emu_t* emu, unsigned ip, unsigned a) {
	Push32(emu, a);
	return emucall(emu, ip);
}


//typedef unsigned char bool;

int main() {
	dll_init();

	struct dll* dll = dll_load("level1.dll");

	unsigned f = dll_find(dll, "_InitializeDLL@4");
	printf("f %x\n", f);
	tOSIRISModuleInit mod;
	emucall1(emu, f, (unsigned)&mod);

	printf("creating instance\n");
	unsigned inst = emucall1(emu, dll_find(dll, "_CreateInstance@4"), 0);

	printf("call instance\n");
	tOSIRISEventInfo info;

	emucall4(emu, dll_find(dll, "_CallInstanceEvent@16"), 0, inst, EVT_LEVELSTART, (unsigned)&info);

	//printf("destroying instance %x\n", inst);
	//emucall2(emu, dll_find(dll, "_DestroyInstance@8"), 0, inst);

	emucall(emu, dll_find(dll, "_ShutdownDLL@0"));

	dll_free(dll);

	dll_done();

	return 0;
}
#endif
