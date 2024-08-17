#ifdef WIN32
#define _CRT_DECLARE_NONSTDC_NAMES 1
#include <io.h>
#endif

#include <stdio.h>
#include <stdint.h>
extern "C" {
#include <x86emu.h>
#include "x86emu_private.h"
#include "x86run.h"
#include "regs.h"
#include "box86context.h"
#include "box86stack.h"
}
#include <string.h>
#include <stdlib.h>
#ifndef WIN32
#include <unistd.h>
#include <sys/mman.h>
#endif
#include <fcntl.h>
#include <stdarg.h>
#include <malloc.h>


#ifdef TEST
#include "nocfile.h"
#else
#include "CFILE.H"
#endif
#include "globals.h"
#define INCLUDED_FROM_D3
#include "osiris_import.h"
#include "osiris_dll.h"

#include "dminwindef.h"
#include "dntimage.h"
#include "dwinnt.h"
#include "dll.h"
#include "msafenames.h"
#if 0
#define FILE CFILE
#define fopen cfopen
#define fclose cfclose
#define fread cfread
#define fseek cfseek
#define fgets cfgets
#endif
#undef min
#undef max
#include <vector>
#include <unordered_map>
struct heapfree {
	emu_ptr_t start, end;
};

#define MAX_TLS_VALS 20

struct dll_address_space {
	uint8_t *base;
	emu_ptr_t img_ofs, stack_ofs, heap_ofs, heap_size;
	emu_ptr_t size;
	emu_ptr_t virt_ofs;
};

struct dll_t {
	struct dll_address_space  as;
	emu_ptr_t entry;
	emu_ptr_t endcode, zeroword, funcode, tls_vals_ofs;
	emu_ptr_t *tls_vals;
	emu_ptr_t tls_next;
	emu_ptr_t last_error;
	IMAGE_EXPORT_DIRECTORY *exports;
	std::unordered_map<emu_ptr_t, emu_ptr_t> allocs;
	std::unordered_map<emu_ptr_t, heapfree *> free;
};

void heap_init(dll_t *dll, emu_ptr_t heap_start, emu_ptr_t heap_size) {
	emu_ptr_t heap_end = heap_start + heap_size;
	heapfree *entry = new heapfree();
	entry->start = heap_start;
	entry->end = heap_end;
	dll->free.insert({heap_start, entry});
	dll->free.insert({heap_end, entry});
}

int heap_free(dll_t *dll, emu_ptr_t start) {
	emu_ptr_t end;
	auto size_entry = dll->allocs.find(start);
	if (size_entry == dll->allocs.end())
		return -1;
	end = start + size_entry->second;
	auto fr_start = dll->free.find(start);
	auto fr_end = dll->free.find(end);
	if (fr_start != dll->free.end()) {
		if (fr_end != dll->free.end()) {
			fr_start->second->end = fr_end->second->end;
			dll->free.erase(fr_end->second->start);
			heapfree *entry = fr_end->second;
			fr_end->second = fr_start->second;
			delete entry;
		} else
			fr_start->second->end = end;
	} else if (fr_end != dll->free.end()) {
		fr_end->second->start = start;
	} else {
		heapfree *entry = new heapfree();
		entry->start = start;
		entry->end = end;
		dll->free.insert({start, entry});
		dll->free.insert({end, entry});
	}
	return 0;
}

emu_ptr_t heap_alloc(dll_t *dll, emu_ptr_t size) {
	size = (size + 3) & ~3;
	for (auto key_entry : dll->free) {
		struct heapfree *entry = key_entry.second;
		if (entry->end - entry->start >= size) {
			emu_ptr_t ret = entry->start;
			entry->start += size;
			dll->free.erase(ret);
			if (entry->start == entry->end) {
				dll->free.erase(entry->end);
				delete entry;
			} else
				dll->free.insert({entry->start, entry});
			dll->allocs.insert({ret, size});
			return ret;
		}
	}
	return 0;
}

#if 0
struct {
  struct {
    unsigned segment;
    unsigned offset;
  } start;
  unsigned load;
  unsigned max_instructions;
  unsigned bits_32:1;
  char *file;
} opt = { .bits_32 = 1 };
#endif

#define B86EMU

#ifndef B86EMU
#define EMU_R_ESP emu->x86.R_ESP
#else
enum { X86EMU_PERM_R = 1, X86EMU_PERM_W = 2, X86EMU_PERM_X = 4 };
enum { INTR_TYPE_FAULT = 1, INTR_TYPE_USER = 2 };
typedef uint8_t u8;
int do_int(x86emu_t *emu, u8 num, unsigned type);

extern "C" {
int box86_log;
FILE* ftrace;
box86context_t bc, *my_context = &bc;
uint32_t default_fs;
uintptr_t trace_start, trace_end;
int trace_xmm, trace_emm;
//void* getAlternate(void* addr) { return addr; }
uint32_t RunFunctionWithEmu(x86emu_t *emu, int QuitOnLongJump, uintptr_t fnc, int nargs, ...) { return 0; }
void emit_signal(x86emu_t* emu, int sig, void* addr, int code) {
	do_int(emu, sig, INTR_TYPE_FAULT);
}

int my_setcontext(x86emu_t* emu, void* ucp) { return 0; }
void my_cpuid(x86emu_t* emu, uint32_t tmp32u) {}
x86emu_t* x86emu_fork(x86emu_t* e, int forktype) { return NULL; }
void* GetSegmentBase(uint32_t desc) { return NULL; }
int GetTID() { return 0; }

void x86Syscall(x86emu_t *emu) {
	do_int(emu, 0x80, INTR_TYPE_USER);
}

void x86Int3(x86emu_t *emu) {
	//StopEmu(emu, "int 3");
	emu->quit = 1;
}
uintptr_t AddCheckBridge(bridge_t* bridge, void* /*wrapper_t*/ w, void* fnc, int N, const char* name) {
	static uint8_t code[] = {0xcc, 'S', 'C'};
	return (uintptr_t)&code;
}
}

#define EMU_R_ESP R_ESP
#define EMU_R_EAX R_EAX
#define EMU_R_AL R_AL
#define EMU_R_EIP R_EIP

x86emu_t *x86emu_new(int perm, int unk) {
	ftrace = stderr;
	return NewX86Emu(my_context, 0, 0, 1048576, 1);
}

void x86emu_done(x86emu_t *emu) {
	FreeX86Emu(&emu);
}

void x86emu_stop(x86emu_t *emu) {
	emu->quit = 1;
}

#if 0
enum { X86EMU_RUN_MAX_INSTR = 1, X86EMU_RUN_NO_EXEC = 2, X86EMU_RUN_NO_CODE = 4, X86EMU_RUN_LOOP = 8 };


uint8_t x86emu_read_byte(x86emu_t *emu, unsigned addr) {
	return *(uint8_t *)addr;
}
uint32_t x86emu_read_dword(x86emu_t *emu, unsigned addr) {
	return *(uint32_t *)addr;
}
void x86emu_write_byte(x86emu_t *emu, unsigned addr, uint8_t val) {
	*(uint8_t *)addr = val;
}
void x86emu_write_dword(x86emu_t *emu, unsigned addr, uint32_t val) {
	*(uint32_t *)addr = val;
}
void x86emu_set_intr_handler(x86emu_t *emu, void *fun) {}
void x86emu_set_seg_register(x86emu_t *emu, int reg, unsigned addr) {}

int EMU_R_CS_LIMIT,
		EMU_R_DS_LIMIT,
		EMU_R_ES_LIMIT,
		EMU_R_FS_LIMIT,
		EMU_R_GS_LIMIT,
		EMU_R_SS_LIMIT;
int EMU_R_CS_ACC, EMU_R_SS_ACC, EMU_R_CS_SEL;
#endif

#endif

