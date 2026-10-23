#ifndef GUARD_63_LOADING3_H
#define GUARD_63_LOADING3_H

#include "c2_hooks.h"
#include "brender/brender.h"
#include "rec2_types.h"

extern int C2_HOOK_FASTCALL LoadBunchOfPixies(const char* pathRoot, const char* textureName, br_pixelmap** textureBuffer, size_t bufferCapacity);

extern br_pixelmap* C2_HOOK_FASTCALL DRLdImg(const char* texturePathDir, const char* textureName, br_pixelmap* pPalette, int flags, int* errorCode);

extern br_pixelmap* C2_HOOK_FASTCALL RealLoadPixelmap(const char* pPath_name);

extern br_pixelmap* C2_HOOK_FASTCALL LoadPixelmap(const char* pPath_name);

extern br_pixelmap* C2_HOOK_FASTCALL LoadPixelmap2(const char* pPath_name);

extern int C2_HOOK_FASTCALL LoadInterfacePixelmaps(const char *pPath_name, br_pixelmap** pMaps, int pCapacity);

extern br_pixelmap* C2_HOOK_FASTCALL LoadShadeTable(const char* pName);

extern br_material* C2_HOOK_FASTCALL LoadMaterial(const char* pThe_path);

extern br_model* C2_HOOK_FASTCALL LoadModel(const char* pName);

extern br_actor* C2_HOOK_FASTCALL LoadActor(const char* pName);

extern void C2_HOOK_FASTCALL DRLoadPalette(const char* pPath_name);

extern void C2_HOOK_FASTCALL DRLoadShadeTable(const char* pPath_name);

extern void C2_HOOK_FASTCALL DRLoadMaterials(const char* pPath_name);

extern void C2_HOOK_FASTCALL DRLoadModels(const char* pPath_name);

extern void C2_HOOK_FASTCALL DRLoadActors(const char* pPath_name);

extern void C2_HOOK_FASTCALL DRLoadLights(const char* pPath_name);

extern void C2_HOOK_FASTCALL LoadInFiles(const char* pThe_path, const char* pArchive_name, tPDForEveryFileRecurse_cbfn pAction_routine);

extern int C2_HOOK_FASTCALL TestForOriginalCarmaCDinDrive(void);

extern int C2_HOOK_FASTCALL OriginalCarmaCDinDrive(void);

extern int C2_HOOK_FASTCALL CarmaCDinDriveOrFullGameInstalled(void);

extern void C2_HOOK_FASTCALL SetDefaultPixelmapFolderName(void);

extern char* gPedTexturePath;

extern void C2_HOOK_FASTCALL ConfigureBloodPedTexturePath(void);

extern void C2_HOOK_FASTCALL ConfigureZombiePedTexturePath(void);

extern void C2_HOOK_FASTCALL ConfigureAlienPedTexturePath(void);

#endif // GUARD_63_LOADING3_H
