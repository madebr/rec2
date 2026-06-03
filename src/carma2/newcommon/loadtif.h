#ifndef GUARD_LOADTIF_H
#define GUARD_LOADTIF_H

#include "brender/brender.h"

extern br_pixelmap* C2_HOOK_FASTCALL LoadTiffTexture_Ex(const char* texturePathDir, const char* textureName, br_pixelmap* pPalette, int flags, int* errorCode);

#endif // GUARD_LOADTIF_H
