#ifndef DLL_H_
#define DLL_H_

typedef struct dll_t dll_t;
void dll_init();
dll_t *dll_load(const char *filename);
void dll_free(dll_t *dll);
unsigned dll_find(dll_t *dll, const char *name);
unsigned dllfun_call(dll_t *dll, unsigned fun, int argc, ...);
unsigned dll_push(dll_t *dll, const void *buf, int size);
void dll_pop(dll_t *dll, int size);
void dll_get(dll_t *dll, unsigned vmp, void *buf, int size);
unsigned dll_get32(dll_t *dll, unsigned vmp);

#endif
