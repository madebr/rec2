#ifndef REC2_FILE_H
#define REC2_FILE_H

#include "c2_hooks.h"

#include "brender/br_types.h"

void C2_HOOK_CDECL _BrFileFree(void* res, br_uint_8 res_class, br_size_t size);

br_uint_32 C2_HOOK_CDECL BrFileAttributes(void);

void* C2_HOOK_CDECL BrFileOpenRead(char* name, br_size_t n_magics, br_mode_test_cbfn* mode_test, int* mode_result);

void* C2_HOOK_CDECL BrFileOpenWrite(char* name, int mode);

void C2_HOOK_CDECL BrFileClose(void* f);

int C2_HOOK_CDECL BrFileEof(void* f);

int C2_HOOK_CDECL BrFileGetChar(void* f);

void C2_HOOK_CDECL BrFilePutChar(int c, void* f);

int C2_HOOK_CDECL BrFileRead(void* buf, int size, int n, void* f);

int C2_HOOK_CDECL BrFileWrite(void* buf, int size, int n, void* f);

int C2_HOOK_CDECL BrFileGetLine(char* buf, br_size_t buf_len, void* f);

void C2_HOOK_CDECL BrFilePutLine(char* buf, void* f);

void C2_HOOK_CDECL BrFileAdvance(long count, void* f);

int C2_HOOK_CDECL BrFilePrintf(void* f, char* fmt, ...);

#endif // REC2_FILE_H
