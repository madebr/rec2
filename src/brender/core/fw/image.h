#ifndef REC2_IMAGE_H
#define REC2_IMAGE_H

#include "c2_hooks.h"

#include "brender/br_types.h"

br_boolean C2_HOOK_CDECL BrImageAdd(br_image* img);

br_boolean C2_HOOK_CDECL BrImageRemove(br_image* img);

br_image* C2_HOOK_CDECL BrImageFind(char* pattern);

br_image* C2_HOOK_STDCALL imageLoadHost(char* name);

br_image* C2_HOOK_CDECL BrImageReference(char* name);

void* C2_HOOK_STDCALL imageLookupName(br_image* img, char* name, br_uint_32 hint);

void* C2_HOOK_CDECL BrImageLookupName(br_image* img, char* name, br_uint_32 hint);

void* C2_HOOK_CDECL BrImageLookupOrdinal(br_image* img, br_uint_32 ordinal);

void C2_HOOK_CDECL BrImageDereference(br_image* image);

void C2_HOOK_CDECL BrImageFree(br_image* image);

void C2_HOOK_CDECL _BrImageFree(void* res, br_uint_8 res_class, br_size_t size);

#endif // REC2_IMAGE_H
