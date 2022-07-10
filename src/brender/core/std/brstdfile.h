#ifndef REC2_BRSTDFILE_H
#define REC2_BRSTDFILE_H

#include "c2_hooks.h"

#include "brender/br_types.h"

C2_HOOK_VARIABLE_DECLARE(br_filesystem*, _BrDefaultFilesystem);

br_uint_32 C2_HOOK_CDECL BrStdioAttributes();

void* C2_HOOK_CDECL BrStdioOpenRead(char* name, br_size_t n_magics, br_mode_test_cbfn* identify, int* mode_result);

void* C2_HOOK_CDECL BrStdioOpenWrite(char* name, int mode);

void C2_HOOK_CDECL BrStdioClose(void* f);

int C2_HOOK_CDECL BrStdioEof(void* f);

int C2_HOOK_CDECL BrStdioGetChar(void* f);

void C2_HOOK_CDECL BrStdioPutChar(int c, void* f);

br_size_t C2_HOOK_CDECL BrStdioRead(void* buf, br_size_t size, unsigned int n, void* f);

br_size_t C2_HOOK_CDECL BrStdioWrite(void* buf, br_size_t size, unsigned int n, void* f);

br_size_t C2_HOOK_CDECL BrStdioGetLine(char* buf, br_size_t buf_len, void* f);

void C2_HOOK_CDECL BrStdioPutLine(char* buf, void* f);

void C2_HOOK_CDECL BrStdioAdvance(br_size_t count, void* f);

#endif // REC2_BRSTDFILE_H
