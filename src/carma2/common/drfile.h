#ifndef REC2_DRFILE_H
#define REC2_DRFILE_H

#include "c2_hooks.h"

#include "brender/brender.h"

#include "c2_stdio.h"
#include "c2_stdlib.h"

void* C2_HOOK_CDECL DRStdioOpenRead(const char* name, br_size_t n_magics, br_mode_test_cbfn* identify, int* mode_result);

void* C2_HOOK_CDECL DRStdioOpenWrite(const char* name, int mode);

void C2_HOOK_CDECL DRStdioClose(void* f);

int C2_HOOK_CDECL DRStdioEOF(void* f);

int C2_HOOK_CDECL DRStdioGetChr(void* f);

br_size_t C2_HOOK_CDECL DRStdioRead(void* buf, br_size_t size, unsigned int n, void* f);

br_size_t C2_HOOK_CDECL DRStdioWrite(void* buf, br_size_t size, unsigned int n, void* f);

br_size_t C2_HOOK_CDECL DRStdioGetLine(char* buf, br_size_t buf_len, void* f);

void C2_HOOK_FASTCALL InstallDRFileCalls(void);

#endif // REC2_DRFILE_H
