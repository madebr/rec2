#include "himage.h"

#include <windows.h>

void* (C2_HOOK_CDECL * HostImageLoad_original)(char* name);
void* C2_HOOK_CDECL HostImageLoad(char* name) {
#if defined(C2_HOOKS_ENABLED)
    return HostImageLoad_original(name);
#else
#error "not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0053fb50, HostImageLoad, HostImageLoad_original)


void (C2_HOOK_CDECL * HostImageUnload_original)(void* image);
void C2_HOOK_CDECL HostImageUnload(void* image) {
#if defined(C2_HOOKS_ENABLED)
    HostImageUnload_original(image);
#else
#error "not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0053fb60, HostImageUnload, HostImageUnload_original)

void* (C2_HOOK_CDECL * HostImageLookupName_original)(void* img, char* name, br_uint_32 hint);
void* C2_HOOK_CDECL HostImageLookupName(void* img, char* name, br_uint_32 hint) {
#if 0//defined(C2_HOOKS_ENABLED)
    return HostImageLookupName_original(img, name, hint);
#else

    return GetProcAddress((HMODULE)img, name);
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0053fb70, HostImageLookupName, HostImageLookupName_original)

void* (C2_HOOK_CDECL * HostImageLookupOrdinal_original)(void* img, br_uint_32 ordinal);
void* C2_HOOK_CDECL HostImageLookupOrdinal(void* img, br_uint_32 ordinal) {
#if 0//defined(C2_HOOKS_ENABLED)
    return HostImageLookupOrdinal_original(img, ordinal);
#else

    return NULL;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0053fb90, HostImageLookupOrdinal, HostImageLookupOrdinal_original)
