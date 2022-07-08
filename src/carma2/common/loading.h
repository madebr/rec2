#ifndef REC2_LOADING_H
#define REC2_LOADING_H

#include "c2_hooks.h"

#include "c2_stdio.h"

#include "brender/br_types.h"
#include "rec2_types.h"

#define TWT_MOUNT_SUCCEEDED(TWTVFS) ((TWTVFS) >= 0)

C2_HOOK_VARIABLE_DECLARE_ARRAY(tTwatVfsMountPoint, gTwatVfsMountPoints, 5);

void C2_HOOK_FASTCALL StripCRNL(char* line);

tTWTFILE* C2_HOOK_FASTCALL DRfopen(const char* pFilename, const char* pMode);

char* C2_HOOK_FASTCALL DRreadline(tTWTFILE* pFile, char* pBuffer);

void C2_HOOK_FASTCALL DRfclose(tTWTFILE* pFile);

br_size_t C2_HOOK_FASTCALL DRfread(void* buf, br_size_t size, unsigned int n, void* f);

br_size_t C2_HOOK_FASTCALL DRfwrite(void* buf, br_size_t size, unsigned int n, void* f);

int C2_HOOK_FASTCALL DRfeof(tTWTFILE* pFile);

int C2_HOOK_FASTCALL DRfgetc(tTWTFILE* pFile);

char* C2_HOOK_FASTCALL DRfgets(char* buffer, br_size_t size, tTWTFILE* pFile);

/// FIXME: rename to DRxxx
tU32 C2_HOOK_FASTCALL TWT_ReadBinaryU32(FILE* file);

void C2_HOOK_FASTCALL TWT_Init(void);

tTWTVFS C2_HOOK_FASTCALL TWT_Mount(const char* path);

tTWTVFS C2_HOOK_FASTCALL TWT_MountEx(const char* path);

br_pixelmap* C2_HOOK_FASTCALL DRLoadPixelmap(char* pPath_name);

#endif //REC2_LOADING_H
