#ifndef REC2_LOADING_H
#define REC2_LOADING_H

#include "c2_hooks.h"

#include "c2_stdio.h"

#include "rec2_types.h"

#define TWT_MOUNT_SUCCEEDED(TWTVFS) ((TWTVFS) >= 0)

C2_HOOK_VARIABLE_DECLARE_ARRAY(tTwatVfsMountPoint, gTwatVfsMountPoints, 5);

void C2_HOOK_FASTCALL StripCRNL(char* line);

tTWTFILE* C2_HOOK_FASTCALL DRfopen(const char* pFilename, const char* pMode);

char* C2_HOOK_FASTCALL DRreadline(tTWTFILE* pFile, char* pBuffer);

void C2_HOOK_FASTCALL DRfclose(tTWTFILE* pFile);

void C2_HOOK_FASTCALL TWT_Init(void);

tU32 C2_HOOK_FASTCALL TWT_ReadBinaryU32(FILE* file);

tTWTVFS C2_HOOK_FASTCALL TWT_Mount(const char* path);

tTWTVFS C2_HOOK_FASTCALL TWT_MountEx(const char* path);

#endif //REC2_LOADING_H
