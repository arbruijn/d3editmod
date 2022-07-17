#ifndef DLL_H_
#define DLL_H_

void dll_init();
struct dll *dll_load(const char *filename);
void dll_free(struct dll *dll);
unsigned dll_find(struct dll *dll, const char *name);
unsigned dllfun_call(unsigned fun, int argc, ...);

#endif
