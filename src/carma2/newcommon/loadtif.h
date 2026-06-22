#ifndef GUARD_LOADTIF_H
#define GUARD_LOADTIF_H

#include "brender/brender.h"

extern br_pixelmap* C2_HOOK_FASTCALL CreatePalettePixelmapFromRGBChannels(br_uint_16* pRed_channel, br_uint_16* pGrn_channel, br_uint_16* pBlu_channel, int pRGB555);

extern br_pixelmap* C2_HOOK_FASTCALL LoadDefaultPalette(const char* pData_dir_path, int pFlags, int *pError_code);

extern br_pixelmap* C2_HOOK_FASTCALL LoadTiffTexture_Ex2(const char* texturePathDir, const char* textureName, br_pixelmap* pPalette, int flags, int* errorCode, int useTiffx);

extern br_pixelmap* C2_HOOK_FASTCALL LoadTiffTexture_Ex(const char* texturePathDir, const char* textureName, br_pixelmap* pPalette, int flags, int* errorCode);

extern int C2_HOOK_FASTCALL WriteFileLink(const char* pLink_dest, const char* pLink_source);

extern int C2_HOOK_FASTCALL ReadFileLink(char* pDest, const char* pPath);

extern int C2_HOOK_FASTCALL PDGetLastModificationTime(const char* pPath);

extern int C2_HOOK_FASTCALL PDFileExists(const char *pPath);

extern int C2_HOOK_FASTCALL PDmkdir(const char *pPath);

extern br_pixelmap* C2_HOOK_FASTCALL LoadTiff8bit(const char *pPath, int pFlags, int *pError_code);

extern br_pixelmap* C2_HOOK_FASTCALL LoadTiff16bit(const char *pPath, int pFlags, int *pError_code);

extern br_pixelmap* C2_HOOK_FASTCALL LoadTiffUsingPalette(const char* pPath, br_pixelmap* pPalette, int pFlags, int* pError_code);

extern int BuildPathWithDirAndSuffix(char* pDest_dir_path, char* pDest_file_path, const char* pOld_path, const char* pNew_dirname, const char *pOld_suffix, const char* pNew_suffix);

extern int C2_HOOK_FASTCALL DRstrrcasestr(int* pOffset, const char* pHaystack, int pLength_haystack, const char* pNeedle);

extern int C2_HOOK_FASTCALL FindPaletteColour(br_colour pRgb, br_pixelmap *pPalette);

#endif // GUARD_LOADTIF_H
