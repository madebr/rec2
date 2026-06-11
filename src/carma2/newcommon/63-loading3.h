#ifndef GUARD_63_LOADING3_H
#define GUARD_63_LOADING3_H

#include "c2_hooks.h"
#include "brender/brender.h"
#include "rec2_types.h"

// NearestPowerOfTwo

extern int C2_HOOK_FASTCALL LoadBunchOfPixies(const char* pathRoot, const char* textureName, br_pixelmap** textureBuffer, size_t bufferCapacity);

extern br_pixelmap* C2_HOOK_FASTCALL DRLdImg(const char* texturePathDir, const char* textureName, br_pixelmap* pPalette, int flags, int* errorCode);

extern br_pixelmap* C2_HOOK_FASTCALL RealLoadPixelmap(const char* pPath_name);

extern br_pixelmap* C2_HOOK_FASTCALL LoadPixelmap(const char* pPath_name);

// LoadShadeTable

// LoadMaterial

// LoadModel

// LoadActor

extern void C2_HOOK_FASTCALL DRLoadPalette(const char* pPath_name);

extern void C2_HOOK_FASTCALL DRLoadShadeTable(const char* pPath_name);

extern void C2_HOOK_FASTCALL DRLoadMaterials(const char* pPath_name);

// DRLoadModels

// DRLoadActors

// DRLoadLights

extern void C2_HOOK_FASTCALL LoadInFiles(const char* pThe_path, const char* pArchive_name, tPDForEveryFileRecurse_cbfn pAction_routine);

extern int C2_HOOK_FASTCALL TestForOriginalCarmaCDinDrive(void);

extern void C2_HOOK_FASTCALL SetDefaultPixelmapFolderName(void);

#endif // GUARD_63_LOADING3_H
