#ifndef GUARD_63_LOADING3_H
#define GUARD_63_LOADING3_H

#include "c2_hooks.h"
#include "brender/brender.h"

// NearestPowerOfTwo

// LoadBunchOfPixies

// DRLdImg

// RealLoadPixelmap

extern br_pixelmap* C2_HOOK_FASTCALL LoadPixelmap(const char* pPath_name);

// LoadShadeTable

// LoadMaterial

// LoadModel

// LoadActor

// DRLoadPalette

// DRLoadShadeTable

// DRLoadMaterials

// DRLoadModels

// DRLoadActors

// DRLoadLights

// LoadInFiles

extern int C2_HOOK_FASTCALL TestForOriginalCarmaCDinDrive(void);

extern void C2_HOOK_FASTCALL SetDefaultPixelmapFolderName(void);

#endif // GUARD_63_LOADING3_H