x86emu_t *emu_new()
{
	x86emu_t *emu = x86emu_new(X86EMU_PERM_R | X86EMU_PERM_W | X86EMU_PERM_X, 0);
	return emu;
}


//unsigned tls_vars = 1;
//unsigned tls_vals[20];
//unsigned last_error;
#define ELMS(x) (sizeof(x)/sizeof((x)[0]))
#define TLS_OUT_OF_INDEXES 0xffffffff
#define ERROR_INVALID_PARAMETER 87
#define ERROR_NOT_ENOUGH_MEMORY 8

#if 0
struct heap {
	int size;
	int limit;
	unsigned *allocs;
};
struct heap *heap_create() {
	struct heap *heap = (struct heap *)malloc(sizeof(*heap));
	heap->size = 256;
	heap->allocs = (unsigned *)malloc(heap->size * sizeof(unsigned));
	heap->limit = 0;
	return heap;
}
int heap_del(struct heap *heap, unsigned ptr) {
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
int heap_add(struct heap *heap, unsigned ptr) {
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
	unsigned *new_allocs = (unsigned *)realloc(heap->allocs, new_size * sizeof(unsigned));
	if (!new_allocs)
		return 0;
	heap->size = new_size;
	heap->allocs = new_allocs;
	heap->allocs[heap->limit++] = ptr;
	return 1;
}
void heap_free_all(struct heap *heap) {
	for (int i = 0; i < heap->limit; i++)
		if (heap->allocs[i])
			free((void *)heap->allocs[i]);
	heap->limit = 0;
}
void heap_done(struct heap *heap) {
	free(heap->allocs);
	free(heap);
}
#endif

static x86emu_t *emu;
static void *funcode;
static void *stack;
static dll_t *curdll;

unsigned myGetStdHandle(unsigned type) { return 1; }
unsigned myWriteFile(unsigned hFile, unsigned buf, unsigned size, unsigned pret, unsigned ovl) {
	int ret = write((int)hFile, BASE + buf, size);
	if (pret && ret >= 0)
		*(unsigned *)(BASE + pret) = ret;
	return ret >= 0;
}
unsigned myVirtualAlloc(unsigned lpAddress, unsigned dwSize, unsigned flAllocationType, unsigned flProtect) {
	#ifdef WIN32
	return (unsigned)VirtualAlloc((LPVOID)lpAddress, dwSize, flAllocationType, flProtect);
	#else
	if (lpAddress)
		return flAllocationType == 0x1000 ? lpAddress : 0;
	//return (unsigned)((uint8_t*)memalign(4096, dwSize) - curdll->as.base);
	if (curdll->as.virt_ofs + dwSize > curdll->as.size) {
		curdll->last_error = ERROR_NOT_ENOUGH_MEMORY;
		return 0;
	}
	unsigned ret = curdll->as.virt_ofs;
	curdll->as.virt_ofs += dwSize;
	return ret;
	#endif
}
unsigned myIsBadWritePtr(unsigned a, unsigned b) { return 0; }
unsigned myIsBadReadPtr(unsigned a, unsigned b) { return 0; }
unsigned myHeapValidate(unsigned a, unsigned b, unsigned c) { return 1; }
unsigned myGetCommandLineA() { return curdll->zeroword; /*""*/ }
unsigned myGetVersion() { return 0; }
unsigned myGetProcAddress(unsigned lib, unsigned name) { printf("getprocaddr %x %s\n", lib, (char *)(BASE + name)); return 0; }
unsigned myGetModuleHandleA(unsigned name) { printf("getmodhan %s\n", name ? (char *)(BASE + name) : NULL); return curdll->zeroword; }
unsigned myGetCurrentThreadId() { return 0; }
unsigned myTlsSetValue(unsigned n, unsigned val) { if (n >= curdll->tls_next) { curdll->last_error = ERROR_INVALID_PARAMETER; return 0; } curdll->tls_vals[n] = val; return 1; }
unsigned myTlsAlloc() { if (curdll->tls_next == MAX_TLS_VALS) return TLS_OUT_OF_INDEXES; return curdll->tls_next++; }
unsigned myTlsFree(unsigned v) { if (v == curdll->tls_next - 1) curdll->tls_next--; return 1; }
unsigned mySetLastError(unsigned err) { curdll->last_error = err; return 0; }
unsigned myTlsGetValue(unsigned n) { if (n < curdll->tls_next) { curdll->last_error = 0; return curdll->tls_vals[n]; } curdll->last_error = ERROR_INVALID_PARAMETER; return 0; }
unsigned myGetLastError() { return curdll->last_error; }
unsigned myDebugBreak() { return 0; }
unsigned myInterlockedDecrement(unsigned a) { return 0; }
unsigned myOutputDebugStringA(unsigned msg) { printf("OutputDebugString: %s\n", (char *)(BASE + msg)); return 0; }
unsigned myLoadLibraryA(unsigned name) { printf("loadlib %s\n", (char *)(BASE + name)); return 0; }
unsigned myInterlockedIncrement(unsigned a) { return 0; }
unsigned myGetModuleFileNameA(unsigned a, unsigned b, unsigned c) { return 0; }
unsigned myExitProcess(unsigned a) { x86emu_stop(emu); return 0; }
unsigned myTerminateProcess(unsigned a, unsigned b) { return 0; }
unsigned myGetCurrentProcess() { return 0; }
unsigned myInitializeCriticalSection(unsigned a) { return 0; }
unsigned myDeleteCriticalSection(unsigned a) { return 0; }
unsigned myEnterCriticalSection(unsigned a) { return 0; }
unsigned myLeaveCriticalSection(unsigned a) { return 0; }
unsigned myRtlUnwind(unsigned a, unsigned b, unsigned c, unsigned d) { return 0; }
#if 0
unsigned myHeapAlloc(unsigned heap, unsigned flags, unsigned size) { unsigned ptr = (unsigned)((uint8_t *)malloc(size) - BASE); heap_add((struct heap *)(BASE + heap), ptr); return ptr; }
unsigned myHeapFree(unsigned heap, unsigned b, unsigned ptr) { if (!heap_del((struct heap *)(BASE + heap), ptr)) return 0; free(BASE + ptr); return 1; }
unsigned myHeapReAlloc(unsigned a, unsigned b, unsigned c, unsigned d) { return 0; }
unsigned myHeapDestroy(unsigned heap) { heap_free_all((struct heap *)(BASE + heap)); heap_done((struct heap *)heap); return 1; }
unsigned myHeapCreate(unsigned a, unsigned b, unsigned c) { return (unsigned)((uint8_t *)heap_create() - BASE); }
#else
unsigned myHeapAlloc(unsigned heap, unsigned flags, unsigned size) { unsigned ret = heap_alloc(curdll, size); if (!ret) curdll->last_error = ERROR_NOT_ENOUGH_MEMORY; return ret; }
unsigned myHeapReAlloc(unsigned a, unsigned b, unsigned c, unsigned d) { return 0; }
unsigned myHeapFree(unsigned heap, unsigned b, unsigned ptr) { if (heap_free(curdll, ptr)) { curdll->last_error = ERROR_INVALID_PARAMETER; return 0; } return 1; }
unsigned myHeapDestroy(unsigned heap) { return 1; }
unsigned myHeapCreate(unsigned a, unsigned b, unsigned c) { return 0x10000; }
#endif
unsigned myVirtualFree(unsigned ptr, unsigned b, unsigned free_type) { /*if (free_type == MEM_RELEASE) free((void *)ptr);*/ return 1; }
unsigned myGetEnvironmentVariableA(unsigned a, unsigned b, unsigned c) { return 0; }
unsigned myGetVersionExA(unsigned a) { return 0; }
unsigned myGetCPInfo(unsigned a, unsigned b) { return 0; }
unsigned myGetACP() { return 1252; }
unsigned myGetOEMCP() { return 437; }
unsigned mySetHandleCount(unsigned a) { return 0; }
unsigned myGetFileType(unsigned a) { return 0; }
unsigned myGetStartupInfoA(unsigned info) { memset((BASE + info), 0, 17 * 4); return 0; }
unsigned myFreeEnvironmentStringsA(unsigned a) { return 0; }
unsigned myFreeEnvironmentStringsW(unsigned a) { return 0; }
unsigned myWideCharToMultiByte(unsigned a, unsigned b, unsigned srcn, unsigned srclen, unsigned destn, unsigned destlen, unsigned x, unsigned y) {
	uint16_t *src = (uint16_t *)(BASE + srcn);
	uint8_t *dest = (uint8_t *)(BASE + destn), *p = dest;
	if (srclen == 0xffffffff) { uint16_t *ps = src; while (*ps++) {} srclen = ps - src; }
	if (!dest)
		return srclen;
	while (srclen-- && destlen--) *p++ = *src++;
	return p - dest;
}
unsigned myGetEnvironmentStrings() { /*static uint8_t strs[] = {0, 0};*/ return curdll->zeroword; }
unsigned myGetEnvironmentStringsW() { /*static uint16_t strs[] = {0, 0};*/ return curdll->zeroword; }
unsigned mySetFilePointer(unsigned a, unsigned b, unsigned c, unsigned d) { return 0; }
unsigned myMultiByteToWideChar(unsigned a, unsigned b, unsigned c, unsigned d, unsigned e, unsigned f) { return 0; }
unsigned myGetStringTypeA(unsigned a, unsigned b, unsigned c, unsigned d, unsigned e) { return 0; }
unsigned myGetStringTypeW(unsigned a, unsigned b, unsigned c, unsigned d, unsigned e) { return 0; }
unsigned myLCMapStringA(unsigned a, unsigned b, unsigned c, unsigned d, unsigned e, unsigned f) { return 0; }
unsigned myLCMapStringW(unsigned a, unsigned b, unsigned c, unsigned d, unsigned e, unsigned f) { return 0; }
unsigned myRaiseException(unsigned a, unsigned b, unsigned c, unsigned d) { return 0; }
unsigned mySetStdHandle(unsigned a, unsigned b) { return 0; }
unsigned myFlushFileBuffers(unsigned a) { return 0; }
unsigned myCloseHandle(unsigned a) { return 0; }

#define MAX_FILES 20
CFILE *files[MAX_FILES];

unsigned File_Open(unsigned name, unsigned mode) {
	unsigned f = 1;
	while (f < MAX_FILES && !files[f])
		f++;
	if (f == MAX_FILES)
		return 0;
	printf("File_Open %s %s\n", (char *)(BASE + name), (char *)(BASE + mode));
	files[f] = cfopen((char *)(BASE + name), (char *)(BASE + mode));
	return f;
}

unsigned File_eof(unsigned f) {
	if (f >= MAX_FILES || !files[f])
		return 0;
	return cfeof(files[f]);
}

unsigned File_ReadString(unsigned buf, unsigned size, unsigned f) {
	if (f >= MAX_FILES || !files[f])
		return 0;
	return (unsigned)cf_ReadString((char *)(BASE + buf), size, files[f]);
}

unsigned File_Close(unsigned f) {
	if (f >= MAX_FILES || !files[f])
		return 0;
	cfclose(files[f]);
	files[f] = NULL;
	return 0;
}

unsigned FindName(unsigned name) {
	printf("FindName %s\n", (char *)(BASE + name));
	return 0;
}
unsigned Scrpt_FindObjectName(unsigned name) {
	printf("FindObjectName %s\n", (char *)(BASE + name));
	return osipf_FindObjectName((char *)(BASE + name));
}
unsigned Scrpt_FindRoomName(unsigned name) {
	printf("FindRoomName %s\n", (char *)(BASE + name));
	return osipf_FindRoomName((char *)(BASE + name));
}
unsigned Scrpt_FindTriggerName(unsigned name) {
	printf("FindTriggerName %s\n", (char *)(BASE + name));
	return osipf_FindTriggerName((char *)(BASE + name));
}
unsigned Scrpt_FindLevelGoalName(unsigned name) {
	printf("FindLevelGoalName %s\n", (char *)(BASE + name));
	return osipf_FindLevelGoalName((char *)(BASE + name));
}
unsigned Scrpt_FindTextureName(unsigned name) {
	printf("FindTextureName %s\n", (char *)(BASE + name));
	return osipf_FindTextureName((char *)(BASE + name));
}
unsigned Scrpt_GetTriggerFace(unsigned trigger) { return osipf_GetTriggerFace(trigger); }
unsigned Scrpt_GetTriggerRoom(unsigned trigger) { return osipf_GetTriggerRoom(trigger); }
unsigned Scrpt_CreateTimer(unsigned timer) { return Osiris_CreateTimer((tOSIRISTIMER *)(BASE + timer)); }
unsigned Cine_StartCanned(unsigned info) { return 0; }
extern void msafe_CallFunction(ubyte type, msafe_struct *mstruct);
extern void msafe_GetValue(ubyte type, msafe_struct *mstruct);
extern void msafe_DoPowerup(msafe_struct *mstruct);
extern void osipf_LGoalValue(char action,char type,void *val,int goal,int item);
extern void osipf_MatcenValue(int h,char op,char type,void *val,intptr_t prod);
union bitfloat { unsigned bit; float f; };
extern void osipf_ObjectCustomAnim(int objnum,float start,float end,float time,char flags,int sound,char next_type);
unsigned MSafe_CallFunction(unsigned type, unsigned mstruct) { msafe_CallFunction(type, (msafe_struct *)(BASE + mstruct)); return 0;}
unsigned MSafe_GetValue(unsigned type, unsigned mstruct) { msafe_GetValue(type, (msafe_struct *)(BASE + mstruct)); return 0;}
unsigned MSafe_DoPowerup(unsigned mstruct) { printf("mpowerup\n"); msafe_DoPowerup((msafe_struct *)(BASE + mstruct)); return 0;}
unsigned LGoal_Value(unsigned act, unsigned type, unsigned val, unsigned goal, unsigned item) { osipf_LGoalValue(act, type, (BASE + val), goal, item); return 0; }
unsigned Matcen_Value(unsigned h,unsigned op, unsigned type, unsigned val, unsigned prod) { osipf_MatcenValue(h, op, type, (BASE + val), prod); return 0; }
unsigned Obj_SetCustomAnim(unsigned objnum,unsigned start,unsigned end,unsigned time,unsigned flags,unsigned sound,unsigned next_type) {
	union bitfloat start_float = { start }, end_float = { end }, time_float = { time };
	osipf_ObjectCustomAnim(objnum,start_float.f,end_float.f,time_float.f,flags,sound,next_type);
	return 0;
}
void osipf_ObjKill(int handle,int killer_handle,float damage,int flags,float min_time,float max_time);
unsigned Obj_Kill(unsigned handle,unsigned killer_handle,unsigned damage,unsigned flags,unsigned min_time,unsigned max_time) {
	union bitfloat damagef = { damage  }, min_timef = { min_time }, max_timef = { max_time };
	osipf_ObjKill(handle,killer_handle,damagef.f,flags,min_timef.f,max_timef.f);
	return 0;
}

struct {
	const char *name;
	int args;
	union {
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
	{"GetStdHandle", 1, {.fun1 = myGetStdHandle} },
	{"WriteFile", 5, {.fun5 = myWriteFile}},
	{"VirtualAlloc", 4, {.fun4 = myVirtualAlloc}},
	{"IsBadWritePtr", 2, {.fun2 = myIsBadWritePtr}},
	{"IsBadReadPtr", 2, {.fun2 = myIsBadReadPtr}},
	{"HeapValidate", 3, {.fun3 = myHeapValidate}},
	{"GetCommandLineA", 0, {.fun0 = myGetCommandLineA}},
	{"GetVersion", 0, {.fun0 = myGetVersion}},
	{"GetProcAddress", 2, {.fun2 = myGetProcAddress}},
	{"GetModuleHandleA", 1, {.fun1 = myGetModuleHandleA}},
	{"GetCurrentThreadId", 0, {.fun0 = myGetCurrentThreadId}},
	{"TlsSetValue", 2, {.fun2 = myTlsSetValue}},
	{"TlsAlloc", 0, {.fun0 = myTlsAlloc}},
	{"TlsFree", 1, {.fun1 = myTlsFree}},
	{"SetLastError", 1, {.fun1 = mySetLastError}},
	{"TlsGetValue", 1, {.fun1 = myTlsGetValue}},
	{"GetLastError", 0, {.fun0 = myGetLastError}},
	{"DebugBreak", 0, {.fun0 = myDebugBreak}},
	{"InterlockedDecrement", 1, {.fun1 = myInterlockedDecrement}},
	{"OutputDebugStringA", 1, {.fun1 = myOutputDebugStringA}},
	{"LoadLibraryA", 1, {.fun1 = myLoadLibraryA}},
	{"InterlockedIncrement", 1, {.fun1 = myInterlockedIncrement}},
	{"GetModuleFileNameA", 3, {.fun3 = myGetModuleFileNameA}},
	{"ExitProcess", 1, {.fun1 = myExitProcess}},
	{"TerminateProcess", 2, {.fun2 = myTerminateProcess}},
	{"GetCurrentProcess", 0, {.fun0 = myGetCurrentProcess}},
	{"InitializeCriticalSection", 1, {.fun1 = myInitializeCriticalSection}},
	{"DeleteCriticalSection", 1, {.fun1 = myDeleteCriticalSection}},
	{"EnterCriticalSection", 1, {.fun1 = myEnterCriticalSection}},
	{"LeaveCriticalSection", 1, {.fun1 = myLeaveCriticalSection}},
	{"RtlUnwind", 4, {.fun4 = myRtlUnwind}},
	{"HeapAlloc", 3, {.fun3 = myHeapAlloc}},
	{"HeapReAlloc", 4, {.fun4 = myHeapReAlloc}},
	{"HeapFree", 3, {.fun3 = myHeapFree}},
	{"VirtualFree", 3, {.fun3 = myVirtualFree}},
	{"GetEnvironmentVariableA", 3, {.fun3 = myGetEnvironmentVariableA}},
	{"GetVersionExA", 1, {.fun1 = myGetVersionExA}},
	{"HeapDestroy", 1, {.fun1 = myHeapDestroy}},
	{"HeapCreate", 3, {.fun3 = myHeapCreate}},
	{"GetCPInfo", 2, {.fun2 = myGetCPInfo}},
	{"GetACP", 0, {.fun0 = myGetACP}},
	{"GetOEMCP", 0, {.fun0 = myGetOEMCP}},
	{"SetHandleCount", 1, {.fun1 = mySetHandleCount}},
	{"GetFileType", 1, {.fun1 = myGetFileType}},
	{"GetStartupInfoA", 1, {.fun1 = myGetStartupInfoA}},
	{"FreeEnvironmentStringsA", 1, {.fun1 = myFreeEnvironmentStringsA}},
	{"FreeEnvironmentStringsW", 1, {.fun1 = myFreeEnvironmentStringsW}},
	{"WideCharToMultiByte", 8, {.fun8 = myWideCharToMultiByte}},
	{"GetEnvironmentStrings", 0, {.fun0 = myGetEnvironmentStrings}},
	{"GetEnvironmentStringsW", 0, {.fun0 = myGetEnvironmentStringsW}},
	{"SetFilePointer", 4, {.fun4 = mySetFilePointer}},
	{"MultiByteToWideChar", 6, {.fun6 = myMultiByteToWideChar}},
	{"GetStringTypeA", 5, {.fun5 = myGetStringTypeA}},
	{"GetStringTypeW", 5, {.fun5 = myGetStringTypeW}},
	{"LCMapStringA", 6, {.fun6 = myLCMapStringA}},
	{"LCMapStringW", 6, {.fun6 = myLCMapStringW}},
	{"RaiseException", 4, {.fun4 = myRaiseException}},
	{"SetStdHandle", 2, {.fun2 = mySetStdHandle}},
	{"FlushFileBuffers", 1, {.fun1 = myFlushFileBuffers}},
	{"CloseHandle", 1, {.fun1 = myCloseHandle}},
	{"MonoPrintf", -1 },
	{"osipf_CFopen", 2 | 0x100, {.fun2 = File_Open}},
	{"osipf_CFeof", 1 | 0x100, {.fun1 = File_eof}},
	{"osipf_CFReadtring", 3 | 0x100, {.fun3 = File_ReadString}},
	{"osipf_CFclose", 1 | 0x100, {.fun1 = File_Close}},
	{"osipf_FindDoorName", 1 | 0x100, {.fun1 = FindName}},
	{"osipf_FindSoundName", 1 | 0x100, {.fun1 = FindName}},
	{"osipf_FindPathName", 1 | 0x100, {.fun1 = FindName}},
	{"osipf_FindMatcenName", 1 | 0x100, {.fun1 = FindName}},
	{"osipf_FindObjectName", 1 | 0x100, {.fun1 = Scrpt_FindObjectName}},
	{"osipf_FindRoomName", 1 | 0x100, {.fun1 = Scrpt_FindRoomName}},
	{"osipf_FindTriggerName", 1 | 0x100, {.fun1 = Scrpt_FindTriggerName}},
	{"osipf_FindLevelGoalName", 1 | 0x100, {.fun1 = Scrpt_FindLevelGoalName}},
	{"osipf_FindTextureName", 1 | 0x100, {.fun1 = Scrpt_FindTextureName}},
	{"osipf_GetTriggerFace", 1 | 0x100, {.fun1 = Scrpt_GetTriggerFace}},
	{"osipf_GetTriggerRoom", 1 | 0x100, {.fun1 = Scrpt_GetTriggerRoom}},
	{"Osiris_CreateTimer", 1 | 0x100, {.fun1 = Scrpt_CreateTimer}},
	{"Cinematic_StartCannedScript", 1 | 0x100, {.fun1 = Cine_StartCanned}},
	{"msafe_CallFunction", 2 | 0x100, {.fun2 = MSafe_CallFunction}},
	{"msafe_GetValue", 2 | 0x100, {.fun2 = MSafe_GetValue}},
	{"msafe_DoPowerup", 1 | 0x100, {.fun1 = MSafe_DoPowerup}},
	{"osipf_ObjectCustomAnim", 7 | 0x100, {.fun7 = Obj_SetCustomAnim}},
	{"osipf_ObjKill", 6 | 0x100, {.fun6 = Obj_Kill}},
	{"osipf_LGoalValue", 5 | 0x100, {.fun5 = LGoal_Value}},
	{"osipf_MatcenValue", 5 | 0x100, {.fun5 = Matcen_Value}},
	};
#define MAX_FUNS (sizeof(funs) / sizeof(funs[0]))

int do_int(x86emu_t *emu, u8 num, unsigned type)
{
	//printf("int num %x\n", num);
	if (num == 0x10) {
		putchar(EMU_R_EAX & 0xff);
		fflush(stdout);
		return 1;
	}
	if (num == 0x80 && EMU_R_AL < MAX_FUNS) {
		int n = EMU_R_AL;
		emu_ptr_t *buf = (emu_ptr_t *)(BASE + EMU_R_ESP);
		int c = funs[n].args == -1 ? -1 : funs[n].args & 0xff;
		#ifndef __EMSCRIPTEN__
		printf("run fun %d %s ret %x", n, funs[n].name, buf[0]);
		bool msafe = strncmp(funs[n].name,"MSafe_", 6)==0 && funs[n].name[6] != 'D';
		for (int i = 0; i < c; i++) {
			printf(" %x", buf[i + 1]);
			if (msafe && !i) printf(" (%s)", msafe_names[buf[i + 1]]);
		}
		printf("\n");
		#endif
		switch (c) {
			case -1:
				//vprintf((char *)(BASE + buf[2]), (va_list)(BASE + buf[3]));
				printf("%s", (char *)(BASE + buf[2]));
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
	if((type & 0xff) == INTR_TYPE_FAULT || num == 0x20) {
		printf("fault %d at %x\n", num, EMU_R_EIP);
		x86emu_stop(emu);
	}

	return 0;
}

void error(const char *msg, ...) {
	va_list vp;
	va_start(vp, msg);
	vfprintf(stderr, msg, vp);
	va_end(vp);
}

#if 0
void *mapfile(const char *filename, intptr_t base, int *psize) {
	int fd;
	void *ptr;
	
	if ((fd = open(filename, O_RDONLY)) == -1) {
		perror(filename);
		return NULL;
	}
	
	int size = lseek(fd, 0, SEEK_END);
	lseek(fd, 0, SEEK_SET);

	if (!(ptr = mmap(NULL, size, PROT_READ, MAP_PRIVATE, fd, 0 ))) {
		perror("mmap");
		return NULL;
	}
	return ptr;
}
#endif


// return vmbase, img, stack, heap ofs
uint8_t *read_file(const char *filename, struct dll_address_space *as) {
	CFILE *f;
	uint8_t *base = NULL;
	IMAGE_DOS_HEADER dos;
	IMAGE_NT_HEADERS nt;
	emu_ptr_t img_ofs, stack_size, heap_size, img_size, align;
	
	if (!(f = cfopen(filename, "rb"))) {
		perror(filename);
		return NULL;
	}
	
	int size = cfilelength(f);

	if (cf_ReadBytes((ubyte *)&dos, sizeof(dos), f) != sizeof(dos))
		goto read_err;
	if (dos.e_magic != 0x5a4d)
		goto read_err;
	cfseek(f, dos.e_lfanew, SEEK_SET);
	if (cf_ReadBytes((ubyte *)&nt, sizeof(nt), f) != sizeof(nt))
		goto read_err;
	if (nt.Signature != 0x4550)
		goto read_err;
	if ((unsigned)size > nt.OptionalHeader.SizeOfImage)
		size = nt.OptionalHeader.SizeOfImage;
	img_ofs = 0x10000;
	stack_size = 0x10000;
	heap_size = 0x10000;
	img_size = nt.OptionalHeader.SizeOfImage;
	img_size = (img_size + 0xffff) & ~0xffff;

	as->img_ofs = img_ofs;
	as->heap_ofs = img_ofs + img_size;
	as->heap_size = heap_size;
	as->stack_ofs = as->heap_ofs + stack_size;
	as->size = as->stack_ofs;
	align = 16 * 1048576;
	as->size = (as->size + align - 1) & ~(align - 1);
	as->virt_ofs = as->stack_ofs;

	#ifndef WIN32
	if (!(base = (uint8_t *)memalign(align, as->size))) {
	#else
	if (!(base = (uint8_t *)VirtualAlloc((void*)0 /*0x10000000*/, nt.OptionalHeader.SizeOfImage, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE))) {//mmap((void *)0x10000000, nt.OptionalHeader.SizeOfImage, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0))) {
	#endif
		perror("memalign");
		return NULL;
	}
	//if (psize)
	//	*psize = nt.OptionalHeader.SizeOfImage;
	as->base = base;
	cfseek(f, 0, SEEK_SET);	
	if (cf_ReadBytes(as->base + as->img_ofs, size, f) != (size_t)size)
		goto read_err;
	cfclose(f);
	return as->base + as->img_ofs;
read_err:
	perror(filename);
	cfclose(f);
	if (base)
		free(base);
	return NULL;
}

// funcode must have MAX_FUNS * 5 bytes
int funcode_size() {
	return MAX_FUNS * 5;
}

void gen_funcode(uint8_t *funcode) {
	for (unsigned i = 0; i < MAX_FUNS; i++) {
		uint8_t *fun = funcode + i * 5;
		*fun++ = 0xb0; // mov ah, constant
		*fun++ = i;
		*fun++ = 0xcd; // int 0x80
		*fun++ = 0x80;
		*fun++ = 0xc3;  // ret
	}
}

// pFile is source (mapped) file
// pBase is destination memory
void image_setup_sections(IMAGE_NT_HEADERS *ntHdr, void *pFile, void *pBase) {
	PIMAGE_SECTION_HEADER firstSect = (IMAGE_SECTION_HEADER *)((char *)ntHdr + sizeof(IMAGE_NT_HEADERS));
	for (int sectIdx = ntHdr->FileHeader.NumberOfSections - 1; sectIdx >= 0; sectIdx--) {
		PIMAGE_SECTION_HEADER sect = firstSect + sectIdx;
		#ifdef LOG
		printf("sect %x fileptr %x memptr %x filesize %x memsize %x\n",
			sectIdx, sect->PointerToRawData, sect->VirtualAddress,
			sect->SizeOfRawData, sect->Misc.VirtualSize);
		#endif
		if (sect->VirtualAddress != sect->PointerToRawData)
			memmove((char *)pBase + sect->VirtualAddress,
				(char *)pFile + sect->PointerToRawData, sect->SizeOfRawData);
		if (sect->Misc.VirtualSize > sect->SizeOfRawData)
			memset((char *)pBase + sect->VirtualAddress + sect->SizeOfRawData, 0,
				sect->Misc.VirtualSize - sect->SizeOfRawData);
	}
}

void image_reloc(IMAGE_NT_HEADERS *ntHdr, void *pBase, emu_ptr_t img_ofs) {
	IMAGE_DATA_DIRECTORY *reloc_dir = &ntHdr->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC];
	IMAGE_BASE_RELOCATION *reloc = (IMAGE_BASE_RELOCATION *)((char *)pBase + reloc_dir->VirtualAddress);
	IMAGE_BASE_RELOCATION *reloc_end = (IMAGE_BASE_RELOCATION *)((char *)pBase + reloc_dir->VirtualAddress + reloc_dir->Size);
	
	DWORD delta = img_ofs - ntHdr->OptionalHeader.ImageBase;
	while (reloc < reloc_end) {
		DWORD va = reloc->VirtualAddress, size = reloc->SizeOfBlock;
		if (!size)
			abort();
		WORD *p = (WORD *)(reloc + 1), *pe = (WORD *)((char *)reloc + size);
		while (p < pe) {
			WORD val = *p++;
			void *dest = (void *)((char *)pBase + va + (val & 0xfff));
			val >>= 12;
			if (val == IMAGE_REL_BASED_HIGHLOW)
				*(DWORD *)dest += delta;
			else if (val == IMAGE_REL_BASED_LOW)
				*(WORD *)dest += delta & 0xffff;
			else if (val == IMAGE_REL_BASED_HIGH)
				*(WORD *)dest += delta >> 16;
		}
		reloc = (IMAGE_BASE_RELOCATION *)pe;
	}
}

emu_ptr_t find_fun(dll_t *dll, const char *name) {
	for (unsigned i = 0; i < MAX_FUNS; i++)
		if (strcmp(name, funs[i].name) == 0)
			return dll->funcode + i * 5;
	printf("not found %s\n", name);
	return 0;
}

void image_import(dll_t *dll, IMAGE_NT_HEADERS *ntHdr, void *pBase) {
	IMAGE_DATA_DIRECTORY *dir = &ntHdr->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT];
	IMAGE_IMPORT_DESCRIPTOR *imp = (IMAGE_IMPORT_DESCRIPTOR *)((char *)pBase + dir->VirtualAddress);
	for (; imp->Name; imp++) {
		const char *libName = (char *)pBase + imp->Name;
		//printf("import %s\n", libName);
		#if 0
		HMODULE lib = LoadLibrary(libName);
		if (!lib) {
			error(libName);
			return 0;
		}
		#endif
		PIMAGE_THUNK_DATA src, dst;
		dst = (IMAGE_THUNK_DATA *)((char *)pBase + imp->FirstThunk);
		if (imp->OriginalFirstThunk)
			src = (IMAGE_THUNK_DATA *)((char *)pBase + imp->OriginalFirstThunk);
		else
			src = dst;
		for (; src->u1.AddressOfData; src++, dst++) {
			const char *name;
			if (IMAGE_SNAP_BY_ORDINAL(src->u1.Ordinal)) {
				//name = (char *)IMAGE_ORDINAL(src->u1.Ordinal);
				name = NULL;
				printf("ordinal %d\n", src->u1.Ordinal);
			} else {
				PIMAGE_IMPORT_BY_NAME pImport = (IMAGE_IMPORT_BY_NAME *)((char *)pBase + src->u1.AddressOfData);
				name = (char *)pImport->Name;
				//printf("import function %s\n", name);
				//dst->u1.Function = 0; //NULL; //(DWORD)GetProcAddress(lib, name);
				dst->u1.Function = find_fun(dll, name);
			}
			if (!dst->u1.Function) {
				printf("\t{\"%s\", my%s, 0},\n", name, name);
				//error(name);
				//return 0;
			}
		}
	}
}

void dll_done() {
	if (!emu)
		return;
	free(stack);
	free(funcode);
	x86emu_done(emu);
	stack = NULL;
	funcode = NULL;
	emu = NULL;
}

void dll_init() {
	if (emu)
		return;
	//static uint8_t endcode[] = {0xcc, 'S', 'C'};
	//static unsigned unwind2[] = {(unsigned)-1, (unsigned)&endcode, (unsigned)&endcode};
	emu = emu_new();
	/*
	//funcode = gen_funcode();
	int stack_size = 1048576;
	stack = malloc(stack_size);
	EMU_R_ESP = (unsigned)stack + stack_size;
	Push32(emu, (unsigned)-1);
	Push32(emu, (unsigned)&unwind2);
	Push32(emu, (unsigned)&endcode);
	Push32(emu, 0);
	tls_vals[0] = EMU_R_ESP;
	emu->segs_offs[_FS] = (uint32_t)&tls_vals;
	*/
	atexit(dll_done);
	msafenames_init();
}

unsigned emucall(dll_t *dll, x86emu_t *emu, unsigned ip) {
	curdll = dll;
	emu->segs_offs[_FS] = dll->tls_vals_ofs;
	Push32(emu, dll->endcode);
	R_EIP = ip;
	Run(emu, 0);
	emu->quit = 0;
	curdll = NULL;
	return R_EAX;
}

unsigned dllfun_call(dll_t *dll, unsigned fun, int argc, ...) {
	va_list vp;
	va_start(vp, argc);
	BASE = dll->as.base;
	R_ESP = dll->as.stack_ofs;
	R_ESP -= argc * 4;
	for (int i = 0; i < argc; i++)
		*(unsigned *)(dll->as.base + R_ESP + i * 4) = va_arg(vp, unsigned);
	unsigned ret = emucall(dll, emu, fun);
	va_end(vp);
	return ret;
}

dll_t *dll_load(const char *filename) {
	uint8_t *pFile, *pBase;
	dll_t *dll;
	
	dll = new dll_t();

	if (!(pFile = read_file(filename, &dll->as)))
		return 0;

	memset(dll->as.base, 0xcc, 256);

	dll->as.stack_ofs -= 4;
	dll->endcode = dll->as.stack_ofs;
	static uint8_t endcode[] = {0xcc, 'S', 'C'};
	memcpy(dll->as.base + dll->endcode, endcode, sizeof(endcode));

	dll->as.stack_ofs -= 4;
	dll->zeroword = dll->as.stack_ofs;
	memset(dll->as.base + dll->zeroword, 0, 4);

	dll->as.stack_ofs -= (funcode_size() + 3) & ~3;
	dll->funcode = dll->as.stack_ofs;
	gen_funcode(dll->as.base + dll->funcode);

	dll->as.stack_ofs -= MAX_TLS_VALS * 4;
	dll->tls_vals_ofs = dll->as.stack_ofs;
	dll->tls_vals = (emu_ptr_t *)(dll->as.base + dll->tls_vals_ofs);
	memset(dll->tls_vals, 0, MAX_TLS_VALS * 4);
	dll->tls_next = 0;

	static emu_ptr_t unwind2[] = {(emu_ptr_t)-1, dll->endcode, dll->endcode};
	dll->as.stack_ofs -= sizeof(unwind2);
	dll->tls_vals[0] = dll->as.stack_ofs;
	memcpy(dll->as.base + dll->tls_vals[0], unwind2, sizeof(unwind2));

	dll->last_error = 0;

	IMAGE_DOS_HEADER *pDosHeader = (IMAGE_DOS_HEADER *)pFile;
	IMAGE_NT_HEADERS *ntHdr = (IMAGE_NT_HEADERS *)((char *)pFile + pDosHeader->e_lfanew);

	pBase = pFile;
	image_setup_sections(ntHdr, pFile, pBase);
	image_reloc(ntHdr, pBase, dll->as.img_ofs);
	image_import(dll, ntHdr, pBase);
	
	dll->entry = dll->as.img_ofs + ntHdr->OptionalHeader.AddressOfEntryPoint;

	IMAGE_DATA_DIRECTORY *export_data = &ntHdr->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT];
	IMAGE_EXPORT_DIRECTORY *export_dir = (IMAGE_EXPORT_DIRECTORY *)(pBase + export_data->VirtualAddress);
	dll->exports = export_dir;

	for (ULONG i = 0; i < export_dir->NumberOfNames; i++) {
		unsigned f = ((uint16_t *)((char *)pBase + export_dir->AddressOfNameOrdinals))[i];
		unsigned addr = ((unsigned *)((char *)pBase + export_dir->AddressOfFunctions))[f];
		printf("%s %d %x %p\n", (char *)pBase + ((unsigned *)((char *)pBase + export_dir->AddressOfNames))[i], f, addr, pBase + addr);
	}

	heap_init(dll, dll->as.heap_ofs, dll->as.heap_size);

	if (!dllfun_call(dll, dll->entry, 3, dll->as.img_ofs, DLL_PROCESS_ATTACH, 0)) {
		free(dll->as.base);
		free(dll);
		return NULL;
	}

	return dll;
}

void dll_free(dll_t *dll) {
	if (!dll)
		return;
	dllfun_call(dll, dll->entry, 3, dll->as.img_ofs, DLL_PROCESS_DETACH, 0);
	//free(dll->mem);
#ifdef WIN32
	VirtualFree(dll->mem, 0, MEM_RELEASE);
#else
	free(dll->as.base);
	//munmap(dll->mem, dll->memsize);
#endif
	delete dll;
}

emu_ptr_t dll_find(dll_t *dll, const char *name) {
	IMAGE_EXPORT_DIRECTORY *export_dir = dll->exports;
	uint8_t *base = dll->as.base + dll->as.img_ofs;
	DWORD *names = (DWORD *)(base + export_dir->AddressOfNames);
	for (ULONG i = 0; i < export_dir->NumberOfNames; i++) {
		if (strcmp((char *)(base + names[i]), name) == 0) {
			uint16_t idx = ((uint16_t *)(base + export_dir->AddressOfNameOrdinals))[i];
			DWORD addr = ((unsigned *)(base + export_dir->AddressOfFunctions))[idx];
			return dll->as.img_ofs + addr;
		}
	}
	return 0;
}

void osiris_setup(dll_t *dll, tOSIRISModuleInit *mod) {
	memset(mod, 0, sizeof(*mod));
	mod->game_checksum = 0x87888d9b;
	#if 0
	mod->mprintf = (mprintf_fp)find_fun(dll, "mprintf");
	mod->File_ReadString = (File_ReadString_fp)find_fun(dll, "File_ReadString");
	mod->File_Open = (File_Open_fp)find_fun(dll, "File_Open");
	mod->File_Close = (File_Close_fp)find_fun(dll, "File_Close");
	mod->File_eof = (File_eof_fp)find_fun(dll, "File_eof");
	mod->Scrpt_FindRoomName  = (Scrpt_FindRoomName_fp)find_fun(dll, "Scrpt_FindRoomName");
	mod->Scrpt_FindSoundName  = (Scrpt_FindRoomName_fp)find_fun(dll, "FindName");
	mod->Scrpt_FindLevelGoalName  = (Scrpt_FindLevelGoalName_fp)find_fun(dll, "Scrpt_FindLevelGoalName");
	mod->Scrpt_FindTextureName  = (Scrpt_FindTextureName_fp)find_fun(dll, "Scrpt_FindTextureName");
	mod->Scrpt_FindPathName  = (Scrpt_FindRoomName_fp)find_fun(dll, "FindName");
	mod->Scrpt_FindTriggerName  = (Scrpt_FindTriggerName_fp)find_fun(dll, "Scrpt_FindTriggerName");
	mod->Scrpt_FindDoorName  = (Scrpt_FindRoomName_fp)find_fun(dll, "FindName");
	mod->Scrpt_FindObjectName  = (Scrpt_FindObjectName_fp)find_fun(dll, "Scrpt_FindObjectName");
	mod->Scrpt_FindMatcenName  = (Scrpt_FindRoomName_fp)find_fun(dll, "FindName");
	mod->Scrpt_GetTriggerFace = (Scrpt_GetTriggerFace_fp)find_fun(dll, "Scrpt_GetTriggerFace");
	mod->Scrpt_GetTriggerRoom = (Scrpt_GetTriggerRoom_fp)find_fun(dll, "Scrpt_GetTriggerRoom");
	mod->Scrpt_CreateTimer = (Scrpt_CreateTimer_fp)find_fun(dll, "Scrpt_CreateTimer");
	mod->Cine_StartCanned = (Cine_StartCanned_fp)find_fun(dll, "Cine_StartCanned");
	mod->MSafe_CallFunction = (MSafe_CallFunction_fp)find_fun(dll, "MSafe_CallFunction");
	mod->MSafe_GetValue = (MSafe_GetValue_fp)find_fun(dll, "MSafe_GetValue");
	mod->MSafe_DoPowerup = (MSafe_DoPowerup_fp)find_fun(dll, "MSafe_DoPowerup");
	mod->Obj_SetCustomAnim = (Obj_SetCustomAnim_fp)find_fun(dll, "Obj_SetCustomAnim");
	mod->Obj_Kill = (Obj_Kill_fp)find_fun(dll, "Obj_Kill");
	mod->LGoal_Value = (LGoal_Value_fp)find_fun(dll, "LGoal_Value");
	mod->Matcen_Value = (Matcen_Value_fp)find_fun(dll, "Matcen_Value");
	#endif
	int i = 0;
	mod->fp[i++] = find_fun(dll, "MonoPrintf");
	mod->fp[i++] = find_fun(dll, "msafe_CallFunction");
	mod->fp[i++] = find_fun(dll, "msafe_GetValue");
	mod->fp[i++] = find_fun(dll, "osipf_CallObjectEvent");
	mod->fp[i++] = find_fun(dll, "osipf_CallTriggerEvent");
	mod->fp[i++] = find_fun(dll, "osipf_SoundTouch");
	mod->fp[i++] = find_fun(dll, "osipf_ObjectFindID");
	mod->fp[i++] = find_fun(dll, "osipf_WeaponFindID");
	mod->fp[i++] = find_fun(dll, "osipf_ObjectGetTimeLived");
	mod->fp[i++] = find_fun(dll, "osipf_GetGunPos");
	mod->fp[i++] = find_fun(dll, "osipf_RoomValue");
	mod->fp[i++] = find_fun(dll, "osipf_IsRoomValid");
	mod->fp[i++] = find_fun(dll, "osipf_GetAttachParent");
	mod->fp[i++] = find_fun(dll, "osipf_GetNumAttachSlots");
	mod->fp[i++] = find_fun(dll, "osipf_GetAttachChildHandle");
	mod->fp[i++] = find_fun(dll, "osipf_AttachObjectAP");
	mod->fp[i++] = find_fun(dll, "osipf_AttachObjectRad");
	mod->fp[i++] = find_fun(dll, "osipf_UnattachFromParent");
	mod->fp[i++] = find_fun(dll, "osipf_UnattachChild");
	mod->fp[i++] = find_fun(dll, "osipf_UnattachChildren");
	mod->fp[i++] = find_fun(dll, "osipf_RayCast");
	mod->fp[i++] = find_fun(dll, "osipf_AIGetPathID");
	mod->fp[i++] = find_fun(dll, "osipf_AIGoalFollowPathSimple");
	mod->fp[i++] = find_fun(dll, "osipf_AIPowerSwitch");
	mod->fp[i++] = find_fun(dll, "osipf_AITurnTowardsVectors");
	mod->fp[i++] = find_fun(dll, "osipf_AISetType");
	mod->fp[i++] = find_fun(dll, "osipf_AIFindHidePos");
	mod->fp[i++] = find_fun(dll, "osipf_AIGoalAddEnabler");
	mod->fp[i++] = find_fun(dll, "osipf_AIGoalAdd");
	mod->fp[i++] = find_fun(dll, "osipf_AIGoalClear");
	mod->fp[i++] = find_fun(dll, "osipf_AIValue");
	mod->fp[i++] = find_fun(dll, "osipf_AIFindObjOfType");
	mod->fp[i++] = find_fun(dll, "osipf_AIGetRoomPathPoint");
	mod->fp[i++] = find_fun(dll, "osipf_AIFindEnergyCenter");
	mod->fp[i++] = find_fun(dll, "osipf_AIGetDistToObj");
	mod->fp[i++] = find_fun(dll, "osipf_AISetGoalFlags");
	mod->fp[i++] = find_fun(dll, "osipf_AISetGoalCircleDist");
	mod->fp[i++] = find_fun(dll, "osipf_CFReadBytes");
	mod->fp[i++] = find_fun(dll, "osipf_CFReadInt");
	mod->fp[i++] = find_fun(dll, "osipf_CFReadShort");
	mod->fp[i++] = find_fun(dll, "osipf_CFReadByte");
	mod->fp[i++] = find_fun(dll, "osipf_CFReadFloat");
	mod->fp[i++] = find_fun(dll, "osipf_CFReadDouble");
	mod->fp[i++] = find_fun(dll, "osipf_CFReadString");
	mod->fp[i++] = find_fun(dll, "osipf_CFWriteBytes");
	mod->fp[i++] = find_fun(dll, "osipf_CFWriteString");
	mod->fp[i++] = find_fun(dll, "osipf_CFWriteInt");
	mod->fp[i++] = find_fun(dll, "osipf_CFWriteShort");
	mod->fp[i++] = find_fun(dll, "osipf_CFWriteByte");
	mod->fp[i++] = find_fun(dll, "osipf_CFWriteFloat");
	mod->fp[i++] = find_fun(dll, "osipf_CFWriteDouble");
	mod->fp[i++] = find_fun(dll, "Osiris_AllocateMemory");
	mod->fp[i++] = find_fun(dll, "Osiris_FreeMemory");
	mod->fp[i++] = find_fun(dll, "Osiris_CancelTimer");
	mod->fp[i++] = find_fun(dll, "Osiris_CreateTimer");
	mod->fp[i++] = find_fun(dll, "msafe_DoPowerup");
	mod->fp[i++] = find_fun(dll, "osipf_ObjCreate");
	mod->fp[i++] = find_fun(dll, "osipf_GameTime");
	mod->fp[i++] = find_fun(dll, "osipf_FrameTime");
	mod->fp[i++] = find_fun(dll, "osipf_ObjWBValue");
	mod->fp[i++] = find_fun(dll, "Osiris_TimerExists");
	mod->fp[i++] = find_fun(dll, "osipf_ObjectValue");
	mod->fp[i++] = find_fun(dll, "osipf_MatcenValue");
	mod->fp[i++] = find_fun(dll, "osipf_MatcenReset");
	mod->fp[i++] = find_fun(dll, "osipf_MatcenCopy");
	mod->fp[i++] = find_fun(dll, "osipf_MatcenCreate");
	mod->fp[i++] = find_fun(dll, "osipf_MatcenFindId");
	mod->fp[i++] = find_fun(dll, "osipf_MissionFlagSet");
	mod->fp[i++] = find_fun(dll, "osipf_MissionFlagGet");
	mod->fp[i++] = find_fun(dll, "osipf_PlayerValue");
	mod->fp[i++] = find_fun(dll, "osipf_ObjectCustomAnim");
	mod->fp[i++] = find_fun(dll, "osipf_PlayerAddHudMessage");
	mod->fp[i++] = find_fun(dll, "osipf_ObjGhost");
	mod->fp[i++] = find_fun(dll, "osipf_ObjBurning");
	mod->fp[i++] = find_fun(dll, "osipf_ObjIsEffect");
	mod->fp[i++] = find_fun(dll, "osipf_CFopen");
	mod->fp[i++] = find_fun(dll, "osipf_CFclose");
	mod->fp[i++] = find_fun(dll, "osipf_CFtell");
	mod->fp[i++] = find_fun(dll, "osipf_CFeof");
	mod->fp[i++] = find_fun(dll, "osipf_SoundStop");
	mod->fp[i++] = find_fun(dll, "osipf_SoundPlay2d");
	mod->fp[i++] = find_fun(dll, "osipf_SoundPlay3d");
	mod->fp[i++] = find_fun(dll, "osipf_SoundFindId");
	mod->fp[i++] = find_fun(dll, "osipf_AIIsObjFriend");
	mod->fp[i++] = find_fun(dll, "osipf_AIIsObjEnemy");
	mod->fp[i++] = find_fun(dll, "osipf_AIGoalValue");
	mod->fp[i++] = find_fun(dll, "osipf_AIGetNearbyObjs");
	mod->fp[i++] = find_fun(dll, "osipf_AIGetCurGoalIndex");
	mod->fp[i++] = find_fun(dll, "Osiris_OMMS_Malloc");
	mod->fp[i++] = find_fun(dll, "Osiris_OMMS_Attach");
	mod->fp[i++] = find_fun(dll, "Osiris_OMMS_Detach");
	mod->fp[i++] = find_fun(dll, "Osiris_OMMS_Free");
	mod->fp[i++] = find_fun(dll, "Osiris_OMMS_Find");
	mod->fp[i++] = find_fun(dll, "Osiris_OMMS_GetInfo");
	mod->fp[i++] = find_fun(dll, "Cinematic_Start");
	mod->fp[i++] = find_fun(dll, "Cinematic_Stop");
	mod->fp[i++] = find_fun(dll, "osipf_FindSoundName");
	mod->fp[i++] = find_fun(dll, "osipf_FindRoomName");
	mod->fp[i++] = find_fun(dll, "osipf_FindTriggerName");
	mod->fp[i++] = find_fun(dll, "osipf_FindObjectName");
	mod->fp[i++] = find_fun(dll, "osipf_GetTriggerRoom");
	mod->fp[i++] = find_fun(dll, "osipf_GetTriggerFace");
	mod->fp[i++] = find_fun(dll, "osipf_FindDoorName");
	mod->fp[i++] = find_fun(dll, "osipf_FindTextureName");
	mod->fp[i++] = find_fun(dll, "osipf_CreateRandomSparks");
	mod->fp[i++] = find_fun(dll, "Osiris_CancelTimerID");
	mod->fp[i++] = find_fun(dll, "osipf_GetGroundPos");
	mod->fp[i++] = find_fun(dll, "osipf_EnableShip");
	mod->fp[i++] = find_fun(dll, "osipf_IsShipEnabled");
	mod->fp[i++] = find_fun(dll, "osipf_PathGetInformation");
	mod->fp[i++] = find_fun(dll, "Cinematic_StartCannedScript");
	mod->fp[i++] = find_fun(dll, "osipf_FindMatcenName");
	mod->fp[i++] = find_fun(dll, "osipf_FindPathName");
	mod->fp[i++] = find_fun(dll, "osipf_FindLevelGoalName");
	mod->fp[i++] = find_fun(dll, "osipf_ObjectFindType");
	mod->fp[i++] = find_fun(dll, "osipf_LGoalValue");
	mod->fp[i++] = find_fun(dll, "osipf_ObjMakeListOfType");
	mod->fp[i++] = find_fun(dll, "osipf_ObjKill");
	mod->fp[i++] = find_fun(dll, "osipf_AIIsDestReachable");
	mod->fp[i++] = find_fun(dll, "osipf_AIIsObjReachable");
	mod->fp[i++] = find_fun(dll, "osipf_GameGetDiffLevel");
	mod->fp[i++] = find_fun(dll, "osipf_GetLanguageSetting");
	mod->fp[i++] = find_fun(dll, "osipf_PathValue");
}

unsigned dll_push(dll_t *dll, const void *mem, int size) {
	dll->as.stack_ofs -= size;
	if (mem)
		memcpy(dll->as.base + dll->as.stack_ofs, mem, size);
	return dll->as.stack_ofs;
}

void dll_pop(dll_t *dll, int size) {
	dll->as.stack_ofs += size;
}

unsigned dll_get32(dll_t *dll, unsigned vmp) {
	return *(unsigned *)(dll->as.base + vmp);
}

void dll_get(dll_t *dll, unsigned vmp, void *buf, int size) {
	memcpy(buf, dll->as.base + vmp, size);
}

#ifdef TEST
unsigned emucall4(dll_t *dll, x86emu_t *emu, unsigned ip, unsigned a, unsigned b, unsigned c, unsigned d) {
	Push32(emu, d);
	Push32(emu, c);
	Push32(emu, b);
	Push32(emu, a);
	return emucall(dll, emu, ip);
}

//typedef unsigned char bool;

int main() {
	dll_init();

	dll_t *dll = dll_load("level1.dll");

	if (!dll) {
		fprintf(stderr, "loading dll failed\n");
		return EXIT_FAILURE;
	}

	unsigned f = dll_find(dll, "_InitializeDLL@4");
	printf("f %x\n", f);
	tOSIRISModuleInit mod;
	osiris_setup(dll, &mod);
	dllfun_call(dll, f, 1, dll_push(dll, &mod, sizeof(mod)));
	dll_pop(dll, sizeof(mod));

	printf("creating instance\n");
	unsigned inst = dllfun_call(dll, dll_find(dll, "_CreateInstance@4"), 1, 0);

	printf("call instance\n");
	tOSIRISEventInfo info;
	
	dllfun_call(dll, dll_find(dll, "_CallInstanceEvent@16"),
		4, 0, inst, EVT_LEVELSTART, dll_push(dll, &info, sizeof(info)));
	dll_pop(dll, sizeof(info));

	//printf("destroying instance %x\n", inst);
	//emucall2(emu, dll_find(dll, "_DestroyInstance@8"), 0, inst);

	dllfun_call(dll, dll_find(dll, "_ShutdownDLL@0"), 0);

	dll_free(dll);

	dll_done();
	
	return 0;
}
#endif
