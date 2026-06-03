#ifndef GUARD_14_DRFILE_H
#define GUARD_14_DRFILE_H

#include "c2_hooks.h"

#include "brender/brender.h"

extern int C2_HOOK_CDECL DRStdioEOF(void* f);

extern int C2_HOOK_CDECL DRStdioGetChr(void* f);

extern br_size_t C2_HOOK_CDECL DRStdioGetLine(char* buf, br_size_t buf_len, void* f);

extern void C2_HOOK_CDECL DRStdioAdvance(br_size_t count, void *f);

extern void* C2_HOOK_CDECL DRStdioOpenRead(const char* name, br_size_t n_magics, br_mode_test_cbfn* identify, int* mode_result);

extern void* C2_HOOK_CDECL DRStdioOpenWrite(const char* name, int mode);

extern void C2_HOOK_CDECL DRStdioClose(void* f) ;

extern br_size_t C2_HOOK_CDECL DRStdioRead(void* buf, br_size_t size, unsigned int n, void* f);

extern br_size_t C2_HOOK_CDECL DRStdioWrite(const void* buf, br_size_t size, unsigned int n, void* f);

extern void C2_HOOK_FASTCALL InstallDRFileCalls(void);

#endif // GUARD_14_DRFILE_H
