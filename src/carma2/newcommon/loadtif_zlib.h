#ifndef LOADTIF_ZLIB_H
#define LOADTIF_ZLIB_H

#include "c2_hooks.h"
#include "brender/brender.h"

extern br_filesystem gZlib_filesystem;

extern br_uint_32 C2_HOOK_CDECL ZlibFsGetAttributes(void);

extern void* C2_HOOK_CDECL ZlibFsOpenRead(const char* buffer, br_size_t capacity, br_mode_test_cbfn* cbfn, int* type);

extern void* C2_HOOK_CDECL ZlibFsOpenWrite(const char* path, int type);

extern void C2_HOOK_CDECL ZlibFsClose(void* context);

extern int C2_HOOK_CDECL ZlibFsEof(void* context);

extern int C2_HOOK_CDECL ZlibFsGetChr(void* context);

extern void C2_HOOK_CDECL ZlibFsPutChr(int chr, void* context);

extern br_size_t C2_HOOK_CDECL ZlibFsRead(void* buffer, br_size_t elem_size, unsigned int count, void* context);

extern br_size_t C2_HOOK_CDECL ZlibFsWrite(const void* buffer, br_size_t elem_size, unsigned int count, void* context);

extern br_size_t C2_HOOK_CDECL ZlibFsGetLine(char* buffer, br_size_t capacity, void* context);;

extern void C2_HOOK_CDECL ZlibFsPutLine(char* line, void* context);

extern void C2_HOOK_CDECL ZlibFsAdvance(br_size_t advance, void* context);

#endif // LOADTIF_ZLIB_H
