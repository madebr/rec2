#ifndef REC2_LOADING_H
#define REC2_LOADING_H

#include "c2_hooks.h"

#include "rec2_types.h"

void C2_HOOK_FASTCALL StripCRNL(char* line);

DRFILE* C2_HOOK_FASTCALL DRfopen(const char* pFilename, const char* pMode);

char* C2_HOOK_FASTCALL DRreadline(DRFILE* pFile, char* pBuffer);

void C2_HOOK_FASTCALL DRfclose(DRFILE* pFile);

#endif //REC2_LOADING_H
