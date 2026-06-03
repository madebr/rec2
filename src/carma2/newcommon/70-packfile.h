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

// PFfgetc

// PFgetc

// PFfgets

// PFungetc

// PFfread

// PFfwrite

// PFftell

// PFfseek

// PFrewind

// PFfeof

// PFForEveryFile

// PFForEveryFile2

extern tTWTVFS C2_HOOK_FASTCALL OpenPackFileAndSetTiffLoading(const char* path);

extern void C2_HOOK_FASTCALL ClosePackFileAndSetTiffLoading(tTWTVFS twt);

// PackFileRevertTiffLoading

// PackFileRerevertTiffLoading

#endif // GUARD_70_PACKFILE_H
