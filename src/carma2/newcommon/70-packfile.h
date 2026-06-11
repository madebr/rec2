#ifndef GUARD_70_PACKFILE_H
#define GUARD_70_PACKFILE_H

#include "rec2_types.h"
#include "c2_hooks.h"

#include <stdio.h>

extern int gDisableTiffConversion;

extern void C2_HOOK_FASTCALL InitPackFiles(void);

extern tTWTVFS C2_HOOK_FASTCALL OpenPackFile(const char* path);

// ClosePackFile

extern void C2_HOOK_FASTCALL PFfclose(FILE* pFile);

extern FILE* C2_HOOK_FASTCALL PFfopen(const char* pPath, const char* mode);

extern int C2_HOOK_FASTCALL PFfgetc(FILE* pFile);

// PFgetc

extern char* C2_HOOK_FASTCALL PFfgets(char* buffer, br_size_t size, FILE* pFile);

extern int C2_HOOK_FASTCALL PFungetc(int ch, FILE* file);

extern br_size_t C2_HOOK_FASTCALL PFfread(void* buf, br_size_t size, unsigned int n, void* f);

extern br_size_t C2_HOOK_FASTCALL PFfwrite(const void* buf, br_size_t size, unsigned int n, void* f);

extern int C2_HOOK_FASTCALL PFftell(FILE* pF);

extern int C2_HOOK_FASTCALL PFfseek(FILE* pF, int offset, int whence);

extern void C2_HOOK_FASTCALL PFrewind(FILE* pF);

extern int C2_HOOK_FASTCALL PFfeof(FILE* pFile);

extern void C2_HOOK_FASTCALL PFForEveryFile(const char* pThe_path, tPDForEveryFileRecurse_cbfn pAction_routine);

extern void C2_HOOK_FASTCALL PFForEveryFile2(const char* path, tEnumPathCallback pCallback, void* data);

extern tTWTVFS C2_HOOK_FASTCALL OpenPackFileAndSetTiffLoading(const char* path);

extern void C2_HOOK_FASTCALL ClosePackFileAndSetTiffLoading(tTWTVFS twt);

extern void C2_HOOK_FASTCALL PackFileRevertTiffLoading(void);

// PackFileRerevertTiffLoading

#endif // GUARD_70_PACKFILE_H
