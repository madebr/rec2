#ifndef GUARD_70_PACKFILE_H
#define GUARD_70_PACKFILE_H

#include "c2_hooks.h"

#include <stdio.h>

// InitPackFiles

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

// OpenPackFileAndSetTiffLoading

// ClosePackFileAndSetTiffLoading

// PackFileRevertTiffLoading

// PackFileRerevertTiffLoading

#endif // GUARD_70_PACKFILE_H
