#ifndef REC2_HIMAGE_H
#define REC2_HIMAGE_H

#include "c2_hooks.h"

#include "brender/br_types.h"

void* C2_HOOK_CDECL HostImageLoad(char* name);

void C2_HOOK_CDECL HostImageUnload(void* image);

void* C2_HOOK_CDECL HostImageLookupName(void* img, char* name, br_uint_32 hint);

void* C2_HOOK_CDECL HostImageLookupOrdinal(void* img, br_uint_32 ordinal);

#endif // REC2_HIMAGE_H
