#include <string.h>
#include <stdarg.h>
#include <stdio.h>

int stricmp(const char *a, const char *b) {
    return strcasecmp(a, b);
}
int strnicmp(const char *a, const char *b, int n) {
    return strncasecmp(a, b, n);
}
void ddio_InternalKeyClose() {}
void ddio_CopyFileTime(char*, char const*) {}
void ddio_SplitPath(char const*src, char*path, char*fname, char*ext) {
    const char *p = strrchr(src, '/');
    if (p) {
        p++;
        memcpy(path, src, p - src);
        path[p - src] = 0;
    } else {
        path[0] = 0;
        p = src;
    }
    const char *pe = strrchr(p, '.');
    if (!pe)
        pe = p + strlen(p);
    memcpy(fname, p, pe - p);
    fname[pe - p] = 0;
    strcpy(ext, pe);
}
void ddio_MakePath(char*dest, char const*src1, char const*src2, ...) {
    va_list vp;
    va_start(vp, src2);
    strcpy(dest, src1);
    for (;;) {
        if (!src2)
            break;
        int i = strlen(dest);
        if (i && dest[i - 1] != '/')
            dest[i++] = '/';
        strcpy(dest + i, src2);
        src2 = va_arg(vp, const char *);
    }
    va_end(vp);
}
bool ddio_GetFullPath(char*, char const*) { return 1; }
int ddio_GetFileLength(FILE *f) {
    long pos = ftell(f);
    fseek(f, 0, SEEK_END);
    long len = ftell(f);
    fseek(f, pos, SEEK_SET);
    return (int)len;
}
bool ddio_FileDiff(const char* fileNameA, const char* fileNameB) { return false; }
