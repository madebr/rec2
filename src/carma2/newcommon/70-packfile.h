#ifndef GUARD_70_PACKFILE_H
#define GUARD_70_PACKFILE_H

#include "rec2_types.h"
#include "c2_hooks.h"

#include <stdio.h>

extern void C2_HOOK_FASTCALL InitPackFiles(void);

// OpenPackFile

// ClosePackFile

extern void C2_HOOK_FASTCALL PFfclose(FILE* pFile);

extern FILE* C2_HOOK_FASTCALL PFfopen(const char* pPath, const char* mode);

extern int C2_HOOK_FASTCALL PFfgetc(FILE* pFile);

// PFgetc

extern char* C2_HOOK_FASTCALL PFfgets(char* buffer, br_size_t size, FILE* pFile);

// PFungetc

extern br_size_t C2_HOOK_FASTCALL PFfread(void* buf, br_size_t size, unsigned int n, void* f);

extern br_size_t C2_HOOK_FASTCALL PFfwrite(const void* buf, br_size_t size, unsigned int n, void* f);

// PFftell

extern int C2_HOOK_FASTCALL PFfseek(FILE* pF, int offset, int whence);

// PFrewind

extern int C2_HOOK_FASTCALL PFfeof(FILE* pFile);

// PFForEveryFile

// PFForEveryFile2

extern tTWTVFS C2_HOOK_FASTCALL OpenPackFileAndSetTiffLoading(const char* path);

extern void C2_HOOK_FASTCALL ClosePackFileAndSetTiffLoading(tTWTVFS twt);

// PackFileRevertTiffLoading

// PackFileRerevertTiffLoading

#endif // GUARD_70_PACKFILE_H
